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
    void playerMove(const std::string &nick, const std::string &move, const float x, const float y);
    void setMap(std::string map);
    void bombExplode(const std::string &bomber, unsigned x, unsigned y);
    void plantedBomb(const std::string &bomber, unsigned x, unsigned y);
    void setConnected(bool connected);

private:
    /* private methods */
    void OnInit();
    void OnUpdate();
    void drawMap();
    void drawPlayers();
    void drawExplosions();
    void drawFPS();
    void drawStatus();
    bool canMove(Direction movement);
    void plantBomb();
    CPlayer *getPlayerFromNick(const std::string &nick);

    /* Images and Sprites */
    sf::Sprite m_wall;
    sf::Sprite m_box;
    sf::Sprite m_bomb;
    sf::Sprite m_bonus;

    QVector<Frame> m_flames;


    QList<CPlayer *> m_playersList;
    QList<CExplosion *> m_explosionsList;
    QString m_nick;
    QTcpSocket *m_socket;
    QTimer *warmupTimer;
    bool m_gameBegin;
    bool m_connected;
    unsigned m_warmupTime;
    CMap m_map;
    Status m_status;

private slots:
    void checkWarmupTime();
};

#endif // CGAMEBOARD_H
