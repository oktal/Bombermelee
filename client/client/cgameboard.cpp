#include "cgameboard.h"
#include "Animated.h"
#include "cplayer.h"
#include "cimagemanager.h"
#include <QtGui>
#include <QtMultimedia>

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

static void send(QTcpSocket *to, const std::string &what)
{
    const qint64 len = what.length() + 2;
    qDebug() << what.c_str();
    while (to->write(_m(what)) != len)
    {

    }
}

CGameBoard::CGameBoard(QWidget *parent, const QPoint &position, const QSize &size, unsigned int FrameTime) :
        QSFMLCanvas(parent, position, size, FrameTime)
{
    CPlayer *me = new CPlayer();
    me->Play();
    m_playersList.append(me);
    warmupTimer = new QTimer(this);
    m_gameBegin = true;
    m_warmupTime = 0;
    m_status = Waiting_Players;

}

CGameBoard::~CGameBoard()
{
    QList<CPlayer *>::const_iterator it = m_playersList.begin();
    while (it != m_playersList.end())
    {
        CPlayer *player = *it;
        ++it;
        delete player;
    }
}

/**
  Function called when SFML Canvas is shown
  We load sprites
*/
void CGameBoard::OnInit()
{
    CImageManager *imageManager = CImageManager::GetInstance();
    m_wall.SetImage(*imageManager->GetImage("../mur.png"));

    m_box.SetImage(*imageManager->GetImage("../box.png"));

    m_bomb.SetImage(*imageManager->GetImage("../bomb.png"));

    m_bonus.SetImage(*imageManager->GetImage("../bonus.png"));
}

/**
  Function called to refresh the view on canvas
*/
void CGameBoard::OnUpdate()
{
    Clear(sf::Color(195, 195, 195)); /* Grey */
    sf::Shape rightBorder = sf::Shape::Rectangle(510, 0, 630, 510, sf::Color(127, 127, 127));
    Draw(rightBorder);
    drawMap();
    drawFPS();
    drawStatus();

    /* Handling Event */
    CPlayer *me = m_playersList[0]; /* I am the first player of the list */
    if (GetInput().IsKeyDown(sf::Key::Right))
    {
        if (me->canMove(Right, m_map))
        {
            if (me->getDirection() != Right)
            {
                send(m_socket , "MOVE " + me->getNick() + " RIGHT");
                me->setDirection(Right);
            }
         }
        else
        {
            if (me->getDirection() != Stopped)
            {
                me->setDirection(Stopped);
                send(m_socket, "MOVE " + me->getNick() + " STOP");
            }
        }
        if (me->IsPaused())
        {
            me->Play();
        }
    }
     else if (GetInput().IsKeyDown(sf::Key::Left))
     {
         if (me->canMove(Left, m_map))
         {
              if (me->getDirection() != Left)
              {
                  send(m_socket, "MOVE " + me->getNick() + " LEFT");
                  me->setDirection(Left);
              }
          }
         else
         {
             if (me->getDirection() != Stopped)
             {
                 me->setDirection(Stopped);
                 send(m_socket, "MOVE " + me->getNick() + " STOP");
             }
         }
         if (me->IsPaused())
         {
             me->Play();
         }
     }
     else if (GetInput().IsKeyDown(sf::Key::Down))
     {
         if (me->canMove(Down, m_map))
         {
              if (me->getDirection() != Down)
              {
                   send(m_socket, "MOVE " + me->getNick() + " DOWN");
                   me->setDirection(Down);
              }
          }
         else
         {
             if (me->getDirection() != Stopped)
             {
                 me->setDirection(Stopped);
                 send(m_socket, "MOVE " + me->getNick() + " STOP");
             }
         }
         if (me->IsPaused())
         {
             me->Play();
         }
     }
     else if (GetInput().IsKeyDown(sf::Key::Up))
     {
         if (me->canMove(Up, m_map))
         {
               if (me->getDirection() != Up)
               {
                    send(m_socket, "MOVE " + me->getNick() + " UP");
                    me->setDirection(Up);
               }
         }
         else
         {
             if (me->getDirection() != Stopped)
             {
                 me->setDirection(Stopped);
                 send(m_socket, "MOVE " + me->getNick() + " STOP");
             }
         }
         if (me->IsPaused())
         {
             me->Play();
         }

     }
     else if (GetInput().IsKeyDown(sf::Key::Space))
     {
         plantBomb();
     }
     else
     {
         if (me->getDirection() != Stopped)
         {
             me->setDirection(Stopped);
             send(m_socket, "MOVE " + me->getNick() + " STOP");
         }
         me->Pause();
     }
    drawPlayers();
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
                m_box.SetPosition(34*i, 34*j);
                Draw(m_box);
                break;
            case Bonus:
                m_bonus.SetPosition(34*i, 34*j);
                Draw(m_bonus);
                break;
            case Floor:
                break;
            case Player:
                break;
            case Bomb:
                m_bomb.SetPosition((34 * i) + 6, (34 * j) + 6);
                Draw(m_bomb);
                break;
            default:
                break;
            }
        }
    }
}

