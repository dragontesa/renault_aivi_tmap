/*!
 * PoiCacheDto.h
 *
 */

#ifndef UI_DOMAIN_POICACHEDTO_H_
#define UI_DOMAIN_POICACHEDTO_H_

namespace SKT {

T_SHAREABLE(PoiCacheDto)
T_SHAREABLE(PoiCacheInfo)

struct PoiCacheInfo
{
	int poiId;      // local poi id
	int dbKind;     // 시설물 분류 (S:SK, A:주소)
	int navX;       // 입구점 X좌표
	int navY;       // 입구점 Y좌표
	int centerX;    // 중심 X좌표
	int centerY;    // 중심 Y좌표
	int rpFlag;
	int dataKind;     // 업종별 분기 구분자(3:주유소, 4:충전소)
	int mlClass;    // 산/대지 구분자 (0:알수없음, 1;대지
	int parkYn;     // 주차 가능(1:가능, 0:불가능)
//	int iconType;   // 아이콘, 표시용 업종 코드

	int premiumGasolinYn; // 고급휘발유 판매유무(0:판매안함, 1:판매함)
	int lowestOilYn;     // 최저가점포여부("1":최저가 , 0:아님)
	int gasolinPrice;     // 휘발유가격
	int dieselPrice;  // 경유가격
	int lpgPrice;     // LPG 가격
	int premiumGasolinPrice; // 고급휘발유 가격
	int premiumDieselPrice; // 고급경유 가격
	int premiumLpgPrice; // 고급경유 가격
	QString oilBaseSdt;   // 기준일 (YYYYMMDD)

//	int holidayN;    // 연중무휴(1:연중무휴)
//	int recomPoint;  // 좋아요 평점
	QString navSeq;  // 입구점 일련번호
//	QString bizCategory; // 업종
//	QString grpId;   // 시설물 그룹ID
	QString oilCompany;   // 주유소브랜드	("SK","GS","기타",...)

	QString fullName; // 정싱명칭
	QString aliasName; // 대표명칭
	QString address;
	QString primaryBun;
	QString secondaryBun;
	QString lcdName;  // 주소 대분류
	QString mcdName;  // 주소 중분류
	QString scdName;  // 주소 소분류
	QString dcdName;  // 주소 세분류
	QString zipCode;
	QString telNum;
	QString webSite;      // 홈페이지
	QString radius;       // 거리
	QString roadName;     // 도로명
	QString bldNo1;       // 건물번호1 (새주소)
	QString bldNo2;       // 건물번호2 (새주소)
	QString pubTraffic;       // 건물번호2 (새주소)
	QString stationId;       // 건물번호2 (새주소)
//	QString joinStoreYn;  // 가맹점여부 세미콜론(;) 구분자 표시
//	QString joinViewType; // 가맹점 화면 Type	가맹점여부정보와 1:1 대응하며 세미콜론(;) 구분자로 나열하여 표시
//	QString oilBaseSdt; // 오일 기준일 (YYYYMMDD)

