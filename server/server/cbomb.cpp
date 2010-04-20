#include "cbomb.h"

CBomb::CBomb()
{

}

CBomb::CBomb(const QString &bomber, unsigned x, unsigned y)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(ExplodeTime);
    m_timer->start();
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(bombExplode()));

    this->x = x;
    this->y = y;
    this->bomber = bomber;
}


void CBomb::bombExplode()
{
    emit explode();
}
