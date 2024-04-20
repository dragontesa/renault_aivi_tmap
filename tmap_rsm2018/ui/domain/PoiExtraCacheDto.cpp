#include "PoiExtraCacheDto.h"

/*
 * PoiExtraCacheDto.cpp
 *
 */


namespace SKT
{

PoiExtraCacheDto::PoiExtraCacheDto(QObject* parent):
		QObject(parent)
{

}

PoiExtraCacheDto::~PoiExtraCacheDto()
{
}

QVariant PoiExtraCacheDto::id() const
{
	return mId;
}

QVariant PoiExtraCacheDto::poiId() const
{
	return mPoiId;
}

void PoiExtraCacheDto::setPoiId(const QVariant& value)
{
	mPoiId = value;
}

QVariant PoiExtraCacheDto::subGroupList() const
{
	return mSubGroupList;
}

void PoiExtraCacheDto::setSubGroupList(const QVariant& value)
{
	mSubGroupList = value;
}

QVariant PoiExtraCacheDto::advContents() const
{
	return mAdvContents;
}

void PoiExtraCacheDto::setAdvContents(const QVariant& value)
{
	mAdvContents = value;
}


#if 0
QVariant PoiExtraCacheDto::poiImageNum() const
{
	return mPoiImageNum;
}

void PoiExtraCacheDto::setPoiImageNum(const QVariant& value) const
{
	mPoiImageNum = value;
}

QVariant PoiExtraCacheDto::poiParkNum() const
{
	return mPoiParkNum;
}


void PoiExtraCacheDto::setPoiParkNum(const QVariant& value)
{
	mPoiParkNum = value;
}

QVariant PoiExtraCacheDto::bizImageSize() const
{
	return mBizImageSize;
}

void PoiExtraCacheDto::setBizImageSize(QVariant& value)
{

}

QVariant PoiExtraCacheDto::waytoGo() const
{
	return mWaytoGo;
}

void PoiExtraCacheDto::setWaytoGo(const QVariant& value)
{
	mWaytoGo = value;
}

QVariant PoiExtraCacheDto::subInfo() const
{
	return mSubInfo;
}

void PoiExtraCacheDto::setSubInfo(const QVariant& value)
{
	mSubInfo = value;
}

QVariant PoiExtraCacheDto::introduction() const
{
	return mIntroduction;
}

void PoiExtraCacheDto::setIntroduction(const QVariant& value)
{
	mIntroduction = value;
}

QVariant PoiExtraCacheDto::bizImage() const
{
	return mBizImage;
}

void PoiExtraCacheDto::setBizImage(const QVariant& value)
{
	mBizImage = value;
}

QVariant PoiExtraCacheDto::poiImageInfo() const
{
	return mPoiImageInfo;
}

void PoiExtraCacheDto::setPoiImageInfo(const QVariant& value)
{
	mPoiImageInfo = value;
}

QVariant PoiExtraCacheDto::poiParkInfo() const
{
	return mPoiParkInfo;
}

void PoiExtraCacheDto::setPoiParkInfo(const QVariant& value)
{
	mPoiParkInfo = value;
}

QVariant PoiExtraCacheDto::menu1() const
{
	return mMenu1;
}

void PoiExtraCacheDto::setMenu1(const QVariant& value)
{
	mMenu1 = value;
}

QVariant PoiExtraCacheDto::menu2() const
{
	return mMenu2;
}

void PoiExtraCacheDto::setMenu2(const QVariant& value)
{
	mMenu2 = value;
}

QVariant PoiExtraCacheDto::menu3() const
{
	return mMenu3;
}

void PoiExtraCacheDto::setMenu3(const QVariant& value)
{
	mMenu3 = value;
}


QVariant PoiExtraCacheDto::menu4() const
{
	return mMenu4;
}

void PoiExtraCacheDto::setMenu4(const QVariant& value)
{
	mMenu4 = value;
}


QVariant PoiExtraCacheDto::menu5() const
{
	return mMenu5;
}

void PoiExtraCacheDto::setMenu5(const QVariant& value)
{
	mMenu5 = value;
}
#endif
} /* namespace SKT */
