#include "TUpdateBuilder.h"
#include "TDao.h"
#include "TTableInfo.h"
#include "TQuery.h"
#include "TOrm.h"

using namespace SKT;
using namespace SKT::orm;

TUpdateBuilder::TUpdateBuilder()
{
}

TUpdateBuilder::~TUpdateBuilder()
{
}

void TUpdateBuilder::onComposeWith( const TTableInfo* tableInfo )
{
	mTable = tableInfo->tableName();

	for ( int i = 0, pos = 0, count = tableInfo->fieldInfos().count(); i < count; i++ ) {
		const TFieldInfoPtr& fi = tableInfo->fieldInfos().at( i );
		if ( !fi->isFieldCreatable() ) {
			continue;
		}
		column( fi->name() );
	}
}

void TUpdateBuilder::onComposeWith( const QObject& dto )
{
	auto tableInfo = TOrm::instance()->tableInfo( dto.metaObject() );
	mTable = tableInfo->tableName();

	for ( int i = 0, pos = 0, count = tableInfo->fieldInfos().count(); i < count; i++ ) {
		const TFieldInfoPtr& fi = tableInfo->fieldInfos().at( i );
		if ( !fi->isFieldCreatable() ) {
			continue;
		}

		auto var = dto.property( fi->name().T_CSTR() );
		if ( !var.isNull() ) {
			column( fi->name() );
		}		
	}
}

TUpdateBuilder& TUpdateBuilder::table( const QString& table )
{
	mTable = table;

	return *this;
}

TUpdateBuilder& TUpdateBuilder::column( const QString& column )
{
	mColumns.append( column );
	
	return *this;
}

TUpdateBuilder& TUpdateBuilder::where( const QString& where )
{
	mWhere = where;

	return *this;
}

QString TUpdateBuilder::build() const
{
	QString ret;

    ret.append( QStringLiteral("update "));
	ret.append( mTable );
    ret.append( QStringLiteral(" set "));

	for ( int i = 0, count = mColumns.count(); i < count; i++ ) {
		if ( i > 0 ) {
            ret.append( QStringLiteral(",") );
		}

		QString column = mColumns.at( i );

		ret.append( column);
        ret.append( QStringLiteral(" = :") );
		ret.append( column);
	}

	if ( !mWhere.isEmpty() ) {
        ret.append( QStringLiteral(" where ") );
		ret.append( mWhere );
	}

	return ret;
}
