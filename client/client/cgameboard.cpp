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
#include <ctime>

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
    while (to->write(_m(what)) != len)
    {

    }
}

CGameBoard::CGameBoard(QWidget *parent, const QPoint &position, const QSize &size, unsigned int FrameTime) :
        QSFMLCanvas(parent, position, size, FrameTime)
{
    CPlayer *me = new CPlayer();
    me->setStopTime(0.15f);
    m_playersList.append(me);
    warmupTimer = new QTimer(this);
    m_gameBegin = true;
    m_connected = false;
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
    srand(time(NULL));
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
    if (me->gotBonus)
    {
        unsigned x = me->getX();
        unsigned y = me->getY();
        m_map.setBlock(x, y, Floor);
        if (m_bonusCanvas == NULL)
        {
            m_bonusCanvas = new CBonusCanvas(0.1f, 4.0f, sf::Rect<int>(535, 320,605, 390));
        }
        else
        {
            m_bonusCanvas->Reset();
            m_bonusCanvas->Play();
        }
        qDebug() << "You got a Bonus";
        me->gotBonus = false;
        QSound::play("../bonus.wav");
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
            break;
        default:
            break;
        }
    }


    if (GetInput().IsKeyDown(sf::Key::Right))
    {
        if (me->canMove(Right, m_map))
        {
            if (me->getDirection() != Right)
            {
                send(m_socket , "MOVE " + me->getNick() + " RIGHT " + pos);
                me->setDirection(Right);
                qDebug() << "RIGHT";
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
                  me->setDirection(Left);
                  send(m_socket , "MOVE " + me->getNick() + " LEFT " + pos);
                  qDebug() << "LEFT";
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
                   me->setDirection(Down);
                   send(m_socket , "MOVE " + me->getNick() + " DOWN " + pos);
                   qDebug() << "DOWN";
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
                    send(m_socket, "MOVE " + me->getNick() + " UP " + pos);
                    me->setDirection(Up);
                    qDebug() << "UP";
               }
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
            case BonusBox:
                m_box.SetPosition(34*i, 34*j);
                Draw(m_box);
                break;
            case Bonus:
                m_bonus.SetPosition((34 * i) + 5, (34 * j) + 5);
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
        player->updateBonusTime(GetFrameTime());
        if (player->gotBonus && i != 0)
        {
            unsigned x = player->getX();
            unsigned y = player->getY();
            m_map.setBlock(x, y, Floor);
            QSound::play("../bonus.wav");
            player->gotBonus = false;
        }

        if (player->getDirection() != Stopped)
        {
            if (player->canMove(player->getDirection(), m_map))
            {
                player->move(player->getDirection(), GetFrameTime());
            }
            player->anim(GetFrameTime());
        }
        Draw(*player);
    }
}

void CGameBoard::drawExplosions()
{
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
        }
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

void CGameBoard::drawBonusCanvas()
{
    if (m_bonusCanvas != NULL)
    {
        sf::Shape header = sf::Shape::Rectangle(
                m_bonusCanvas->getCanvasPosition().Left, m_bonusCanvas->getCanvasPosition().Top - 20,
                m_bonusCanvas->getCanvasPosition().Right, m_bonusCanvas->getCanvasPosition().Bottom - 20,
                sf::Color(127, 127, 127), 1.0f, sf::Color(0, 0, 0));
        Draw(header);
        sf::String bonusName;
        bonusName.SetText(m_bonusCanvas->getBonus().toString());
        bonusName.SetSize(11.0f);
        bonusName.SetColor(sf::Color(255, 255, 255));
        bonusName.SetPosition(m_bonusCanvas->getCanvasPosition().Left + 5,
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
                CBonus bonus = m_bonusCanvas->getBonus();
                switch (bonus.getType())
                {
                case CBonus::BombDown:
                    me->newBonus(new CBonus(CBonus::BombDown));
                    break;
                case CBonus::BombUp:
                    me->newBonus(new CBonus(CBonus::BombUp));
                    send(m_socket, "BONUS " + me->getNick() + " BOMBUP");
                    break;
                case CBonus::SpeedDown:
                    if (!me->alreadyHasBonus(CBonus::SpeedDown))
                    {
                        me->newBonus(new CLimitedBonus(CBonus::SpeedDown, 10.f));
                        send(m_socket, "BONUS " + me->getNick() + " SPEEDDOWN");
                    }
                    break;
                case CBonus::SpeedUp:
                    if (!me->alreadyHasBonus(CBonus::SpeedUp))
                    {
                        me->newBonus(new CLimitedBonus(CBonus::SpeedUp, 10.f));
                        send(m_socket, "BONUS " + me->getNick() + " SPEEDUP");
                    }
                    break;
                case CBonus::FireUp:
                    me->newBonus(new CBonus(CBonus::FireUp));
                    break;
                case CBonus::FireDown:
                    me->newBonus(new CBonus(CBonus::FireDown));
                    break;
                default:
                    break;
                }
                m_bonusCanvas->Pause();
            }
            else
            {
                CBonus *lastBonus = me->getLastBonus();
                if (lastBonus != NULL && dynamic_cast<CLimitedBonus *>(lastBonus))
                {
                    CLimitedBonus *bonus = dynamic_cast<CLimitedBonus *>(lastBonus);
                    sf::Shape footer = sf::Shape::Rectangle(
                     m_bonusCanvas->getCanvasPosition().Left, m_bonusCanvas->getCanvasPosition().Bottom,
                     m_bonusCanvas->getCanvasPosition().Right, m_bonusCanvas->getCanvasPosition().Bottom + 20,
                     sf::Color(127, 127, 127), 1.0f, sf::Color(0, 0, 0));
                    Draw(footer);
                    unsigned remainingTime = static_cast<unsigned>(bonus->getRemainingTime());
                    QString time = QString("%1s left").arg(remainingTime);
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
    m_explosionsList.push_back(new CExplosion(x, y));
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
        player->newBonus(new CLimitedBonus(CBonus::SpeedUp, 10.0f));
    }
    else if (bonus == "SPEEDDOWN")
    {
        player->newBonus(new CLimitedBonus(CBonus::SpeedDown, 10.0f));
    }
    else if (bonus == "BOMBUP")
    {
        player->newBonus(new CBonus(CBonus::BombUp));
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
