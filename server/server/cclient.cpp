#include "cclient.h"

CClient::CClient(QTcpSocket *socket, const QString &nick, const QString &color) :
        m_socket(socket), m_nick(nick), m_color(color)
{

}

QString CClient::getColor() const
{
    return m_color;
}

QString CClient::getNick() const
{
    return m_nick;
}

QTcpSocket *CClient::getSocket() const
{
    return m_socket;
}

void CClient::setColor(const QString &color)
{
    m_color = color;
}

void CClient::setNick(const QString &nick)
{
    m_nick = nick;
}

void CClient::send(const std::string &what)
{
    const qint64 len = what.length() + 2;
    qint64 bytesWritten;
    do
    {
        bytesWritten = m_socket->write(_m(what));
        m_socket->waitForBytesWritten();
    } while (bytesWritten != len);
}
