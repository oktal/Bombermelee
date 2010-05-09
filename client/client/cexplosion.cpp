#include "cexplosion.h"
#include "cimagemanager.h"

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
    //CImageManager *imageManager = CImageManager::GetInstance();
    m_explosion.LoadFromFile("../explosion.png");
    m_x = x;
    m_y = y;

    m_particles.push_back(new CParticle(CParticle::Middle, m_explosion));
    m_particles.push_back(new CParticle(CParticle::HorizontalRight, m_explosion));
    m_particles.push_back(new CParticle(CParticle::HorizontalLeft, m_explosion));
    m_particles.push_back(new CParticle(CParticle::VerticalUp, m_explosion));
    m_particles.push_back(new CParticle(CParticle::VerticalDown, m_explosion));

    m_particles[0]->SetPosition(x * BLOCK_SIZE, y * BLOCK_SIZE);
    m_particles[1]->SetPosition((x + 1) * BLOCK_SIZE, y * BLOCK_SIZE);
    m_particles[2]->SetPosition((x - 1) * BLOCK_SIZE, y * BLOCK_SIZE);
    m_particles[3]->SetPosition(x * BLOCK_SIZE, (y - 1) * BLOCK_SIZE);
    m_particles[4]->SetPosition(x * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
}

QVector<CParticle *> CExplosion::getParticles() const
{
    return m_particles;
}
