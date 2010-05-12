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

    CBonus(BonusType type);
    virtual ~CBonus();
    BonusType getType() const;
    std::string toString() const;

private:
    BonusType m_type;
};

#endif // CBONUS_H
