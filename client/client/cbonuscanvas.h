#ifndef CBONUSCANVAS_H
#define CBONUSCANVAS_H

#include <SFML/Graphics.hpp>
#include "cbonus.h"
#include <QMap>
#include <QPair>

class CBonusCanvas : public sf::Sprite
{
public:
    CBonusCanvas(float time, float timeToLeave, sf::Rect<int> position);
    void playNextBonus(const float &elapsedTime);
    CBonus *getBonus() const;
    bool isFinished() const;
    void Reset();
    void Pause();
    void Play();
    bool isPaused() const;
    sf::Rect<int> getCanvasPosition() const;

private:
    QMap<CBonus *, QPair<int, int> > m_bonusList;
    float m_elapsedTime;
    float m_time; /* time between each bonus frame */
    float m_totalTime;
    float m_timeToLeave;
    unsigned m_currentBonus;
    unsigned m_lastRandom;
    bool m_finished;
    bool m_paused;
    sf::Rect<int> m_position;

    void setBonusImage();
};

#endif // CBONUSCANVAS_H
