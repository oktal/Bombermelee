#ifndef CLOGER_H
#define CLOGER_H

#include <string>
#include <QByteArray>
#include <QFile>

/**
  * @class CLoger
    Log packets into a file
*/

class CLoger
{
public:
    CLoger(const std::string &fileName = "");
    ~CLoger();
    /**
      * Log a packet
    */
    void logPacket(QByteArray buffer);
    /**
      * Log an invalid packet
    */
    void logInvalidPacket(QByteArray buffer);
    std::string getFileName() const;
    void setFileName(const std::string &fileName);

private:
    std::string m_fileName;
    QFile m_out;
};

#endif // CLOGER_H
