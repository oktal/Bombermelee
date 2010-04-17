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
    CPlayer(const std::string &nick = "", const std::string &color = "white");
    void move(Direction move, const float &ElapsedTime);
    void setDirection(Direction move);
    void setColor(const std::string &color);
    void setNick(const std::string &nick);
    void setCorrectPosition();
    const std::string &getNick();
    void explode();

private:
    std::string m_color;
    std::string m_nick;
    sf::Image m_img_player;
    Anim m_player_up;
    Anim m_player_down;
    Anim m_player_left;
    Anim m_player_right;
    Anim m_player_explode;
};

#endif // CPLAYER_H
