#include "TOrm.h"
#include "TObjectUtils.h"
#include "TDao.h"
#include "TTableInfo.h"
#include "TStopwatch.h"

using namespace SKT;
using namespace SKT::orm;

QSqlQuery TOrm::query( const QSqlDatabase& db, const QString& stmt )
{
	QSqlQuery ret( db );

	if ( TOrm::instance()->enableLog() ) {
		T_DEBUG( stmt );
	}
	T_SCOPED_STOPWATCH( t1 ) {
		if ( !ret.exec( stmt ) ) {
            T_DEBUG( QString( "[sql]%1 [error]%2" )
                .arg( stmt )
                .arg( ret.lastError().text() ) );
		}
	}
	if ( TOrm::instance()->enableLog() ) {
        T_DEBUG( QString( "query elapsed %1" ).arg( t1 ) );
	}

	return ret;
}

QSqlRecord TOrm::querySingleRecord( const QSqlDatabase& db, const QString& stmt, bool throwNoResults, bool throwMultipleResults )
{
	QSqlRecord ret;

	QSqlQuery qry;

	if ( TOrm::instance()->enableLog() ) {
		T_DEBUG( stmt );
	}
	T_SCOPED_STOPWATCH( t1 ) {
		qry = query( db, stmt );
	
		if ( !qry.next() ) {
			if ( throwNoResults ) {
                T_DEBUG( "no results were returned by the query." );
			}

			ret = qry.record();

			if ( throwMultipleResults ) {
				if ( qry.next() ) {
                    T_DEBUG( "multiple results were returned by the query." );
				}
			}
		}	
		else {
			ret = qry.record();
		}
	}
	if ( TOrm::instance()->enableLog() ) {
        T_DEBUG( QString( "query elapsed %1" ).arg( t1 ) );
	}

	return ret;
}

QVariant TOrm::querySingleValue( const QSqlDatabase& db, const QString& sql, int valueIndex, bool throwNoResults, bool throwMultipleResults )
{
	QVariant ret;

	QSqlRecord rec;
	
	rec = querySingleRecord( db, sql, throwNoResults, throwMultipleResults );

	if ( !rec.isEmpty() ) {
		ret = rec.value( valueIndex );
	}

	return ret;
}

TOrm::TOrm()
    : mEnableLog( false )
    , mDriver( nullptr )
{
}

TOrm::~TOrm()
{
	QMapIterator<const QMetaObject*, TTableInfo*> iter(mTableInfos);
    while(iter.hasNext()) {
        iter.next();
        
		delete iter.value();
    }
}

TOrm* TOrm::instance()
{
	static TOrm instance;

	return &instance;
}

bool TOrm::enableLog()
{
	return mEnableLog;
}

void TOrm::setEnableLog( bool enable )
{
	mEnableLog = enable;
}

void TOrm::setDriver( IDriver* driver )
{
    mDriver = driver;
}

void TOrm::create( const QSqlDatabase& db, const QMetaObject* mo )
{
    mDriver->createTable( db, mo );
}

TDaoPtr TOrm::dao( const QMetaObject* mo )
{
    TDaoPtr ret;

    TTableInfo* ti = buildTableInfo( mo );
    ret = TDaoPtr( new TDao( ti ) );

    return ret;
}

TTableInfo* TOrm::buildTableInfo( const QMetaObject* mo )
{
	TTableInfo* ret = 0;

	if ( mTableInfos.contains( mo ) ) {
		ret = mTableInfos[mo];;
	}
	else {
        ret = mDriver->buildTableInfo( mo );
		mTableInfos[mo] = ret;
	}

	return ret;
}

