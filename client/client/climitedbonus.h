#ifndef CLIMITEDBONUS_H
#define CLIMITEDBONUS_H

#include "cbonus.h"

/**
  @class CLimitedBonus
  represents a limited bonus in the time
*/

class CLimitedBonus : public CBonus
{
public:
    CLimitedBonus(BonusType type, float time);
    bool isFinished() const;
    void updateTime(const float &elapsedTime);
    BonusInstanceType getInstanceType() const;

private:
    float m_time;
    float m_elapsedTime;
    bool m_finished;
};

#endif // CLIMITEDBONUS_H
