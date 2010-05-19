#ifndef CCLIENT_H
#define CCLIENT_H

#include <QWidget>
#include <QtGui>
#include <QTcpSocket>
#include "cgameboard.h"
#include "cnetworkmanager.h"

static const float PingSendingTime = 500; /* Each 500ms we send a Ping Packet */

class CClient : public QWidget
{
    Q_OBJECT
public:
    CClient(QWidget *parent, const QString &address, const QString &nick);

private:
    QTextEdit *m_chatBox;
    QLineEdit *m_message;
    QPushButton *m_btn_send;
    QListWidget *m_lst_users;
    CGameBoard *m_gameBoard;

    QTcpSocket *m_socket;
    QByteArray m_buffer;
    QString m_address;
    QString m_nick;

    QList<QString> m_usersList;
    QTimer *m_pingTimer;
    QTime m_pingTime;

    CNetworkManager *m_networkManager;

    void connectToServer();
    void processData();
    void appendToChatBox(const QString &text);
    void appendToUsersList(const QString &nick, const QString &color);
    bool eventFilter(QObject *obj, QEvent *ev);

private slots:
    void onConnected();
    void onError();
    void processReadyRead();
    void sendMessage();
    void sendPing();

};

#endif // CCLIENT_H
