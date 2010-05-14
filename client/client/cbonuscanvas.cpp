#include "cbonuscanvas.h"
#include "cimagemanager.h"
#include <algorithm>
#include <ctime>

CBonusCanvas::CBonusCanvas(float time, float timeToLeave, sf::Rect<int> position)
{
    m_bonusList.push_back(new CBonus(CBonus::BombUp));
    m_bonusList.push_back(new CBonus(CBonus::FireUp));
    m_bonusList.push_back(new CBonus(CBonus::SpeedUp));
    m_bonusList.push_back(new CBonus(CBonus::SpeedDown));
    m_bonusList.push_back(new CBonus(CBonus::BombDown));
    m_bonusList.push_back(new CBonus(CBonus::FireDown));
    m_bonusList.push_back(new CBonus(CBonus::RemoteMine));

    //std::random_shuffle(m_bonusList.begin(), m_bonusList.end());

    m_time = time;
    m_totalTime = 0.0f;
    m_timeToLeave = timeToLeave;
    m_elapsedTime = 0.0f;
    m_currentBonus = 0;
    m_finished = false;
    m_paused = false;
    SetPosition(position.Left, position.Top);
    m_position = position;
}

void CBonusCanvas::setBonusImage()
{
    CImageManager *imageManager = CImageManager::GetInstance();
    switch (m_bonusList[m_currentBonus]->getType())
    {
    case CBonus::BombDown:
        SetImage(*imageManager->GetImage("../bomb_down70.png"));
        break;
    case CBonus::BombUp:
        SetImage(*imageManager->GetImage("../bomb_up70.png"));
        break;
    case CBonus::SpeedDown:
        SetImage(*imageManager->GetImage("../speed_down70.png"));
        break;
    case CBonus::SpeedUp:
        SetImage(*imageManager->GetImage("../speed_up70.png"));
        break;
    case CBonus::FireDown:
        SetImage(*imageManager->GetImage("../fire_down70.png"));
        break;
    case CBonus::FireUp:
        SetImage(*imageManager->GetImage("../fire_up70.png"));
        break;
    case CBonus::FullFire:
        break;
    case CBonus::RemoteMine:
        SetImage(*imageManager->GetImage("../remote_mine70.png"));
        break;
    }
}

CBonus CBonusCanvas::getBonus() const
{
    return *m_bonusList[m_currentBonus];
}

sf::Rect<int> CBonusCanvas::getCanvasPosition() const
{
    return m_position;
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
        unsigned bonus;
        do
        {
            bonus = sf::Randomizer::Random(0, m_bonusList.size() - 1);
        } while (bonus == m_currentBonus);
        m_currentBonus = bonus;
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


