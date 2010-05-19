#ifndef CGAMEBOARD_H
#define CGAMEBOARD_H

#include "QSFMLCanvas.hpp"
#include "Animated.h"
#include "cplayer.h"
#include <QString>
#include <string>
#include <QHash>
#include <QTcpSocket>
#include <QList>
#include "cimagemanager.h"
#include "cexplosion.h"
#include "cbonuscanvas.h"
#include "cbomb.h"
#include "cnetworkmanager.h"
#include "cbonus.h"

#include <SFML/System.hpp>

static const unsigned WarmupTime = 5; /* 5 seconds before the game begins */

enum Status
{
    Waiting_Players, Warmup, None
};

class CGameBoard : public QSFMLCanvas
{
    Q_OBJECT
public:
    CGameBoard(QWidget *parent, const QPoint &position, const QSize &size, unsigned int FrameTime = 0);
    ~CGameBoard();
    void setPlayerColor(std::string color);
    void setSocket(QTcpSocket *socket);
    void setNick(const std::string &nick);

    void newPlayer(const std::string &nick, const std::string &color);
    void playerLeft(const std::string &nick);
    void playerMove(const std::string &nick, Direction direction, const float x, const float y);
    void playerGotBonus(const std::string &nick, CBonus::BonusType type);
    void remoteExplode(const std::string &bomber, unsigned x, unsigned y);

    void pongReceived();

    void setMap(std::string map);
    void plantedBomb(const std::string &bomber, unsigned x, unsigned y,
                     CBomb::BombType type);
    void setConnected(bool connected);
    void setPingTime(unsigned pingTime);

private:
    /* private methods */
    void OnInit();
    void OnUpdate();
    void drawMap();
    void drawPlayers();
    void drawExplosions();
    void drawFPS();
    void drawStatus();
    void drawBonusCanvas();
    bool canMove(Direction movement);
    void plantBomb();
    void useSpecialBonus();
    CPlayer *getPlayerFromNick(const std::string &nick);

    /* Images and Sprites */
    sf::Sprite m_wall;
    sf::Sprite m_box;
    sf::Sprite m_bonus;
    sf::Image m_explosion;

    QList<CPlayer *> m_playersList;
    QList<CExplosion *> m_explosionsList;
    QList<CBonus *> m_bonusList;
    QList<CBomb *> m_bombsList;
    QString m_nick;
    QTcpSocket *m_socket;
    QTimer *warmupTimer;
    CBonusCanvas *m_bonusCanvas;
    CNetworkManager *m_networkManager;
    bool m_gameBegin;
    bool m_connected;
    unsigned m_warmupTime;
    float m_frameRate;
    unsigned m_pingTime;
    CMap m_map;
    Status m_status;
    sf::Clock m_fpsRefreshTime;

private slots:
    void checkWarmupTime();
};

#endif // CGAMEBOARD_H
