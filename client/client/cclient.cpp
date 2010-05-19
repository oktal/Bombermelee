#include "cclient.h"
#include "cconnect.h"
#include "cnetworkmanager.h"
#include "cplayer.h"
#include "cbomb.h"
#include "cbonus.h"
#include <QtGui>
#include <QTcpSocket>
#include <QEvent>
#include <cctype>
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
static const QChar SeparatorToken = ' ';

/**
  * function _m transforms a message into a valid message request (ending by \r\n)
  * @return message request
*/
static inline char const *_m(const std::string &message)
{
    return std::string(message + "\r\n").c_str();
}

CClient::CClient(QWidget *parent, const QString &address, const QString &nick) :
        m_address(address), m_nick(nick)
{
    m_gameBoard = new CGameBoard(this, QPoint(10, 10), QSize(630, 510));
    m_gameBoard->setNick(nick.toStdString());

    m_chatBox = new QTextEdit(this);
    m_chatBox->setReadOnly(true);
    m_chatBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_chatBox->move(QPoint(10, 530));
    m_chatBox->resize(QSize(530, 100));

    m_message = new QLineEdit(this);
    m_message->move(QPoint(10, 640));
    m_message->resize(QSize(420, 20));
    m_message->installEventFilter(this);

    m_btn_send = new QPushButton(this);
    m_btn_send->setText(tr("Send"));
    m_btn_send->move(QPoint(440, 640));
    m_btn_send->resize(QSize(100, 20));

    m_lst_users = new QListWidget(this);
    m_lst_users->move(QPoint(550, 530));
    m_lst_users->resize(QSize(90, 100));

    m_socket = new QTcpSocket();
    m_gameBoard->setSocket(m_socket);

    m_pingTimer = new QTimer(this);
    m_pingTimer->start(PingSendingTime);

    QObject::connect(m_btn_send, SIGNAL(clicked()), this, SLOT(sendMessage()));
    QObject::connect(m_pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));

    setFixedSize(650, 680);
    connectToServer();
}

void CClient::connectToServer()
{
    m_socket->connectToHost(m_address, DefaultPort);
    QObject::connect(m_socket, SIGNAL(connected()), this, SLOT(onConnected()));
    QObject::connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError()));
    QObject::connect(m_socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));

    m_networkManager = new CNetworkManager(m_socket);
}

void CClient::onConnected()
{
    m_networkManager->sendEhloPacket();
}

void CClient::onError()
{
    QMessageBox::critical(this, "Error", "Couldn't connect to server: " + m_socket->errorString());
    CConnect *c = new CConnect();
    c->show();
    close();
}

void CClient::sendPing()
{
    m_networkManager->sendPingPacket();
    m_pingTime.restart();
}

void CClient::processReadyRead()
{
    if (m_socket->bytesAvailable() < (int)sizeof(quint32))
    {
        return;
    }
    m_buffer.clear();
    do
    {
        QByteArray c = m_socket->read(1);
        m_buffer.append(c);
    } while (m_socket->bytesAvailable());

    processData();
}

