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
    m_loger = new CLoger("games.log");
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
        CNetworkManager networkManager(socket);
        networkManager.sendFullPacket();
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

    QList<CClient *>::const_iterator it;
    for (it = m_clientsList.begin(); it != m_clientsList.end(); ++it)
    {
        CClient *client = *it;
        client->networkManager->sendPartPacket(nick);
    }
}

void CServer::processReadyRead()
{
    /* sender of the message */
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket->bytesAvailable() < (int)sizeof(quint32))
    {
        return;
    }
    m_buffer.clear();
    do
    {
        QByteArray c = socket->read(1);
        m_buffer.append(c);
    } while (socket->bytesAvailable());
    processData(socket);
}

void CServer::processData(QTcpSocket *sender)
{
    quint32 packet;
    CNetworkManager networkManager(sender);
    QDataStream in(&m_buffer, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    m_loger->logPacket(m_buffer);

    quint32 blockSize;
    in >> blockSize;

    if (!blockSize ||
        static_cast<quint32>(m_buffer.size() - sizeof(quint32)) != blockSize)
    {
        qDebug() << "Invalid packet received";
        return;
    }

    in >> packet;
    QString s;

    switch (static_cast<CNetworkManager::PacketType>(packet))
    {
    case CNetworkManager::Ehlo:
        qDebug() << "Received Ehlo Packet";
        networkManager.sendEhloPacket();
        break;
    case CNetworkManager::Nick:
        qDebug() << "Received Nick Packet";
        {
            QString nick;
            in >> nick;
            if (nickAlreadyInUse(nick))
            {
                networkManager.sendBadnickPacket();
                return;
            }

            QString color = m_colors[rand() % m_colors.size()];
            CClient *newClient = new CClient(sender, nick, color);
            m_colors.removeOne(color);
            networkManager.sendOkPacket(color);
            appendToConsole(tr("<strong>%1</strong> has joined").arg(nick));
            m_clientsList.append(newClient);

            QList<CClient *>::iterator it;
            for (it = m_clientsList.begin(); it != m_clientsList.end(); ++it)
            {
                CClient *client = *it;
                if (client->getSocket() != sender)
                {
                    client->networkManager->sendJoinPacket(nick, color);
                }
            }

        }
        break;
    case CNetworkManager::Users:
        qDebug() << "Received Users Packet";
        {
             /* Send the whole users list */
             QList<QString> users;
             for (int i = 0; i < m_clientsList.size(); ++i)
             {
                 QString user = m_clientsList[i]->getNick() + ":" + m_clientsList[i]->getColor();
                 users.push_back(user);
             }
             networkManager.sendUsersPacket(users);
        }
        break;
    case CNetworkManager::Say:
        in >> s;
        qDebug() << "Received Say Packet from " << s;
        broadcast(m_buffer);
        break;
    case CNetworkManager::Move:
        in >> s;
        qDebug() << "Received Move Packet from " << s;
        broadcast(m_buffer);
        break;
    case CNetworkManager::Bomb:
        in >> s;
        qDebug() << "Received Bomb Packet from " << s;
        broadcast(m_buffer);
        break;
    case CNetworkManager::Bonus:
        in >> s;
        qDebug() << "Received Bonus Packet from " << s;
        broadcast(m_buffer);
        break;
    case CNetworkManager::Boom:
        in >> s;
        qDebug() << "Received Boom Packet from " << s;
        broadcast(m_buffer);
        break;
    case CNetworkManager::Pong:
    case CNetworkManager::Undefined:
        break;
    default:
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

void CServer::broadcast(const QByteArray &data, QTcpSocket *except)
{
    QList<CClient *>::const_iterator it = m_clientsList.constBegin();
    while (it != m_clientsList.constEnd())
    {
        CClient *client = *it;
        if (client->getSocket() != except)
        {
            client->send(data);
        }
        ++it;
    }
}

void CServer::sendMapToClients()
{
    CMapGen map;
    std::string request;
    map.generateMap();
    QList<CClient *>::const_iterator it;
    for (it = m_clientsList.begin(); it != m_clientsList.end(); ++it)
    {
        CClient *client = *it;
        client->networkManager->sendMapPacket(map.getMap());
    }
}

bool CServer::nickAlreadyInUse(const QString &nick)
{
    return getClientFromNick(nick) != NULL;
}

void CServer::appendToConsole(const QString &text)
{
    m_console->append(text);
}



CServer::~CServer()
{
    m_buffer.clear();
    m_clientsList.clear();
}

void CServer::changeEvent(QEvent *e)
{

}
