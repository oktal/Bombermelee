#include "cnetworkmanager.h"
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <string>

CNetworkManager::CNetworkManager(QTcpSocket *socket)
{
    setSocket(socket);
    m_endOfFrame.resize(3);
    m_endOfFrame[0] = 0x00; /* NUL */
    m_endOfFrame[1] = 0x0D; /* CR */
    m_endOfFrame[2] = 0x0A; /* LF */
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
    out << (quint32)(block.size() - sizeof(quint32));
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
    out << (quint32)(block.size() - sizeof(quint32));
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
    out << (quint32)(block.size() - sizeof(quint32));
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
    out << (quint32)(block.size() - sizeof(quint32));
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
    out << (quint32)(block.size() - sizeof(quint32));
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
    out << (quint32)(block.size() - sizeof(quint32));
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
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendMapPacket(const std::string &map, int roundsNumber)
{
    qDebug() << "Sending Map Packet";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint32) 0;
    out << (quint32) Map;
    out << QString(map.c_str());
    out << (quint8) roundsNumber;
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
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    sendData(block);
}

void CNetworkManager::sendData(QByteArray block)
{
    m_socket->write(block);
}

QList<QByteArray> CNetworkManager::getPacketsFromBuffer(QByteArray &buffer)
{
    QList<QByteArray> ret;
    QByteArray packet;
    QBuffer in;
    in.setBuffer(&buffer);
    in.open(QIODevice::ReadOnly);
    do
    {
        QByteArray c = in.read(1);
        if (c[0] == m_endOfFrame[0]) /* NUL */
        {
            QByteArray cr = in.read(1);
            if (cr[0] != m_endOfFrame[1]) /* CR */
            {
                packet.clear();
            }
            else
            {
                QByteArray lf = in.read(1);
                if (lf[0] != m_endOfFrame[2]) /* LF */
                {
                    packet.clear();
                }
                else
                {
                    ret.append(packet);
                }
            }
        }
        else
        {
            packet.append(c);
        }
    } while (in.bytesAvailable());
    return ret;
}
