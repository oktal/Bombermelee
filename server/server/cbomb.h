#ifndef CBOMB_H
#define CBOMB_H

#include <QObject>
#include <QTimer>

const unsigned ExplodeTime = 5000; /* 5 seconds */
const unsigned ExplodeTimeRemote = 10000;

class CBomb : public QObject
{
    Q_OBJECT

public:
    CBomb();
    CBomb(const QString &bomber, unsigned x, unsigned y, unsigned explodeTime = ExplodeTime);
    QString bomber;
    unsigned x;
    unsigned y;
    void stopTimer();

signals:
    void explode();

private slots:
    void bombExplode();

private:
    QTimer *m_timer;

};

#endif // CBOMB_H
