#include "climitedbonus.h"

CLimitedBonus::CLimitedBonus(BonusType type, float time) :
        CBonus(type)
{
    m_time = time;
}

bool CLimitedBonus::isFinished() const
{
    return m_clock.GetElapsedTime() >= m_time;
}

float CLimitedBonus::getRemainingTime() const
{
    return m_time - m_clock.GetElapsedTime();
}
