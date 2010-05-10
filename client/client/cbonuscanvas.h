#ifndef CBONUSCANVAS_H
#define CBONUSCANVAS_H

#include <SFML/Graphics.hpp>
#include "cbonus.h"
#include <QList>

class CBonusCanvas : public sf::Sprite
{
public:
    CBonusCanvas(float time, float timeToLeave, float x, float y);
    void playNextBonus(const float &elapsedTime);
    CBonus getBonus() const;
    bool isFinished() const;
    void Reset();
    void Pause();
    void Play();
    bool isPaused() const;

private:
    QList<CBonus *> m_bonusList;
    float m_elapsedTime;
    float m_time; /* time between each bonus frame */
    float m_totalTime;
    float m_timeToLeave;
    unsigned m_currentBonus;
    bool m_finished;
    bool m_paused;

    void setBonusImage();
};

#endif // CBONUSCANVAS_H
