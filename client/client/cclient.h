#ifndef CCLIENT_H
#define CCLIENT_H

#include <QWidget>
#include <QtGui>
#include <QTcpSocket>
#include "cgameboard.h"

class CClient : public QWidget
{
    Q_OBJECT
public:
    CClient(QWidget *parent, const QString &address, const QString &nick);
    enum DataMessageType
    {
        Undefined,
        Full,
        Ehlo,
        Badnick, /* BADNICK */
        Ok,
        Join,
        Part,
        Users,
        Say,
        Move,
        Map,
        Boom, /* A bomb explode */
        Bomb,
        Bonus,
        /* For future */
        Ping /* Ping Request */
    };

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

    DataMessageType messageType;

    void connectToServer() const;
    void readProtocolHeader();
    void processData();
    void appendToChatBox(const QString &text);
    void appendToUsersList(const QString &nick, const QString &color);
    bool eventFilter(QObject *obj, QEvent *ev);

private slots:
    void onConnected();
    void onError();
    void processReadyRead();
    void sendMessage();


};

#endif // CCLIENT_H
