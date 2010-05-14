#ifndef CEXPLOSION_H
#define CEXPLOSION_H

#include <SFML/Graphics.hpp>
#include <QVector>
#include "cparticle.h"
#include "cmap.h"

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

class CExplosion
{
public:
    CExplosion(unsigned x, unsigned y, unsigned range,
               CMap map, const std::string &bomber,
               sf::Image explosion);
    QVector<CParticle *> getParticles() const;
    std::string getBomber() const;

private:
    QVector<CParticle *> m_particles;
    unsigned m_x;
    unsigned m_y;
    std::string m_bomber;
};

#endif // CEXPLOSION_H
