/*
 * PoiExtraCacheDto.h
 *
 */

#ifndef UI_DOMAIN_POIEXTRACACHE_H_
#define UI_DOMAIN_POIEXTRACACHE_H_

namespace SKT
{

struct PoiExtraCacheInfo
{
	int poiId;      // local poi id

	QByteArray  subGroupList;
	QByteArray  advContents;
//	int poiImageNum;  // POI 이미지 정보 개수
//	int poiParkNum;   // POI 주차장 정보 개수
//	int bizImageSize; // 업종 대표 이미지 크기
//	QString waytoGo;      // 찾아가는 길
//	QString subInfo;      // 부가정보
//	QString introduction;  // 업소 소개글
//	QString bizImage;      // 업체정보 이미지 base64 encoding

};

class PoiExtraCacheDto: public QObject
{
	Q_OBJECT
	Q_CLASSINFO( "table", "poi_extra_cache" )
	Q_CLASSINFO( "column", "id INTEGER PRIMARY KEY AUTOINCREMENT" )
//	Q_CLASSINFO( "column", "poiId INTEGER,fk=poiID,reftable=poi_cache,refcol=poiID,not_null=true" )
	Q_CLASSINFO( "column", "poiId INT UNIQUE ON CONFLICT REPLACE" )        // POI id , reference to poi_cache.poiID
	Q_CLASSINFO( "column", "subGroupList BLOB ")   // 하부 그룹 리스트
	Q_CLASSINFO( "column", "advContents BLOB ")   // 광고 음원 정보 리스트
//	Q_CLASSINFO( "column", "subName TEXT")   // 하부-순번 (그룹인 경우  1 이상의 값이 존재)
//	Q_CLASSINFO( "column", "subNavX TEXT")   			// 하부-시설물입구X좌표
//	Q_CLASSINFO( "column", "subNavY TEXT")   			// 하부-시설물입구X좌표
//	Q_CLASSINFO( "column", "subRpFlag INT")   			// 하부-RP FLAG
//	Q_CLASSINFO( "column", "subNavSeq INT")   			// 하부-입구점번호
//	Q_CLASSINFO( "column", "adType TEXT")   			// 광고 타입 “IN10”: 제휴사 광고(검색키워드 배너광고)
//	Q_CLASSINFO( "column", "adCode INT")   			    // 광고 코드
//	Q_CLASSINFO( "column", "adKindCd INT")   			//  광고종류  구분코드 "2701“: 광고, “2703“: 컨텐츠
//	Q_CLASSINFO( "column", "adRegDate TEXT")   			//   등록일(YYYYMMDDHHMI)
//	Q_CLASSINFO( "column", "adStartDate TEXT")   		//  광고 시작일자(YYYYMMDDHH24MI)
//	Q_CLASSINFO( "column", "adEndDate TEXT")   		    //  광고 시작일자(YYYYMMDDHH24MI)
//	Q_CLASSINFO( "column", "downloadURL TEXT")   		//  광고 배너 이미지 다운로드 URL
//	Q_CLASSINFO( "column", "linkURL TEXT")   		    //  Link URL

//	Q_CLASSINFO( "column", "poiImageNum INT" )        // POI 이미지정보 개수
//	Q_CLASSINFO( "column", "poiParkNum INT" )          // POI 주차장 정보 개수
//	Q_CLASSINFO( "column", "bizImageSize INT" )           // 업종 대표 이미지 크기
//	Q_CLASSINFO( "column", "waytoGo TEXT" )                       // 찾아가는 길
//	Q_CLASSINFO( "column", "subInfo TEXT" )                    // 부가정보
//	Q_CLASSINFO( "column", "introduction TEXT" )                // 업소 소개글
//	Q_CLASSINFO( "column", "bizImage TEXT" )                // 업체정보 이미지  base on base64 encoding
//	Q_CLASSINFO( "column", "poiImageInfo BLOB" )                  // POI 이미지 정보 목록
//	Q_CLASSINFO( "column", "poiParkInfo BLOB" )                  // POI 주차장 정보 목록
//	Q_CLASSINFO( "column", "menu1 TEXT" )
//	Q_CLASSINFO( "column", "menu2 TEXT" )
//	Q_CLASSINFO( "column", "menu3 TEXT" )
//	Q_CLASSINFO( "column", "menu4 TEXT" )
//	Q_CLASSINFO( "column", "menu5 TEXT" )
	Q_CLASSINFO( "unique index", "poiId" )


public:
	explicit PoiExtraCacheDto(QObject* parent=0);
	virtual ~PoiExtraCacheDto();

