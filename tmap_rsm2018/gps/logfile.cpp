#include <QDebug>
#include <QByteArray>
#include <QBuffer>
#include <QDir>
#include <stdint.h>
#include <stdio.h>
#include <iomanip>
#include <QTimeZone>

#include "logfile.h"

#include "datetimeutil.h"

using namespace std;

namespace SKT {

static char g_LogTime[14] = {0,}; // _MM-dd-hh-mm
static int g_LogFileIdx = -1;

LogFile::LogFile(eLogFileType type, QString logPath)
{
    m_eLogFileType = type;
    m_logPath = logPath;
    m_fileIdx = 0;
    m_fileSize = 0;
    m_lineCount = 0;

    CheckFile();
}

LogFile::~LogFile()
{
    if(m_outf.is_open())
        m_outf.close();
}

void LogFile::CheckFile()
{
    QDir dir(m_logPath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    switch (m_eLogFileType) {
    case eLogFileType_GPS:
        dir.setNameFilters(QStringList() << "position*.dat");
        break;
    case eLogFileType_MM:
        dir.setNameFilters(QStringList() << "mapmatch*.dat");
        break;
    case eLogFileType_MMFB:
        dir.setNameFilters(QStringList() << "sendToMMFB*.dat");
        break;
    default:
        return;
        break;
    }

    QStringList fileList = dir.entryList();
    for (int i = 0; i < fileList.size(); ++i)
    {
        QFileInfo fileInfo = fileList.at(i);
        QString strFilePath = m_logPath + "/" + fileInfo.fileName();

        if(fileInfo.fileName().split("_").length() != 3)
        {
            QFile::remove(strFilePath);
            qDebug() << "remove file  : " << strFilePath;
            continue;
        }
        m_fileList.push_back(strFilePath);
    }

    // 로그파일은 최대 5개만 유지한다.
    while(m_fileList.size() > 5)
    {
        QString srtFilePath = m_fileList.front();
        QFile::remove(srtFilePath);
        qDebug() << "remove file  : " << srtFilePath;

        m_fileList.pop_front();
    }

    // 마지막 파일의 정보를 유지 한다.
    if(m_fileList.size() > 0)
    {
        QString srtFilePath = m_fileList.back();
        fileList = srtFilePath.split("/");
        QString strName = fileList.back();
        QStringList listSplit = strName.split("_");

        m_fileIdx = listSplit[1].split(".")[0].toInt();
        m_outf.open(srtFilePath.toStdString() ,std::ios_base::out | std::ios_base::app);
        m_fileSize = m_outf.tellp();

        if(m_eLogFileType == eLogFileType_GPS)
        {
            g_LogFileIdx = m_fileIdx;
            if (listSplit.count() > 2) //2019.8.1 SongDeukkeun to prevent crash temporary
                strcpy(g_LogTime, listSplit[2].split(".")[0].toStdString().c_str());
            else {
                strcpy(g_LogTime, "00-00-00-00");
            }
            qDebug() << g_LogFileIdx << ", " << g_LogTime;
        }
    }
}

void LogFile::WriteString(QString strName, QString strLog)
{
    struct timespec rtime;
    clock_gettime(CLOCK_MONOTONIC, &rtime);
    ostringstream ossRTTime;
    ossRTTime << setfill('0') << setw(5) << rtime.tv_sec << "."  // 5 digits for 24 hours
        << setw(6) << (int)(rtime.tv_nsec / 1000);  // [ns] => [ms]

    m_logMutex.lock();

    if(m_eLogFileType == eLogFileType_GPS)
    {
//        if(m_fileSize > 2000)
        if(m_fileSize > 20000000)
        {
            if(m_outf.is_open())
                m_outf.close();
            m_fileIdx++;
            m_fileSize = 0;
        }
    }else
    {
        if(m_fileIdx != g_LogFileIdx && g_LogFileIdx >= 0)
        {
            if(m_outf.is_open())
                m_outf.close();
            m_fileIdx = g_LogFileIdx;
            m_fileSize = 0;
        }
    }

    if(!m_outf.is_open())
    {
        QString logName;
        logName = GetLogFileName();

        if(!QFile::exists(logName))
        {
            m_fileList.push_back(logName);

            if(m_fileList.size() > 5)
            {
                QString srtFilePath = m_fileList.front();
                QFile::remove(srtFilePath);
                qDebug() << "remove file  : " << srtFilePath;
                m_fileList.pop_front();
            }
        }
        m_outf.open(logName.toStdString() ,std::ios_base::out | std::ios_base::app);

        if(m_eLogFileType == eLogFileType_GPS)
        {
            QStringList fileList = logName.split("/");
            QString strName = fileList.back();
            QStringList listSplit = strName.split("_");
            g_LogFileIdx = m_fileIdx;
            if (listSplit.count() > 2) //2019.8.1 SongDeukkeun to prevent crash temporary
                strcpy(g_LogTime, listSplit[2].split(".")[0].toStdString().c_str());
            else {
                strcpy(g_LogTime, "00-00-00-00");
            }
            qDebug() << g_LogFileIdx << ", " << g_LogTime;
        }
    }

    m_outf << strName.toStdString() << ","
           << ossRTTime.str()       << ","
           << strLog.toStdString()  << std::endl;

    m_fileSize = m_outf.tellp();
    m_lineCount++;

    m_logMutex.unlock();
}


QString LogFile::GetLogFileName()
{
     QString logName;

     QDateTime time = DateTimeUtil::currentUtcDateTime();

     switch (m_eLogFileType) {
     case eLogFileType_GPS:
         logName = m_logPath + "position_" + QString("%1").arg(m_fileIdx, 5, 10, QChar('0')) + time.toString("_MM-dd-hh-mm") + ".dat";
         break;
     case eLogFileType_MM:
         logName = m_logPath + "mapmatch_" + QString("%1").arg(m_fileIdx, 5, 10, QChar('0')) + "_" + g_LogTime + ".dat";
         break;
     case eLogFileType_MMFB:
         logName = m_logPath + "sendToMMFB_" + QString("%1").arg(m_fileIdx, 5, 10, QChar('0')) + "_" + g_LogTime + ".dat";
         break;
     default:
         return logName;
         break;
     }

    return logName;
}


} //SKT
