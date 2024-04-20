#include "SearchCacheDto.h"

namespace SKT {

SearchCacheDto::SearchCacheDto(QObject* parent):
		QObject(parent)
{

}

QVariant SearchCacheDto::id() const
{
	return mId;
}

QVariant SearchCacheDto::fullName() const
{
	return mFullName;
}

void SearchCacheDto::setFullName(const QVariant& value)
{
	mFullName = value;
}

QVariant SearchCacheDto::stdName() const
{
	return mStdName;
}

void SearchCacheDto::setStdName(const QVariant& value)
{
	mStdName = value;
}

QVariant  SearchCacheDto::address() const
{
	return mAddress;
}

void SearchCacheDto::setAddress(const QVariant& value)
{
	mAddress = value;
}

QVariant SearchCacheDto::phoneNumber() const
{
	return mPhoneNumber;
}

void SearchCacheDto::setPhoneNumber(const QVariant& value)
{
	mPhoneNumber = value;
}

QVariant SearchCacheDto::poiId() const
{
	return mPoiId;
}

void SearchCacheDto::setPoiId(const QVariant& value)
{
	mPoiId = value;
}

QVariant SearchCacheDto::poiIdSeq() const
{
	return mPoiIdSeq;
}

void SearchCacheDto::setPoiIdSeq(const QVariant& value)
{
	mPoiIdSeq = value;
}

QVariant SearchCacheDto::addrCode() const
{
	return mAddrCode;
}

void SearchCacheDto::setAddrCode(const QVariant& value)
{
	mAddrCode = value;
}

QVariant SearchCacheDto::rpFlag() const
{
	return mRpFlag;
}

void SearchCacheDto::setRpFlag(const QVariant& value)
{
	mRpFlag = value;
}

QVariant SearchCacheDto::distance() const
{
	return mDistance;
}

void SearchCacheDto::setDistance(const QVariant& value)
{
	mDistance = value;
}

QVariant SearchCacheDto::degree() const
{
	return mDegree;
}

void SearchCacheDto::setDegree(const QVariant& value)
{
	mDegree = value;
}

QVariant SearchCacheDto::kind() const
{
	return mKind;
}

void SearchCacheDto::setKind(const QVariant& value)
{
	mKind = value;
}

QVariant SearchCacheDto::tpX() const
{
	return mTpX;
}

void SearchCacheDto::setTpX(const QVariant& value)
{
	mTpX = value;
}

QVariant SearchCacheDto::tpY() const
{
	return mTpY;
}

void SearchCacheDto::setTpY(const QVariant& value)
{
	mTpY = value;
}

QVariant SearchCacheDto::opiPoleCate() const
{
	return mOpiPoleCate;
}

void SearchCacheDto::setOpiPoleCate(const QVariant& value)
{
	mOpiPoleCate = value;
}

QVariant SearchCacheDto::opiPolePrice() const
{
	return mOpiPolePrice;
}

void SearchCacheDto::setOpiPolePrice(const QVariant& value)
{
	mOpiPolePrice = value;
}

QVariant SearchCacheDto::gatherYear() const
{
	return mGatherYear;
}

void SearchCacheDto::setGatherYear(const QVariant& value)
{
	mGatherYear = value;
}

QVariant SearchCacheDto::gatherMonth() const
{
	return mGatherMonth;
}

void SearchCacheDto::setGatherMonth(const QVariant& value)
{
	mGatherMonth = value;
}

QVariant SearchCacheDto::gatherDay() const
{
	return mGatherDay;
}

void SearchCacheDto::setGatherDay(const QVariant& value)
{
	mGatherDay = value;
}

QVariant SearchCacheDto::gatherHour() const
{
	return mGatherHour;
}

void SearchCacheDto::setGatherHour(const QVariant& value)
{
	mGatherHour = value;
}

QVariant SearchCacheDto::gatherMinute() const
{
	return mGatherMinute;
}

void SearchCacheDto::setGatherMinute(const QVariant& value)
{
	mGatherMinute = value;
}

QVariant SearchCacheDto::gatherSecond() const
{
	return mGatherSecond;
}

void SearchCacheDto::setGatherSecond(const QVariant& value)
{
	mGatherSecond = value;
}

QVariant SearchCacheDto::gatherWeek() const
{
	return mGatherWeek;
}

void SearchCacheDto::setGatherWeek(const QVariant& value)
{
	mGatherWeek = value;
}

QVariant SearchCacheDto::premiumGasolinYn() const
{
	return mPremiumGasolinYn;
}

void SearchCacheDto::setPremiumGasolinYn(const QVariant& value)
{
	mPremiumGasolinYn = value;
}

QVariant SearchCacheDto::lowestOilYn() const
{
	return mLowestOilYn;
}

void SearchCacheDto::setLowestOilYn(const QVariant& value)
{
	mLowestOilYn = value;
}

QVariant SearchCacheDto::opiGasolinePrice() const
{
	return mOpiGasolinePrice;
}

void SearchCacheDto::setOpiGasolinePrice(const QVariant& value)
{
	mOpiGasolinePrice = value;
}

QVariant SearchCacheDto::opiDieselPrice() const
{
	return mOpiDieselPrice;
}

void SearchCacheDto::setOpiDieselPrice(const QVariant& value)
{
	mOpiDieselPrice = value;
}

QVariant SearchCacheDto::opiLpgPrice() const
{
	return mOpiLpgPrice;
}

void SearchCacheDto::setOpiLpgPrice(const QVariant& value)
{
	mOpiLpgPrice = value;
}

QVariant SearchCacheDto::opiHiGasolinePrice() const
{
	return mOpiHiGasolinePrice;
}

void SearchCacheDto::setOpiHiGasolinePrice(const QVariant& value)
{
	mOpiHiGasolinePrice = value;
}

QVariant SearchCacheDto::opiHiLpgPrice() const
{
	return mOpiHiLpgPrice;
}

void SearchCacheDto::setOpiHiLpgPrice(const QVariant& value)
{
	mOpiHiLpgPrice = value;
}

QVariant SearchCacheDto::navSeq() const
{
	return mNavSeq;
}

void SearchCacheDto::setNavSeq(const QVariant& value)
{
	mNavSeq = value;
}

QVariant SearchCacheDto::oilCompany() const
{
	return mOilCompany;
}

void SearchCacheDto::setOilCompany(const QVariant& value)
{
	mOilCompany = value;
}

}
