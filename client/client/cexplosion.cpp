#include "cexplosion.h"
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

CExplosion::CExplosion(unsigned x, unsigned y, unsigned range,
                       CMap map, const std::string &bomber,
                       sf::Image explosion)
{
    m_x = x;
    m_y = y;
    m_bomber = bomber;
    m_particles.push_back(new CParticle(CParticle::Middle,
                                        x, y,
                                        explosion));

    /* LEFT */
    for (unsigned i = 1; i <= range; ++i)
    {
        if (map.getBlock((x - i), y) == Wall)
        {
            break;
        }
        else if (map.getBlock(x - i, y) == Box ||
                 map.getBlock(x - i, y) == BonusBox ||
                 i == range)
        {
            m_particles.push_back(new CParticle(CParticle::HorizontalLeft, x - i, y,
                                                explosion));
            break;
        }
        else if (x - i == 0)
        {
            m_particles.push_back(new CParticle(CParticle::HorizontalLeft, x - i, y,
                                                    explosion));
            break;
        }
        else
        {
            m_particles.push_back(new CParticle(CParticle::Horizontal, x - i, y,
                                                explosion));
        }
    }

    /* RIGHT */
    for (unsigned i = 1; i <= range; ++i)
    {
        if (map.getBlock((x + i), y) == Wall ||
            x == MAP_WIDTH - 1)
        {
            break;
        }
        else if (map.getBlock(x + i, y) == Box ||
                 map.getBlock(x + i, y) == BonusBox ||
                 i == range)
        {
            m_particles.push_back(new CParticle(CParticle::HorizontalRight, x + i, y,
                                                explosion));
            break;
        }
        else if (x + i == MAP_WIDTH - 1)
        {
            m_particles.push_back(new CParticle(CParticle::HorizontalRight, x + i, y,
                                                    explosion));
            break;
        }
        else
        {
            m_particles.push_back(new CParticle(CParticle::Horizontal, x + i, y,
                                                explosion));
        }
    }

    /* TOP */
    for (unsigned i = 1; i <= range; ++i)
    {
        if (map.getBlock(x, y - i) == Wall)
        {
            break;
        }
        else if (map.getBlock(x, y - i) == Box ||
                 map.getBlock(x, y - i) == BonusBox ||
                 i == range)
        {
            m_particles.push_back(new CParticle(CParticle::VerticalUp, x, y - i,
                                                explosion));
            break;
        }
        else if (y - i == 0)
        {
            m_particles.push_back(new CParticle(CParticle::VerticalUp, x, y - i,
                                                explosion));
            break;
        }
        else
        {
            m_particles.push_back(new CParticle(CParticle::Vertical, x, y - i,
                                                explosion));
        }
    }

    /* DOWN */
    for (unsigned i = 1; i <= range; ++i)
    {
        if (map.getBlock(x, y + i) == Wall)
        {
            break;
        }
        else if (map.getBlock(x, y + i) == Box ||
                 map.getBlock(x, y + i) == BonusBox ||
                 i == range)
        {
            m_particles.push_back(new CParticle(CParticle::VerticalDown, x, y + i,
                                                explosion));
            break;
        }
        else if (y + i == MAP_HEIGHT)
        {
            m_particles.push_back(new CParticle(CParticle::VerticalDown, x, y + i,
                                                explosion));
            break;
        }
        else
        {
            m_particles.push_back(new CParticle(CParticle::Vertical, x, y + i,
                                                explosion));
        }
    }
}

QVector<CParticle *> CExplosion::getParticles() const
{
    return m_particles;
}

std::string CExplosion::getBomber() const
{
    return m_bomber;
}