void CGameBoard::drawPlayers()
{
    for (int i = 0; i < m_playersList.size(); ++i)
    {
        CPlayer *player = m_playersList[i];
        if (player->getDirection() != Stopped)
        {
            player->move(player->getDirection(), GetFrameTime());
        }
        player->anim(GetFrameTime());
        Draw(*player);
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
    else if (m_status == Waiting_Players)
    {
        sf::String status("Waiting for players");
        status.SetSize(20.f);
        status.SetStyle(sf::String::Bold);
        status.SetPosition(150, 5);
        Draw(status);
    }
    else if (m_status == Warmup)
    {
        QString s;
        sf::String status;
        if (m_warmupTime == WarmupTime)
        {
            s = "GO !";
            status.SetPosition(240, 5);
        }
        else
        {
            s = QString("Game will begin in %1 second(s)").arg(WarmupTime - m_warmupTime);
            status.SetPosition(100, 5);
        }
        status.SetText(s.toStdString());
        status.SetSize(20.f);
        status.SetStyle(sf::String::Bold);
        Draw(status);
    }
}

/**
  Plant a bomb
*/
void CGameBoard::plantBomb()
{
    CPlayer *me = m_playersList[0];
    int pos_x = 0, pos_y = 0;
    int x = 0, y = 0;

    if (me->pausedBombs == me->maxBombs) /* We reached the limit */
    {
        return;
    }
    pos_x = me->GetPosition().x + (me->GetSubRect().GetWidth() / 2);
    pos_y = me->GetPosition().y + (me->GetSubRect().GetHeight() / 2);
    x = pos_x / 34;
    y = pos_y / 34;
    m_map.setBlock(x, y, Bomb);

    QString pos = QString("%1 %2").arg(x).arg(y);
    m_socket->write(_m("BOMB " + me->getNick() + " " + pos.toStdString()));
    me->pausedBombs++;
    QSound::play("../plant.wav");
}

void CGameBoard::plantedBomb(const std::string &bomber, unsigned x, unsigned y)
{
    m_map.setBlock(x, y, Bomb);
    QSound::play("../plant.wav");
}

void CGameBoard::bombExplode(const std::string &bomber, unsigned x, unsigned y)
{
    if (bomber == m_playersList[0]->getNick()) /* If I am the bomber */
    {
        m_playersList[0]->pausedBombs--;
    }
    m_map.setBlock(x, y, Floor);
    QSound::play("../explosion.wav");
}


void CGameBoard::setPlayerColor(std::string color)
{
    m_playersList[0]->setColor(color);
    m_playersList[0]->setCorrectPosition();
}

void CGameBoard::setSocket(QTcpSocket *socket)
{
    m_socket = socket;
}

void CGameBoard::setNick(const std::string &nick){
    m_playersList[0]->setNick(nick);
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
    for (int i = 0; i < m_playersList.size(); ++i)
    {
        m_playersList[i]->setCorrectPosition();
    }
}

void CGameBoard::checkWarmupTime()
{
    m_warmupTime++;

    if (m_warmupTime == WarmupTime + 1)
    {
        m_gameBegin = true;
        m_status = None;
        m_warmupTime = 0;
        warmupTimer->stop();
    }
}

void CGameBoard::setConnected(bool connected)
{
    m_connected = connected;
}

/**
  A new player joins the game
*/
void CGameBoard::newPlayer(const std::string &nick, const std::string &color)
{
    /* Create and add the new player on the other players list */
    CPlayer *player = new CPlayer(nick, color);
    player->setCorrectPosition();
    m_playersList.append(player);
    m_status = None;
}

/**
  A player leaves the game
*/
void CGameBoard::playerLeft(const std::string &nick)
{
    /* We remove the player from the list */
    for (int i = 0; i < m_playersList.size(); ++i)
    {
        if (m_playersList[i]->getNick() == nick)
        {
            m_playersList.removeAt(i);
            break;
        }
    }
}

void CGameBoard::playerMove(const std::string &nick, const std::string &move)
{
    CPlayer *player = getPlayerFromNick(nick);
    if (move == "LEFT")
    {
        player->setDirection(Left);
        player->Play();
    }
    else if (move == "RIGHT")
    {
        player->setDirection(Right);
        player->Play();
    }
    else if (move == "UP")
    {
        player->setDirection(Up);
        player->Play();
    }
    else if (move == "DOWN")
    {
        player->setDirection(Down);
        player->Play();
    }
    else if (move == "STOP")
    {
        player->setDirection(Stopped);
        player->Stop();
    }
}

CPlayer *CGameBoard::getPlayerFromNick(const std::string &nick)
{
    CPlayer *player = NULL;
    for (int i = 0; i < m_playersList.size(); ++i)
    {
        if (m_playersList[i]->getNick() == nick)
        {
            player = m_playersList[i];
            break;
        }
    }
    return player;
}
