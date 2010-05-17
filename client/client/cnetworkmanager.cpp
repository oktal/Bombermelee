#include "cnetworkmanager.h"
#include "cbomb.h"
#include "cplayer.h"
#include <QByteArray>
#include <QDataStream>

CNetworkManager::CNetworkManager(QTcpSocket *socket)
{
    m_socket = socket;
}

void CNetworkManager::setSocket(QTcpSocket *socket)
{
    m_socket = socket;
}

void CNetworkManager::sendEhloPacket()
{
    qDebug() << "Sending EHLO Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Ehlo;
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

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendData(QByteArray block)
{
    m_socket->write(block);
    m_socket->waitForBytesWritten();
}