	QByteArray  subGroupList;
	QByteArray  advContents;
};

class PoiCacheDto: public QObject
{
	Q_OBJECT
	Q_CLASSINFO( "table", "poi_cache" )
	Q_CLASSINFO( "column", "id INTEGER PRIMARY KEY AUTOINCREMENT" )
	Q_CLASSINFO( "column", "poiId INT UNIQUE ON CONFLICT REPLACE" )
	Q_CLASSINFO( "column", "dbKind INT" )              // 시설물 분류 (S:SK, A:주소)
	Q_CLASSINFO( "column", "navX INT" )                  // 입구점 X좌표
	Q_CLASSINFO( "column", "navY INT" )                  // 입구점 Y좌표
	Q_CLASSINFO( "column", "centerX INT" )               // 중심 X좌표
	Q_CLASSINFO( "column", "centerY INT" )               // 중심 Y좌표
	Q_CLASSINFO( "column", "rpFlag INT" )
	Q_CLASSINFO( "column", "dataKind INT" )               // 업종별 분기 구분자("3": 주유소, "4":충전소)
	Q_CLASSINFO( "column", "mlClass INT" )                  // 산/대지 구분자 ("0": 알수없음, "1":대지,
	Q_CLASSINFO( "column", "parkYn INT" )                // 주차 가능 여부 ("1":가능, "0":불가능)
//	Q_CLASSINFO( "column", "holidayN INT" )              // 연중무휴 Flag("1":연중무휴)
//	Q_CLASSINFO( "column", "recomPoint INT" )            // 좋아요 평점
//	Q_CLASSINFO( "column", "iconType INT" )              // 아이콘, 표시용 업종코드
	Q_CLASSINFO( "column", "premiumGasolinYn INT" )          // 고급휘발유 판매유무(0:판매안함, 1:판매함)
	Q_CLASSINFO( "column", "lowestOilYn INT" )              // 최저가점포여부("1":최저가 , 0:아님)
	Q_CLASSINFO( "column", "gasolinPrice INT" )              // 휘발유 가격
	Q_CLASSINFO( "column", "dieselPrice INT" )           // 경유 가격
	Q_CLASSINFO( "column", "lpgPrice INT" )              // LPG 가격
	Q_CLASSINFO( "column", "premiumGasolinPrice INT" )       // 고급휘발유 가격
	Q_CLASSINFO( "column", "premiumDieselPrice INT" )    // 고급경유 가격
	Q_CLASSINFO( "column", "premiumLpgPrice INT" )    // 고급LPG 가격
	Q_CLASSINFO( "column", "oilBaseSdt TEXT" )                // 기준일 (YYYYMMDD)
	Q_CLASSINFO( "column", "navSeq TEXT" )                  // 입구점 일련번호
//	Q_CLASSINFO( "column", "bizCategory TEXT" )               // 업종
//	Q_CLASSINFO( "column", "grpId TEXT" )                   // 시설물 그룹 ID
	Q_CLASSINFO( "column", "oilCompany TEXT" )                   // 주유소 브랜드("SK", "GS", "기타"
	Q_CLASSINFO( "column", "fullName TEXT" )                  // 정식명칭
	Q_CLASSINFO( "column", "aliasName TEXT" )                 // 대표명칭
	Q_CLASSINFO( "column", "address TEXT" )
	Q_CLASSINFO( "column", "primaryBun TEXT" )               // 주소 본번
	Q_CLASSINFO( "column", "secondaryBun TEXT" )             // 주소 부번
	Q_CLASSINFO( "column", "lcdName TEXT" )                   // 주소 대분류
	Q_CLASSINFO( "column", "mcdName TEXT" )                   // 주소 중분류
	Q_CLASSINFO( "column", "scdName TEXT" )                   // 주소 소분류
	Q_CLASSINFO( "column", "dcdName TEXT" )                   // 주소 세분류
	Q_CLASSINFO( "column", "zipCode TEXT" )                  // 우편번호
	Q_CLASSINFO( "column", "telNum TEXT" )                    // 전화번호
	Q_CLASSINFO( "column", "webSite TEXT" )                       // 홈페이지
	Q_CLASSINFO( "column", "radius TEXT" )                     // 거리
	Q_CLASSINFO( "column", "roadName TEXT" )                     // 도로명
	Q_CLASSINFO( "column", "bldNo1 TEXT" )                     // 건물번호1(새주소)
	Q_CLASSINFO( "column", "bldNo2 TEXT" )                     // 건물번호2(새주소)
	Q_CLASSINFO( "column", "pubTraffic INT" )                     // 대중교통 구분(0:없음, 1:버스 2:지하철)
	Q_CLASSINFO( "column", "stationId TEXT" )                     // 대중교통 구분이 1이면 버스정류장 ID, 대중교통 구분이 2이면 지하철역 ID


//	Q_CLASSINFO( "column", "joinStoreYn TEXT" )           // 가맹점여부 세미콜론(;) 구분자 표시
//	Q_CLASSINFO( "column", "joinViewType TEXT" )          // 가맹점 화면 Type	가맹점여부정보와 1:1 대응하며 세미콜론(;) 구분자로 나열하여 표시
	Q_CLASSINFO( "unique index", "poiId" )
public:
	explicit PoiCacheDto(QObject* parent =0 );
	virtual ~PoiCacheDto();

