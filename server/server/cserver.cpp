#include "cserver.h"
#include "cclient.h"
#include "cmapgen.h"
#include <QTcpSocket>
#include <QNetworkInterface>
#include <ctime>
#include <cstdlib>

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

CServer::CServer(QWidget *parent) :
    QMainWindow(parent)
{
    m_console = new QTextEdit();
    m_console->setStyleSheet("background-color: black; color: white;");
    m_console->setText("Bombermelee dedicated server");
    m_console->setReadOnly(true);
    m_btn_launch = new QPushButton("Launch Game");
    m_btn_pause = new QPushButton("Pause Game");
    m_message = new QLineEdit();
    m_btn_send = new QPushButton("send");

    QWidget *mainWidget = new QWidget();
    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(m_console, 0, 0, 1, 0);
    mainLayout->addWidget(m_btn_launch, 1, 0);
    mainLayout->addWidget(m_btn_pause, 1, 1);
    /*mainLayout->addWidget(m_message, 2, 0);
    mainLayout->addWidget(m_btn_send, 2, 1);*/
    mainWidget->setLayout(mainLayout);

    setCentralWidget(mainWidget);
    setWindowTitle("Bombermelee Dedicated Server");
    setFixedSize(600, 400);

    m_colors += "white";
    m_colors += "red";
    m_colors += "green";
    m_colors += "blue";
    srand(time(NULL));

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
    QString nick = "", color = "";
    for (int i = 0; i < m_clientsList.size(); ++i)
    {
        if (m_clientsList[i]->getSocket() == socket)
        {
            nick = m_clientsList[i]->getNick();
            color = m_clientsList[i]->getColor();
            appendToConsole(tr("<strong>%1</strong> has left").arg(nick));
            m_colors += color;
            m_clientsList.removeAt(i);
            break;
        }
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
    else if (l[0] == "MOVE")
    {
        messageType = Move;
    }
    else if (l[0] == "BOMB")
    {
        messageType = Bomb;
    }
    else if (l[0] == "BONUS")
    {
        messageType = Bonus;
    }
    else if (l[0] == "BOOM")
    {
        messageType = Boom;
    }
    else
    {
        messageType = Undefined;
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
        {
            QString color = m_colors[rand() % m_colors.size()];
            CClient *newClient = new CClient(sender, l[1], color);
            m_colors.removeOne(color);
            send(sender, "OK " + color);
            appendToConsole(tr("<strong>%1</strong> has joined").arg(l[1]));
            m_clientsList.append(newClient);
            broadcast("JOIN " + l[1] + " " + color, sender);
        }
        break;
    case Users:
        {
             /* Send the whole users list */
             QString response = "USERS ";
             for (int i = 0; i < m_clientsList.size(); ++i)
             {
                 response += m_clientsList[i]->getNick() + ":" + m_clientsList[i]->getColor();
                 if (i < m_clientsList.count() - 1)
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
    case Move:
        broadcast(QString(m_buffer.data()), sender);
        break;
    case Bomb:
        {
            unsigned x = strtol(l[2].toStdString().c_str(), NULL, 10);
            unsigned y = strtol(l[3].toStdString().c_str(), NULL, 10);
            QString type = l[4];
            CBomb *bomb;
            if (l[4] == "NORMAL")
            {
                bomb = new CBomb(l[1], x, y, ExplodeTime);
            }
            else
            {
                bomb = new CBomb(l[1], x, y, ExplodeTimeRemote);
            }
            m_bombsList.enqueue(bomb);
            QObject::connect(bomb, SIGNAL(explode()), this, SLOT(bombExplode()));
            broadcast(QString(m_buffer.data()), sender);
        }
        break;
    case Bonus:
        broadcast(QString(m_buffer.data()), sender);
        break;
    case Boom:
        broadcast(QString(m_buffer.data()), sender);
        {
            unsigned x = strtol(l[2].toStdString().c_str(), NULL, 10);
            unsigned y = strtol(l[3].toStdString().c_str(), NULL, 10);
            for (int i = 0; i < m_bombsList.size(); ++i)
            {
                CBomb *bomb = m_bombsList[i];
                if (bomb->x == x && bomb->y == y)
                {
                    bomb->stopTimer();
                    m_bombsList.removeAt(i);
                    break;
                }
            }
        }
        break;
    case Pong:
    case Undefined:
        break;

    }
}

CClient *CServer::getClientFromNick(const QString &nick)
{
    CClient *ret = NULL;
    QList<CClient *>::const_iterator it = m_clientsList.constBegin();
    while (it != m_clientsList.constEnd())
    {
        CClient *client = *it;
        if (client->getNick() == nick)
        {
            ret = *it;
            break;
        }
        ++it;
    }
    return ret;
}

/**
  * Broadcast a message to all connected clients
*/

void CServer::broadcast(const QString &message, QTcpSocket *except)
{
    QList<CClient *>::const_iterator it = m_clientsList.constBegin();
    while (it != m_clientsList.constEnd())
    {
        CClient *client = *it;
        if (client->getSocket() != except)
        {
            client->send(message.toStdString());
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

void CServer::bombExplode()
{
    CBomb *bomb = qobject_cast<CBomb *>(sender());
    QString bomber = bomb->bomber;
    unsigned x = bomb->x;
    unsigned y = bomb->y;
    m_bombsList.removeOne(bomb);
    delete bomb;

    QString request = QString("BOOM %1 %2 %3").arg(bomber).arg(x).arg(y);
    broadcast(request, NULL);
}

bool CServer::nickAlreadyInUse(const QString &nick)
{
    return getClientFromNick(nick) != NULL;
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
