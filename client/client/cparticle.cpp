#include "cparticle.h"
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

CParticle::CParticle(ParticleType type, sf::Image explosion) :
        Animated(true, true, 0.05f)
{
    m_explosion = explosion;
    switch (type)
    {
    case Middle:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(34, 29, 64, 59)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(34, 159, 64, 189)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(31, 294, 64, 324)));
        break;
    case HorizontalLeft:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(4, 34, 32, 54)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(4, 159, 32, 184)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(0, 429, 32, 460)));
        break;
    case HorizontalRight:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(99, 34, 127, 50)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(99, 161, 127, 184)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(97, 429, 127, 459)));
        break;
    case VerticalUp:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(40, 0, 57, 27)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(38, 126, 61, 157)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(32, 260, 64, 292)));
        break;
    case VerticalDown:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(41, 93, 59, 121)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(37, 223, 60, 253)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(31, 358, 63, 389)));
        break;
    case Horizontal:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(66, 34, 98, 54)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(66, 161, 98, 185)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(64, 294, 96, 324)));
        break;
    case Vertical:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(39, 60, 58, 92)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(38, 190, 61, 222)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(32, 326, 63, 357)));
        break;
    }
    SetAnim(&m_anim);
    m_type = type;
}

CParticle::ParticleType CParticle::getType() const
{
    return m_type;
}
