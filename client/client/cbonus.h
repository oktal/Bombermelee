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
        SpeedUp, SpeedDown,
        RemoteMine
    };

    CBonus(BonusType type);
    virtual ~CBonus();
    BonusType getType() const;
    std::string toString() const;

protected:
    BonusType m_type;
};

#endif // CBONUS_H
