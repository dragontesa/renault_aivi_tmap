#include "AddressSearchResultVo.h"

using namespace SKT;

AddressSearchResultVo::AddressSearchResultVo()
{

}

AddressSearchResultVo::AddressSearchResultVo(
    const QString& code,
    const QString& name,
    int jibunStIdx,
    int jibunCnt,
    int aptStIdx,
    short aptCnt,
    int newAddrStIdx,
    int newAddrCnt,
    int jibunRicode
) : mCode( code ),
    mName( name ),
    mJibunStIdx( jibunStIdx ),
    mJibunCnt( jibunCnt ),
    mAptStIdx( aptStIdx ),
    mAptCnt( aptCnt ),
    mNewAddrStIdx( newAddrStIdx ),
    mNewAddrCnt( newAddrCnt ),
    mJibunRicode (jibunRicode)
{
}

AddressSearchResultVo::AddressSearchResultVo( const AddressSearchResultVo& other )
{
    mCode = other.mCode;
    mName = other.mName;
    mJibunStIdx = other.mJibunStIdx;
    mJibunCnt = other.mJibunCnt;
    mAptStIdx = other.mAptStIdx;
    mAptCnt = other.mAptCnt;
    mNewAddrStIdx = other.mNewAddrStIdx;
    mNewAddrCnt = other.mNewAddrCnt;
    mJibunRicode = other.mJibunRicode;

}

QString AddressSearchResultVo::code() const
{
    return mCode;
}

void AddressSearchResultVo::setCode( const QString& value )
{
    mCode = value;
}

QString AddressSearchResultVo::name() const
{
    return mName;
}

void AddressSearchResultVo::setName( const QString& value )
{
    mName = value;
}

int AddressSearchResultVo::jibunStIdx() const
{
    return mJibunStIdx;
}

int AddressSearchResultVo::jibunCnt() const
{
    return mJibunCnt;
}

int AddressSearchResultVo::aptStIdx() const
{
    return mAptStIdx;
}

short AddressSearchResultVo::aptCnt() const
{
    return mAptCnt;
}

int AddressSearchResultVo::newAddrStIdx() const
{
    return mNewAddrStIdx;
}

int AddressSearchResultVo::newAddrCnt() const
{
    return mNewAddrCnt;
}

int AddressSearchResultVo::jibunRicode() const
{
    return mJibunRicode;
}

void AddressSearchResultVo::setJibunStIdx( int value )
{
    mJibunStIdx = value;
}

void AddressSearchResultVo::setJibunCnt( int value )
{
    mJibunCnt = value;
}

void AddressSearchResultVo::setAptStIdx( int value )
{
    mAptStIdx = value;
}

void AddressSearchResultVo::setAptCnt( short value )
{
    mAptCnt = value;
}

void AddressSearchResultVo::setNewAddrStIdx( int value )
{
    mNewAddrStIdx = value;
}

void AddressSearchResultVo::setNewAddrCnt( int value )
{
    mNewAddrCnt = value;
}

void AddressSearchResultVo::setJibunRicode( int value )
{
    mJibunRicode = value;
}
