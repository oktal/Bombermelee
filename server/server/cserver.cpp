#include "cserver.h"
#include "cmapgen.h"
#include <QTcpSocket>
#include <QNetworkInterface>

/*
This file is part of Bombermelee.

    Bombermelee is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bombermelee is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Bombermelee.  If not, see <http://www.gnu.org/licenses/>.
*/

static const quint16 DefaultPort = 45000;
static const quint16 MaximumSlots = 4; /* 4 Players maximum */
static const QChar SeparatorToken = ' ';

/**
  * function _m transforms a message into a valid message request (ending by \r\n)
  * @return message request
*/
static inline char const *_m(const std::string &message)
{
    return std::string(message + "\r\n").c_str();
}

CServer::CServer(QWidget *parent) :
    QMainWindow(parent)
{
    m_console = new QTextEdit();
    m_console->setStyleSheet("background-color: black; color: white;");
    m_console->setText("Bombermelee dedicated server");
    m_console->setReadOnly(true);
    m_btn_launch = new QPushButton("Launch Game");
    m_btn_pause = new QPushButton("Pause Game");

    QWidget *mainWidget = new QWidget();
    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(m_console, 0, 0, 1, 0);
    mainLayout->addWidget(m_btn_launch, 1, 0);
    mainLayout->addWidget(m_btn_pause, 1, 1);
    mainWidget->setLayout(mainLayout);

    setCentralWidget(mainWidget);
    setWindowTitle("Bombermelee Dedicated Server");
    setFixedSize(600, 400);

    startServer();
    QObject::connect(m_btn_launch, SIGNAL(clicked()), this, SLOT(sendMapToClients()));
}

void CServer::startServer()
{
    m_server = new QTcpServer(this);
    if (!m_server->listen(getLocalIpAddress(), DefaultPort))
    {
        abort();
    }
    setWindowTitle(tr("Bombermelee Dedicated Server - listening on %1:%2").arg(m_server->serverAddress().toString()
                                                                          ).arg(m_server->serverPort()));
    QObject::connect(m_server, SIGNAL(newConnection()), this, SLOT(onConnect()));

}

/**
  get the local ipv4 address
  @return the local ipv4 address
*/
QHostAddress CServer::getLocalIpAddress()
{
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        if (interface.flags().testFlag(QNetworkInterface::IsRunning))
        {
            foreach (QNetworkAddressEntry entry, interface.addressEntries())
            {
                if (entry.ip().toString().contains(".") &&
                    entry.ip().toString() != "127.0.0.1")
                {
                    //return entry.ip();
                }
            }
        }
    }
    return QHostAddress::Any;
}


/* When a new client connects to the server */
void CServer::onConnect()
{
    QTcpSocket *socket = m_server->nextPendingConnection();
    if (m_clientsList.size() == MaximumSlots)
    {
        send(socket, "FULL");
        return;
    }
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
}

/* When a client disconnects */
void CServer::onDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    QHash<QString, QTcpSocket *>::const_iterator it = m_clientsList.constBegin();
    QString nick = "";
    while (it != m_clientsList.constEnd())
    {
        QTcpSocket *client = it.value();
        if (client == socket)
        {
            nick = it.key();
            appendToConsole(tr("<strong>%1</strong> has left").arg(nick));
            m_clientsList.remove(nick);
            break;
        }
        ++it;
    }
    broadcast("PART " + nick, socket);


}

void CServer::processReadyRead()
{
    /* sender of the message */
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket->bytesAvailable() < (int)sizeof(quint16))
    {
        return;
    }
    m_buffer.clear();
    do
    {
        QByteArray c = socket->read(1);
        /* A valid message must be ended by "\r\n". If not, we don't treat it */
        if (c == "\r")
        {
            if ((c = socket->read(1)) != "\n")
            {
                return;
            }
        }
        else
        {
            m_buffer.append(c);
        }
    } while (socket->bytesAvailable());
    appendToConsole(m_buffer.constData());
    readProtocolHeader();
    processData(socket);
}

/**
  * Read the protocol Header
*/
void CServer::readProtocolHeader()
{
    QStringList l = QString(m_buffer.data()).split(SeparatorToken);
    if (l[0] == "EHLO")
    {
        messageType = Ehlo;
    }
    else if (l[0] == "NICK")
    {
        messageType = Nick;
    }
    else if (l[0] == "PONG")
    {
        messageType = Pong;
    }
    else if (l[0] == "USERS")
    {
        messageType = Users;
    }
    else if (l[0] == "SAY")
    {
        messageType = Say;
    }
    else
    {
        messageType = PlainDataText;
    }
}

void CServer::processData(QTcpSocket *sender)
{
    QStringList l = QString(m_buffer.data()).split(SeparatorToken);
    switch (messageType)
    {
    case Ehlo:
        send(sender, "EHLO");
        break;
    case Nick:
        /* Let's check if request is conformly formed */
        if (l.size() < 1)
        {
            return;
        }
        /* Let's check if nick is not already in use */
        if (nickAlreadyInUse(l[1]))
        {
            send(sender, "BADNICK");
            return;
        }
        send(sender, "OK");
        appendToConsole(tr("<strong>%1</strong> has joined").arg(l[1]));
        m_clientsList[l[1]] = sender;
        broadcast("JOIN " + l[1], sender);
        break;
    case Users:
        {
             /* Send the whole users list */
             QString response = "USERS ";
             QList<QString> nicks = m_clientsList.keys();
             for (int i = 0; i < nicks.count(); ++i)
             {
                 response += nicks[i];
                 if (i < nicks.count() - 1)
                 {
                    response += " ";
                }
             }
            send(sender, response);
        }
        break;
    case Say:
        /* Broadcast the message to all over clients */
        broadcast(QString(m_buffer.data()), sender);
        break;
    case PlainDataText:
    case Pong:
    case Undefined:
        break;

    }
}

/**
  * Broadcast a message to all connected clients
*/

void CServer::broadcast(const QString &message, QTcpSocket *except)
{
    QHash<QString, QTcpSocket *>::const_iterator it = m_clientsList.constBegin();
    while (it != m_clientsList.constEnd())
    {
        QTcpSocket *client = it.value();
        if (client != except)
        {
            send(client, message);
        }
        ++it;
    }
}

void CServer::sendMapToClients()
{
    CMapGen map;
    std::string request;
    map.generateMap();
    request = "MAP " + map.getMap();
    broadcast(QString(request.c_str()), NULL);
}

bool CServer::nickAlreadyInUse(const QString &nick)
{
    return m_clientsList.contains(nick);
}

void CServer::appendToConsole(const QString &text)
{
    m_console->append(text);
}

void CServer::send(QTcpSocket *to, const QString &what)
{
    const qint64 len = what.length() + 2;
    while (to->write(_m(what.toStdString())) != len)
    {

    }
}

CServer::~CServer()
{
    m_buffer.clear();
    m_clientsList.clear();
}

void CServer::changeEvent(QEvent *e)
{

}