	Q_PROPERTY(QVariant id READ id)
	Q_PROPERTY(QVariant poiId READ poiId WRITE setPoiId)
	Q_PROPERTY(QVariant dbKind READ dbKind WRITE setDbKind)
	Q_PROPERTY(QVariant navX READ navX WRITE setNavX)
	Q_PROPERTY(QVariant navY READ navY WRITE setNavY)
	Q_PROPERTY(QVariant centerX READ centerX WRITE setCenterX)
	Q_PROPERTY(QVariant centerY READ centerY WRITE setCenterY)
	Q_PROPERTY(QVariant rpFlag READ rpFlag WRITE setRpFlag)
	Q_PROPERTY(QVariant dataKind READ dataKind WRITE setDataKind)
	Q_PROPERTY(QVariant mlClass READ mlClass WRITE setMlClass)
	Q_PROPERTY(QVariant parkYn READ parkYn WRITE setParkYn)
//	Q_PROPERTY(QVariant holidayN READ holidayN WRITE setHolidayN)
//	Q_PROPERTY(QVariant recomPoint READ recomPoint WRITE setRecomPoint)
//	Q_PROPERTY(QVariant iconType READ iconType WRITE setIconType)
	Q_PROPERTY(QVariant premiumGasolinYn READ premiumGasolinYn WRITE setPremiumGasolinYn)
	Q_PROPERTY(QVariant lowestOilYn READ lowestOilYn WRITE setLowestOilYn)
	Q_PROPERTY(QVariant gasolinPrice READ gasolinPrice WRITE setGasolinPrice)
	Q_PROPERTY(QVariant dieselPrice READ dieselPrice WRITE setDieselPrice)
	Q_PROPERTY(QVariant lpgPrice READ lpgPrice WRITE setLpgPrice)
	Q_PROPERTY(QVariant premiumGasolinPrice READ premiumGasolinPrice WRITE setPremiumGasolinPrice)
	Q_PROPERTY(QVariant premiumDieselPrice READ premiumDieselPrice WRITE setPremiumDieselPrice)
	Q_PROPERTY(QVariant premiumLpgPrice READ premiumLpgPrice WRITE setPremiumLpgPrice)
	Q_PROPERTY(QVariant oilBaseSdt READ oilBaseSdt WRITE setOilBaseSdt)
	Q_PROPERTY(QVariant navSeq READ navSeq WRITE setNavSeq)
//	Q_PROPERTY(QVariant bizCategory READ bizCategory WRITE setBizCategory)
//	Q_PROPERTY(QVariant grpId READ grpId WRITE setGrpId)
	Q_PROPERTY(QVariant oilCompany READ oilCompany WRITE setOilCompany)

	Q_PROPERTY(QVariant fullName READ fullName WRITE setFullName)
	Q_PROPERTY(QVariant aliasName READ aliasName WRITE setAliasName)
	Q_PROPERTY(QVariant address READ address WRITE setAddress)
	Q_PROPERTY(QVariant primaryBun READ primaryBun WRITE setPrimaryBun)
	Q_PROPERTY(QVariant secondaryBun READ secondaryBun WRITE setSecondaryBun)
	Q_PROPERTY(QVariant lcd READ lcd WRITE setLcd)
	Q_PROPERTY(QVariant mcd READ mcd WRITE setMcd)
	Q_PROPERTY(QVariant scd READ scd WRITE setScd)
	Q_PROPERTY(QVariant dcd READ dcd WRITE setDcd)
	Q_PROPERTY(QVariant zipCode READ zipCode WRITE setZipCode)
	Q_PROPERTY(QVariant telNum READ telNum WRITE setTelNum)
	Q_PROPERTY(QVariant website READ website WRITE setWebsite)
	Q_PROPERTY(QVariant radius READ radius WRITE setRadius)
	Q_PROPERTY(QVariant roadName READ roadName WRITE setRoadName)
	Q_PROPERTY(QVariant bldNo1 READ bldNo1 WRITE setBldNo1)
	Q_PROPERTY(QVariant bldNo2 READ bldNo2 WRITE setBldNo2)
	Q_PROPERTY(QVariant pubTraffic READ pubTraffic WRITE setPubTraffic)
	Q_PROPERTY(QVariant stationId READ stationId WRITE setStationId)
//	Q_PROPERTY(QVariant joinStoreYn READ joinStoreYn WRITE setJoinStoreYn)
//	Q_PROPERTY(QVariant joinViewType READ joinViewType WRITE setJoinViewType)

