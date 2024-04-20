#include "TSelectBuilder.h"
#include "TDao.h"
#include "TTableInfo.h"
#include "TQuery.h"

using namespace SKT;
using namespace SKT::orm;

TSelectBuilder::TSelectBuilder()
{
}

TSelectBuilder::~TSelectBuilder()
{
}

void TSelectBuilder::onComposeWith( const TTableInfo* tableInfo )
{
	mTable = tableInfo->tableName();

	for ( int i = 0, pos = 0, count = tableInfo->fieldInfos().count(); i < count; i++ ) {
		const TFieldInfoPtr& fi = tableInfo->fieldInfos().at( i );
		column( fi->name() );
	}
}

TSelectBuilder& TSelectBuilder::table( const QString& table )
{
	mTable = table;

	return *this;
}

TSelectBuilder& TSelectBuilder::column( const QString& column )
{
	mColumns.append( column );

	return *this;
}

TSelectBuilder& TSelectBuilder::column( const TSelectBuilderPtr& sb, const QString column, const QString& asName )
{
	QString temp;
	
    temp.append( QStringLiteral("(") );

	temp.append( sb->buildAsColumnSubQuery( column ) );
	
    temp.append( QStringLiteral(") ") );
    temp.append( QStringLiteral("as ") );
	temp.append( asName );

	mColumns.append( temp );

	return *this;
}

TSelectBuilder& TSelectBuilder::removeColumns()
{
	mColumns.clear();

	return *this;
}

TSelectBuilder& TSelectBuilder::top( int top )
{
	mTop.setValue( top );

	return *this;
}

TSelectBuilder& TSelectBuilder::limitN(int n)
{
	mLimitN = n;
	return *this;
}

TSelectBuilder& TSelectBuilder::limitOffset(int offset)
{
	mLimitOffset = offset;
	return *this;
}

TSelectBuilder& TSelectBuilder::where( const QString& where )
{
	mWhere = where;

	return *this;
}

TSelectBuilder& TSelectBuilder::orderBy( const QString& orderBy )
{
	mOrderBy = orderBy;

	return *this;
}

QString TSelectBuilder::build() const
{
	QString ret;

    ret.append( QStringLiteral("select "));

	if ( !mTop.isNull() ) {
        ret.append( QString( QStringLiteral("top %1 ")).arg( mTop.toInt() ) );
	}

	ret.append( mColumns.join( ',' ) );

    ret.append( QStringLiteral(" from "));
	ret.append( mTable );

	if ( !mWhere.isEmpty() ) {
        ret.append( QStringLiteral(" where ") );
		ret.append( mWhere );
	}

	if ( !mOrderBy.isEmpty() ) {
        ret.append( QStringLiteral(" order by ") );
		ret.append( mOrderBy );
	}

	if ( !mLimitN.isNull() )
	{
        ret.append(QStringLiteral(" limit "));
		if ( !mLimitOffset.isNull() ) {
			ret.append( QString("%1, ").arg(mLimitOffset.toInt()));
		}

        ret.append(QString(QStringLiteral("%1")).arg(mLimitN.toInt()));
	}

	return ret;
}

QString TSelectBuilder::buildAsColumnSubQuery( const QString& column ) const
{
	QString ret;

    ret.append( QStringLiteral("select "));
	ret.append( column );

    ret.append( QStringLiteral(" from "));
	ret.append( mTable );

    ret.append( QStringLiteral(" where ") );
	ret.append( mWhere );

	return ret;
}
