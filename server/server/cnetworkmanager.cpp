#include "cnetworkmanager.h"
#include <QByteArray>
#include <QDataStream>
#include <string>

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
    out.device()->seek(0);
    out << (quint32)(block.size());
    sendData(block);

}

void CNetworkManager::sendOkPacket(const QString &color)
{
    qDebug() << "Sending Ok Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Ok;
    out << color;
    out.device()->seek(0);
    out << (quint32)(block.size());
    sendData(block);
}

void CNetworkManager::sendBadnickPacket()
{
    qDebug() << "Sending Badnick Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Badnick;
    out.device()->seek(0);
    out << (quint32)(block.size());
    sendData(block);
}

void CNetworkManager::sendJoinPacket(const QString &nick, const QString &color)
{
    qDebug() << "Sending Join Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Join;
    out << nick;
    out << color;
    out.device()->seek(0);
    out << (quint32)(block.size());
    sendData(block);
}

void CNetworkManager::sendPartPacket(const QString &nick)
{
    qDebug() << "Sending Part Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Part;
    out << nick;
    out.device()->seek(0);
    out << (quint32)(block.size());
    sendData(block);
}

void CNetworkManager::sendFullPacket()
{
    qDebug() << "Sending Full Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Full;
    out.device()->seek(0);
    out << (quint32)(block.size());
    sendData(block);
}

void CNetworkManager::sendUsersPacket(QList<QString> users)
{
    qDebug() << "Sending Users Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Users;
    out << users;
    out.device()->seek(0);
    out << (quint32)(block.size());
    sendData(block);
}

void CNetworkManager::sendMapPacket(const std::string &map)
{
    qDebug() << "Sending Map Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Map;
    out << QString(map.c_str());
    out.device()->seek(0);
    out << (quint32)(block.size());
    sendData(block);
}

void CNetworkManager::sendPingPacket()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Ping;
    out.device()->seek(0);
    out << (quint32)(block.size());
    sendData(block);
}

void CNetworkManager::sendPongPacket()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Pong;
    out.device()->seek(0);
    out << (quint32)(block.size());
    sendData(block);
}

void CNetworkManager::sendData(QByteArray block)
{
    m_socket->write(block);
    m_socket->waitForBytesWritten();
}

