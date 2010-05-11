#include <QtGui>
#include "cplayer.h"
#include "cimagemanager.h"
#include "cbonus.h"
#include "climitedbonus.h"
#include <list>
#include <iostream>
#include <typeinfo>

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

static inline bool collision(const sf::Sprite& s1,const sf::Sprite& s2, const int AlphaMax = -1)
{
    sf::Vector2f pos = s1.GetPosition() - s1.GetCenter();
    const sf::FloatRect r1(pos.x, pos.y, pos.x + s1.GetSize().x, pos.y + s1.GetSize().y);
    pos = s2.GetPosition() - s2.GetCenter();
    const sf::FloatRect r2(pos.x, pos.y, pos.x + s2.GetSize().x, pos.y + s2.GetSize().y);

    // Espace de collision potentiel.
    sf::FloatRect zone;

    // Testons si les Sprites se superposent.
    if (r1.Intersects(r2, &zone))
    {
        if (AlphaMax >= 0)
        {
            int left1 = static_cast<int>(zone.Left - r1.Left);
            int top1 = static_cast<int>(zone.Top - r1.Top);
            int left2 = static_cast<int>(zone.Left - r2.Left);
            int top2 = static_cast<int>(zone.Top - r2.Top);
            int width = static_cast<int>(zone.GetWidth());
            int height = static_cast<int>(zone.GetHeight());

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    if (s1.GetPixel(x+left1, y+top1).a > AlphaMax && s2.GetPixel(x+left2, y+top2).a > AlphaMax)
                        return true;
                }
            }
            return false;
        }
        //qDebug() << "Collision";
        return true;
    }
    return false;
}

CPlayer::CPlayer(const std::string &nick, const std::string &color) :
        Animated(true, true, 0.2f),
        m_nick(nick)
{
    setColor(color);
    pausedBombs = 0;
    maxBombs = 1; /* 1 bomb maximum at the begining */
    gotBonus = false; /* No bonus */

    /* UP ANIMATION */
    m_player_up.PushFrame(Frame(&m_img_player, sf::Rect<int>(2, 32, 18, 55)));
    m_player_up.PushFrame(Frame(&m_img_player, sf::Rect<int>(23, 32, 39, 55)));
    m_player_up.PushFrame(Frame(&m_img_player, sf::Rect<int>(44, 32, 60, 55)));

    /* DOWN ANIMATION */
    m_player_down.PushFrame(Frame(&m_img_player, sf::Rect<int>(2, 2, 18, 25)));
    m_player_down.PushFrame(Frame(&m_img_player, sf::Rect<int>(23, 2, 39, 25)));
    m_player_down.PushFrame(Frame(&m_img_player, sf::Rect<int>(44, 2, 60, 25)));

    /* LEFT ANIMATION */
    m_player_left.PushFrame(Frame(&m_img_player, sf::Rect<int>(65, 2, 84, 25)));
    m_player_left.PushFrame(Frame(&m_img_player, sf::Rect<int>(89, 2, 105, 25)));
    m_player_left.PushFrame(Frame(&m_img_player, sf::Rect<int>(110, 2, 126, 25)));

    /* RIGHT ANIMATION */
    m_player_right.PushFrame(Frame(&m_img_player, sf::Rect<int>(66, 32, 82, 55)));
    m_player_right.PushFrame(Frame(&m_img_player, sf::Rect<int>(89, 32, 105, 55)));
    m_player_right.PushFrame(Frame(&m_img_player, sf::Rect<int>(110, 32, 126, 55)));

    /* EXPLODE ANIMATION */
    m_player_explode.PushFrame(Frame(&m_img_player, sf::Rect<int>(155, 2, 176, 25)));
    m_player_explode.PushFrame(Frame(&m_img_player, sf::Rect<int>(182, 2, 204, 25)));
    m_player_explode.PushFrame(Frame(&m_img_player, sf::Rect<int>(209, 2, 232, 25)));

    SetAnim(&m_player_right);
    m_direction = Stopped;
    m_elapsedTime = 0.0;
    m_stopTime = 0.0;
    m_speed = Speed;
}

unsigned CPlayer::getX() const
{
    return (GetPosition().x + GetSubRect().GetWidth() / 2) / BLOCK_SIZE;
}

unsigned CPlayer::getY() const
{
    return (GetPosition().y + GetSubRect().GetHeight() / 2) / BLOCK_SIZE;
}

/**
  * Set the direction and set the animation
  * @param direction Left, Right, Up or Down
*/
void CPlayer::setDirection(Direction direction)
{
    m_direction = direction;
    switch (direction)
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
    case Stopped:
        /* Reset the elapsedTime */
        m_elapsedTime = 0.0f;
        break;
    }
}

/**
  * Get the current direction
  * @return the current direction
*/
Direction CPlayer::getDirection() const
{
    return m_direction;
}

