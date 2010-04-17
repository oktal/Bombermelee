#ifndef CCLIENT_H
#define CCLIENT_H

#include <QTcpSocket>
#include <QString>

/**
  * function _m transforms a message into a valid message request (ending by \r\n)
  * @return message request
*/
inline char const *_m(const std::string &message)
{
    return std::string(message + "\r\n").c_str();
}

class CClient
{
public:
    CClient(QTcpSocket *socket, const QString &nick, const QString &color = "");
    QString getNick() const;
    QString getColor() const;
    QTcpSocket *getSocket() const;
    void setNick(const QString &nick);
    void setColor(const QString &color);
    void send(const std::string &what);

private:
    QTcpSocket *m_socket;
    QString m_nick;
    QString m_color;
};

#endif // CCLIENT_H
