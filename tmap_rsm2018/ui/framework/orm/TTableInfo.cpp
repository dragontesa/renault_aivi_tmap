#include "TTableInfo.h"

using namespace SKT;
using namespace SKT::orm;

TTableInfo::TTableInfo()
	: mIdField( 0 )
{
}

TTableInfo::~TTableInfo()
{
}

QString TTableInfo::tableName() const
{
	return mTableName;
}
	
void TTableInfo::setTableName( const QString& tableName )
{
	mTableName = tableName;
}

const TFieldInfo* TTableInfo::idField() const
{
	return mIdField;
}

const QList<TFieldInfoPtr>& TTableInfo::fieldInfos() const
{
	return mFieldInfos;
}

void TTableInfo::addFieldInfo( TFieldInfo* fieldInfo )
{
	if ( fieldInfo->isIdentity() ) {
		mIdField = fieldInfo;
	}
	mFieldInfos.append( TFieldInfoPtr( fieldInfo ) );
}

const TFieldInfo* TTableInfo::getFieldInfoByColumnName( const QString& columnName ) const
{
	const TFieldInfo* ret = 0;

	TTableInfo* thisPtr = const_cast<TTableInfo*>( this );

	if ( mFieldNameMap.isEmpty() ) {
		for ( int i = 0, count = mFieldInfos.size(); i < count; i++ ) {
			auto fi = mFieldInfos.at( i );
			thisPtr->mFieldNameMap.insert( fi->name(), fi.data() );
		}
	}

	ret = mFieldNameMap[columnName];

	if ( ret == 0 ) {
        T_DEBUG( QString( "unknown column name '%1'" ).arg( columnName ) );
	}

	return ret;
}

bool TTableInfo::isForeignIdField( const TFieldInfo* foreignField ) const
{
	bool ret = false;

	if ( foreignField->foreignTable() == mTableName ) {
		if ( foreignField->foreignColumn() == mIdField->name() ) {
			ret = true;
		}
	}

	return ret;
}
