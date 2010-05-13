#ifndef CPLAYER_H
#define CPLAYER_H

#include <string>
#include "Animated.h"
#include "cmap.h"
#include "cbonus.h"
#include <list>

enum Direction
{
    Left, Right, Down, Up, Stopped
};
static const unsigned Speed = 120; /* Speed of the bomberman */
static const unsigned MaxBombs = 4; /* Maximum bombs per player */

class CPlayer : public Animated
{
public:
    CPlayer(const std::string &nick = "", const std::string &color = "white");
    void move(Direction direction, const float &ElapsedTime);

    /* Setters */
    void setDirection(Direction direction);
    void setColor(const std::string &color);
    void setNick(const std::string &nick);
    void setStopTime(const float &stopTime);
    void setCorrectPosition();

    /* Getters */
    float getStopTime() const;
    float getElapsedTime() const;
    unsigned getSpeed() const;
    const std::string &getNick() const;
    Direction getDirection() const;

    bool canMove(Direction direction, CMap &map);
    void explode();

    /* Position on the map */
    unsigned getX() const;
    unsigned getY() const;

    /* Bonus methods */
    void addBonus(CBonus *bonus);
    void removeBonus(CBonus::BonusType type);
    CBonus *getBonus(CBonus::BonusType type);
    std::list<CBonus *> getBonusList() const;
    void updateBonusTime(const float &elapsedTime);

    /* Public attributes */
    unsigned maxBombs;
    unsigned pausedBombs;
    bool gotBonus;


private:
    std::string m_color;
    std::string m_nick;
    sf::Image m_img_player;
    Anim m_player_up;
    Anim m_player_down;
    Anim m_player_left;
    Anim m_player_right;
    Anim m_player_explode;
    Direction m_direction;
    float m_elapsedTime;
    float m_stopTime;
    float m_oldStopTime;
    unsigned m_speed;
    std::list<CBonus *> m_bonusList;

};

#endif // CPLAYER_H
