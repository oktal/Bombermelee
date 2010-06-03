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
    float getRemainingTime() const;

private:
    float m_time;
    sf::Clock m_clock;
};

#endif // CLIMITEDBONUS_H
