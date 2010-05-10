#ifndef CBONUS_H
#define CBONUS_H

#include "animated.h"


/**
  @class CBonus
  repesents a bonus in the game
*/

class CBonus
{
public:
    enum BonusType
    {
        BombUp, BombDown,
        FireUp, FireDown, FullFire,
        SpeedUp, SpeedDown
    };

    enum BonusInstanceType
    {
        NormalBonus, LimitedBonus
    };

    CBonus(BonusType type);
    BonusType getType() const;
    virtual BonusInstanceType getInstanceType() const;

private:
    BonusType m_type;
};

#endif // CBONUS_H
