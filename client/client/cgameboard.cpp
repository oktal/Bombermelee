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

static const int SPEED = 300;

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
        if (canMove())
        {
            m_player.move(m_player.Right, ElapsedTime);
        }

    }
    else if (GetInput().IsKeyDown(sf::Key::Left))
    {
        if (canMove())
        {
            m_player.move(m_player.Left, ElapsedTime);
        }
    }
    else if (GetInput().IsKeyDown(sf::Key::Down))
    {
        if (canMove())
        {
            m_player.move(m_player.Down, ElapsedTime);
        }
    }
    else if (GetInput().IsKeyDown(sf::Key::Up))
    {
        if (canMove())
        {
            m_player.move(m_player.Up, ElapsedTime);
        }
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

bool CGameBoard::canMove()
{
    const sf::Vector2f &currentPosition = m_player.GetPosition();
    unsigned x = (static_cast<int>(currentPosition.x) + 0) / 34;
    unsigned y = (static_cast<int>(currentPosition.y) + m_player.GetImage()->GetHeight()) / 34;
    QString p = QString("%1 %2").arg(x).arg(y);
    sf::String s(p.toStdString());
    s.SetSize(20.f);
    s.SetPosition(520, 10);
    Draw(s);
    return true;
}
