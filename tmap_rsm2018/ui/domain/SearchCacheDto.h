#ifndef SEARCHCACHEDTO_H
#define SEARCHCACHEDTO_H

namespace SKT {

T_SHAREABLE(SearchCacheDto)

class SearchCacheDto : public QObject
{
	Q_OBJECT
	Q_CLASSINFO( "table", "search_cache" )
	Q_CLASSINFO( "column", "id INTEGER PRIMARY KEY AUTOINCREMENT" )
	Q_CLASSINFO( "column", "poiId INT" )
    Q_CLASSINFO( "column", "poiIdSeq INT UNIQUE ON CONFLICT REPLACE" ) // added 2018.11.16
	Q_CLASSINFO( "column", "addrCode INT" )
	Q_CLASSINFO( "column", "rpFlag INT" )
	Q_CLASSINFO( "column", "distance INT" )
	Q_CLASSINFO( "column", "degree INT" )
	Q_CLASSINFO( "column", "kind INT" )
	Q_CLASSINFO( "column", "tpX INT" )
	Q_CLASSINFO( "column", "tpY INT" )
	Q_CLASSINFO( "column", "opiPoleCate INT" )
	Q_CLASSINFO( "column", "opiPrice INT" )
	Q_CLASSINFO( "column", "gatherYear INT DEFAULT 0" )
	Q_CLASSINFO( "column", "gatherMonth INT DEFAULT 0" )
	Q_CLASSINFO( "column", "gatherDay INT DEFAULT 0" )
	Q_CLASSINFO( "column", "gatherHour INT DEFAULT 0" )
	Q_CLASSINFO( "column", "gatherMinute INT DEFAULT 0" )
	Q_CLASSINFO( "column", "gatherSecond INT DEFAULT 0" )
	Q_CLASSINFO( "column", "gatherWeek INT DEFAULT 0" )
	Q_CLASSINFO( "column", "fullName TEXT" )
	Q_CLASSINFO( "column", "stdName TEXT" )
	Q_CLASSINFO( "column", "address TEXT" )
	Q_CLASSINFO( "column", "phoneNumber VCHAR(32)" )

	Q_CLASSINFO( "column", "premiumGasolinYn INT" )          // 고급휘발유 판매유무(0:판매안함, 1:판매함)
	Q_CLASSINFO( "column", "lowestOilYn INT" )              // 최저가점포여부("1":최저가 , 0:아님)
	Q_CLASSINFO( "column", "opiGasolinePrice INT" )              // 휘발유 가격
	Q_CLASSINFO( "column", "opiDieselPrice INT" )           // 경유 가격
	Q_CLASSINFO( "column", "opiLpgPrice INT" )              // LPG 가격
	Q_CLASSINFO( "column", "opiHiGasolinePrice INT" )       // 고급휘발유 가격
	Q_CLASSINFO( "column", "opiHiLpgPrice INT" )        // 고급LPG 가격
	Q_CLASSINFO( "column", "navSeq TEXT" )                  // 입구점 일련번호
	Q_CLASSINFO( "column", "oilCompany TEXT" )                   // 주유소 브랜드("SK", "GS", "기타"
	Q_CLASSINFO( "unique index", "poiIdSeq" )
	Q_CLASSINFO( "index", "tpX" )
	Q_CLASSINFO( "index", "tpY" )
    

public:
    explicit SearchCacheDto(QObject* parent=0);
    Q_PROPERTY(QVariant id READ id)
    Q_PROPERTY(QVariant fullName READ fullName WRITE setFullName)
    Q_PROPERTY(QVariant stdName READ stdName WRITE setStdName)
    Q_PROPERTY(QVariant address READ address WRITE setAddress)
    Q_PROPERTY(QVariant phoneNumber READ phoneNumber WRITE setPhoneNumber)
	Q_PROPERTY(QVariant poiId READ poiId WRITE setPoiId)
	Q_PROPERTY(QVariant addrCode READ addrCode WRITE setAddrCode)
	Q_PROPERTY(QVariant rpFlag READ rpFlag WRITE setRpFlag)
	Q_PROPERTY(QVariant distance READ distance WRITE setDistance)
	Q_PROPERTY(QVariant degree READ degree WRITE setDegree)
	Q_PROPERTY(QVariant kind READ kind WRITE setKind)
	Q_PROPERTY(QVariant tpX READ tpX WRITE setTpX)
	Q_PROPERTY(QVariant tpY READ tpY WRITE setTpY)
	Q_PROPERTY(QVariant opiPoleCate READ opiPoleCate WRITE setOpiPoleCate)
	Q_PROPERTY(QVariant opiPolePrice READ opiPolePrice WRITE setOpiPolePrice)
	Q_PROPERTY(QVariant gatherYear READ gatherYear WRITE setGatherYear)
	Q_PROPERTY(QVariant gatherMonth READ gatherMonth WRITE setGatherMonth)
	Q_PROPERTY(QVariant gatherDay READ gatherDay WRITE setGatherDay)
	Q_PROPERTY(QVariant gatherHour READ gatherHour WRITE setGatherHour)
	Q_PROPERTY(QVariant gatherMinute READ gatherMinute WRITE setGatherMinute)
	Q_PROPERTY(QVariant gatherSecond READ gatherSecond WRITE setGatherSecond)
	Q_PROPERTY(QVariant gatherWeek READ gatherWeek WRITE setGatherWeek)