void CClient::processData()
{
    quint32 packet;
    CNetworkManager networkManager(m_socket);
    QDataStream in(&m_buffer, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    quint32 blockSize;
    in >> blockSize;

    if (!blockSize ||
        static_cast<quint32>(m_buffer.size()) != blockSize)
    {
        qDebug() << "Invalid packet received";
        return;
    }

    in >> packet;

    switch (static_cast<CNetworkManager::PacketType>(packet))
    {
    case CNetworkManager::Ehlo:
         m_networkManager->sendNickPacket(m_nick);
         break;
    case CNetworkManager::Badnick:
         QMessageBox::warning(this, "Warning", tr("The nick you chosed is already used, please chose an other one"));
         {
             CConnect *c = new CConnect();
             c->show();
             close();
         }
         break;
    case CNetworkManager::Full:
         QMessageBox::warning(this, "Warning", tr("Can't connect to server because server is full"));
         {
             CConnect *c = new CConnect();
             c->show();
             close();
         }
         break;
    case CNetworkManager::Ok:
         {
            QString color;
            in >> color;
            appendToChatBox(tr("<font color='blue'><em>You are connected</em></font>"));
            appendToChatBox(tr("<font color='blue'><em>You have been assigned color "
                           "<strong>%2</strong></em></font>").arg(color));
            m_gameBoard->setPlayerColor(color.toStdString());
            m_gameBoard->setConnected(true);
            setWindowTitle(m_nick + " connected on " + m_address);
            m_networkManager->sendUsersPacket();
        }
        break;
    case CNetworkManager::Pong:
        {
            m_gameBoard->setPingTime(static_cast<unsigned>(m_pingTime.elapsed()));
        }
        break;
    case CNetworkManager::Join:
        {
            QString nick, color;
            in >> nick;
            in >> color;
            appendToChatBox(tr("<font color='blue'><em>%1 has joined the game an "
                               "has been assigned color "
                               "<strong>%2</strong></em></font>").arg(nick).arg(color));
            appendToUsersList(nick, color);
            m_gameBoard->newPlayer(nick.toStdString(), color.toStdString());
        }
        break;
    case CNetworkManager::Part:
        {
            QString nick;
            in >> nick;
            appendToChatBox(tr("<font color='blue'><em>%1 has left the game</em></font>").arg(nick));
            for (int i = 0; i < m_lst_users->count(); ++i)
            {
                if (m_lst_users->item(i)->text() == nick)
                {
                    QListWidgetItem *it = m_lst_users->takeItem(i);
                    delete it;
                    break;
                }
            }
            m_gameBoard->playerLeft(nick.toStdString());
        }
        break;
    case CNetworkManager::Users:
        {
            qDebug() << "Received Users Packet";
            QList<QString> users;
            in >> users;
            for (int i = 0; i < users.length(); ++i)
            {
                QStringList c = users[i].split(":");
                appendToUsersList(c[0], c[1]);
                if (c[0] != m_nick)
                {
                    m_gameBoard->newPlayer(c[0].toStdString(), c[1].toStdString());
                }
            }
        }
        break;
    case CNetworkManager::Say:
        {
            qDebug() << "Received Say Packet";
            QString nick, message;
            in >> nick;
            in >> message;
            QString time = QTime::currentTime().toString();
            if (nick != m_nick)
            {
                appendToChatBox(tr("<font color='blue'>[%1] %2 : %3</font>").arg(time).arg(nick).arg(message));
            }
        }
        break;
    case CNetworkManager::Move:
        {
            QString nick;
            float x;
            float y;
            quint8 direction;
            in >> nick;
            in >> direction;
            in >> x;
            in >> y;
            if (nick != m_nick)
            {
                m_gameBoard->playerMove(nick.toStdString(), static_cast<Direction>(direction), x, y);
            }
        }
        break;
    case CNetworkManager::Map:
        {
            QString map;
            in >> map;
            m_gameBoard->setMap(map.toStdString());
        }
        break;
    case CNetworkManager::Bomb:
        {
            QString nick;
            unsigned x, y;
            quint8 type;
            in >> nick;
            in >> x;
            in >> y;
            in >> type;
            if (nick != m_nick)
            {
                m_gameBoard->plantedBomb(nick.toStdString(), x, y, static_cast<CBomb::BombType>(type));
            }
        }
        break;
    case CNetworkManager::Boom:
        {
            QString nick;
            unsigned x, y;
            in >> nick;
            in >> x;
            in >> y;
            if (nick != m_nick)
            {
                m_gameBoard->remoteExplode(nick.toStdString(), x, y);
            }
        }
        break;
    case CNetworkManager::Bonus:
        {
            QString nick;
            quint8 bonustype;
            in >> nick;
            in >> bonustype;
            QString type = "";
            if (nick != m_nick)
            {
                m_gameBoard->playerGotBonus(nick.toStdString(), static_cast<CBonus::BonusType>(bonustype));
                CBonus bonus(static_cast<CBonus::BonusType>(bonustype));
                QStringList bonusList, malusList;
                bonusList << "Speed Up" << "Bomb uP" << "Fire Up" << "Full Fire" << "Remote Mine"
                          << "Bomb Kick" << "Bomb Pass";
                malusList << "Speed Down" << "Bomb Down" << "Fire Down";
                if(bonusList.contains(QString(bonus.toString().c_str())))
                {
                    type = "Bonus";
                }
                else if (malusList.contains(QString(bonus.toString().c_str())))
                {
                    type = "Malus";
                }
                appendToChatBox(tr("<font color='blue'><em>%1 has got <strong>%2</strong> %3</em></font>")
                                .arg(nick).arg(QString(bonus.toString().c_str())).arg(type));
            }
        }
        break;
    default:
        break;
    }
}

void CClient::appendToChatBox(const QString &text)
{
    m_chatBox->append(text);
}

void CClient::appendToUsersList(const QString &nick, const QString &color)
{
    QListWidgetItem *item = new QListWidgetItem(nick);
    QString iconFileName = QString("../bullet-%1.png").arg(color);
    item->setIcon(QIcon(iconFileName));
    m_lst_users->addItem(item);
}

void CClient::sendMessage()
{
    if (m_message->text().isEmpty())
    {
        return;
    }
    m_networkManager->sendSayPacket(m_nick, m_message->text());
    QString time = QTime::currentTime().toString();
    appendToChatBox(tr("<font color='blue'>[%1] %2 : %3</font>").arg(time).arg(m_nick).arg(m_message->text()));
    m_message->clear();
}


bool CClient::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == m_message)
    {
        if (ev->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);
            if (keyEvent->key() == Qt::Key_Enter ||
                keyEvent->key() == Qt::Key_Return)
            {
                sendMessage();
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return QWidget::eventFilter(obj, ev);
    }
}
