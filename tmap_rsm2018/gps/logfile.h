#ifndef LOGFILE_H
#define LOGFILE_H

#include <QObject>
#include <QString>
#include <QMutex>

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace  SKT {

enum eLogFileType
{
    eLogFileType_None = 0,
    eLogFileType_GPS,
    eLogFileType_MM,
    eLogFileType_MMFB
};

class LogFile
{
public:
    LogFile(eLogFileType type, QString logPath);
    ~LogFile();
	
    void WriteString(QString strName, QString strLog);

private:
    void CheckFile();
    QString GetLogFileName();


private:
    QMutex m_logMutex;
    eLogFileType m_eLogFileType;
    QString m_logPath;
    QStringList m_fileList;
    int m_fileIdx;
    int64_t m_fileSize;
    int m_lineCount;
    std::ofstream m_outf;

};

}

#endif // LOGFILE_H