/**
  * Get the elapsedTime since last movement
  * @return the elapsed time
*/

float CPlayer::getElapsedTime() const
{
    return m_elapsedTime;
}

float CPlayer::getStopTime() const
{
    return m_stopTime;
}

unsigned CPlayer::getSpeed() const
{
    return m_speed;
}
/**
  * Set the stop time
*/

void CPlayer::setStopTime(const float &stopTime)
{
    m_stopTime = stopTime;
}

/**
  Set the color by loading the right sheet
*/
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

/**
  * Set the nick
*/
void CPlayer::setNick(const std::string &nick)
{
    m_nick = nick;
}

/**
  * @return the current nick
*/
const std::string &CPlayer::getNick() const
{
    return m_nick;
}


/**
  * Tell if the player can move or not
  * @param direction Left, Right, Up or Down
  * @map map current map
  * @return true if the player can move, false if not
*/

bool CPlayer::canMove(Direction direction, CMap &map)
{
    int _x = GetPosition().x, _y = GetPosition().y;
    /* In which block are we ? */
    unsigned x = (_x + GetSubRect().GetWidth() / 2) / BLOCK_SIZE;
    unsigned y = (_y + GetSubRect().GetHeight() / 2) / BLOCK_SIZE;
    sf::Sprite tmp;
    CImageManager *imageManager = CImageManager::GetInstance();
    switch (direction)
    {
    case Up:
        if (_y <= 0) /* Out of the map ? */
        {
            return false;
        }
        if (map.getBlock(x, y - 1) == Wall ||
            map.getBlock(x, y - 1) == Box)
        {
            tmp.SetImage(*imageManager->GetImage("../mur.png"));
            tmp.SetPosition(x * BLOCK_SIZE, (y - 1) * BLOCK_SIZE);
        }
        else if (map.getBlock(x, y - 1) == Bomb)
        {
            tmp.SetImage(*imageManager->GetImage("../bomb.png"));
            tmp.SetPosition((x * BLOCK_SIZE) + 6, ((y - 1) * BLOCK_SIZE) + 6);
        }
        else if (map.getBlock(x, y) == Bonus)
        {
            tmp.SetImage(*imageManager->GetImage("../bonus.png"));
            tmp.SetPosition((x * BLOCK_SIZE) + 5, ((y) * BLOCK_SIZE) + 5);
            gotBonus = collision(*this, tmp, 0);
            return true;
        }

        else if (map.getBlock(x, y - 1) == Floor)
        {
            return true;
        }
        break;            
    case Down:
        if (static_cast<unsigned>(_y + GetSubRect().GetHeight()) >=
            BLOCK_SIZE * MAP_HEIGHT) /* Out of the map ? */
        {
            return false;
        }
        if (map.getBlock(x, y + 1) == Wall ||
            map.getBlock(x, y + 1) == Box)
        {
            tmp.SetImage(*imageManager->GetImage("../mur.png"));
            tmp.SetPosition(x * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
        }
        else if (map.getBlock(x, y + 1) == Bomb)
        {
            tmp.SetImage(*imageManager->GetImage("../bomb.png"));
            tmp.SetPosition((x * BLOCK_SIZE) + 6, ((y + 1) * BLOCK_SIZE) + 6);
        }
        else if (map.getBlock(x, y) == Bonus)
        {
            tmp.SetImage(*imageManager->GetImage("../bonus.png"));
            tmp.SetPosition((x * BLOCK_SIZE) + 5, ((y) * BLOCK_SIZE) + 5);
            gotBonus = collision(*this, tmp, 0);
            return true;
        }
        else if (map.getBlock(x, y + 1) == Floor)
        {
            return true;
        }
        break;
    case Left:
        if (_x <= 0) /* Out of the map ? */
        {
            return false;
        }
        if (map.getBlock(x - 1, y) == Wall ||
            map.getBlock(x - 1, y) == Box)
        {
            tmp.SetImage(*imageManager->GetImage("../mur.png"));
            tmp.SetPosition((x - 1) * BLOCK_SIZE, y * BLOCK_SIZE);
        }
        else if (map.getBlock(x - 1, y) == Bomb)
        {
            tmp.SetImage(*imageManager->GetImage("../bomb.png"));
            tmp.SetPosition(((x - 1) * BLOCK_SIZE) + 6, (y * BLOCK_SIZE) + 6);
        }
        else if (map.getBlock(x, y) == Bonus)
        {
            tmp.SetImage(*imageManager->GetImage("../bonus.png"));
            tmp.SetPosition(((x) * BLOCK_SIZE) + 5, (y * BLOCK_SIZE) + 5);
            gotBonus = collision(*this, tmp, 0);
            return true;
        }
        else if (map.getBlock(x - 1, y) == Floor)
        {
            return true;
        }
        break;
    case Right:
        if (static_cast<unsigned>(_x + GetSubRect().GetWidth()) >=
            BLOCK_SIZE * MAP_WIDTH) /* Out of the map ? */
        {
            return false;
        }
        if (map.getBlock(x + 1, y) == Wall ||
            map.getBlock(x + 1, y) == Box)
        {
            tmp.SetImage(*imageManager->GetImage("../mur.png"));
            tmp.SetPosition((x + 1) * BLOCK_SIZE, y * BLOCK_SIZE);
        }
        else if (map.getBlock(x + 1, y) == Bomb)
        {
            tmp.SetImage(*imageManager->GetImage("../bomb.png"));
            tmp.SetPosition(((x + 1) * BLOCK_SIZE) + 6, (y * BLOCK_SIZE) + 6);
        }
        else if (map.getBlock(x, y) == Bonus)
        {
            tmp.SetImage(*imageManager->GetImage("../bonus.png"));
            tmp.SetPosition(((x) * BLOCK_SIZE) + 5, (y * BLOCK_SIZE) + 5);
            gotBonus = collision(*this, tmp, 0);
            return true;
        }
        else if (map.getBlock(x + 1, y) == Floor)
        {
            return true;
        }
        break;
    default:
        break;
    }
    return !collision(*this, tmp);
}


/**
  * Move the player
  * @param direction Left, Right Up or Down
  * @param ElapsedTime the frameTime
*/
void CPlayer::move(Direction direction, const float &ElapsedTime)
{
    setDirection(direction);
    switch (direction)
    {
    case Right:
            Move(m_speed * ElapsedTime, 0);
            m_elapsedTime += ElapsedTime;
            break;
    case Left:
            Move(m_speed * -ElapsedTime, 0);
            m_elapsedTime += ElapsedTime;
            break;
    case Up:
            Move(0, m_speed * -ElapsedTime);
            m_elapsedTime += ElapsedTime;
            break;
    case Down:
            Move(0, m_speed * ElapsedTime);
            m_elapsedTime += ElapsedTime;
            break;
    default:
            break;
    }
}

void CPlayer::explode()
{
    if (GetAnim() != &m_player_explode)
    {
        SetAnim(&m_player_explode);
    }
}

/**
  * Set the correct position according to the color
*/
void CPlayer::setCorrectPosition()
{
    if (m_color == "white")
    {
        /* Left hand corner */
        SetAnim(&m_player_right);
        SetPosition(0, 0);
    }
    else if (m_color == "red")
    {
        /* Right hand corner */
        SetAnim(&m_player_left);
        SetPosition(510 - GetSubRect().GetWidth(), 0);
    }
    else if (m_color == "green")
    {
        /* Bottom left hand corner */
        SetAnim(&m_player_right);
        SetPosition(0, 510 - GetSubRect().GetHeight());
    }
    else if (m_color == "blue")
    {
        /* Bottom right hand corner */
        SetAnim(&m_player_left);
        SetPosition(510 - GetSubRect().GetWidth(), 510 - GetSubRect().GetHeight());
    }
}

void CPlayer::newBonus(CBonus *bonus)
{
    m_bonusList.push_back(bonus);
    switch (bonus->getType())
    {
    case CBonus::SpeedUp:
        m_speed *= 2;
        m_oldStopTime = m_stopTime;
        m_stopTime = 0.0f;
        break;
    case CBonus::SpeedDown:
        m_speed /= 2;
        break;
    case CBonus::BombUp:
        if (maxBombs < MaxBombs)
        {
            maxBombs++;
        }
        break;
    default:
        break;
    }
}

bool CPlayer::alreadyHasBonus(CBonus::BonusType type)
{
    std::list<CBonus *>::iterator it;
    for (it = m_bonusList.begin(); it != m_bonusList.end(); ++it)
    {
        if ((*it)->getType() == type)
        {
            return true;
        }
    }
    return false;
}

void CPlayer::updateBonusTime(const float &elapsedTime)
{
    std::list<CBonus *>::iterator it = m_bonusList.begin();
    while (it != m_bonusList.end())
    {
        /* Let's check if the current bonus is a limited bonus */
        if (dynamic_cast<CLimitedBonus *>(*it))
        {
            CLimitedBonus *bonus = static_cast<CLimitedBonus *>(*it);
            /* update the bonus */
            bonus->updateTime(elapsedTime);
            if (bonus->isFinished())
            {
                switch (bonus->getType())
                {
                case CBonus::SpeedUp:
                    m_stopTime = m_oldStopTime;
                    m_speed = Speed;
                    break;
                case CBonus::SpeedDown:
                    m_speed = Speed;
                    break;
                default:
                    break;
                }
                it = m_bonusList.erase(it);
                delete bonus;
            }
            else
            {
                ++it;
            }
        }
        else
        {
            ++it;
        }
    }
}
