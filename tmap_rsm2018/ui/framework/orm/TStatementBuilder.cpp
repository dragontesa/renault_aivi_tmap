#include "TStatementBuilder.h"
#include "TDao.h"

using namespace SKT;
using namespace SKT::orm;

TStatementBuilder::TStatementBuilder()
	: mTableInfo( 0 )
{
}

TStatementBuilder::~TStatementBuilder()
{
}

void TStatementBuilder::composeWith( const TTableInfo* tableInfo )
{
	mTableInfo = tableInfo;

	onComposeWith( mTableInfo );
}

void TStatementBuilder::composeWith( const QObject* dto )
{
	onComposeWith( *dto );
}

void TStatementBuilder::composeWith( const QObject& dto )
{
	onComposeWith( dto );
}

TQueryPtr TStatementBuilder::buildQuery( const QSqlDatabase& db ) 
{
	return TQueryPtr( new TQuery( db, build() ) );
}

