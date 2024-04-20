#include "TInsertBuilder.h"
#include "TDao.h"
#include "TTableInfo.h"
#include "TQuery.h"

using namespace SKT;
using namespace SKT::orm;

TInsertBuilder::TInsertBuilder( bool insertOrReplace )
    : mIdentityInsert( false ),
    mInsertOrReplace( insertOrReplace )
{
}

TInsertBuilder::~TInsertBuilder()
{
}

void TInsertBuilder::onComposeWith( const TTableInfo* tableInfo )
{
	mTable = tableInfo->tableName();

	const QList<TFieldInfoPtr>& fieldInfos = tableInfo->fieldInfos();
	for ( int i = 0, pos = 0, count = fieldInfos.count(); i < count; i++ ) {
		const TFieldInfoPtr& fi = fieldInfos.at( i );
		if ( !mIdentityInsert && !fi->isFieldCreatable() ) {
			continue;
		}
		column( fi->name() );
	}
}

TInsertBuilder& TInsertBuilder::table( const QString& table )
{
	mTable = table;

	return *this;
}

TInsertBuilder& TInsertBuilder::column( const QString& column )
{
	mColumns.append( column );
	mHolders.append( ":" + column );

	return *this;
}

QString TInsertBuilder::build() const
{
	QString ret;

    if ( !mInsertOrReplace ) {
        ret.append( QStringLiteral("insert into "));
    }
    else {
        ret.append( QStringLiteral("insert or replace into "));
    }

	ret.append( mTable );
    ret.append( QStringLiteral("( ") );
	ret.append( mColumns.join( ',' ) );
    ret.append( QStringLiteral(")") );
    ret.append( QStringLiteral("values") );
    ret.append( QStringLiteral("( ") );
	ret.append( mHolders.join( ',' ) );
    ret.append( QStringLiteral(")") );

	return ret;
}

void TInsertBuilder::setIdentityInsert( bool identityInsert )
{
	mIdentityInsert = identityInsert;
}
