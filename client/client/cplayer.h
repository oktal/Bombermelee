#ifndef CPLAYER_H
#define CPLAYER_H

#include <string>
#include "Animated.h"
#include "cmap.h"

enum Direction
{
    Left, Right, Down, Up, Stopped
};
static const int Speed = 120; /* Speed of the bomberman */

class CPlayer : public Animated
{
public:
    CPlayer(const std::string &nick = "", const std::string &color = "white");
    void move(Direction direction, const float &ElapsedTime);
    void setDirection(Direction direction);
    void setMoving(bool moving);
    void setColor(const std::string &color);
    void setNick(const std::string &nick);
    void setStopTime(const float &stopTime);
    void setCorrectPosition();
    float getStopTime() const;
    float getElapsedTime() const;
    const std::string &getNick() const;
    Direction getDirection() const;
    bool canMove(Direction direction, CMap &map) const;
    void explode();
    unsigned maxBombs;
    unsigned pausedBombs;

private:
    std::string m_color;
    std::string m_nick;
    sf::Image m_img_player;
    Anim m_player_up;
    Anim m_player_down;
    Anim m_player_left;
    Anim m_player_right;
    Anim m_player_explode;
    Direction m_direction;
    float m_elapsedTime;
    float m_stopTime;
};

#endif // CPLAYER_H
