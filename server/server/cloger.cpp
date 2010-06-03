#include "cloger.h"
#include "cnetworkmanager.h"
#include <fstream>
#include <iostream>
#include <QDataStream>
#include <QTextStream>

CLoger::CLoger(const std::string &fileName) :
        m_fileName(fileName)
{
    m_out.setFileName(QString(fileName.c_str()));
    m_out.open(QIODevice::WriteOnly | QIODevice::Truncate);
}

CLoger::~CLoger()
{
    m_out.close();
}

void CLoger::setFileName(const std::string &fileName)
{
    m_fileName = fileName;
    m_out.setFileName(QString(fileName.c_str()));
}

std::string CLoger::getFileName() const
{
    return m_fileName;
}

void CLoger::logPacket(QByteArray buffer)
{
    quint32 packet;
    QDataStream in(&buffer, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    quint32 blockSize;
    quint8 direction;
    QString sender, nick;
    float f_x, f_y;
    unsigned u_x, u_y;
    in >> blockSize;
    in >> packet;

    QTextStream out(&m_out);


    switch (static_cast<CNetworkManager::PacketType>(packet))
    {
    case CNetworkManager::Ehlo:
        out << "Received Ehlo Packet";
        out << "\r\n";
        break;
    case CNetworkManager::Nick:
        in >> nick;
        out << "Received Nick Packet: " << nick;
        out << "\r\n";
        break;
    case CNetworkManager::Move:
        in >> sender;
        in >> direction;
        in >> f_x;
        in >> f_y;
        out << "Received Move Packet from " << sender
                << ": " << f_x << " " << f_y;
        out << "\r\n";
        break;
    case CNetworkManager::Bomb:
        in >> sender;
        in >> u_x;
        in >> u_y;
        out << "Received Bomb Packet from " << sender
                << ": " << u_x << " " << u_y;
        out << "\r\n";
        break;
    case CNetworkManager::Boom:
        in >> sender;
        in >> u_x;
        in >> u_y;
        out << "Received Boom Packet from " << sender
                << ": " << u_x << " " << u_y;
        out << "\r\n";
        break;
    case CNetworkManager::Bonus:
        in >> sender;
        out << "Received Bonus Packet from " << sender;
        out << "\r\n";
        break;
    default:
        break;
    }
}


void CLoger::logInvalidPacket(QByteArray buffer)
{
    quint32 packet;
    QDataStream in(&buffer, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    /*
    QString sender;
    if (!(in >> packet >> sender))
    {
        out << "Invalid Packet Received\r\n";
    }
    else
    {
        out << "Invalid Packet Received from " << sender;
        out << "\r\n";
    }
    */
}
