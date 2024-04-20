#include "TDeleteBuilder.h"
#include "TDao.h"
#include "TTableInfo.h"
#include "TQuery.h"

using namespace SKT;
using namespace SKT::orm;

TDeleteBuilder::TDeleteBuilder()
{
}

TDeleteBuilder::~TDeleteBuilder()
{
}

void TDeleteBuilder::onComposeWith( const TTableInfo* tableInfo )
{
	mTable = tableInfo->tableName();

	/*
	for ( int i = 0, pos = 0, count = tableInfo->fieldInfos().count(); i < count; i++ ) {
		const TFieldInfoPtr& fi = tableInfo->fieldInfos().at( i );
		if ( !fi->isFieldCreatable() ) {
			continue;
		}
		column( fi->name() );
	}
	*/
}

TDeleteBuilder& TDeleteBuilder::table( const QString& table )
{
	mTable = table;

	return *this;
}

TDeleteBuilder& TDeleteBuilder::where( const QString& where )
{
	mWhere = where;

	return *this;
}

QString TDeleteBuilder::build() const
{
	QString ret;

    ret.append( QStringLiteral("delete from "));
	ret.append( mTable );
	
	if ( !mWhere.isEmpty() ) {
        ret.append( QStringLiteral(" where ") );
		ret.append( mWhere );
	}

	return ret;
}
