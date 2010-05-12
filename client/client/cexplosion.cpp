#include "cexplosion.h"
#include "cimagemanager.h"
#include <iostream>

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

CExplosion::CExplosion(unsigned x, unsigned y)
{
    CImageManager *imageManager = CImageManager::GetInstance();
    m_explosion = *imageManager->GetImage("../explosion.png");
    m_x = x;
    m_y = y;

    m_particles.push_back(new CParticle(CParticle::Middle,
                                        x, y, m_explosion));
    m_particles.push_back(new CParticle(CParticle::HorizontalRight,
                                        x + 2, y, m_explosion));
    m_particles.push_back(new CParticle(CParticle::HorizontalLeft,
                                        x - 1, y, m_explosion));
    m_particles.push_back(new CParticle(CParticle::VerticalUp,
                                        x, y - 1, m_explosion));
    m_particles.push_back(new CParticle(CParticle::VerticalDown,
                                        x, y + 1, m_explosion));
    m_particles.push_back(new CParticle(CParticle::Horizontal,
                                        x + 1, y, m_explosion));
}

QVector<CParticle *> CExplosion::getParticles() const
{
    return m_particles;
}
