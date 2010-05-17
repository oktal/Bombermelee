#ifndef CNETWORKMANAGER_H
#define CNETWORKMANAGER_H

#include <QTcpSocket>
#include <QByteArray>
#include "cbonus.h"
#include "cplayer.h"
#include "cbomb.h"

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
        Pong      = Boom << 1
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

private:
    void sendData(QByteArray block);
    QTcpSocket *m_socket;

};

#endif // CNETWORKMANAGER_H
