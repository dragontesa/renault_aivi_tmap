/*
 * PoiCacheDto.cpp
 *
  */

#include "PoiCacheDto.h"

namespace SKT {

PoiCacheDto::PoiCacheDto(QObject* parent) :
		QObject(parent)
{

}

PoiCacheDto::~PoiCacheDto()
{

}

QVariant PoiCacheDto::id() const
{
	return mId;
}

QVariant PoiCacheDto::poiId() const
{
	return mPoiId;
}

void PoiCacheDto::setPoiId(const QVariant& value)
{
	mPoiId = value;
}

QVariant PoiCacheDto::dbKind() const
{
	return mDbKind;
}
void PoiCacheDto::setDbKind(const QVariant& value)
{
	mDbKind = value;
}

QVariant PoiCacheDto::navSeq() const
{
	return mNavSeq;
}

void PoiCacheDto::setNavSeq(const QVariant& value)
{
	mNavSeq = value;
}

QVariant PoiCacheDto::navX() const
{
	return mNavX;
}

void PoiCacheDto::setNavX(const QVariant& value)
{
	mNavX = value;
}

QVariant PoiCacheDto::navY() const
{
	return mNavY;
}

void PoiCacheDto::setNavY(const QVariant& value)
{
	mNavY = value;
}

QVariant PoiCacheDto::centerX() const
{
	return mCenterX;
}

void PoiCacheDto::setCenterX(const QVariant& value)
{
	mCenterX = value;
}

QVariant PoiCacheDto::centerY() const
{
	return mCenterY;
}

void PoiCacheDto::setCenterY(const QVariant& value)
{
	mCenterY = value;
}

QVariant PoiCacheDto::rpFlag() const
{
	return mRpFlag;
}

void PoiCacheDto::setRpFlag(const QVariant& value)
{
	mRpFlag = value;
}

QVariant PoiCacheDto::dataKind() const
{
	return mDataKind;
}

void PoiCacheDto::setDataKind(const QVariant& value)
{
	mDataKind = value;
}

QVariant PoiCacheDto::mlClass() const
{
	return mMlClass;
}

void PoiCacheDto::setMlClass(const QVariant& value)
{
	mMlClass = value;
}

QVariant PoiCacheDto::parkYn() const
{
	return mParkYn;
}

void PoiCacheDto::setParkYn(const QVariant& value)
{
	mParkYn = value;
}


QVariant PoiCacheDto::premiumGasolinYn() const
{
	return mPremiumGasolinYn;
}

void PoiCacheDto::setPremiumGasolinYn(const QVariant& value)
{
	mPremiumGasolinYn = value;
}

QVariant PoiCacheDto::lowestOilYn() const
{
	return mLowestOilYn;
}

void PoiCacheDto::setLowestOilYn(const QVariant& value)
{
	mLowestOilYn = value;
}

QVariant PoiCacheDto::gasolinPrice() const
{
	return mGasolinPrice;
}

void PoiCacheDto::setGasolinPrice(const QVariant& value)
{
	mGasolinPrice = value;
}

QVariant PoiCacheDto::dieselPrice() const
{
	return mDieselPrice;
}

void PoiCacheDto::setDieselPrice(const QVariant& value)
{
	mDieselPrice = value;
}

QVariant PoiCacheDto::lpgPrice() const
{
	return mLpgPrice;
}

void PoiCacheDto::setLpgPrice(const QVariant& value)
{
	mLpgPrice = value;
}

QVariant PoiCacheDto::premiumGasolinPrice() const
{
	return mPremiumGasolinPrice;
}

void PoiCacheDto::setPremiumGasolinPrice(const QVariant& value)
{
	mPremiumGasolinPrice = value;
}

QVariant PoiCacheDto::premiumDieselPrice() const
{
	return mPremiumDieselPrice;
}

void PoiCacheDto::setPremiumDieselPrice(const QVariant& value)
{
	mPremiumDieselPrice = value;
}

QVariant PoiCacheDto::premiumLpgPrice() const
{
	return mPremiumLpgPrice;
}

void PoiCacheDto::setPremiumLpgPrice(const QVariant& value)
{
	mPremiumLpgPrice = value;
}

QVariant PoiCacheDto::oilBaseSdt() const
{
	return mOilBaseSdt;
}

void PoiCacheDto::setOilBaseSdt(const QVariant& value)
{
	mOilBaseSdt = value;
}

QVariant PoiCacheDto::oilCompany() const
{
	return mOilCompany;
}

void PoiCacheDto::setOilCompany(const QVariant& value)
{
	mOilCompany = value;
}


QVariant PoiCacheDto::fullName() const
{
	return mFullName;
}

void PoiCacheDto::setFullName(const QVariant& value)
{
	mFullName = value;
}

QVariant PoiCacheDto::aliasName() const
{
	return mAliasName;
}

void PoiCacheDto::setAliasName(const QVariant& value)
{
	mAliasName = value;
}


QVariant PoiCacheDto::address() const
{
	return mAddress;
}

void PoiCacheDto::setAddress(const QVariant& value)
{
	mAddress = value;
}


QVariant PoiCacheDto::primaryBun() const
{
	return mPrimaryBun;
}

void PoiCacheDto::setPrimaryBun(const QVariant& value)
{
	mPrimaryBun = value;
}

QVariant PoiCacheDto::secondaryBun() const
{
	return mSecondaryBun;
}

void PoiCacheDto::setSecondaryBun(const QVariant& value)
{
	mSecondaryBun = value;
}

QVariant PoiCacheDto::lcd() const
{
	return mLcd;
}

void PoiCacheDto::setLcd(const QVariant& value)
{
	mLcd = value;
}

QVariant PoiCacheDto::mcd() const
{
	return mMcd;
}

void PoiCacheDto::setMcd(const QVariant& value)
{
	mMcd = value;
}

QVariant PoiCacheDto::scd() const
{
	return mScd;
}

void PoiCacheDto::setScd(const QVariant& value)
{
	mScd = value;
}

QVariant PoiCacheDto::dcd() const
{
	return mDcd;
}

void PoiCacheDto::setDcd(const QVariant& value)
{
	mDcd = value;
}

QVariant PoiCacheDto::zipCode() const
{
	return mZipCode;
}

void PoiCacheDto::setZipCode(const QVariant& value)
{
	mZipCode = value;
}

QVariant PoiCacheDto::telNum() const
{
	return mTelNum;
}

void PoiCacheDto::setTelNum(const QVariant& value)
{
	mTelNum = value;
}


QVariant PoiCacheDto::website() const
{
	return mWebsite;
}

void PoiCacheDto::setWebsite(const QVariant& value)
{
	mWebsite = value;
}

QVariant PoiCacheDto::radius() const
{
	return mRadius;
}

void PoiCacheDto::setRadius(const QVariant& value)
{
	mRadius = value;
}

QVariant PoiCacheDto::roadName() const
{
	return mRoadName;
}

void PoiCacheDto::setRoadName(const QVariant& value)
{
	mRoadName = value;
}

QVariant PoiCacheDto::bldNo1() const
{
	return mBldNo1;
}

void PoiCacheDto::setBldNo1(const QVariant& value)
{
	mBldNo1 = value;
}


QVariant PoiCacheDto::bldNo2() const
{
	return mBldNo2;
}


void PoiCacheDto::setBldNo2(const QVariant& value)
{
	mBldNo2 = value;
}

QVariant PoiCacheDto::pubTraffic() const
{
	return mPubTraffic;
}

void PoiCacheDto::setPubTraffic(const QVariant& value)
{
	mPubTraffic = value;
}

QVariant PoiCacheDto::stationId() const
{
	return mStationId;
}

void PoiCacheDto::setStationId(const QVariant& value)
{
	mStationId = value;
}

}
