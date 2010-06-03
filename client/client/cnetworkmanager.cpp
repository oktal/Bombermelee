#include "cnetworkmanager.h"
#include "cbomb.h"
#include "cplayer.h"
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>

#include <winsock2.h>


CNetworkManager::CNetworkManager(QTcpSocket *socket)
{
   setSocket(socket);
}

void CNetworkManager::setSocket(QTcpSocket *socket)
{
    m_socket = socket;
    if (m_socket != NULL)
    {
        m_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
        /*
        SOCKET sockID = m_socket->socketDescriptor();
        int ret;
        BOOL bOptVal = true;      // seems to be true here, but not completely sure.
        ret = setsockopt(sockID, IPPROTO_TCP, TCP_NODELAY, (char*)&bOptVal, sizeof(bool));
        */
    }
}

void CNetworkManager::sendEhloPacket()
{
    qDebug() << "Sending EHLO Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Ehlo;

    out << (quint32) 0xFFFF;
    out << (quint32) 0xFFFF;

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);

}

void CNetworkManager::sendNickPacket(const QString &nick)
{
    qDebug() << "Sending Nick Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Nick;
    out << nick;

    out << (quint32) 0xFFFF << (quint32) 0xFFFF;
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    sendData(block);
}

void CNetworkManager::sendMovePacket(const std::string &nick, Direction direction, float x, float y)
{
    qDebug() << "Sending Move Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Move;
    out << QString(nick.c_str());
    out << (quint8) direction;
    out << x;
    out << y;

    out << (quint32) 0xFFFF << (quint32) 0xFFFF;

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendBombPacket(const std::string &nick, unsigned x, unsigned y, CBomb::BombType type)
{
    qDebug() << "Sending Bomb Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Bomb;
    out << QString(nick.c_str());
    out << x;
    out << y;
    out << (quint8) type;

    out << (quint32) 0xFFFF << (quint32) 0xFFFF; 

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendBoomPacket(const std::string &nick, unsigned x, unsigned y)
{
    qDebug() << "Sending Boom Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Boom;
    out << QString(nick.c_str());
    out << x;
    out << y;

    out << (quint32) 0xFFFF << (quint32) 0xFFFF;

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendBonusPacket(const std::string &nick, CBonus::BonusType type)
{
    qDebug() << "Sending Bonus Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Bonus;
    out << QString(nick.c_str());
    out << (quint8) type;

    out << (quint32) 0xFFFF << (quint32) 0xFFFF; 

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendUsersPacket()
{
    qDebug() << "Sending Users Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Users;

    out << (quint32) 0xFFFF << (quint32) 0xFFFF;

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendSayPacket(const QString &nick, const QString &message)
{
    qDebug() << "Sending Say Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Say;
    out << nick;
    out << message;

    out << (quint32) 0xFFFF << (quint32) 0xFFFF;

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendPingPacket()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Ping;

    out << (quint32) 0xFFFF << (quint32) 0xFFFF;

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendPongPacket()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Pong;

    out << (quint32) 0xFFFF << (quint32) 0xFFFF;

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendData(QByteArray block)
{
    m_socket->write(block);
    m_socket->waitForBytesWritten();
}

QList<QByteArray> CNetworkManager::getPacketsFromBuffer(QByteArray &buffer)
{
    QList<QByteArray> ret;
    QByteArray packet;
    QByteArray endOfFrame;
    QDataStream stream(&endOfFrame, QIODevice::WriteOnly);
    stream << (quint32) 0xFFFF << (quint32) 0xFFFF;

    QBuffer in;
    in.setBuffer(&buffer);
    in.open(QIODevice::ReadOnly);
    do
    {
        QByteArray c = in.read(1);
        packet.append(c);
        if (packet.contains(endOfFrame))
        {
            ret.append(packet);
            packet.clear();
        }
    } while (in.bytesAvailable());
    return ret;
}
