#ifndef CSERVER_H
#define CSERVER_H

#include <QMainWindow>
#include <QtGui>
#include <QTcpServer>
#include <QHash>
#include "cclient.h"

class CServer : public QMainWindow {
    Q_OBJECT
public:
    CServer(QWidget *parent = 0);
    ~CServer();

    enum DataMessageType
    {
        Undefined,
        Ehlo, /* EHLO on connect */
        Nick, /* NICK */
        Badnick,
        Users, /* USERS Request */
        Say,
        Map,
        Move, /* Player MOVE */
        Bomb, /* Player plants a BOMB */
        Bonus, /* Player got a BONUS */
        Boom, /* Player used remote mine bonus */
        /* For future */
        Pong /* Pong Answer */
    };

protected:
    void changeEvent(QEvent *e);

private slots:
    void onConnect();
    void onDisconnect();
    void sendMapToClients();
    void processReadyRead();

private:
    QTextEdit *m_console;
    QPushButton *m_btn_launch;
    QPushButton *m_btn_pause;
    QLineEdit *m_message;
    QPushButton *m_btn_send;

    QTcpServer *m_server;
    QByteArray m_buffer;
    QList<CClient *> m_clientsList;
    QList<QString> m_colors;
    DataMessageType messageType;

    void startServer();
    QHostAddress getLocalIpAddress();
    bool nickAlreadyInUse(const QString &nick);
    void appendToConsole(const QString &text);
    void processData(QTcpSocket *sender);
    void broadcast(const QByteArray &data, QTcpSocket *except = 0);
    CClient *getClientFromNick(const QString &nick);
};

#endif // CDIALOG_H
