#ifndef CGAMEBOARD_H
#define CGAMEBOARD_H

#include "QSFMLCanvas.hpp"
#include "Animated.h"
#include "cplayer.h"
#include <QFrame>

class CGameBoard : public QSFMLCanvas
{
    Q_OBJECT
public:
    CGameBoard(QWidget *parent, const QPoint &position, const QSize &size);

private:
    void OnInit();
    void OnUpdate();
    void drawWalls();
    bool canMove(Direction movement, const float &ElapsedTime);
    sf::Image wall;
    sf::Image box;
    sf::Sprite m_wall;
    sf::Sprite m_box;
    CPlayer m_player;
    CMap m_map;
};

#endif // CGAMEBOARD_H