	Q_PROPERTY(QVariant premiumGasolinYn READ premiumGasolinYn WRITE setPremiumGasolinYn)
	Q_PROPERTY(QVariant lowestOilYn READ lowestOilYn WRITE setLowestOilYn)
	Q_PROPERTY(QVariant opiGasolinePrice READ opiGasolinePrice WRITE setOpiGasolinePrice)
	Q_PROPERTY(QVariant opiDieselPrice READ opiDieselPrice WRITE setOpiDieselPrice)
	Q_PROPERTY(QVariant opiLpgPrice READ opiLpgPrice WRITE setOpiLpgPrice)
	Q_PROPERTY(QVariant opiHiGasolinePrice READ opiHiGasolinePrice WRITE setOpiHiGasolinePrice)
	Q_PROPERTY(QVariant opiHiLpgPrice READ opiHiLpgPrice WRITE setOpiHiLpgPrice)
	Q_PROPERTY(QVariant navSeq READ navSeq WRITE setNavSeq)
	Q_PROPERTY(QVariant oilCompany READ oilCompany WRITE setOilCompany)
	
    Q_PROPERTY(QVariant poiIdSeq READ poiIdSeq WRITE setPoiIdSeq) // POIID + 입구점번호 

	QVariant id() const;

    QVariant fullName() const;
    void setFullName(const QVariant& value);

    QVariant stdName() const;
    void setStdName(const QVariant& value);

    QVariant address() const;
    void setAddress(const QVariant& value);

    QVariant phoneNumber() const;
    void setPhoneNumber(const QVariant& value);

    QVariant poiId() const;
    void setPoiId(const QVariant& value);

    QVariant poiIdSeq() const;
    void setPoiIdSeq(const QVariant& value);

    QVariant addrCode() const;
    void setAddrCode(const QVariant& value);

    QVariant rpFlag() const;
    void setRpFlag(const QVariant& value);

    QVariant distance() const;
    void setDistance(const QVariant& value);

    QVariant degree() const;
    void setDegree(const QVariant& value);

    QVariant kind() const;
    void setKind(const QVariant& value);

    QVariant tpX() const;
    void setTpX(const QVariant& value);

    QVariant tpY() const;
    void setTpY(const QVariant& value);

    QVariant opiPoleCate() const;
    void setOpiPoleCate(const QVariant& value);

    QVariant opiPolePrice() const;
    void setOpiPolePrice(const QVariant& value);

    QVariant gatherYear() const;
    void setGatherYear(const QVariant& value);
    QVariant gatherMonth() const;
    void setGatherMonth(const QVariant& value);
    QVariant gatherDay() const;
    void setGatherDay(const QVariant& value);
    QVariant gatherHour() const;
    void setGatherHour(const QVariant& value);
    QVariant gatherMinute() const;
    void setGatherMinute(const QVariant& value);
    QVariant gatherSecond() const;
    void setGatherSecond(const QVariant& value);
    QVariant gatherWeek() const;
    void setGatherWeek(const QVariant& value);

    QVariant premiumGasolinYn() const;
    void setPremiumGasolinYn(const QVariant& value);
    QVariant lowestOilYn() const;
    void setLowestOilYn(const QVariant& value);
    QVariant opiGasolinePrice() const;
    void setOpiGasolinePrice(const QVariant& value);
    QVariant opiDieselPrice() const;
    void setOpiDieselPrice(const QVariant& value);
    QVariant opiLpgPrice() const;
    void setOpiLpgPrice(const QVariant& value);
    QVariant opiHiGasolinePrice() const;
    void setOpiHiGasolinePrice(const QVariant& value);
    QVariant opiHiLpgPrice() const;
    void setOpiHiLpgPrice(const QVariant& value);
    QVariant navSeq() const;
    void setNavSeq(const QVariant& value);
    QVariant oilCompany() const;
    void setOilCompany(const QVariant& value);

private:
    QVariant mId;
    QVariant mFullName;
    QVariant mStdName;
    QVariant mAddress;
    QVariant mPhoneNumber;
    QVariant mPoiId;
    QVariant mPoiIdSeq;
    QVariant mAddrCode;
    QVariant mRpFlag;
    QVariant mDistance;
    QVariant mDegree;
    QVariant mKind;
    QVariant mTpX;
    QVariant mTpY;
    QVariant mOpiPoleCate;
    QVariant mOpiPolePrice;
    QVariant mGatherYear;
    QVariant mGatherMonth;
    QVariant mGatherDay;
    QVariant mGatherHour;
    QVariant mGatherMinute;
    QVariant mGatherSecond;
    QVariant mGatherWeek;
    QVariant mPremiumGasolinYn;
    QVariant mLowestOilYn;
    QVariant mOpiGasolinePrice;
    QVariant mOpiDieselPrice;
    QVariant mOpiLpgPrice;
    QVariant mOpiHiGasolinePrice;
    QVariant mOpiHiLpgPrice;
    QVariant mNavSeq;
    QVariant mOilCompany;
};

}
#endif // SEARCHCACHEDTO_H