	QVariant id() const;
	QVariant poiId() const;
	void setPoiId(const QVariant& value);
	QVariant dbKind() const;
	void setDbKind(const QVariant& value);
	QVariant navX() const;
	void setNavX(const QVariant& value);
	QVariant navY() const;
	void setNavY(const QVariant& value);
	QVariant centerX() const;
	void setCenterX(const QVariant& value);
	QVariant centerY() const;
	void setCenterY(const QVariant& value);
	QVariant rpFlag() const;
	void setRpFlag(const QVariant& value);
	QVariant dataKind() const;
	void setDataKind(const QVariant& value);
	QVariant mlClass() const;
	void setMlClass(const QVariant& value);
	QVariant parkYn() const;
	void setParkYn(const QVariant& value);
//	QVariant holidayN() const;
//	void setHolidayN(const QVariant& value);
//	QVariant recomPoint() const;
//	void setRecomPoint(const QVariant& value);
//	QVariant iconType() const;
//	void setIconType(const QVariant& value);
	QVariant premiumGasolinYn() const;
	void setPremiumGasolinYn(const QVariant& value);
	QVariant lowestOilYn() const;
	void setLowestOilYn(const QVariant& value);
	QVariant gasolinPrice() const;
	void setGasolinPrice(const QVariant& value);
	QVariant dieselPrice() const;
	void setDieselPrice(const QVariant& value);
	QVariant lpgPrice() const;
	void setLpgPrice(const QVariant& value);
	QVariant premiumGasolinPrice() const;
	void setPremiumGasolinPrice(const QVariant& value);
	QVariant premiumDieselPrice() const;
	void setPremiumDieselPrice(const QVariant& value);
	QVariant premiumLpgPrice() const;
	void setPremiumLpgPrice(const QVariant& value);
	QVariant oilBaseSdt() const;
	void setOilBaseSdt(const QVariant& value);
	QVariant navSeq() const;
	void setNavSeq(const QVariant& value);

//	QVariant bizCategory() const;
//	void setBizCategory(const QVariant& value);

//	QVariant grpId() const;
//	void setGrpId();
	QVariant oilCompany() const;
	void setOilCompany(const QVariant& value);

	QVariant fullName() const;
	void setFullName(const QVariant& value);
	QVariant aliasName() const;
	void setAliasName(const QVariant& value);
	QVariant address() const;
	void setAddress(const QVariant& value);
	QVariant primaryBun() const;
	void setPrimaryBun(const QVariant& value);
	QVariant secondaryBun() const;
	void setSecondaryBun(const QVariant& value);
	QVariant lcd() const;
	void setLcd(const QVariant& value);
	QVariant mcd() const;
	void setMcd(const QVariant& value);
	QVariant scd() const;
	void setScd(const QVariant& value);
	QVariant dcd() const;
	void setDcd(const QVariant& value);
	QVariant zipCode() const;
	void setZipCode(const QVariant& value);
	QVariant telNum() const;
	void setTelNum(const QVariant& value);
	QVariant website() const;
	void setWebsite(const QVariant& value);

	QVariant radius() const;
	void setRadius(const QVariant& value);
	QVariant roadName() const;
	void setRoadName(const QVariant& value);
	QVariant bldNo1() const;
	void setBldNo1(const QVariant& value);
	QVariant bldNo2() const;
	void setBldNo2(const QVariant& value);
	QVariant pubTraffic() const;
	void setPubTraffic(const QVariant& value);
	QVariant stationId() const;
	void setStationId(const QVariant& value);

//	QVariant joinStoreYn() const;
//	void setJoinStoreYn(const QVariant& value);
//	QVariant joinViewType() const;
//	void setJoinViewType(const QVariant& value);


private:
	QVariant mId;
	QVariant mPoiId;
	QVariant mDbKind;
	QVariant mNavX;
	QVariant mNavY;
	QVariant mCenterX;
	QVariant mCenterY;
	QVariant mRpFlag;
	QVariant mDataKind;
	QVariant mMlClass;
	QVariant mParkYn;
//	QVariant mHolidayN;
//	QVariant mRecomPoint;
//	QVariant mIconType;
//	QVariant mJoinStoreYn;
//	QVariant mJoinViewType;
	QVariant mPremiumGasolinYn;
	QVariant mLowestOilYn;
	QVariant mGasolinPrice;
	QVariant mDieselPrice;
	QVariant mLpgPrice;
	QVariant mPremiumGasolinPrice;
	QVariant mPremiumDieselPrice;
	QVariant mPremiumLpgPrice;
	QVariant mOilBaseSdt;
	QVariant mOilCompany;
//	QVariant mViewId;
	QVariant mNavSeq;

	QVariant mFullName;
	QVariant mAliasName;
	QVariant mBizCategory;
	QVariant mAddress;
	QVariant mPrimaryBun;
	QVariant mSecondaryBun;
	QVariant mLcd;
	QVariant mMcd;
	QVariant mScd;
	QVariant mDcd;
	QVariant mZipCode;
	QVariant mTelNum;
	QVariant mWebsite;
	QVariant mRadius;
	QVariant mRoadName;
	QVariant mBldNo1;
	QVariant mBldNo2;
	QVariant mPubTraffic;
	QVariant mStationId;

};

}
#endif /* UI_DOMAIN_POICACHEDTO_H_ */
