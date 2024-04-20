#include "nmeadata.h"

#define NMEADATA_RESERVE_SATELLITE      64
#define NMEADATA_CHECK_CHECKSUM

//#define NMEADATA_SUPPORT_BD
//#define NMEADATA_SUPPORT_GA
//#define NMEADATA_SUPPORT_QZ

namespace SKT {

/**
 * @brief parseNmeaInt
 * @param data
 * @param size
 * @param def
 * @return
 */
static int parseNmeaInt(const char* data, int size, int def = -1)
{
    int v = 0;

    if (size == 0) {
        return def;
    }

    for (int i = 0; i < size; i++) {
        char ch = data[i];
        if (ch < '0' || ch > '9') {
            return def;
        }
        v = v * 10 + (ch - '0');
    }

    return v;
}

/**
 * @brief parseNmeaDate
 * @param data
 * @param size
 * @param date
 * @return
 */
static bool parseNmeaDate(const char* data, int size, NMEA::NmeaTime *date)
{
    if (size != 6) {
        return false;
    }

    int day = parseNmeaInt(&data[0], 2);
    if (day < 1 || day > 31) return false;

    int mon = parseNmeaInt(&data[2], 2);
    if (mon < 1 || mon > 12) return false;

    int year = parseNmeaInt(&data[4], 2);
    if (year < 0) return false;

    if (year < 90) {
        year += 100;
    }

    date->year = year;
    date->month = mon - 1;
    date->day = day;

    return true;
}

/**
 * @brief parseNmeaTime
 * @param data
 * @param size
 * @param time
 * @return
 */
static bool parseNmeaTime(const char* data, int size, NMEA::NmeaTime *time)
{
    if (size < 6) {
        return false;
    }

    int dotIndex = -1;
    for (int i = 0; i < size; i++) {
        if (data[i] == '.') {
            dotIndex = i;
            break;
        }
    }

    // parse "hhmmss"
    int hour = parseNmeaInt(&data[0], 2);
    if (hour < 0 || hour > 23) return false;

    int min = parseNmeaInt(&data[2], 2);
    if (min < 0 || min > 59) return false;

    int sec = parseNmeaInt(&data[4], 2);
    if (sec < 0 || sec > 59) return false;

    int msec = 0;
    if (dotIndex > 0) {
        int length = size - dotIndex - 1;

        msec = parseNmeaInt(&data[dotIndex + 1], length);
        if (msec < 0) return false;

        if (length == 1) {  // "hhmmss.s"
            msec *= 100;
        } else if (length == 2) {   // "hhmmss.ss"
            msec *= 10;
        } else if (length == 3) {   // "hhmmss.sss"
            // no-op
        } else {
            return false;
        }
    }
    if (msec < 0 || msec > 999) return false;

    time->hour = hour;
    time->min = min;
    time->sec = sec;
    time->msec = msec;

    return true;
}

/**
 * @brief ValidateNmeaChecksum
 * @param data
 * @param size
 * @param asteriskIndex
 * @return
 */
static bool ValidateNmeaChecksum(const char *data, int size, int csPos = -1)
{
    const int CS_LEN = 2;

    if (csPos == -1) {
        for (int i = 0; i < size; ++i) {
            if (data[i] == '*') {
                csPos = i;
                break;
            }
        }
    }

    if (csPos < 0 || csPos + CS_LEN >= size)
        return false;

    int calculatedCs = 0;
    for (int i = 1; i < csPos; ++i)
        calculatedCs ^= data[i];

    QByteArray csBytes(&data[csPos + 1], 2);

    bool ok = false;
    int cs = csBytes.toInt(&ok,16);
    if (!ok) {
        return false;
    }

    return cs == calculatedCs;
}

/**
 * @brief NmeaData::NmeaData
 */
NmeaData::NmeaData() : m_mask(0)
{
    m_satelliteInfo.reserve(NMEADATA_RESERVE_SATELLITE);
}

/**
 * @brief NmeaData::reset
 */
void NmeaData::reset()
{
    m_activeSatellites.clear();
    m_satelliteInfo.clear();
    m_mask = 0;
}

/**
 * @brief NmeaData::hasGGA
 * @return
 */
bool NmeaData::hasGGA() const
{
    return m_mask & NMEA::GGA;
}

/**
 * @brief NmeaData::hasRMC
 * @return
 */
bool NmeaData::hasRMC() const
{
    return m_mask & NMEA::RMC;
}

/**
 * @brief NmeaData::hasGSA
 * @return
 */
bool NmeaData::hasGSA() const
{
   return m_mask & NMEA::GSA;
}

/**
 * @brief parseGGA
 * @param data
 * @param size
 * @param gga
 * @return
 */
bool NmeaData::parseGGA(const char *data, int size, NMEA::NmeaGGA *gga)
{
    const QList<QByteArray> parts = QByteArray::fromRawData(data, size).split(',');

    if (parts.count() != 15) {
        return false;
    }

    if (!parseNmeaTime(parts[1].constData(), parts[1].count(), &gga->utc)) {
        return false;
    }

    gga->utc.year = 0;
    gga->utc.month = 0;
    gga->utc.day = 0;

    gga->lat = parts[2].toDouble();
    if (parts[3].count() > 0) {
        gga->ns = parts[3][0];
    } else {
        gga->ns = 0;
    }

    gga->lon = parts[4].toDouble();
    if (parts[5].count() > 0) {
        gga->ew = parts[5][0];
    } else {
        gga->ew = 0;
    }

    gga->sig = parts[6].toInt();
    gga->satInUse = parts[7].toInt();
    gga->HDOP = parts[8].toDouble();
    gga->altitude = parts[9].toDouble();
    if (parts[10].count() > 0) {
        gga->altitudeUnit = parts[10][0];
    } else {
        gga->altitudeUnit = 0;
    }

    gga->diff = parts[11].toDouble();
    if (parts[12].count() > 0) {
        gga->diffUnit = parts[12][0];
    } else {
        gga->diffUnit = 0;
    }

    gga->dgpsAge = parts[13].toDouble();
    gga->dgpsSid = parts[14].toInt();

    return true;
}

/**
 * @brief parseGSA
 * @param data
 * @param size
 * @param gsa
 * @return
 */
bool NmeaData::parseGSA(const char *data, int size, NMEA::NmeaGSA *gsa)
{
    const QList<QByteArray> parts = QByteArray::fromRawData(data, size).split(',');

    if (parts.count() != 18 && parts.count() != 19) {
        return false;
    }

    if (parts[1].count() > 0) {
        gsa->fixMode = parts[1][0];
    } else {
        gsa->fixMode = 0;
    }

    gsa->fixType = parts[2].toInt();
    if (gsa->fixType == 0) {
        gsa->fixType = 1;  // No Fix
    }

    int i = 0;
    for (int j = 3; j < 15; j++) {
        int prn = parts[j].toInt();
        if (prn) {
            gsa->satPrn[i++] = prn;
        }
    }

    gsa->satCount = i;
    gsa->PDOP = parts[15].toDouble();
    gsa->HDOP = parts[16].toDouble();
    gsa->VDOP = parts[17].toDouble();

    return true;
}

/**
 * @brief parseGPGSV
 * @param data
 * @param size
 * @param gsv
 * @return
 */
bool NmeaData::parseGSV(const char *data, int size, NMEA::NmeaGSV *gsv, NMEA::Talker talker)
{
    const QList<QByteArray> parts = QByteArray::fromRawData(data, size).split(',');
    int partLen = parts.count();

    if (partLen < 4) {
        return false;
    }

    gsv->messageCount = parts[1].toInt();
    if (gsv->messageCount < 1) return false;

    gsv->messageIndex = parts[2].toInt();
    if (gsv->messageIndex < 1) return false;

    gsv->totalSatCount = parts[3].toInt();
    if (gsv->totalSatCount < 0) return false;

    // the number of satellites expected in this data
    int nsat = (gsv->messageIndex - 1) * NMEA_GSV_SAT_IN_MESSAGE;
    nsat = (nsat + NMEA_GSV_SAT_IN_MESSAGE > gsv->totalSatCount) ?
                gsv->totalSatCount - nsat : NMEA_GSV_SAT_IN_MESSAGE;
    if (nsat < 0) {
        return false;
    }

    gsv->satCount = nsat;

    nsat = nsat * 4 + 4; // with GPGSV and first three parts

    if (partLen < nsat || partLen > (NMEA_GSV_SAT_IN_MESSAGE * 4 + 4)) {
        return false;
    }

    memset(gsv->satData, 0, sizeof(gsv->satData));

    for (int i = 0, j = 4; i < gsv->satCount; i++, j += 4) {
        gsv->satData[i].satelliteType = talker;
        gsv->satData[i].id = parts[j].toInt();
        gsv->satData[i].elevation = parts[j+1].toInt();
        gsv->satData[i].azimuth = parts[j+2].toInt();
        gsv->satData[i].snr = parts[j+3].toInt();

        // check satellites tracked
        gsv->satData[i].tracked = !parts[j+1].isEmpty() && !parts[j+2].isEmpty() && !parts[j+3].isEmpty();
    }

    return true;
}

/**
 * @brief parseRMC
 * @param data
 * @param size
 * @param rmc
 * @return
 */
bool NmeaData::parseRMC(const char *data, int size, NMEA::NmeaRMC *rmc)
{
    QList<QByteArray> parts = QByteArray::fromRawData(data, size).split(',');

    // NMEA version 2.3 (and later) has the Mode field.
    if (parts.count() != 13 && parts.count() != 12) {
        return false;
    }

    if (!parseNmeaTime(parts[1].constData(), parts[1].count(), &rmc->utc)) {
        return false;
    }

    rmc->status = parts[2][0];
    rmc->lat = parts[3].toDouble();
    if (parts[4].count() > 0) {
        rmc->ns = parts[4][0];
    } else {
        rmc->ns = 0;
    }
    rmc->lon = parts[5].toDouble();
    if (parts[6].count() > 0) {
        rmc->ew = parts[6][0];
    } else {
        rmc->ew = 0;
    }
    rmc->speed = parts[7].toDouble();
    rmc->direction = parts[8].toDouble();

    if (!parseNmeaDate(parts[9].constData(), parts[9].count(), &rmc->utc)) {
        return false;
    }

    rmc->declination = parts[10].toDouble();

    if (parts[11].count() > 0) {
        rmc->declination_ew = parts[11][0];
    } else {
        rmc->declination_ew = 0;
    }

    if (parts.count() > 12 && parts[12].count() > 0) {
        rmc->mode = parts[12][0];
    } else {
        rmc->mode = 0;
    }

    return true;
}

/**
 * @brief getNmeaType
 * @param data
 * @return
 */
NMEA::NmeaType NmeaData::getNmeaType(const char *data, int size, NMEA::Talker* talker)
{
    if (!data || size < 6 || data[0] != '$') {
        return NMEA::UNKNOWN;
    }

    NMEA::Talker t = NMEA::UnknownTalker;

    if (data[1] == 'G' && data[2] == 'P') {
        t = NMEA::GP;
    }
#ifdef NMEADATA_SUPPORT_GA
    else if (data[1] == 'G' && data[2] == 'A') {
        t = NMEA::GA;
    }
#endif
    else if (data[1] == 'G' && data[2] == 'L') {
        t = NMEA::GL;
    }
    else if (data[1] == 'G' && data[2] == 'N') {
        t = NMEA::GN;
    }
#ifdef NMEADATA_SUPPORT_BD
    else if (data[1] == 'B' && data[2] == 'D') {
        t = NMEA::BD;
    }
#endif
#ifdef NMEADATA_SUPPORT_QZ
    else if (data[1] == 'Q' && data[2] == 'Z') {
        t = NMEA::QZ;
    }
#endif

    if (talker) {
        *talker = t;
    }

    if (data[3] == 'G' && data[4] == 'G' && data[5] == 'A') {
        return NMEA::GGA;
    } else if (data[3] == 'G' && data[4] == 'S' && data[5] == 'A') {
        return NMEA::GSA;
    } else if (data[3] == 'G' && data[4] == 'S' && data[5] == 'V') {
        return NMEA::GSV;
    } else if (data[3] == 'R' && data[4] == 'M' && data[5] == 'C') {
        return NMEA::RMC;
    }

    return NMEA::UNKNOWN;
}

/**
 * @brief NmeaData::parseSentence
 * @param data
 * @param size
 * @param parseTypes
 * @return
 */
int NmeaData::parseSentence(const char *data, int size, int parseTypes)
{
    NMEA::Talker talker = NMEA::UnknownTalker;
    NMEA::NmeaType nmeaType = getNmeaType(data, size, &talker);

    if (nmeaType == NMEA::UNKNOWN || talker == NMEA::UnknownTalker ||
            (parseTypes & nmeaType) == 0) {
        return 0;
    }

    // find '*'
    int asteriskIndex = -1;
    for (int i = size - 1; i >= 0; i--) {
        if (data[i] == '*') {
            asteriskIndex = i;
            break;
        }
    }
    if (asteriskIndex < 0) {
        return 0;
    }
#ifdef NMEADATA_CHECK_CHECKSUM
    if (!ValidateNmeaChecksum(data, size, asteriskIndex)) {
        return 0;
    }
#endif

    size = asteriskIndex; // before '*'

    switch (nmeaType) {
    case NMEA::GGA: {
        if (parseGGA(data, size, &m_gga)) {
            m_mask |= NMEA::GGA;
        }
        break;
    }
    case NMEA::GSA: {
        if (parseGSA(data, size, &m_gsa)) {
            m_activeSatellites.clear();
            m_satelliteInfo.clear();

            for (int i = 0; i < m_gsa.satCount; i++) {
                m_activeSatellites.insert(m_gsa.satPrn[i]);
            }
            m_mask |= NMEA::GSA;    // TODO: consider satellite type
        }
        break;
    }
    case NMEA::GSV: {
        NMEA::NmeaGSV gsv;
        if (parseGSV(data, size, &gsv, talker)) {
            for (int i = 0; i < gsv.satCount; i++) {
                if (gsv.satData[i].id) {
                    m_satelliteInfo.append(gsv.satData[i]);
                }
            }
            if (gsv.messageIndex == gsv.messageCount) {
                m_mask |= NMEA::GSV;    // TODO: consider satellite type
            }
        }
        break;
    }
    case NMEA::RMC: {
        if (parseRMC(data, size, &m_rmc)) {
            m_mask |= NMEA::RMC;
        }
        break;
    }
    default:
        break;
    }

    return nmeaType;
}

}
