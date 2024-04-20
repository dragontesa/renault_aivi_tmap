#ifndef TVASPARSER_H
#define TVASPARSER_H

#include <QObject>

namespace SKT {

#pragma pack(1)

struct sTvasTileCoord
{
    unsigned short	usTileX;
    unsigned short	usTileY;
    unsigned short	usOffsetX;
    unsigned short	usOffsetY;
};

//TVAS 4.5 해더
struct sTvasHeader45
{
    //////////////////////////////////////////////////////////////////////////
    int                 nSize;				// 포멧 전체 크기
    //////////////////////////////////////////////////////////////////////////
    char                nTvasVersion;		// TVAS 포멧 버전
    unsigned char       byReserved;
    unsigned short		uMapCode;			// 맵 제공사 코드
    char                szMapVersion[8];	// 맵 버전 (YYYYMMDD) - NULL 없음
    //////////////////////////////////////////////////////////////////////////
    unsigned char       uRouteOptCode;		// 경로 탐색 옵션 코드 (link)
    unsigned char		uPriceOptCode;		// 요금 가중치 옵션 코드 (link)
    unsigned char		uStartDayCode;		// 출발시점 요일 구분코드 (0:평일, 1:토요일, 2:휴일)
    unsigned char		uNextDayCode;		// 다음날 요일 구분코드 (0:평일, 1:토요일, 2:휴일)
    int		nTotalDist;			// 총 거리(단위, m)
    int		nTotalTime;			// 총 소요 시간(단위, 초)
    int		nTaxiPrice;			// 택시요금 (단위, 원)
    unsigned char		uRouteCtrlCode;		// 경로상 통제여부 코드(link)
    unsigned char		uRouteType;			// 경로 구분(1:추천경로, 2대안경로)
    unsigned short		uAnotherRouteVE;	// 대안경로 분기 보간점
    unsigned char		byReserved2[4];
    //////////////////////////////////////////////////////////////////////////
    char		nCoordType;			// 좌표계 타입
    char		nTileInfo;			// 타일 정보
    unsigned char		byReserved3[2];
    sTvasTileCoord sStartPosition;	// 출발지 위치
    sTvasTileCoord sGoalPosition;	// 목적지 위치
    char		szStartName[100];	// 출발지 명칭(NULL 포함)
    char		szGoalName[100];	// 목적지 명칭(NULL 포함)
    unsigned short		uServiceCnt;		// 총 정보 인덱스 개수
    unsigned char		byReserved4[2];
};

#pragma pack()

union TvasHeader{
    sTvasHeader45 header45;
};


struct sTvasData{
    TvasHeader header;
};


class TvasParse : public QObject{
    Q_OBJECT
public:
    sTvasData parsedData;

    void parse(char *pBuffer, unsigned int nSize);
};

}



#endif // TVASPARSER_H
