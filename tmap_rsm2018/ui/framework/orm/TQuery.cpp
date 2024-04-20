#include "TQuery.h"
#include "TTableInfo.h"
#include "TLogger.h"
#include "TStopwatch.h"
#include "TOrm.h"

using namespace SKT;
using namespace SKT::orm;

TQuery::TQuery( const QSqlDatabase& db )
	: mDb( db ),
	mPrepared( false ),
	mQry( db )
{
}

TQuery::TQuery( const QSqlDatabase& db, const QString& stmt )
	: mDb( db ),
	mStmt( stmt ),
	mPrepared( false ),
	mQry( db )
{
}

TQuery::TQuery( const TQuery& other )
	: mDb( mDb ),
	mStmt( other.mStmt ),
	mQry( other.mQry ),
	mPrepared( other.mPrepared )
{
}

TQuery::~TQuery()
{
	finish();
}

TQuery& TQuery::operator=( const TQuery& other )
{
	mDb = other.mDb;
	mStmt = other.mStmt;
	mQry = other.mQry;
	mPrepared = other.mPrepared;
	
	return *this;
}

void TQuery::finish()
{
	if ( mPrepared ) {
		mQry.clear();
		mPrepared = false;
	}
}

QSqlQuery& TQuery::raw()
{
	return mQry;
}

void TQuery::bind( const QString& holder, const QVariant& value )
{
	mQry.bindValue( holder, value );
}

void TQuery::bind( const QObject* dto )
{
    //t_todo( 현재 dto 필드에 기본값이 설정되어 null 지정 방식을 사용할 수 없다 )
    //t_todo( null 지정 방식을 지원하려면 dto 필드의 기본값을 없애고, statment builder에 dto로 컬럼을 구성할 수 있도록 한다 )

	if ( !mPrepared ) {
        T_DEBUG( "not prepared" );
	}

	auto mo = dto->metaObject();
	auto keys = mQry.boundValues().keys();
	foreach ( QString key, keys ) {
        T_Q2A( propName, QString( key ).remove( QStringLiteral(":") ) );
		int pos = mo->indexOfProperty( propName );
		if ( pos > -1 ) {
			QVariant var = dto->property( propName );
			mQry.bindValue( key, var );
		}
	}
}

void TQuery::bind( const QMap<QString,QVariant>& values)
{
	if ( !mPrepared ) {
        T_DEBUG( "not prepared" );
	}

	auto it = values.begin();
	while ( it != values.end() ) {		
		mQry.bindValue( it.key(), it.value() );
	}
}

QString TQuery::statement() const
{
	return mQry.lastQuery();
}

void TQuery::setStatement( const QString& stmt )
{
	mStmt = stmt;
}

void TQuery::setForwardOnly( bool forward )
{
	mQry.setForwardOnly( forward );
}

void TQuery::prepare()
{
	if ( mStmt.isEmpty() ) {
        T_DEBUG( "statement is empty" );
	}

	mQry = QSqlQuery( mDb );
	mPrepared = mQry.prepare( mStmt );
}

void TQuery::prepare( const QString& stmt )
{
	mQry = QSqlQuery( mDb );
	mPrepared = mQry.prepare( stmt );
}

bool TQuery::isPrepared() const
{
	return mPrepared;
}

void TQuery::exec()
{
	bool res = false;

	if ( !mPrepared ) {
		if ( mStmt.isEmpty() ) {
            T_DEBUG( "statement is empty" );
		}

		if ( TOrm::instance()->enableLog() ) {
			T_DEBUG( mStmt );
		}
        T_SCOPED_STOPWATCH( t1 ) {
			res = mQry.exec( mStmt );
		}
		if ( TOrm::instance()->enableLog() ) {
            T_DEBUGV( "query elapsed %.4f", t1 );
		}
	}
	else {
		if ( TOrm::instance()->enableLog() ) {
			T_DEBUG( mQry.lastQuery() );
		}
        T_SCOPED_STOPWATCH( t2 ) {
			res = mQry.exec();
		}
		if ( TOrm::instance()->enableLog() ) {
            T_DEBUGV( "query elapsed %.4f", t2 );
		}
	}

	if ( !res ) {
        T_DEBUG( mQry.lastError().text() );
	}
}

int TQuery::numRowsAffected() const
{
	return mQry.numRowsAffected();
}

int TQuery::insert()
{
	if ( !mPrepared ) {
        T_DEBUG( "not prepared" );
	}

	exec();

	return mQry.numRowsAffected();
}

QVariant TQuery::lastInsertId() const
{
	return mQry.lastInsertId();
}

bool TQuery::next()
{
	return mQry.next();
}

//#define TMAP_RSM_UI_ENABLE_ORM_FAST_PROPERTY_LOOKUP

void TQuery::record(  QObject* obj, QList<QMetaProperty>* propertyList )
{
    auto rec = mQry.record();

    auto count = rec.count();
    if ( count == 0 ) {
        return;
    }

#ifndef TMAP_RSM_UI_ENABLE_ORM_FAST_PROPERTY_LOOKUP
    for ( int i = 0; i < count; i++ ) {
        obj->setProperty( rec.fieldName( i ).toLatin1().constData(), rec.value( i ) );
    }
#else    
    if ( propertyList != nullptr ) {
        if ( mTryFastLookup ) {
            if ( propertyList->count() == 0 ) {
                bool isValidPropertyList = true;
                for ( int i = 0; i < count; i++ ) {
                    auto propIdx = obj->metaObject()->indexOfProperty( rec.fieldName( i ).toLatin1().constData() );
                    if ( propIdx < 0 ) {
                        isValidPropertyList = false;
                        mTryFastLookup = false;
                        break;
                    }
                    else {
                        propertyList->append( obj->metaObject()->property( propIdx ) );
                    }
                }
                mIsValidPropertyList = isValidPropertyList;
                mTryFastLookup = false;
            }
        }
    }

    if ( mIsValidPropertyList ) {
        for ( int i = 0; i < count; i++ ) {
            propertyList->at( i ).write( obj, rec.value( i ) );
        }
    }
    else {
        for ( int i = 0; i < count; i++ ) {
            obj->setProperty( rec.fieldName( i ).toLatin1().constData(), rec.value( i ) );
        }
    }
#endif
}

QSqlError TQuery::lastError() const
{
	return mQry.lastError();
}
