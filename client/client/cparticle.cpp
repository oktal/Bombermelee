#include "cparticle.h"
#include "cimagemanager.h"
#include "cmap.h"
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

CParticle::CParticle(ParticleType type, sf::Image explosion) :
        Animated(true, true, 0.05f)
{
    m_explosion = explosion;
    switch (type)
    {
    case Middle:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(169, 215, 199, 244)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(34, 215, 64, 244)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(171, 69, 201, 99)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(34, 67, 64, 96)));
        break;
    case HorizontalLeft:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(142, 219, 166, 242)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(4, 217, 32, 244)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(139, 69, 169, 96)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(0, 68, 32, 97)));
        break;
    case HorizontalRight:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(234, 219, 258, 240)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(99, 217, 127, 243)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(235, 70, 267, 98)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(99, 67, 129, 96)));
        break;
    case VerticalUp:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(172, 156, 196, 180)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(35, 152, 63, 180)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(171, 5, 200, 34)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(35, 0, 64, 32)));
        break;
    case VerticalDown:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(171, 247, 195, 272)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(35, 247, 62, 276)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(172, 100, 201, 129)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(34, 99, 64, 129)));
        break;
    case Horizontal:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(201, 218, 231, 242)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(66, 216, 97, 243)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(203, 70, 234, 99)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(66, 67, 97, 97)));
        break;
    case Vertical:
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(39, 60, 58, 92)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(38, 190, 61, 222)));
        m_anim.PushFrame(Frame(&m_explosion, sf::Rect<int>(32, 326, 63, 357)));
        break;
    }
    SetAnim(&m_anim);
    m_type = type;
    lastFrameCorrected = -1;
}

CParticle::ParticleType CParticle::getType() const
{
    return m_type;
}

void CParticle::correctPosition()
{
    unsigned width = GetSubRect().GetWidth();
    unsigned height = GetSubRect().GetHeight();
    switch (m_type)
    {
    case HorizontalLeft:
        if (width < BLOCK_SIZE && lastFrameCorrected != GetCurrentFrame())
        {
            Move(BLOCK_SIZE - width, 0);
            lastFrameCorrected = GetCurrentFrame();
        }
        break;
    case HorizontalRight:
        if (width < BLOCK_SIZE && lastFrameCorrected != GetCurrentFrame())
        {
            Move(-(BLOCK_SIZE - width), 0);
            lastFrameCorrected = GetCurrentFrame();
}
        break;
    case VerticalUp:
        if (height < BLOCK_SIZE && lastFrameCorrected != GetCurrentFrame())
        {
            Move(0, BLOCK_SIZE - height);
            lastFrameCorrected = GetCurrentFrame();
        }
        break;
    case VerticalDown:
        if (height < BLOCK_SIZE && lastFrameCorrected != GetCurrentFrame())
        {
            Move(0, (-BLOCK_SIZE - height));
            lastFrameCorrected = GetCurrentFrame();
        }
        break;
    case Horizontal:
    case Vertical:
    case Middle:
        break;
    }
}
