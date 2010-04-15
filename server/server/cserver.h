#ifndef CSERVER_H
#define CSERVER_H

#include <QMainWindow>
#include <QtGui>
#include <QTcpServer>
#include <QHash>

class CServer : public QMainWindow {
    Q_OBJECT
public:
    CServer(QWidget *parent = 0);
    ~CServer();

    enum DataMessageType
    {
        Undefined,
        PlainDataText,
        Ehlo, /* EHLO on connect */
        Nick, /* NICK */
        Users, /* USERS Request */
        Say,
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

    QTcpServer *m_server;
    QByteArray m_buffer;
    QHash<QString, QTcpSocket *> m_clientsList;
    DataMessageType messageType;

    void startServer();
    QHostAddress getLocalIpAddress();
    bool nickAlreadyInUse(const QString &nick);
    void appendToConsole(const QString &text);
    void readProtocolHeader();
    void processData(QTcpSocket *sender);
    void broadcast(const QString &message, QTcpSocket *except);
    void send(QTcpSocket *to, const QString &what);
};

#endif // CDIALOG_H
