#include "cbomb.h"
#include "cmap.h"
#include "cimagemanager.h"

CBomb::CBomb(unsigned x, unsigned y, BombType type, const std::string &bomber)
{
    m_x = x;
    m_y = y;
    m_type = type;
    m_bomber = bomber;
    m_explodeTime = (m_type == Normal ? ExplodeTime : RemoteExplodeTime);
    m_direction = Fixed;
    m_speed = 120;
    CImageManager *imageManager = CImageManager::GetInstance();
    switch (m_type)
    {
    case Normal:
        SetImage(*imageManager->GetImage("../bomb.png"));
        break;
    case Remote:
        SetImage(*imageManager->GetImage("../remote_bomb.png"));
        break;
    }
    SetPosition((BLOCK_SIZE * x) + 6, (BLOCK_SIZE * y) + 6);
}

unsigned CBomb::getX() const
{
    return m_x;
}

unsigned CBomb::getY() const
{
    return m_y;
}

CBomb::BombType CBomb::getType() const
{
    return m_type;
}

std::string CBomb::getBomber() const
{
    return m_bomber;
}

bool CBomb::explode() const
{
    return m_clock.GetElapsedTime() >= m_explodeTime;
}

void CBomb::setDirection(Direction direction)
{
    m_direction = direction;
}

CBomb::Direction CBomb::getDirection() const
{
    return m_direction;
}

BlockType CBomb::getCollision(Direction direction, CMap &map) const
{
    BlockType ret = Floor;
    switch (direction)
    {
    case Right:
        ret = (m_x + 1 >= MAP_WIDTH ? Wall : map.getBlock(m_x + 1, m_y));
        break;
    case Left:
        ret = (m_x <= 0 ? Wall : map.getBlock(m_x - 1, m_y));
        break;
    case Up:
        ret = (m_y <= 0 ? Wall : map.getBlock(m_x, m_y - 1));
        break;
    case Down:
        ret = (m_y + 1 >= MAP_HEIGHT ? Wall : map.getBlock(m_x, m_y + 1));
    }
    return ret;
}

void CBomb::move(Direction direction, const float &frameTime)
{
    switch (direction)
    {
    case Right:
        Move(m_speed * frameTime, 0);
        break;
    case Left:
        Move(m_speed * -frameTime, 0);
        break;
    case Up:
        Move(0, m_speed * -frameTime);
        break;
    case Down:
         Move(0, m_speed * frameTime);
         break;
    default:
         break;
    }
    m_x = (GetPosition().x + GetSubRect().GetWidth() / 2) / BLOCK_SIZE;
    m_y = (GetPosition().y + GetSubRect().GetHeight() / 2) / BLOCK_SIZE;
}
