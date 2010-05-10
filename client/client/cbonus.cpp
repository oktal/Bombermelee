#include "cbonus.h"

CBonus::CBonus(BonusType type)
{
    m_type = type;
}

CBonus::BonusType CBonus::getType() const
{
    return m_type;
}

CBonus::BonusInstanceType CBonus::getInstanceType() const
{
    return NormalBonus;
}
