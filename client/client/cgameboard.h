#ifndef CGAMEBOARD_H
#define CGAMEBOARD_H

#include "QSFMLCanvas.hpp"
#include "Animated.h"
#include "cplayer.h"
#include <QString>
#include <string>
#include <QHash>
#include <QTcpSocket>

static const unsigned WarmupTime = 5; /* 5 seconds before the game begins */

enum Status
{
    Waiting_Player, Warmup, None
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
    void drawFPS();
    void drawStatus();
    bool canMove(Direction movement, const float &ElapsedTime);
    void plantBomb();
    CPlayer *getPlayerFromNick(const std::string &nick);

    /* Images and Sprites */
    sf::Image wall;
    sf::Image box;
    sf::Image bomb;
    sf::Sprite m_wall;
    sf::Sprite m_box;
    sf::Sprite m_bomb;
    QList<CPlayer *> m_playersList;
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
