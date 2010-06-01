#ifndef CSERVER_H
#define CSERVER_H

#include <QMainWindow>
#include <QtGui>
#include <QTcpServer>
#include <QHash>
#include "cclient.h"
#include "cloger.h"

class CServer : public QMainWindow {
    Q_OBJECT
public:
    CServer(QWidget *parent = 0);
    ~CServer();

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
    QSpinBox *m_spn_round;

    QTcpServer *m_server;
    QByteArray m_buffer;
    QList<CClient *> m_clientsList;
    QList<QString> m_colors;
    CLoger *m_loger;

    void startServer();
    QHostAddress getLocalIpAddress();
    bool nickAlreadyInUse(const QString &nick);
    void appendToConsole(const QString &text);
    void readData(QTcpSocket *sender);
    void processData(QTcpSocket *sender, QByteArray buffer);
    void broadcast(const QByteArray &data, QTcpSocket *except = 0);
    CClient *getClientFromNick(const QString &nick);
};

#endif // CDIALOG_H
