#include "climitedbonus.h"

CLimitedBonus::CLimitedBonus(BonusType type, float time) :
        CBonus(type)
{
    m_time = time;
    m_elapsedTime = 0.0f;
    m_finished = false;
}

bool CLimitedBonus::isFinished() const
{
    return m_finished;
}

void CLimitedBonus::updateTime(const float &elapsedTime)
{
    m_elapsedTime += elapsedTime;
    if (m_elapsedTime >= m_time)
    {
        m_finished = true;
    }
}

CBonus::BonusInstanceType CLimitedBonus::getInstanceType() const
{
    return LimitedBonus;
}
