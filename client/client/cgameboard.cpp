#include "cgameboard.h"
#include "Animated.h"
#include "cplayer.h"
#include <QtGui>

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


CGameBoard::CGameBoard(QWidget *parent, const QPoint &position, const QSize &size) :
        QSFMLCanvas(parent, position, size)
{
}

void CGameBoard::OnInit()
{
    if (!wall.LoadFromFile("../mur.png"))
    {
        QMessageBox::warning(this, "Warning", tr("Can not load sprite wall.png"));
        return;
    }
    wall.CreateMaskFromColor(sf::Color(255, 255, 255));
    m_wall.SetImage(wall);
}

void CGameBoard::OnUpdate()
{
    Clear(sf::Color(195, 195, 195));
    drawWalls();
    float ElapsedTime = GetFrameTime();
    if (GetInput().IsKeyDown(sf::Key::Right))
    {
        m_player.setDirection(Right);
        if (canMove(Right, ElapsedTime))
        {
            m_player.move(Right, ElapsedTime);
        }
        m_player.anim(ElapsedTime);

    }
    else if (GetInput().IsKeyDown(sf::Key::Left))
    {
        m_player.setDirection(Left);
        if (canMove(Left, ElapsedTime))
        {
            m_player.move(Left, ElapsedTime);
        }
        m_player.anim(ElapsedTime);
    }
    else if (GetInput().IsKeyDown(sf::Key::Down))
    {
        m_player.setDirection(Down);
        if (canMove(Down, ElapsedTime))
        {
            m_player.move(Down, ElapsedTime);
        }
        m_player.anim(ElapsedTime);
    }
    else if (GetInput().IsKeyDown(sf::Key::Up))
    {
        m_player.setDirection(Up);
        if (canMove(Up, ElapsedTime))
        {
            m_player.move(Up, ElapsedTime);
        }
        m_player.anim(ElapsedTime);
    }
    else
    {
        /*if (!m_player.IsStoped())
        {
            m_player.Stop();
        }
        */
    }
    Draw(m_player);
}

void CGameBoard::drawWalls()
{
   for (unsigned i = 1; i < 15; i += 2)
   {
       for (unsigned j = 1; j < 15; j += 2)
       {
           m_wall.SetPosition(34 * i, 34 * j);
           Draw(m_wall);
       }
   }
}

bool CGameBoard::canMove(Direction movement, const float &ElapsedTime)
{
    int pos_x1, pos_y1, pos_x2, pos_y2;
    switch (movement)
    {
    case Left:
        pos_x1 = m_player.GetPosition().x;
        pos_y1 = m_player.GetPosition().y;
        pos_x2 = pos_x1;
        pos_y2 = pos_y1 + m_player.GetSubRect().GetHeight();
        pos_x1 -= (Speed * ElapsedTime);
        pos_x2 -= (Speed * ElapsedTime);
        break;
    case Right:
        pos_x1 = m_player.GetPosition().x + m_player.GetSubRect().GetWidth();
        pos_y1 = m_player.GetPosition().y;
        pos_x2 = pos_x1;
        pos_y2 = pos_y1 + m_player.GetSubRect().GetHeight();
        pos_x1 += (Speed * ElapsedTime);
        pos_x2 += (Speed * ElapsedTime);
        break;
    case Up:
        pos_x1 = m_player.GetPosition().x;
        pos_y1 = m_player.GetPosition().y;
        pos_x2 = pos_x1 + m_player.GetSubRect().GetWidth();
        pos_y2 = pos_y1;
        pos_y1 -= (Speed * ElapsedTime);
        pos_y2 -= (Speed * ElapsedTime);
        break;
    case Down:
        pos_x1 = m_player.GetPosition().x;
        pos_y1 = m_player.GetPosition().y + m_player.GetSubRect().GetHeight();
        pos_x2 = pos_x1 + m_player.GetSubRect().GetWidth();
        pos_y2 = pos_y1;
        pos_y1 += (Speed * ElapsedTime);
        pos_y2 += (Speed * ElapsedTime);
        break;
    }
    if ((movement == Left && pos_x1 < 0) ||
        (movement == Right && pos_x1 > 510) ||
        (movement == Up && pos_y1 < 0) ||
        (movement == Down && pos_y1 > 510))
    {
        return false;
    }
    unsigned x1 = pos_x1 / 34;
    unsigned y1 = pos_y1 / 34;
    unsigned x2 = pos_x2 / 34;
    unsigned y2 = pos_y2 / 34;
    QString s = QString("%1 %2").arg(x1).arg(y1);
    sf::String str(s.toStdString());
    str.SetPosition(520, 10);
    str.SetSize(20.f);
    Draw(str);
    if (m_map.getBlock(x1, y1) == Wall ||
        m_map.getBlock(x2, y2) == Wall)
    {
        return false;
    }
    else
    {
        return true;
    }
}
