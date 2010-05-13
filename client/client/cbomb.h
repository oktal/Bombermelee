#ifndef CBOMB_H
#define CBOMB_H

#include <string>

class CBomb
{

public:
    enum BombType
    {
        Normal,
        Remote
    };

    CBomb(unsigned x, unsigned y, BombType type, const std::string &bomber = "");

    unsigned getX() const;
    unsigned getY() const;
    BombType getType() const;
    std::string getBomber() const;

private:
    unsigned m_x;
    unsigned m_y;
    BombType m_type;
    std::string m_bomber;

};

#endif // CBOMB_H
