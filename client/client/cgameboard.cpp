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

/**
  * function _m transforms a message into a valid message request (ending by \r\n)
  * @return message request
*/
static inline char const *_m(const std::string &message)
{
    return std::string(message + "\r\n").c_str();
}

CGameBoard::CGameBoard(QWidget *parent, const QPoint &position, const QSize &size, unsigned int FrameTime) :
        QSFMLCanvas(parent, position, size, FrameTime)
{
    m_player.Play();
    warmupTimer = new QTimer(this);
    m_gameBegin = true;
    m_warmupTime = 0;
    m_status = None;
}

/**
  Function called when SFML Canvas is shown
  We load sprites
*/
void CGameBoard::OnInit()
{
    if (!wall.LoadFromFile("../mur.png"))
    {
        QMessageBox::warning(this, "Warning", tr("Can not load sprite wall.png"));
        return;
    }
    wall.CreateMaskFromColor(sf::Color(255, 255, 255));
    m_wall.SetImage(wall);

    if (!box.LoadFromFile("../box.png"))
    {
        QMessageBox::warning(this, "Warning", tr("Can not load sprite box.png"));
        return;
    }
    box.CreateMaskFromColor(sf::Color(255, 255, 255));
    m_box.SetImage(box);
}

/**
  Function called to refresh the view on canvas
*/
void CGameBoard::OnUpdate()
{
    Clear(sf::Color(195, 195, 195)); /* Grey */
    sf::Shape rightBorder = sf::Shape::Rectangle(510, 0, 630, 510, sf::Color(127, 127, 127));
    float ElapsedTime = GetFrameTime();
    Draw(rightBorder);
    drawMap();
    drawOtherPlayers();
    drawFPS();
    drawStatus();

    /* RIGHT */
    if (GetInput().IsKeyDown(sf::Key::Right))
    {
        m_player.setDirection(Right);
        if (canMove(Right, ElapsedTime))
        {
            m_player.move(Right, ElapsedTime);
            QString pos = QString("%1 %2").arg(m_player.GetPosition().x).arg(m_player.GetPosition().y);
            m_socket->write(_m("MOVE " + m_player.getNick() + " RIGHT " + pos.toStdString()));
        }
        m_player.anim(ElapsedTime);

    }

    /* LEFT */
    else if (GetInput().IsKeyDown(sf::Key::Left))
    {
        m_player.setDirection(Left);
        if (canMove(Left, ElapsedTime))
        {
            m_player.move(Left, ElapsedTime);
            QString pos = QString("%1 %2").arg(m_player.GetPosition().x).arg(m_player.GetPosition().y);
            m_socket->write(_m("MOVE " + m_player.getNick() + " LEFT " + pos.toStdString()));
        }
        m_player.anim(ElapsedTime);
    }

    /* DOWN */
    else if (GetInput().IsKeyDown(sf::Key::Down))
    {
        m_player.setDirection(Down);
        if (canMove(Down, ElapsedTime))
        {
            m_player.move(Down, ElapsedTime);
            QString pos = QString("%1 %2").arg(m_player.GetPosition().x).arg(m_player.GetPosition().y);
            m_socket->write(_m("MOVE " + m_player.getNick() + " DOWN " + pos.toStdString()));
        }
        m_player.anim(ElapsedTime);
    }

    /* UP */
    else if (GetInput().IsKeyDown(sf::Key::Up))
    {
        m_player.setDirection(Up);
        if (canMove(Up, ElapsedTime))
        {
            m_player.move(Up, ElapsedTime);
            QString pos = QString("%1 %2").arg(m_player.GetPosition().x).arg(m_player.GetPosition().y);
            m_socket->write(_m("MOVE " + m_player.getNick() + " UP " + pos.toStdString()));
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

/**
  Draw all the blocks of the map
*/
void CGameBoard::drawMap()
{
    for(unsigned i = 0; i < 15; i++)
    {
        for(unsigned j = 0; j < 15; j++)
        {
            switch(m_map.getBlock(i,j))
            {
            case Wall:
                m_wall.SetPosition(34*i, 34*j);
                Draw(m_wall);
                break;
            case Box:
            case Bonus:
                m_box.SetPosition(34*i, 34*j);
                Draw(m_box);
                break;
            case Floor:
                break;
            case Player:
                break;
            case Bomb:
                break;
            default:
                break;
            }
        }
    }
}

void CGameBoard::drawOtherPlayers()
{
    for (int i = 0; i < m_otherPlayers.size(); ++i)
    {
        Draw(*m_otherPlayers[i]);
    }
}

void CGameBoard::drawFPS()
{
    float ElapsedTime = GetFrameTime();
    unsigned number_fps = static_cast<int>(ElapsedTime * 1000);
    sf::String FPS(QString("%1 FPS").arg(number_fps).toStdString());
    FPS.SetPosition(580, 0);
    FPS.SetSize(15);
    Draw(FPS);
}

/**
  Draw the current status of the game (text appearing on the top of the gameboard)
*/
void CGameBoard::drawStatus()
{
    if (m_status == None)
    {
        return;
    }
    else if (m_status == Warmup)
    {
        QString s;
        sf::String status;
        if (m_warmupTime == WarmupTime)
        {
            s = "GO !";
            status.SetPosition(240, 10);
        }
        else
        {
            s = QString("Game will begin in %1 second(s)").arg(WarmupTime - m_warmupTime);
            status.SetPosition(100, 10);
        }
        status.SetText(s.toStdString());
        status.SetSize(20.f);
        status.SetStyle(sf::String::Bold);
        Draw(status);
    }
}

/**
  Check if the player can move
  @param movement Direction to go
  @param ElapsedTime FrameTime
  @return true if the player can move, false if not
*/
bool CGameBoard::canMove(Direction movement, const float &ElapsedTime)
{
    int pos_x1, pos_y1, pos_x2, pos_y2;
    pos_x1 = pos_y1 = pos_x2 = pos_y2 = 0;
    if (m_gameBegin == false)
    {
        return false;
    }
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
    /*
      DEBUG
    */
    unsigned x1 = pos_x1 / 34;
    unsigned y1 = pos_y1 / 34;
    unsigned x2 = pos_x2 / 34;
    unsigned y2 = pos_y2 / 34;
    QString s = QString("%1 %2").arg(x1).arg(y1);
    sf::String str(s.toStdString());
    str.SetPosition(520, 10);
    str.SetSize(20.f);
    Draw(str);
    /*
      END DEBUG
    */
    /* If we want to move on a wall or a box, just return false */
    if (m_map.getBlock(x1, y1) == Wall ||
        m_map.getBlock(x2, y2) == Wall ||
        m_map.getBlock(x1, y1) == Box ||
        m_map.getBlock(x2, y2) == Box ||
        m_map.getBlock(x1, y1) == Bonus ||
        m_map.getBlock(x2, y2) == Bonus )
    {
        return false;
    }
    /* We can move */
    else
    {
        return true;
    }
}

void CGameBoard::setPlayerColor(std::string color)
{
    m_player.setColor(color);
    m_player.setCorrectPosition();
}

void CGameBoard::setSocket(QTcpSocket *socket)
{
    m_socket = socket;
}

void CGameBoard::setNick(const std::string &nick){
    m_player.setNick(nick);
}

/**
  Call the setMap method to construct the map from a string
*/
void CGameBoard::setMap(std::string map)
{
    /* Map is received, we can begin the game */
    m_gameBegin = false;
    warmupTimer->setInterval(1000);
    warmupTimer->start();
    QObject::connect(warmupTimer, SIGNAL(timeout()), this, SLOT(checkWarmupTime()));

    m_status = Warmup;
    m_warmupTime = 0;

    m_map.setMap(map);
    m_player.setCorrectPosition();
    for (int i = 0; i < m_otherPlayers.size(); ++i)
    {
        m_otherPlayers[i]->setCorrectPosition();
    }
}

void CGameBoard::checkWarmupTime()
{
    m_warmupTime++;

    if (m_warmupTime == WarmupTime + 1)
    {
        m_gameBegin = true;
        m_status = None;
        warmupTimer->stop();
    }
}

/**
  A new player joins the game
*/
void CGameBoard::newPlayer(const std::string &nick, const std::string &color)
{
    /* Create and add the new player on the other players list */
    CPlayer *player = new CPlayer(nick, color);
    player->setCorrectPosition();
    player->Play();
    m_otherPlayers.append(player);
}

/**
  A player leaves the game
*/
void CGameBoard::playerLeft(const std::string &nick)
{
    /* We remove the player from the list */
    for (int i = 0; i < m_otherPlayers.size(); ++i)
    {
        if (m_otherPlayers[i]->getNick() == nick)
        {
            m_otherPlayers.removeAt(i);
            break;
        }
    }
}

void CGameBoard::playerMove(const std::string &nick, const std::string &move, const float x, const float y)
{
    CPlayer *player = getPlayerFromNick(nick);
    if (move == "LEFT")
    {
        player->setDirection(Left);
    }
    else if (move == "RIGHT")
    {
        player->setDirection(Right);
    }
    else if (move == "UP")
    {
        player->setDirection(Up);
    }
    else if (move == "DOWN")
    {
        player->setDirection(Down);
    }
    player->SetPosition(x, y);
    player->anim(GetFrameTime());
}

CPlayer *CGameBoard::getPlayerFromNick(const std::string &nick)
{
    CPlayer *player = NULL;
    for (int i = 0; i < m_otherPlayers.size(); ++i)
    {
        if (m_otherPlayers[i]->getNick() == nick)
        {
            player = m_otherPlayers[i];
            break;
        }
    }
    return player;
}
