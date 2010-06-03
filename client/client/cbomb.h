#ifndef CBOMB_H
#define CBOMB_H

#include <string>
#include "cmap.h"
#include <SFML/Graphics.hpp>

static const float ExplodeTime = 5.f;
static const float RemoteExplodeTime = 10.f;

class CBomb : public sf::Sprite
{

public:
    enum BombType
    {
        Normal,
        Remote
    };

    enum Direction
    {
        Right,
        Left,
        Up,
        Down,
        Fixed
    };

    CBomb(unsigned x, unsigned y, BombType type, const std::string &bomber = "");

    unsigned getX() const;
    unsigned getY() const;
    BombType getType() const;
    std::string getBomber() const;
    void updateTime(const float &elapsedTime);
    bool explode() const;
    void setDirection(Direction direction);
    Direction getDirection() const;
    BlockType getCollision(Direction direction, CMap &map) const;
    void move(Direction direction, const float &frameTime);

private:
    unsigned m_x;
    unsigned m_y;
    BombType m_type;
    std::string m_bomber;
    sf::Clock m_clock;
    float m_explodeTime;
    Direction m_direction;
    unsigned m_speed;

};

#endif // CBOMB_H
