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
