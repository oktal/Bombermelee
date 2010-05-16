#include "cbonus.h"

CBonus::CBonus(BonusType type)
{
    m_type = type;
}

CBonus::~CBonus()
{

}

CBonus::BonusType CBonus::getType() const
{
    return m_type;
}

std::string CBonus::toString() const
{
    switch (m_type)
    {
    case SpeedUp:
        return "Speed Up";
        break;
    case SpeedDown:
        return "Speed Down";
        break;
    case FireUp:
        return "Fire Up";
        break;
    case FireDown:
        return "Fire Down";
        break;
    case FullFire:
        return "Full Fire";
        break;
    case BombUp:
        return "Bomb Up";
        break;
    case BombDown:
        return "Bomb Down";
        break;
    case RemoteMine:
        return "Remote Mine";
        break;
    case BombPass:
        return "Bomb Pass";
        break;
    case BombKick:
        return "Bomb Kick";
        break;
    default:
        return "";
    }
}
