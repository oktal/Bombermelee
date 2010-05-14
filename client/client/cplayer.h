#ifndef CPLAYER_H
#define CPLAYER_H

#include <string>
#include "Animated.h"
#include "cmap.h"
#include "cbonus.h"
#include <list>

inline bool collision(const sf::Sprite& s1,const sf::Sprite& s2, const int AlphaMax = -1)
{
    sf::Vector2f pos = s1.GetPosition() - s1.GetCenter();
    const sf::FloatRect r1(pos.x, pos.y, pos.x + s1.GetSize().x, pos.y + s1.GetSize().y);
    pos = s2.GetPosition() - s2.GetCenter();
    const sf::FloatRect r2(pos.x, pos.y, pos.x + s2.GetSize().x, pos.y + s2.GetSize().y);

    // Espace de collision potentiel.
    sf::FloatRect zone;

    // Testons si les Sprites se superposent.
    if (r1.Intersects(r2, &zone))
    {
        if (AlphaMax >= 0)
        {
            int left1 = static_cast<int>(zone.Left - r1.Left);
            int top1 = static_cast<int>(zone.Top - r1.Top);
            int left2 = static_cast<int>(zone.Left - r2.Left);
            int top2 = static_cast<int>(zone.Top - r2.Top);
            int width = static_cast<int>(zone.GetWidth());
            int height = static_cast<int>(zone.GetHeight());

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    if (s1.GetPixel(x+left1, y+top1).a > AlphaMax && s2.GetPixel(x+left2, y+top2).a > AlphaMax)
                        return true;
                }
            }
            return false;
        }
        //qDebug() << "Collision";
        return true;
    }
    return false;
}

enum Direction
{
    Left, Right, Down, Up, Stopped
};
static const unsigned Speed = 120; /* Speed of the bomberman */
static const unsigned MaxBombs = 4; /* Maximum bombs per player */
static const unsigned MaxRange = 6; /* Max fire range per player */

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
    void setKiller(const std::string &killer);

    /* Getters */
    float getStopTime() const;
    float getElapsedTime() const;
    unsigned getSpeed() const;
    const std::string &getNick() const;
    Direction getDirection() const;

    bool canMove(Direction direction, CMap &map);
    void explode();
    bool isDead() const;

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
    unsigned bombRange;
    bool gotBonus;


private:
    std::string m_color;
    std::string m_nick;
    std::string m_killer;
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
    bool m_dead;
    std::list<CBonus *> m_bonusList;

};

#endif // CPLAYER_H
