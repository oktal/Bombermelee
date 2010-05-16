#include "cbonuscanvas.h"
#include "cimagemanager.h"
#include <algorithm>
#include <ctime>
#include <QPair>
#include <QtDebug>

static inline bool is_in(int n, QPair<int, int> pair)
{
    return n >= pair.first && n <= pair.second;
}

CBonusCanvas::CBonusCanvas(float time, float timeToLeave, sf::Rect<int> position)
{
    m_bonusList.insert(new CBonus(CBonus::BombUp), qMakePair(0, 5));
    m_bonusList.insert(new CBonus(CBonus::FireUp), qMakePair(6, 9));
    m_bonusList.insert(new CBonus(CBonus::SpeedUp), qMakePair(10, 13));

    m_bonusList.insert(new CBonus(CBonus::SpeedDown), qMakePair(14, 15));
    m_bonusList.insert(new CBonus(CBonus::BombDown), qMakePair(16, 17));
    m_bonusList.insert(new CBonus(CBonus::FireDown), qMakePair(18, 19));

    m_bonusList.insert(new CBonus(CBonus::BombPass), qMakePair(20, 21));
    m_bonusList.insert(new CBonus(CBonus::FullFire), qMakePair(22, 22));
    m_bonusList.insert(new CBonus(CBonus::RemoteMine), qMakePair(23, 23));
    m_bonusList.insert(new CBonus(CBonus::BombKick), qMakePair(24, 25));

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
    switch (m_bonusList.keys()[m_currentBonus]->getType())
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
        SetImage(*imageManager->GetImage("../full_fire70.png"));
        break;
    case CBonus::RemoteMine:
        SetImage(*imageManager->GetImage("../remote_mine70.png"));
        break;
    case CBonus::BombPass:
        SetImage(*imageManager->GetImage("../bomb_pass70.png"));
        break;
    case CBonus::BombKick:
        SetImage(*imageManager->GetImage("../bomb_kick70.png"));
        break;
    }
}

CBonus *CBonusCanvas::getBonus() const
{
    return m_bonusList.keys()[m_currentBonus];
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
        unsigned random;
        unsigned const maxRandom = m_bonusList.values()[m_bonusList.size() - 1].second;
        do
        {
            random = sf::Randomizer::Random(0, maxRandom);
        } while (random == m_lastRandom);
        m_lastRandom = random;
        for (int i = 0; i < m_bonusList.size(); ++i)
        {
            if (is_in(random, m_bonusList.values()[i]))
            {
                m_currentBonus = i;
                break;
            }
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
    m_totalTime = 0.0f;
    m_elapsedTime = 0.0f;
    m_currentBonus = 0;
    m_finished = false;
    m_paused = false;
}


