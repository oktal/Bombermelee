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
    float ElapsedTime = GetFrameTime();
    Draw(rightBorder);
    drawMap();
    drawFPS();
    drawStatus();

    /* RIGHT */
    CPlayer *me = m_playersList[0]; /* I am the first player of the list */
    if (GetInput().IsKeyDown(sf::Key::Right))
    {
        me->setDirection(Right);
        if (canMove(Right, ElapsedTime))
        {
            me->move(Right, ElapsedTime);
            QString pos = QString("%1 %2").arg(me->GetPosition().x).arg(me->GetPosition().y);
            m_socket->write(_m("MOVE " + me->getNick() + " RIGHT " + pos.toStdString()));
        }
        me->anim(ElapsedTime);

    }

    /* LEFT */
    else if (GetInput().IsKeyDown(sf::Key::Left))
    {
        me->setDirection(Left);
        if (canMove(Left, ElapsedTime))
        {
            me->move(Left, ElapsedTime);
            QString pos = QString("%1 %2").arg(me->GetPosition().x).arg(me->GetPosition().y);
            m_socket->write(_m("MOVE " + me->getNick() + " LEFT " + pos.toStdString()));
        }
        me->anim(ElapsedTime);
    }

    /* DOWN */
    else if (GetInput().IsKeyDown(sf::Key::Down))
    {
        me->setDirection(Down);
        if (canMove(Down, ElapsedTime))
        {
            me->move(Down, ElapsedTime);
            QString pos = QString("%1 %2").arg(me->GetPosition().x).arg(me->GetPosition().y);
            m_socket->write(_m("MOVE " + me->getNick() + " DOWN " + pos.toStdString()));
        }
        me->anim(ElapsedTime);
    }

    /* UP */
    else if (GetInput().IsKeyDown(sf::Key::Up))
    {
        me->setDirection(Up);
        if (canMove(Up, ElapsedTime))
        {
            me->move(Up, ElapsedTime);
            QString pos = QString("%1 %2").arg(me->GetPosition().x).arg(me->GetPosition().y);
            m_socket->write(_m("MOVE " + me->getNick() + " UP " + pos.toStdString()));
        }
        me->anim(ElapsedTime);
    }
    /* SPACE (plant bomb) */
    else if (GetInput().IsKeyDown(sf::Key::Space))
    {
        plantBomb();
    }
    else
    {
        /*if (!m_player.IsStoped())
        {
            m_player.Stop();
        }
        */
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
        Draw(*m_playersList[i]);
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

/**
  Check if the player can move
  @param movement Direction to go
  @param ElapsedTime FrameTime
  @return true if the player can move, false if not
*/
bool CGameBoard::canMove(Direction movement, const float &ElapsedTime)
{
    int pos_x1, pos_y1, pos_x2, pos_y2;
    CPlayer *me = m_playersList[0];
    pos_x1 = pos_y1 = pos_x2 = pos_y2 = 0;
    if (!m_gameBegin || !m_connected)
    {
        return false;
    }
    switch (movement)
    {
    case Left:
        pos_x1 = me->GetPosition().x + 2;
        pos_y1 = me->GetPosition().y + 2;
        pos_x2 = pos_x1;
        pos_y2 = pos_y1 + me->GetSubRect().GetHeight() - 4;
        pos_x1 -= (Speed * ElapsedTime);
        pos_x2 -= (Speed * ElapsedTime);
        break;
    case Right:
        pos_x1 = me->GetPosition().x + me->GetSubRect().GetWidth() - 2;
        pos_y1 = me->GetPosition().y + 2;
        pos_x2 = pos_x1;
        pos_y2 = pos_y1 + me->GetSubRect().GetHeight() - 2;
        pos_x1 += (Speed * ElapsedTime);
        pos_x2 += (Speed * ElapsedTime);
        break;
    case Up:
        pos_x1 = me->GetPosition().x + 2;
        pos_y1 = me->GetPosition().y + 2;
        pos_x2 = pos_x1 + me->GetSubRect().GetWidth() - 4;
        pos_y2 = pos_y1;
        pos_y1 -= (Speed * ElapsedTime);
        pos_y2 -= (Speed * ElapsedTime);
        break;
    case Down:
        pos_x1 = me->GetPosition().x + 2;
        pos_y1 = me->GetPosition().y + me->GetSubRect().GetHeight() - 2;
        pos_x2 = pos_x1 + me->GetSubRect().GetWidth() - 4;
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
    s = QString("%1 %2").arg(x2).arg(y2);
    str.SetText(s.toStdString());
    str.SetPosition(520, 30);
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
        m_map.getBlock(x2, y2) == Bonus)
    {
        return false;
    }
    /* If the case we want to move is a bomb */
    else if (m_map.getBlock(x1, y1) == Bomb ||
             m_map.getBlock(x2, y2) == Bomb)
    {
        /* Where are we ? */
        unsigned my_x1 = (me->GetPosition().x + (me->GetSubRect().GetWidth() / 2)) / 34;
        unsigned my_y1 = (me->GetPosition().y + (me->GetSubRect().GetHeight() / 2)) / 34;
        /* If we already are on a bomb, we can run */
        if (m_map.getBlock(my_x1, my_y1) != Bomb)
        {
            return false;
        }
    }
    /* We can move */
    return true;
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
    player->Play();
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
