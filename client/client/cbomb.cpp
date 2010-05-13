#include "cbomb.h"

CBomb::CBomb(unsigned x, unsigned y, BombType type, const std::string &bomber)
{
    m_x = x;
    m_y = y;
    m_type = type;
    m_bomber = bomber;
}

unsigned CBomb::getX() const
{
    return m_x;
}

unsigned CBomb::getY() const
{
    return m_y;
}

CBomb::BombType CBomb::getType() const
{
    return m_type;
}

std::string CBomb::getBomber() const
{
    return m_bomber;
}
