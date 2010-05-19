#ifndef CNETWORKMANAGER_H
#define CNETWORKMANAGER_H

#include <QTcpSocket>
#include <QList>

class CNetworkManager
{
public:

    enum PacketType
    {
        Undefined = 1,

        Ehlo      = Undefined << 1,
        Ok        = Ehlo << 1,
        Badnick   = Ok << 1,
        Full      = Badnick << 1,
        Users     = Full << 1,
        Map       = Users << 1,
        Join      = Map << 1,
        Part      = Join << 1,

        Nick      = Part << 1,
        Say       = Nick << 1,
        Move      = Say << 1,
        Bomb      = Move << 1,
        Bonus     = Bomb << 1,
        Boom      = Bonus << 1,
        /* For future */
        Ping      = Boom << 1,
        Pong      = Ping << 1
    };


    CNetworkManager(QTcpSocket *socket = 0);
    void setSocket(QTcpSocket *socket);
    void sendEhloPacket();
    void sendOkPacket(const QString &color);
    void sendBadnickPacket();
    void sendUsersPacket(QList<QString> users);
    void sendJoinPacket(const QString &nick, const QString &color);
    void sendPartPacket(const QString &nick);
    void sendFullPacket();
    void sendMapPacket(const std::string &map);
    void sendPingPacket();
    void sendPongPacket();

private:
    void sendData(QByteArray block);
    QTcpSocket *m_socket;
};

#endif // CNETWORKMANAGER_H
