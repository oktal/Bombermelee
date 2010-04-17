#ifndef CGAMEBOARD_H
#define CGAMEBOARD_H

#include "QSFMLCanvas.hpp"
#include "Animated.h"
#include "cplayer.h"
#include <QString>
#include <QHash>
#include <QTcpSocket>

class CGameBoard : public QSFMLCanvas
{
    Q_OBJECT
public:
    CGameBoard(QWidget *parent, const QPoint &position, const QSize &size, unsigned int FrameTime = 0);
    void setPlayerColor(std::string color);
    void setSocket(QTcpSocket *socket);
    void setNick(const std::string &nick);
    void newPlayer(const std::string &nick, const std::string &color);
    void playerMove(const std::string &nick, const std::string &move, const float x, const float y);

private:
    void OnInit();
    void OnUpdate();
    void drawWalls();
    void drawOtherPlayers();
    bool canMove(Direction movement, const float &ElapsedTime);
    sf::Image wall;
    sf::Image box;
    sf::Sprite m_wall;
    sf::Sprite m_box;
    CPlayer m_player;
    QHash<QString, CPlayer *> m_otherPlayers;
    QString m_nick;
    QTcpSocket *m_socket;
    CMap m_map;
};

#endif // CGAMEBOARD_H
