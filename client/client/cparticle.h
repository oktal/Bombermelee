#ifndef CPARTICLE_H
#define CPARTICLE_H

#include "Animated.h"

/*
This file is part of Bombermelee.

    Bombermelee is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bombermelee is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Bombermelee.  If not, see <http://www.gnu.org/licenses/>.
*/

class CParticle : public Animated
{
public:
    enum ParticleType
    {
        Middle,
        HorizontalLeft, HorizontalRight,
        VerticalUp, VerticalDown,
        Horizontal, Vertical
    };

    CParticle(ParticleType type, sf::Image explosion);
    ParticleType getType() const;

private:
    Anim m_anim;
    ParticleType m_type;
    sf::Image m_explosion;
};

#endif // CPARTICLE_H
