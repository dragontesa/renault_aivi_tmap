#include "TFieldInfo.h"

using namespace SKT;
using namespace SKT::orm;

TFieldInfo::TFieldInfo()
	: mIsIdentity( false ),
	mIsForeign( false ),
	mIsNotNull( false ),
	mType()
{
}

TFieldInfo::~TFieldInfo()
{
}

QString TFieldInfo::name() const
{
	return mName;
}

void TFieldInfo::setName( const QString& name )
{
	mName = name;
}

QString TFieldInfo::typeName() const
{
	return mTypeName;
}

void TFieldInfo::setTypeName( const QString& typeName )
{
	mTypeName = typeName;
}

QVariant::Type TFieldInfo::type() const
{
	return mType;
}

void TFieldInfo::setType( QVariant::Type type )
{
	mType = type;
}

bool TFieldInfo::isIdentity() const
{
	return mIsIdentity;
}

void TFieldInfo::setIdentity( bool identity )
{
	mIsIdentity = identity;
}

bool TFieldInfo::isForeign() const
{
	return mIsForeign;
}

void TFieldInfo::setForeign( bool foreign )
{
	mIsForeign = foreign;
}

void TFieldInfo::setNotNull( bool notnull )
{
	mIsNotNull = notnull;
}

bool TFieldInfo::isNotNull() const
{
	return mIsNotNull;
}

QString TFieldInfo::foreignTable() const
{
	return mForeignTable;
}

void TFieldInfo::setForeignTable( const QString& table )
{
	mForeignTable = table;
}

QString TFieldInfo::foreignColumn() const
{
	return mForeignColumn;
}

void TFieldInfo::setForeignColumn( const QString& column )
{
	mForeignColumn = column;
}

bool TFieldInfo::isFieldCreatable() const
{
	bool ret = true;

	if ( mIsIdentity ) {
		ret = false;
	}

	return ret;
}
