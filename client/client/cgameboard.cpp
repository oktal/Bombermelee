#include "cgameboard.h"
#include "Animated.h"
#include "cplayer.h"
#include "cimagemanager.h"
#include "cbonus.h"
#include "climitedbonus.h"
#include <QtGui>
#include <QtMultimedia>
#include <QVector>
#include <cassert>

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
    qint64 bytesWritten;
    do
    {
        bytesWritten = to->write(_m(what));
        to->waitForBytesWritten();
    } while (bytesWritten != len);
}

CGameBoard::CGameBoard(QWidget *parent, const QPoint &position, const QSize &size, unsigned int FrameTime) :
        QSFMLCanvas(parent, position, size, FrameTime)
{
    CPlayer *me = new CPlayer();
    me->setStopTime(0.15f);
    me->Pause();
    m_playersList.append(me);
    m_gameBegin = true;
    m_connected = true;
    m_warmupTime = 0;
    m_status = Waiting_Players;
    m_bonusCanvas = NULL;

    /* TEST */
    m_map.setBlock(2, 0, Bonus);
    m_map.setBlock(4, 0, Bonus);
    m_map.setBlock(5, 2, Bonus);
    m_map.setBlock(2, 14, Bonus);
    m_map.setBlock(4, 14, Bonus);
    m_map.setBlock(5, 12, Bonus);
    m_map.setBlock(12, 0, Bonus);
    m_map.setBlock(10, 0, Bonus);
    m_map.setBlock(9, 2, Bonus);
    SetFramerateLimit(60);
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
    if (m_bonusCanvas != NULL)
    {
        delete m_bonusCanvas;
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
    m_bonus.SetImage(*imageManager->GetImage("../bonus.png"));
    m_explosion = *imageManager->GetImage("../explosion.png");

    imageManager->GetImage("../bomb.png");
    imageManager->GetImage("../remote_bomb.png");
    imageManager->GetImage("../speed_up70.png");
    imageManager->GetImage("../speed_down70.png");
    imageManager->GetImage("../fire_up70.png");
    imageManager->GetImage("../fire_down70.png");
    imageManager->GetImage("../bomb_up70.png");
    imageManager->GetImage("../bomb_down70.png");
    imageManager->GetImage("../remote_mine70.png");
    imageManager->GetImage("../bomb_pass70.png");
    imageManager->GetImage("../bomb_kick70.png");
    imageManager->GetImage("../full_fire70.png");
    imageManager->GetImage("../remote_bomb.png");
    imageManager->GetImage("../explosion.png");
}

/**
  Function called to refresh the view on canvas
*/
void CGameBoard::OnUpdate()
{
    Clear(sf::Color(195, 195, 195)); /* Grey */
    sf::Shape rightBorder = sf::Shape::Rectangle(510, 0, 630, 510, sf::Color(127, 127, 127));
    Draw(rightBorder);

    m_frameRate = 1.f / GetFrameTime();
    drawMap();
    drawFPS();
    drawStatus();
    drawExplosions();
    drawPlayers();
    drawBonusCanvas();

    if (!m_gameBegin || !m_connected)
    {
        return;
    }

    /* Handling Event */
    CPlayer *me = m_playersList[0]; /* I am the first player of the list */
    std::string pos = QString("%1 %2").arg(me->GetPosition().x).arg(me->GetPosition().y)
                                      .toStdString();
    if (me->isDead())
    {
        return;
    }

    sf::Event event;
    while (GetEvent(event))
    {
        switch (event.Type)
        {
        case sf::Event::KeyPressed:
            if (event.Key.Code == sf::Key::Space)
            {
                plantBomb();
            }
            else if (event.Key.Code == sf::Key::E)
            {
                useSpecialBonus();
            }
            break;
        default:
            break;
        }
    }


    if (GetInput().IsKeyDown(sf::Key::Right) ||
        GetInput().IsKeyDown(sf::Key::D))
    {
        switch (me->getCollision(Right, m_map))
        {
        case Floor:
        case Bonus:
            if (me->getDirection() != Right)
            {
                send(m_socket , "MOVE " + me->getNick() + " RIGHT " + pos);
                me->setDirection(Right);
                qDebug() << "RIGHT";
            }
            break;
        case Bomb:
            if (me->getBonus(CBonus::BombPass) != NULL)
            {
                me->setDirection(Right);
            }
            else if (me->getBonus(CBonus::BombKick) != NULL)
            {
                QList<CBomb *>::iterator it;
                for (it = m_bombsList.begin(); it != m_bombsList.end(); ++it)
                {
                    CBomb *bomb = *it;
                    if (bomb->getX() == me->getX() + 1 && bomb->getY() == me->getY())
                    {
                        bomb->setDirection(CBomb::Right);
                        m_map.setBlock(bomb->getX(), bomb->getY(), Floor);
                        break;
                    }
                }
            }
            break;
        default:
            break;
        }

        if (me->IsPaused())
        {
            me->Play();
        }
    }
     else if (GetInput().IsKeyDown(sf::Key::Left) ||
              GetInput().IsKeyDown(sf::Key::Q))
     {
         switch (me->getCollision(Left, m_map))
         {
         case Floor:
         case Bonus:
              if (me->getDirection() != Left)
              {
                  me->setDirection(Left);
                  send(m_socket , "MOVE " + me->getNick() + " LEFT " + pos);
                  qDebug() << "LEFT";
              }
              break;
         case Bomb:
              if (me->getBonus(CBonus::BombPass) != NULL)
              {
                  me->setDirection(Left);
              }
              else if (me->getBonus(CBonus::BombKick) != NULL)
              {
                  QList<CBomb *>::iterator it;
                  for (it = m_bombsList.begin(); it != m_bombsList.end(); ++it)
                  {
                      CBomb *bomb = *it;
                      if (bomb->getX() == me->getX() - 1 && bomb->getY() == me->getY())
                      {
                          bomb->setDirection(CBomb::Left);
                          m_map.setBlock(bomb->getX(), bomb->getY(), Floor);
                          break;
                      }
                  }
              }
              break;
         default:
              break;
         }

         if (me->IsPaused())
         {
             me->Play();
         }
     }
     else if (GetInput().IsKeyDown(sf::Key::Down) ||
              GetInput().IsKeyDown(sf::Key::S))
     {
         switch (me->getCollision(Down, m_map))
         {
         case Floor:
         case Bonus:
             if (me->getDirection() != Down)
              {
                   me->setDirection(Down);
                   send(m_socket , "MOVE " + me->getNick() + " DOWN " + pos);
                   qDebug() << "DOWN";
              }
             else if (me->getBonus(CBonus::BombKick) != NULL)
             {
                 QList<CBomb *>::iterator it;
                 for (it = m_bombsList.begin(); it != m_bombsList.end(); ++it)
                 {
                     CBomb *bomb = *it;
                     if (bomb->getX() == me->getX() && bomb->getY() == me->getY() + 1)
                     {
                         bomb->setDirection(CBomb::Down);
                         m_map.setBlock(bomb->getX(), bomb->getY(), Floor);
                         break;
                     }
                 }
             }
             break;
         case Bomb:
             if (me->getBonus(CBonus::BombPass) != NULL)
             {
                 me->setDirection(Down);
             }
             break;
         default:
             break;
         }

         if (me->IsPaused())
         {
             me->Play();
         }

     }
     else if (GetInput().IsKeyDown(sf::Key::Up) ||
              GetInput().IsKeyDown(sf::Key::Z))
     {
         switch (me->getCollision(Up, m_map))
         {
         case Floor:
         case Bonus:
               if (me->getDirection() != Up)
               {
                    send(m_socket, "MOVE " + me->getNick() + " UP " + pos);
                    me->setDirection(Up);
                    qDebug() << "UP";
               }
               break;
         case Bomb:
               if (me->getBonus(CBonus::BombPass) != NULL)
               {
                   me->setDirection(Up);
               }
               else if (me->getBonus(CBonus::BombKick) != NULL)
               {
                   QList<CBomb *>::iterator it;
                   for (it = m_bombsList.begin(); it != m_bombsList.end(); ++it)
                   {
                       CBomb *bomb = *it;
                       if (bomb->getX() == me->getX() && bomb->getY() == me->getY() - 1)
                       {
                           bomb->setDirection(CBomb::Up);
                           m_map.setBlock(bomb->getX(), bomb->getY(), Floor);
                           break;
                       }
                   }
               }
               break;
         default:
             break;
         }

         if (me->IsPaused())
         {
             me->Play();
         }

     }
     else
     {
         if (me->getDirection() != Stopped && me->getElapsedTime() >= me->getStopTime())
         {
             me->setDirection(Stopped);
             send(m_socket, "MOVE " + me->getNick() + " STOP " + pos);
             qDebug() << "STOP";
             me->Pause();
         }
     }
}

/**
  Draw all the blocks of the map
*/
void CGameBoard::drawMap()
{
    /* Drawing the map */
    for(unsigned i = 0; i < MAP_WIDTH; i++)
    {
        for(unsigned j = 0; j < MAP_HEIGHT; j++)
        {
            switch(m_map.getBlock(i, j))
            {
            case Wall:
                m_wall.SetPosition(BLOCK_SIZE * i, BLOCK_SIZE * j);
                Draw(m_wall);
                break;
            case Box:
            case BonusBox:
                m_box.SetPosition(BLOCK_SIZE * i, BLOCK_SIZE * j);
                Draw(m_box);
                break;
            case Bonus:
                m_bonus.SetPosition((BLOCK_SIZE * i) + 5, (BLOCK_SIZE * j) + 5);
                Draw(m_bonus);
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

    /* Drawing the bombs */
    QList<CBomb *>::iterator it = m_bombsList.begin();
    while(it != m_bombsList.end())
    {
        CBomb *bomb = *it;
        unsigned x = bomb->getX();
        unsigned y = bomb->getY();
        /* Update the time */
        bomb->updateTime(GetFrameTime());
        /* It the bomb must explode */
        if (bomb->explode())
        {
            CPlayer *me = m_playersList[0];
            CPlayer *player = getPlayerFromNick(bomb->getBomber());
            QSound::play("../explosion.wav");
            m_explosionsList.push_back(new CExplosion(x, y, player->bombRange, m_map,
                                                      bomb->getBomber(), m_explosion));
            if (bomb->getBomber() == me->getNick()) /* If I am the bomber */
            {
                me->pausedBombs--;
            }
            m_map.setBlock(bomb->getX(), bomb->getY(), Floor);
            it = m_bombsList.erase(it);
            delete bomb;
        }
        else
        {
            /* Let's check if the camp has been kicked (bomb kick) */
            if (bomb->getDirection() != CBomb::Fixed)
            {
                switch (bomb->getCollision(bomb->getDirection(), m_map))
                {
                case Floor:
                case Bonus:
                    {
                    /* The bomb still can move */
                    unsigned last_x = bomb->getX(), last_y = bomb->getY();
                    bomb->move(bomb->getDirection(), GetFrameTime());
                    unsigned new_x = bomb->getX(), new_y = bomb->getY();
                    if (last_x != new_x && last_y != new_y)
                    {
                        if (m_map.getBlock(new_x, new_y) != Bonus)
                        {
                            m_map.setBlock(new_x, new_y, Bomb);
                            if (m_map.getBlock(last_x, last_y) != Bonus)
                            {
                                m_map.setBlock(last_x, last_y, Floor);
                            }
                        }
                    }
                }
                    break;
                case Wall:
                case Box:
                case BonusBox:
                case Bomb:
                    /* We reached a block, we stop the move */
                    bomb->SetPosition((bomb->getX() * BLOCK_SIZE) + 6, (bomb->getY() * BLOCK_SIZE) + 6);
                    bomb->setDirection(CBomb::Fixed);
                    m_map.setBlock(bomb->getX(), bomb->getY(), Bomb);
                    break;
                default:
                    break;
                }
            }
            Draw(*bomb);
            ++it;
        }
    }
}

void CGameBoard::drawPlayers()
{
    for (int i = 0; i < m_playersList.size(); ++i)
    {
        CPlayer *player = m_playersList[i];
        player->updateBonusTime(GetFrameTime());

        if (player->getDirection() != Stopped)
        {
            unsigned x = player->getX();
            unsigned y = player->getY();

            switch (player->getCollision(player->getDirection(), m_map))
            {
            case Floor:
                player->move(player->getDirection(), GetFrameTime());
                break;
            case Box:
            case BonusBox:
            case Wall:
                break;
            case Bonus:
                if (i == 0) /* If I got the bonus */
                {
                    if (m_bonusCanvas == NULL)
                    {
                        m_bonusCanvas = new CBonusCanvas(0.05f, 1.0f, sf::Rect<int>(535, 320,605, 390));
                        m_map.setBlock(x, y, Floor);
                        QSound::play("../bonus.wav");
                    }
                    else
                    {
                        m_bonusCanvas->Reset();
                        m_bonusCanvas->Play();
                        m_map.setBlock(x, y, Floor);
                        QSound::play("../bonus.wav");
                    }
                }
                else
                {
                    QSound::play("../bonus.wav");
                    m_map.setBlock(x, y, Floor);
                }
                break;
            case Bomb:
                if (player->getBonus(CBonus::BombPass) != NULL)
                {
                    player->move(player->getDirection(), GetFrameTime());
                }
                else if (player->getBonus(CBonus::BombKick) != NULL)
                {
                    unsigned _x = 0, _y = 0;
                    CBomb::Direction direction;
                    switch (player->getDirection())
                    {
                    case Right:
                       _x = player->getX() + 1;
                       _y = player->getY();
                       direction = CBomb::Right;
                       break;
                    case Left:
                       _x = player->getX() - 1;
                       _y = player->getY();
                       direction = CBomb::Left;
                       break;
                    case Up:
                       _x = player->getX();
                       _y = player->getY() - 1;
                       direction = CBomb::Up;
                       break;
                    case Down:
                       _x = player->getX();
                       _y = player->getY() + 1;
                       direction = CBomb::Down;
                    default:
                       break;
                    }
                    QList<CBomb *>::iterator it;
                    for (it = m_bombsList.begin(); it != m_bombsList.end(); ++it)
                    {
                        CBomb *bomb = *it;
                        if (bomb->getX() == _x && bomb->getY() == _y)
                        {
                            bomb->setDirection(direction);
                            m_map.setBlock(bomb->getX(), bomb->getY(), Floor);
                            break;
                        }
                    }
                }
                break;
            default:
                break;
            }
        }

        if (player->isDead())
        {
            if (static_cast<unsigned>(player->GetCurrentFrame()) ==
                player->GetAnim()->Size() - 1)
            {
                player->Pause();
                continue;
            }
        }
        player->anim(GetFrameTime());
        Draw(*player);
    }
}

void CGameBoard::drawExplosions()
{
    CPlayer *me = m_playersList[0];
    QList<CExplosion *>::iterator it;
    for (it = m_explosionsList.begin(); it != m_explosionsList.end(); ++it)
    {
        QVector<CParticle *> particles = (*it)->getParticles();
        if (static_cast<unsigned>(particles[0]->GetCurrentFrame()) ==
                particles[0]->GetAnim()->Size() - 1)
        {
            m_explosionsList.removeOne(*it);
            delete *it;
        }

        for (int j = 0; j < particles.size(); j++)
        {
            unsigned x = particles[j]->getX();
            unsigned y = particles[j]->getY();
            if (m_map.getBlock(x, y) == Box)
            {
                m_map.setBlock(x, y, Floor);
            }
            else if (m_map.getBlock(x, y) == BonusBox)
            {
                m_map.setBlock(x, y, Bonus);
            }
            Draw(*particles[j]);
            particles[j]->anim(GetFrameTime());
            if (collision(*particles[j], *me))
            {
                me->explode();
                me->setDirection(Stopped);
                if (me->IsPaused())
                {
                    me->Play();
                }
            }
        }
    }
}

void CGameBoard::drawFPS()
{
    sf::String FPS(QString("%1 FPS").arg(static_cast<unsigned>(m_frameRate)).toStdString());
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

void CGameBoard::drawBonusCanvas()
{
    if (m_bonusCanvas != NULL)
    {
        /* Header : bonus name */
        sf::Shape header = sf::Shape::Rectangle(
                m_bonusCanvas->getCanvasPosition().Left, m_bonusCanvas->getCanvasPosition().Top - 20,
                m_bonusCanvas->getCanvasPosition().Right, m_bonusCanvas->getCanvasPosition().Bottom - 20,
                sf::Color(127, 127, 127), 1.0f, sf::Color(0, 0, 0));
        Draw(header);

        /* Bonus Name */
        sf::String bonusName;
        bonusName.SetText(m_bonusCanvas->getBonus()->toString());
        bonusName.SetSize(11.0f);
        bonusName.SetColor(sf::Color(255, 255, 255));
        bonusName.SetPosition(m_bonusCanvas->getCanvasPosition().Left + 3,
                              m_bonusCanvas->getCanvasPosition().Top - 17);
        Draw(bonusName);

        sf::Shape canvas = sf::Shape::Rectangle(
                m_bonusCanvas->getCanvasPosition().Left, m_bonusCanvas->getCanvasPosition().Top,
                m_bonusCanvas->getCanvasPosition().Right, m_bonusCanvas->getCanvasPosition().Bottom,
                sf::Color(127, 127, 127), 1.0f, sf::Color(0, 0, 0));
        Draw(canvas);
        Draw(*m_bonusCanvas);

        m_bonusCanvas->playNextBonus(GetFrameTime());
        if (m_bonusCanvas->isFinished())
        {
            CPlayer *me = m_playersList[0];
            if (!m_bonusCanvas->isPaused())
            {
                /* Let's check which bonus we've got */
                switch (m_bonusCanvas->getBonus()->getType())
                {
                case CBonus::BombDown:
                    me->addBonus(new CBonus(CBonus::BombDown));
                    break;
                case CBonus::BombUp:
                    me->addBonus(new CBonus(CBonus::BombUp));
                    send(m_socket, "BONUS " + me->getNick() + " BOMBUP");
                    break;
                case CBonus::SpeedDown:
                    if (me->getBonus(CBonus::SpeedDown) == NULL)
                    {
                        me->addBonus(new CLimitedBonus(CBonus::SpeedDown, 10.f));
                        send(m_socket, "BONUS " + me->getNick() + " SPEEDDOWN");
                    }
                    break;
                case CBonus::SpeedUp:
                    if (me->getBonus(CBonus::SpeedUp) == NULL)
                    {
                        me->addBonus(new CLimitedBonus(CBonus::SpeedUp, 10.f));
                        send(m_socket, "BONUS " + me->getNick() + " SPEEDUP");
                    }
                    break;
                case CBonus::FireUp:
                    me->addBonus(new CBonus(CBonus::FireUp));
                    send(m_socket, "BONUS " + me->getNick() + " FIREUP");
                    break;
                case CBonus::FireDown:
                    me->addBonus(new CBonus(CBonus::FireDown));
                    send(m_socket, "BONUS " + me->getNick() + " FIREDOWN");
                    break;
                case CBonus::FullFire:
                    me->addBonus(new CBonus(CBonus::FullFire));
                    break;
                case CBonus::RemoteMine:
                    if (me->getBonus(CBonus::RemoteMine) == NULL)
                    {
                        me->addBonus(new CBonus(CBonus::RemoteMine));
                        send(m_socket, "BONUS " + me->getNick() + " REMOTEMINE");
                    }
                case CBonus::BombPass:
                    me->removeBonus(CBonus::BombKick);
                    me->addBonus(new CBonus(CBonus::BombPass));
                    send(m_socket, "BONUS " + me->getNick() + " BOMBPASS");
                    break;
                case CBonus::BombKick:
                    me->removeBonus(CBonus::BombPass);
                    me->addBonus(new CBonus(CBonus::BombKick));
                    send(m_socket, "BONUS " + me->getNick() + " BOMBKICK");
                    break;
                default:
                    break;
                }
                m_bonusCanvas->Pause();
            }
            else
            {
                CBonus *lastBonus = me->getBonusList().back();
                if (lastBonus != NULL && dynamic_cast<CLimitedBonus *>(lastBonus))
                {
                    CLimitedBonus *bonus = dynamic_cast<CLimitedBonus *>(lastBonus);

                    /* Footer : remaining time */
                    sf::Shape footer = sf::Shape::Rectangle(
                     m_bonusCanvas->getCanvasPosition().Left, m_bonusCanvas->getCanvasPosition().Bottom,
                     m_bonusCanvas->getCanvasPosition().Right, m_bonusCanvas->getCanvasPosition().Bottom + 20,
                     sf::Color(127, 127, 127), 1.0f, sf::Color(0, 0, 0));
                    Draw(footer);

                    unsigned remainingTime = static_cast<unsigned>(bonus->getRemainingTime());
                    QString time = QString("%1s left").arg(remainingTime);

                    /* Remaining time */
                    sf::String timeLeft;
                    timeLeft.SetText(time.toStdString());
                    timeLeft.SetStyle(sf::String::Italic);
                    timeLeft.SetPosition(m_bonusCanvas->getCanvasPosition().Left + 15,
                                     m_bonusCanvas->getCanvasPosition().Bottom + 2);
                    timeLeft.SetSize(13.0f);
                    Draw(timeLeft);
                    if (remainingTime == 0)
                    {
                        delete m_bonusCanvas;
                        m_bonusCanvas = NULL;
                    }
                }
            }
        }
    }
}

/**
  Plant a bomb
*/
void CGameBoard::plantBomb()
{
    CPlayer *me = m_playersList[0];
    unsigned x = 0, y = 0;

    qDebug() << me->pausedBombs << " " << me->maxBombs;
    if (me->pausedBombs == me->maxBombs) /* We reached the limit */
    {
        return;
    }
    x = me->getX();
    y = me->getY();
    m_map.setBlock(x, y, Bomb);

    QString pos = QString("%1 %2").arg(x).arg(y);

    CBonus *bonus;
    bool hasUsed;
    /* Have remote mine bonus ? */
    if ((bonus = me->getBonus(CBonus::RemoteMine)) != NULL)
    {
        QList<CBomb *>::const_iterator it;
        for (it = m_bombsList.begin(); it != m_bombsList.end(); ++it)
        {
            if ((*it)->getType() == CBomb::Remote &&
                (*it)->getBomber() == me->getNick())
            {
                hasUsed = true;
                break;
            }
        }
        if (!hasUsed)
        {
            m_bombsList.append(new CBomb(x, y, CBomb::Remote, me->getNick()));
            send(m_socket, "BOMB " + me->getNick() + " " + pos.toStdString() + " REMOTE");
        }
        else
        {
            m_bombsList.append(new CBomb(x, y, CBomb::Normal, me->getNick()));
            send(m_socket, "BOMB " + me->getNick() + " " + pos.toStdString() + " NORMAL");
        }

    }
    else
    {
        m_bombsList.append(new CBomb(x, y, CBomb::Normal, me->getNick()));
        send(m_socket, "BOMB " + me->getNick() + " " + pos.toStdString() + " NORMAL");
    }
    me->pausedBombs++;
    QSound::play("../plant.wav");
}

void CGameBoard::useSpecialBonus()
{
    CBonus *bonus;
    CPlayer *me = m_playersList[0];
    if ((bonus = me->getBonus(CBonus::RemoteMine)) != NULL)
    {
        QList<CBomb *>::iterator it;
        unsigned x, y;
        bool usedBonus = false;
        for (it = m_bombsList.begin(); it != m_bombsList.end(); ++it)
        {
            CBomb *bomb = *it;
            if (bomb->getType() == CBomb::Remote &&
                bomb->getBomber() == me->getNick())
            {
                m_bombsList.removeOne(*it);
                x = bomb->getX();
                y = bomb->getY();
                delete bomb;
                usedBonus = true;
                break;
            }
        }
        if (usedBonus)
        {
            QSound::play("../explosion.wav");
            QString pos = QString("%1 %2").arg(x).arg(y);
            m_explosionsList.push_back(new CExplosion(x, y, me->bombRange, m_map,
                                                      me->getNick(), m_explosion));
            m_map.setBlock(x, y, Floor);
            me->pausedBombs--;
            me->removeBonus(CBonus::RemoteMine);
            send(m_socket, "BOOM " + me->getNick() + " " + pos.toStdString());
        }
    }
}

void CGameBoard::plantedBomb(const std::string &bomber, unsigned x, unsigned y,
                             const std::string &type)
{
    m_map.setBlock(x, y, Bomb);
    if (type == "NORMAL")
    {
        m_bombsList.append(new CBomb(x, y, CBomb::Normal, bomber));
    }
    else
    {
        m_bombsList.append(new CBomb(x, y, CBomb::Remote, bomber));
    }
    QSound::play("../plant.wav");
}

void CGameBoard::remoteExplode(const std::string &bomber, unsigned x, unsigned y)
{
    CPlayer *player = getPlayerFromNick(bomber);
    m_map.setBlock(x, y, Floor);
    /* Remove the bomb from the list */
    QList<CBomb *>::iterator it;
    for (it = m_bombsList.begin(); it != m_bombsList.end(); ++it)
    {
        CBomb *bomb = *it;
        if (bomb->getX() == x && bomb->getY() == y)
        {
            m_bombsList.removeOne(bomb);
            delete bomb;
            break;
        }
    }
    QSound::play("../explosion.wav");
    m_explosionsList.push_back(new CExplosion(x, y, player->bombRange, m_map,
                                                  bomber, m_explosion));
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
    warmupTimer = new QTimer(this);
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
        delete warmupTimer;
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
    player->Pause();
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

void CGameBoard::playerMove(const std::string &nick, const std::string &move, const float x, const float y)
{
    CPlayer *player = getPlayerFromNick(nick);
    assert(player != NULL);
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
    player->SetPosition(x, y);
}

void CGameBoard::playerGotBonus(const std::string &nick, const std::string &bonus)
{
    CPlayer *player = getPlayerFromNick(nick);
    assert(player != NULL);

    if (bonus == "SPEEDUP")
    {
        player->addBonus(new CLimitedBonus(CBonus::SpeedUp, 10.0f));
    }
    else if (bonus == "SPEEDDOWN")
    {
        player->addBonus(new CLimitedBonus(CBonus::SpeedDown, 10.0f));
    }
    else if (bonus == "BOMBUP")
    {
        player->addBonus(new CBonus(CBonus::BombUp));
    }
    else if (bonus == "FIREUP")
    {
        player->addBonus(new CBonus(CBonus::FireUp));
    }
    else if (bonus == "FIREDOWN")
    {
        player->addBonus(new CBonus(CBonus::FireDown));
    }
    else if (bonus == "FULLFIRE")
    {
        player->addBonus(new CBonus(CBonus::FullFire));
    }
    else if (bonus == "BOMBPASS")
    {
        player->addBonus(new CBonus(CBonus::BombPass));
    }
    else if (bonus == "BOMBKICK")
    {
        player->addBonus(new CBonus(CBonus::BombKick));
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
