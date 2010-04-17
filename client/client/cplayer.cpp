#include <QtGui>
#include "cplayer.h"

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

CPlayer::CPlayer(const std::string &nick, const std::string &color) :
        m_nick(nick)
{
    setColor(color);

    m_player_up.PushFrame(Frame(&m_img_player, sf::Rect<int>(2, 32, 18, 55)));
    m_player_up.PushFrame(Frame(&m_img_player, sf::Rect<int>(23, 32, 39, 55)));
    m_player_up.PushFrame(Frame(&m_img_player, sf::Rect<int>(44, 32, 60, 55)));

    m_player_down.PushFrame(Frame(&m_img_player, sf::Rect<int>(2, 2, 18, 25)));
    m_player_down.PushFrame(Frame(&m_img_player, sf::Rect<int>(23, 2, 39, 25)));
    m_player_down.PushFrame(Frame(&m_img_player, sf::Rect<int>(44, 2, 60, 25)));

    m_player_left.PushFrame(Frame(&m_img_player, sf::Rect<int>(65, 2, 84, 25)));
    m_player_left.PushFrame(Frame(&m_img_player, sf::Rect<int>(89, 2, 105, 25)));
    m_player_left.PushFrame(Frame(&m_img_player, sf::Rect<int>(110, 2, 126, 25)));

    m_player_right.PushFrame(Frame(&m_img_player, sf::Rect<int>(66, 32, 82, 55)));
    m_player_right.PushFrame(Frame(&m_img_player, sf::Rect<int>(89, 32, 105, 55)));
    m_player_right.PushFrame(Frame(&m_img_player, sf::Rect<int>(110, 32, 126, 55)));

    m_player_explode.PushFrame(Frame(&m_img_player, sf::Rect<int>(155, 2, 176, 25)));
    m_player_explode.PushFrame(Frame(&m_img_player, sf::Rect<int>(182, 2, 204, 25)));
    m_player_explode.PushFrame(Frame(&m_img_player, sf::Rect<int>(209, 2, 232, 25)));

    SetAnim(&m_player_right);
    SetFrameTime(0.2f);
}

void CPlayer::setDirection(Direction move)
{
    switch (move)
    {
    case Right:
        if (GetAnim() != &m_player_right)
        {
            SetAnim(&m_player_right);
        }
        break;
    case Left:
        if (GetAnim() != &m_player_left)
        {
            SetAnim(&m_player_left);
        }
        break;
    case Up:
        if (GetAnim() != &m_player_up)
        {
            SetAnim(&m_player_up);
        }
        break;
    case Down:
        if (GetAnim() != &m_player_down)
        {
            SetAnim(&m_player_down);
        }
        break;
    }
}

void CPlayer::setColor(const std::string &color)
{
    m_color = color;
    std::string fileName = "../character-sheet-" + m_color + ".png";
    if (!m_img_player.LoadFromFile(fileName))
    {
        return;
    }
    m_img_player.CreateMaskFromColor(sf::Color(255, 255, 255));
}

void CPlayer::setNick(const std::string &nick)
{
    m_nick = nick;
}

const std::string &CPlayer::getNick()
{
    return m_nick;
}

void CPlayer::move(Direction move, const float &ElapsedTime)
{
    setDirection(move);
    switch (move)
    {
    case Right:
            Move(Speed * ElapsedTime, 0);
            break;
    case Left:
            Move(-Speed * ElapsedTime, 0);
            break;
    case Up:
            Move(0, -Speed * ElapsedTime);
            break;
    case Down:
            Move(0, Speed * ElapsedTime);
            break;
    }
    if (IsPaused())
    {
        Play();
    }
}

void CPlayer::explode()
{
    if (GetAnim() != &m_player_explode)
    {
        SetAnim(&m_player_explode);
    }
}

void CPlayer::setCorrectPosition()
{
    if (m_color == "white")
    {
        SetAnim(&m_player_right);
        SetPosition(0, 0);
    }
    else if (m_color == "red")
    {
        SetAnim(&m_player_left);
        SetPosition(510 - GetSubRect().GetWidth(), 0);
    }
    else if (m_color == "green")
    {
        SetAnim(&m_player_right);
        SetPosition(0, 510 - GetSubRect().GetHeight());
    }
    else if (m_color == "blue")
    {
        SetAnim(&m_player_left);
        SetPosition(510 - GetSubRect().GetWidth(), 510 - GetSubRect().GetHeight());
    }
}
