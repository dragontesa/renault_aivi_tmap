#include <string>
#include <QDebug>
#include <stdio.h>
#include "crashlogger.h"
#include "zlib.h"

namespace SKT {

#define LATEST_CRASH_COUNT  5       // 최근에 발생한 크래시 정보가 밑에 붙는다는 가정 하에, 밑에서부터 5개의 크래시 정보를 추출
//#define COMPRESS_CRASH_LOG        // Crash Log 압축 여부

#if 1
#define INPUT_FILEPATH "/var/opt/bosch/navdata/log/app_log/crash_full.log"
#define OUTPUT_FILEPATH "/var/opt/bosch/navdata/log/app_log/crash.log"
#else
#define INPUT_FILEPATH "/home/hheo/skmns/3sources/CrashLogger/crash_full.log"
#define OUTPUT_FILEPATH "/home/hheo/skmns/3sources/CrashLogger/crash.log"
#endif

#define CRASH_DUMP_BEGIN1 "EXH ****** EXCEPTION in process PID="
//#define CRASH_DUMP_BEGIN2 "(QThread) *******"                     // BEGIN2는 일단 체크하지 않는 것으로 정함
#define CRASH_DUMP_END "EXH ***** end of exception log *****"

//#define VALID_CHECK_STRING "EXH Command line: /opt/bosch/extnav/bin/thirdpartynavigation_out.out any"    // 이 문자열이 없으면 유효하지 않은 것으로 판단. (추후 정책에 따라 문자열 내용이 바뀔 가능성 있음)      // VALID_CHECK_STRING도 일단 체크하지 않는 것으로 정함


bool CrashLogger::run() {
    bool ret = true;

    QFile inputFile(INPUT_FILEPATH);
    ret = inputFile.open(QIODevice::ReadOnly);
    qDebug() << "CrashLogger::run() 1" << ret;
    if (ret == false)
        return false;

    QString log;
    ret = parseCrashDump(inputFile, log);
    qDebug() << "CrashLogger::run() 2" << ret;
    inputFile.close();
    if (ret == false)
        return false;

    QFile outputFile(OUTPUT_FILEPATH);
//    ret = outputFile.remove();
//    qDebug() << "old crash log file remove::" << ret;
    ret = saveCrashLog(outputFile, log);
    qDebug() << "CrashLogger::run() 3" << ret;
    outputFile.close();
    if (ret == false)
        return false;
}

bool CrashLogger::parseCrashDump(QFile &file, QString &log) {
    bool ret = true;
    QString line;
    int start = -1;     // crash dump 시작 지점
    int length = 0;     // crash dump 길이 (끝 지점 - 시작 지점 + 끝 문자열 길이)
    MAP_INDEX map;      // 추출한 crash dump의 인덱스와 길이르 key/value 쌍으로 저장

    int indexAdd = 0;   // 파일 seek index
    while (file.atEnd() == false) {
        line = file.readLine();

//        if ((line.indexOf(CRASH_DUMP_BEGIN1) != -1) && (line.indexOf(CRASH_DUMP_BEGIN2) != -1)) {     // BEGIN2는 일단 체크하지 않는 것으로 정함
        if (line.indexOf(CRASH_DUMP_BEGIN1) != -1) {
            if (start == -1)
                start = indexAdd;
        }

        if (line.indexOf(CRASH_DUMP_END) != -1) {
            if (start != -1 && length == 0) // BEGIN을 찾지 못한 채로 END가 먼저 찾아지는 경우는 무시
            {
                length = indexAdd - start + line.length();
                map[start] = length;
            }

            start = -1;
            length = 0;
        }

        indexAdd += line.length();
    }

// for debug...
//    for (MAP_INDEX::iterator itr = map.begin(); itr != map.end(); itr++) {
//        qDebug() << QString("[%1]").arg(itr.key()) << itr.value();
//    }


    // 검출된 crash log가 한 개도 없으면 false 리턴
    qDebug() << "CrashLogger::parseCrashDump() 1" << "   map size =" << map.size();
    if (map.size() <= 0) {
        qDebug() << "Exception:: crash log not found.";
        return false;
    }


    QStringList logs;   // map의 가장 마지막 정보부터 LATEST_CRASH_COUNT 만큼만 추출해서 저장

    int count = 0;
    for (MAP_INDEX::const_iterator citr = map.end() - 1; citr != map.begin() - 1 && count < LATEST_CRASH_COUNT; citr--) {
        file.seek(citr.key());
        QString value = file.read(citr.value());

//        if (value.indexOf(VALID_CHECK_STRING) == -1)      // VALID_CHECK_STRING도 일단 체크하지 않는 것으로 정함
//            continue;

        qDebug() << QString("[%1]").arg(citr.key()) << citr.value();
        logs.append(value);
        count++;
    }

    log = logs.join("\n\n");
    qDebug() << "CrashLogger::parseCrashDump() 2";
    return true;
}

bool CrashLogger::saveCrashLog(QFile &file, const QString &log) {
    bool ret = true;

#ifdef COMPRESS_CRASH_LOG
    // compress log data
    QByteArray compressed = qCompress(log.toLocal8Bit());
    qDebug() << "CrashLogger::saveCrashLog() 1   compressed size =" << compressed.size();
    if (compressed.size() <= 0)
        return false;

    // create log file
    QFile outputFile(OUTPUT_FILEPATH);
    ret = file.open(QIODevice::WriteOnly);
    qDebug() << "CrashLogger::saveCrashLog() 2" << ret;
    if (ret == false) {
        return false;
    }

    // write log data to file
    qint64 writtenSize = file.write(compressed);
    qDebug() << "CrashLogger::saveCrashLog() 3" << (writtenSize == compressed.size()) << "   written =" << writtenSize << "   compressed =" << compressed.size();
    if (writtenSize != compressed.size()) {
        return false;
    }
#else // COMPRESS_CRASH_LOG
    // create log file
    QFile outputFile(OUTPUT_FILEPATH);
    ret = file.open(QIODevice::WriteOnly);
    qDebug() << "CrashLogger::saveCrashLog() 1" << ret;
    if (ret == false) {
        return false;
    }

    // write log data to file
    qint64 writtenSize = file.write(log.toLocal8Bit());
    qDebug() << "CrashLogger::saveCrashLog() 2" << (writtenSize == log.toLocal8Bit().size()) << "   written =" << writtenSize << "   log size=" << log.toLocal8Bit().size();
    if (writtenSize != log.toLocal8Bit().size()) {
        return false;
    }
#endif // COMPRESS_CRASH_LOG

    file.flush();

    return true;
}

}

/*

appKey          Y       ndds - AuthCode??

projectName     Y       ndds - ModelNo??
userKey         N
crashTime       Y
deviceId        N       ndds - DeviceID
deviceModel     N       ndds - DeviceModelName?? DeviceModelNumber??
VIN             N       ndds - SerialKey??
osName          N       ndds - OSType??
osVersion       N
fmVersion       N
fmMidVersion	N
naviVersion     N       ndds - AppVersion
offlineMapVer	N
offlineMapConf	N
onlineMapVer	N
onlineMapConf	N
gpsInfo         N
networkYN       N
wifiBT          N
logFile         Y

mappingKey      Y

*/
