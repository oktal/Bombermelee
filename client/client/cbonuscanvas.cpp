#include "cbonuscanvas.h"
#include "cimagemanager.h"
#include <algorithm>

CBonusCanvas::CBonusCanvas(float time, float timeToLeave, float x, float y)
{
    m_bonusList.push_back(new CBonus(CBonus::SpeedUp));
    m_bonusList.push_back(new CBonus(CBonus::SpeedDown));
    m_bonusList.push_back(new CBonus(CBonus::BombUp));
    //m_bonusList.push_back(new CBonus(CBonus::BombDown));

    std::random_shuffle(m_bonusList.begin(), m_bonusList.end());

    m_time = time;
    m_totalTime = 0.0f;
    m_timeToLeave = timeToLeave;
    m_elapsedTime = 0.0f;
    m_currentBonus = 0;
    m_finished = false;
    m_paused = false;
    SetPosition(x, y);

}

void CBonusCanvas::setBonusImage()
{
    CImageManager *imageManager = CImageManager::GetInstance();
    switch (m_bonusList[m_currentBonus]->getType())
    {
    case CBonus::BombDown:
        break;
    case CBonus::BombUp:
        SetImage(*imageManager->GetImage("../bomb_up50.png"));
        break;
    case CBonus::SpeedDown:
        SetImage(*imageManager->GetImage("../speed_down50.png"));
        break;
    case CBonus::SpeedUp:
        SetImage(*imageManager->GetImage("../speed_up50.png"));
        break;
    case CBonus::FireDown:
        break;
    case CBonus::FireUp:
        break;
    case CBonus::FullFire:
        break;
    }
}

CBonus CBonusCanvas::getBonus() const
{
    return *m_bonusList[m_currentBonus];
}

bool CBonusCanvas::isFinished() const
{
    return m_finished;
}

bool CBonusCanvas::isPaused() const
{
    return m_paused;
}

void CBonusCanvas::playNextBonus(const float &elapsedTime)
{
    if (m_paused)
    {
        return;
    }

    m_elapsedTime += elapsedTime;
    m_totalTime += elapsedTime;
    if (m_totalTime >= m_timeToLeave)
    {
        m_finished = true;
    }
    else if (m_elapsedTime >= m_time)
    {
        m_elapsedTime = 0.0;
        ++m_currentBonus;
        if (m_currentBonus == static_cast<unsigned>(m_bonusList.size()))
        {
            m_currentBonus = 0;
        }
        setBonusImage();
    }
}

void CBonusCanvas::Play()
{
    m_paused = false;
}

void CBonusCanvas::Pause()
{
    m_paused = true;
}

void CBonusCanvas::Reset()
{
    std::random_shuffle(m_bonusList.begin(), m_bonusList.end());
    m_totalTime = 0.0f;
    m_elapsedTime = 0.0f;
    m_currentBonus = 0;
    m_finished = false;
    m_paused = false;
}


