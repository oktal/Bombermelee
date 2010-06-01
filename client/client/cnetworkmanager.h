#ifndef CNETWORKMANAGER_H
#define CNETWORKMANAGER_H

#include <QTcpSocket>
#include <QByteArray>
#include <QList>
#include "cbonus.h"
#include "cplayer.h"
#include "cbomb.h"

/**
  * @class CNetworkManager
*/
class CNetworkManager
{
public:

    /** All diffents packets */
    enum PacketType
    {
        Undefined = 1,

        Ehlo      = 1 << 1,
        Ok        = 1 << 2,
        Badnick   = 1 << 3,
        Full      = 1 << 4,
        Users     = 1 << 5,
        Map       = 1 << 6,
        Join      = 1 << 7,
        Part      = 1 << 8,

        Nick      = 1 << 9,
        Say       = 1 << 10,
        Move      = 1 << 11,
        Bomb      = 1 << 12,
        Bonus     = 1 << 13,
        Boom      = 1 << 14,
        /* For future */
        Ping      = 1 << 15,
        Pong      = 1 << 16,
    };

    CNetworkManager(QTcpSocket *socket = 0);
    void setSocket(QTcpSocket *socket);
    void sendEhloPacket();
    void sendNickPacket(const QString &nick);
    void sendMovePacket(const std::string &nick, Direction direction, float x, float y);
    void sendBombPacket(const std::string &nick, unsigned x, unsigned y, CBomb::BombType type);
    void sendBoomPacket(const std::string &nick, unsigned x, unsigned y);
    void sendBonusPacket(const std::string &nick, CBonus::BonusType type);
    void sendUsersPacket();
    void sendSayPacket(const QString &nick, const QString &message);
    void sendPingPacket();
    void sendPongPacket();
    QList<QByteArray> getPacketsFromBuffer(QByteArray &buffer);

private:
    void sendData(QByteArray block);
    QTcpSocket *m_socket;
    QByteArray m_endOfFrame;

};

#endif // CNETWORKMANAGER_H
