#include "cclient.h"
#include "cconnect.h"
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

    QObject::connect(m_btn_send, SIGNAL(clicked()), this, SLOT(sendMessage()));

    setFixedSize(650, 680);
    messageType = Undefined;
    connectToServer();
}

void CClient::connectToServer() const
{
    m_socket->connectToHost(m_address, DefaultPort);
    QObject::connect(m_socket, SIGNAL(connected()), this, SLOT(onConnected()));
    QObject::connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError()));
    QObject::connect(m_socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
}

void CClient::onConnected()
{
    m_socket->write(_m("EHLO"));
    m_socket->flush();
}

void CClient::onError()
{
    QMessageBox::critical(this, "Error", "Couldn't connect to server: " + m_socket->errorString());
    CConnect *c = new CConnect();
    c->show();
    close();
}

void CClient::processReadyRead()
{
    if (m_socket->bytesAvailable() < (int)sizeof(quint16))
    {
        return;
    }
    while (!m_buffer.isEmpty())
    {
        m_buffer.clear();
    }
    do
    {
        QByteArray c = m_socket->read(1);
        /* A valid message must be ended by "\r\n". If not, we don't treat it */
        if (c == "\r")
        {
            if ((c = m_socket->read(1)) != "\n")
            {
                return;
            }
        }
        else
        {
            m_buffer.append(c);
        }
    } while (m_socket->bytesAvailable());
    readProtocolHeader();
    processData();
    messageType = Undefined;
}

void CClient::readProtocolHeader()
{
    QStringList l = QString(m_buffer.data()).split(SeparatorToken);
    if (l[0] == "EHLO")
    {
        messageType = Ehlo;
    }
    else if (l[0] == "OK")
    {
        messageType = Ok;
    }
    else if (l[0] == "FULL")
    {
        messageType = Full;
    }
    else if (l[0] == "BADNICK")
    {
        messageType = Badnick;
    }
    else if (l[0] == "JOIN")
    {
        messageType = Join;
    }
    else if (l[0] == "PART")
    {
        messageType = Part;
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
    else if (l[0] == "MAP")
    {
        messageType = Map;
    }
    else if (l[0] == "BOMB")
    {
        messageType = Bomb;
    }
    else if (l[0] == "BOOM")
    {
        messageType = Boom;
    }
    else if (l[0] == "BONUS")
    {
        messageType = Bonus;
    }
}

void CClient::processData()
{
    QStringList l = QString(m_buffer.data()).split(SeparatorToken);
    qDebug() << m_buffer.data();
    switch (messageType)
    {
    case Ehlo:
         m_socket->write(_m("NICK " + m_nick.toStdString()));
         break;
    case Badnick:
         QMessageBox::warning(this, "Warning", tr("The nick you chosed is already used, please chose an other one"));
         {
             CConnect *c = new CConnect();
             c->show();
             close();
         }
         break;
    case Full:
         QMessageBox::warning(this, "Warning", tr("Can't connect to server because server is full"));
         {
             CConnect *c = new CConnect();
             c->show();
             close();
         }
         break;
    case Ok:
        appendToChatBox(tr("<font color='blue'><em>You are connected</em></font>"));
        appendToChatBox(tr("<font color='blue'><em>You have been assigned color "
                           "<strong>%2</strong></em></font>").arg(l[1]));
        m_gameBoard->setPlayerColor(l[1].toStdString());
        m_gameBoard->setConnected(true);
        setWindowTitle(m_nick + " connected on " + m_address);
        m_socket->write(_m("USERS"));
        break;
    case Join:
        appendToChatBox(tr("<font color='blue'><em>%1 has joined the game an "
                           "has been assigned color "
                           "<strong>%2</strong></em></font>").arg(l[1]).arg(l[2]));
        appendToUsersList(l[1], l[2]);
        m_gameBoard->newPlayer(l[1].toStdString(), l[2].toStdString());
        break;
    case Part:
        appendToChatBox(tr("<font color='blue'><em>%1 has left the game</em></font>").arg(l[1]));
        for (int i = 0; i < m_lst_users->count(); ++i)
        {
            if (m_lst_users->item(i)->text() == l[1])
            {
                QListWidgetItem *it = m_lst_users->takeItem(i);
                delete it;
                break;
            }
        }
        m_gameBoard->playerLeft(l[1].toStdString());
        break;
    case Users:
        for (int i = 1; i < l.length(); ++i)
        {
            QStringList c = l[i].split(":");
            appendToUsersList(c[0], c[1]);
            if (c[0] != m_nick)
            {
                m_gameBoard->newPlayer(c[0].toStdString(), c[1].toStdString());
            }
        }
        break;
    case Say:
        {
            QString nick = l[1];
            l.removeFirst();
            l.removeFirst();
            QString message = l.join(" ");
            QString time = QTime::currentTime().toString();
            appendToChatBox(tr("<font color='blue'>[%1] %2 : %3</font>").arg(time).arg(nick).arg(message));
        }
        break;
    case Move:
        {
            const float x = strtod(l[3].toStdString().c_str(), NULL);
            const float y = strtod(l[4].toStdString().c_str(), NULL);
            m_gameBoard->playerMove(l[1].toStdString(), l[2].toStdString(), x, y);
        }
        break;
    case Map:
        m_gameBoard->setMap(l[1].toStdString());
        break;
    case Bomb:
        {
            unsigned x = strtol(l[2].toStdString().c_str(), NULL, 10);
            unsigned y = strtol(l[3].toStdString().c_str(), NULL, 10);
            m_gameBoard->plantedBomb(l[1].toStdString(), x, y, l[4].toStdString());
        }
        break;
    case Boom:
        {
            unsigned x = strtol(l[2].toStdString().c_str(), NULL, 10);
            unsigned y = strtol(l[3].toStdString().c_str(), NULL, 10);
            m_gameBoard->bombExplode(l[1].toStdString(), x, y);
        }
        break;
    case Bonus:
        m_gameBoard->playerGotBonus(l[1].toStdString(), l[2].toStdString());
        {
            QString type = "";
            if (l[2] == "SPEEDUP")
            {
                type = "Bonus";
            }
            else if (l[2] == "SPEEDDOWN")
            {
                type = "Malus";
            }
            else if (l[2] == "BOMBUP")
            {
                type = "Bonus";
            }
            else if (l[2] == "REMOTEMINE")
            {
                type = "Bonus";
            }
            appendToChatBox(tr("<font color='blue'><em>%1 has got <strong>%2</strong> %3</em></font>")
                            .arg(l[1]).arg(l[2]).arg(type));
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
    QString req = "SAY " + m_nick + " ";
    req += m_message->text();
    m_socket->write(_m(req.toStdString()));
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
