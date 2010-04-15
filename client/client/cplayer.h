#ifndef CPLAYER_H
#define CPLAYER_H

#include <string>
#include "Animated.h"
#include "cmap.h"

class CPlayer : public Animated
{
public:
    CPlayer(std::string color = "");
    enum Movement
    {
        Left, Right, Down, Up
    };
    void move(Movement move, const float &ElapsedTime);
    void explode(const float &ElapsedTime);

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
