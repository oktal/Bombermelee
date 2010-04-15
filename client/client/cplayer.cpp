#include <QtGui>
#include "cplayer.h"

static const int Speed = 100;

CPlayer::CPlayer(std::string color) :
        m_color(color)
{
    if (!m_img_player.LoadFromFile("../perso2.png"))
    {
        return;
    }
    m_img_player.CreateMaskFromColor(sf::Color(255, 255, 255));

    m_player_up.PushFrame(Frame(&m_img_player, sf::Rect<int>(2, 32, 18, 55)));
    m_player_up.PushFrame(Frame(&m_img_player, sf::Rect<int>(23, 32, 39, 55)));
    m_player_up.PushFrame(Frame(&m_img_player, sf::Rect<int>(44, 32, 60, 55)));

    m_player_down.PushFrame(Frame(&m_img_player, sf::Rect<int>(2, 2, 18, 25)));
    m_player_down.PushFrame(Frame(&m_img_player, sf::Rect<int>(23, 2, 39, 25)));
    m_player_down.PushFrame(Frame(&m_img_player, sf::Rect<int>(44, 2, 60, 25)));

    m_player_left.PushFrame(Frame(&m_img_player, sf::Rect<int>(65, 2, 84, 25)));
    m_player_left.PushFrame(Frame(&m_img_player, sf::Rect<int>(89, 2, 105, 25)));
    m_player_left.PushFrame(Frame(&m_img_player, sf::Rect<int>(110, 2, 126, 25)));

    m_player_right.PushFrame(Frame(&m_img_player, sf::Rect<int>(66, 32, 82, 55)));
    m_player_right.PushFrame(Frame(&m_img_player, sf::Rect<int>(89, 32, 105, 55)));
    m_player_right.PushFrame(Frame(&m_img_player, sf::Rect<int>(110, 32, 126, 55)));

    m_player_explode.PushFrame(Frame(&m_img_player, sf::Rect<int>(155, 2, 176, 25)));
    m_player_explode.PushFrame(Frame(&m_img_player, sf::Rect<int>(182, 2, 204, 25)));
    m_player_explode.PushFrame(Frame(&m_img_player, sf::Rect<int>(209, 2, 232, 25)));

    SetAnim(&m_player_right);
    SetFrameTime(0.2f);
}

void CPlayer::move(Movement move, const float &ElapsedTime)
{
    switch (move)
    {
    case Right:
            if (GetAnim() != &m_player_right)
            {
                SetAnim(&m_player_right);
            }
            Move(Speed * ElapsedTime, 0);
            break;
    case Left:
            if (GetAnim() != &m_player_left)
            {
                SetAnim(&m_player_left);
            }
            Move(-Speed * ElapsedTime, 0);
            break;
    case Up:
            if (GetAnim() != &m_player_up)
            {
                SetAnim(&m_player_up);
            }
            Move(0, -Speed * ElapsedTime);
            break;
    case Down:
            if (GetAnim() != &m_player_down)
            {
                SetAnim(&m_player_down);
            }
            Move(0, Speed * ElapsedTime);
            break;
    }
    if (IsPaused())
    {
        Play();
    }
    anim(ElapsedTime);
}

void CPlayer::explode(const float &ElapsedTime)
{
    if (GetAnim() != &m_player_explode)
    {
        SetAnim(&m_player_explode);
    }
    anim(ElapsedTime);
}