	Q_PROPERTY(QVariant id READ id)
	Q_PROPERTY(QVariant poiId READ poiId WRITE setPoiId)

	Q_PROPERTY(QVariant subGroupList READ subGroupList WRITE setSubGroupList)
	Q_PROPERTY(QVariant advContents READ advContents WRITE setAdvContents)
//	Q_PROPERTY(QVariant poiImageNum READ poiImageNum WRITE setPoiImageNum)
//	Q_PROPERTY(QVariant poiParkNum READ poiParkNum WRITE setPoiParkNum)
//	Q_PROPERTY(QVariant bizImageSize READ bizImageSize WRITE setBizImageSize)
//	Q_PROPERTY(QVariant waytoGo READ waytoGo WRITE setWaytoGo)
//	Q_PROPERTY(QVariant subInfo READ subInfo WRITE setSubInfo)
//	Q_PROPERTY(QVariant introduction READ introduction WRITE setIntroduction)
//	Q_PROPERTY(QVariant bizImage READ bizImage WRITE setBizImage)
//	Q_PROPERTY(QVariant poiImageInfo READ poiImageInfo WRITE setPoiImageInfo)
//	Q_PROPERTY(QVariant poiParkInfo READ poiParkInfo WRITE setPoiParkInfo)
//	Q_PROPERTY(QVariant menu1 READ menu1 WRITE setMenu1)
//	Q_PROPERTY(QVariant menu2 READ menu2 WRITE setMenu2)
//	Q_PROPERTY(QVariant menu3 READ menu3 WRITE setMenu3)
//	Q_PROPERTY(QVariant menu4 READ menu4 WRITE setMenu4)
//	Q_PROPERTY(QVariant menu5 READ menu5 WRITE setMenu5)

	QVariant id() const;

	QVariant poiId() const;
	void setPoiId(const QVariant& value);

	QVariant subGroupList() const;
	void setSubGroupList(const QVariant& value);
	QVariant advContents() const;
	void setAdvContents(const QVariant& value);

//	QVariant poiImageNum() const;
//	void setPoiImageNum(const QVariant& value) const;
//
//	QVariant poiParkNum() const;
//	void setPoiParkNum(const QVariant& value);
//
//	QVariant bizImageSize() const;
//	void setBizImageSize(QVariant& value);
//
//	QVariant waytoGo() const;
//	void setWaytoGo(const QVariant& value);
//
//	QVariant subInfo() const;
//	void setSubInfo(const QVariant& value);
//
//	QVariant introduction() const;
//	void setIntroduction(const QVariant& value);
//
//	QVariant bizImage() const;
//	void setBizImage(const QVariant& value);
//
//	QVariant poiImageInfo() const;
//	void setPoiImageInfo(const QVariant& value);
//
//	QVariant poiParkInfo() const;
//	void setPoiParkInfo(const QVariant& value);
//
//	QVariant menu1() const;
//	void setMenu1(const QVariant& value);
//
//	QVariant menu2() const;
//	void setMenu2(const QVariant& value);
//
//	QVariant menu3() const;
//	void setMenu3(const QVariant& value);
//
//	QVariant menu4() const;
//	void setMenu4(const QVariant& value);
//
//	QVariant menu5() const;
//	void setMenu5(const QVariant& value);


private:
	QVariant mId;
	QVariant mPoiId;
	QVariant mSubGroupList;
	QVariant mAdvContents;

//	QVariant mPoiImageNum;
//	QVariant mPoiParkNum;
//	QVariant mBizImageSize;
//	QVariant mWaytoGo;
//	QVariant mSubInfo;
//	QVariant mIntroduction;
//	QVariant mBizImage;
//	QVariant mPoiImageInfo;
//	QVariant mPoiParkInfo;
//	QVariant mMenu1;
//	QVariant mMenu2;
//	QVariant mMenu3;
//	QVariant mMenu4;
//	QVariant mMenu5;

};

} /* namespace SKT */

#endif /* UI_DOMAIN_POIEXTRACACHEDTO_H_ */
