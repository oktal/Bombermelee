#ifndef CLOGER_H
#define CLOGER_H

#include <string>
#include <QByteArray>
#include <QFile>

class CLoger
{
public:
    CLoger(const std::string &fileName = "");
    ~CLoger();
    void logPacket(const QByteArray buffer);
    std::string getFileName() const;
    void setFileName(const std::string &fileName);

private:
    std::string m_fileName;
    QFile m_out;
};

#endif // CLOGER_H
