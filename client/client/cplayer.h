#ifndef CPLAYER_H
#define CPLAYER_H

#include <string>
#include "Animated.h"
#include "cmap.h"

enum Direction
{
    Left, Right, Down, Up
};
static const int Speed = 100; /* Speed of the bomberman */

class CPlayer : public Animated
{
public:
    CPlayer(std::string color = "");
    void move(Direction move, const float &ElapsedTime);
    void setDirection(Direction move);
    void explode();

private:
    std::string m_color;
    sf::Image m_img_player;
    Anim m_player_up;
    Anim m_player_down;
    Anim m_player_left;
    Anim m_player_right;
    Anim m_player_explode;
};

#endif // CPLAYER_H
