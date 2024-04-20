#include "TDao.h"

#include <qobject.h>
//#include <qsharedpointer_impl.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <qstring.h>
#include <qvariant.h>

#include "../core/TCore.h"
#include "../core/TLogger.h"
#include "../core/TStopwatch.h"
#include "TDeleteBuilder.h"
#include "TFieldInfo.h"
#include "TInsertBuilder.h"
#include "TOrm.h"
#include "TQuery.h"
#include "TSelectBuilder.h"
#include "TTableInfo.h"
#include "TUpdateBuilder.h"

using namespace SKT;
using namespace SKT::orm;

TDao::TDao( TTableInfo* tableInfo )
	: mTableInfo( tableInfo ),
	mIdentityInsert( false )
{
}

TDao::~TDao()
{	
	finish();
}

void TDao::finish()
{
	if ( !mDeleteByIdQry.isNull() ) {
		mDeleteByIdQry->finish();
	}

	if ( !mUpdateByIdQry.isNull() ) {
		mUpdateByIdQry->finish();
	}	
}

const TTableInfo* TDao::tableInfo() const
{
	return mTableInfo;
}

QString TDao::idToString( const QVariant& id )
{
	QString ret;

	if ( mTableInfo->idField()->type() == QVariant::Int ) {
        ret = QString( QStringLiteral("%1") ).arg( id.toInt() );
	}
	else if ( mTableInfo->idField()->type() == QVariant::LongLong ) {
        ret = QString( QStringLiteral("%1") ).arg( id.toLongLong() );
	}

	return ret;
}

TInsertBuilderPtr TDao::insertBuilder( bool insertOrReplace )
{
    TInsertBuilder* sb = new TInsertBuilder( insertOrReplace );
	sb->setIdentityInsert( mIdentityInsert );
	sb->composeWith( mTableInfo );	

	return TInsertBuilderPtr( sb );
}

int TDao::insert( const QSqlDatabase& db, QObject* dto )
{
	auto sb = insertBuilder();

	return insert( db, sb->buildQuery( db ), dto );
}

int TDao::insert( const QSqlDatabase& db, QObject& dto )
{
	auto sb = insertBuilder();

	return insert( db, sb->buildQuery( db ), dto );
}

int TDao::insert( const QSqlDatabase& db, const TQueryPtr& qry, QObject* dto )
{
	return insert( db, qry, *dto );
}

int TDao::insert( const QSqlDatabase& db, const TQueryPtr& qry, QObject& dto )
{
	int ret = 0;

	if ( !qry->isPrepared() ) {
		qry->prepare();
	}

	qry->bind( &dto );

	ret = qry->insert();

	auto idField = mTableInfo->idField()->name();
	if ( !idField.isEmpty() ) {
		// id가 qint64인 경우, lastInsertId는 double형 variant를 넘긴다.
		// 이것을 그대로 dto 객체의 id 필드에 설정하면 예외를 던지도록 코딩이 되어있어 아래와 같은 변환을 해준다.
		mLastInsertId = qry->lastInsertId();
		
		QVariant::Type idType = mTableInfo->getFieldInfoByColumnName( idField )->type();

		//auto var = dto.property( idField.T_CSTR() );
		//QVariant id;

		switch ( idType ) {
		case QVariant::Int:
			dto.setProperty( idField.T_CSTR(), mLastInsertId.toInt() );
			break;
		case QVariant::LongLong:
			dto.setProperty( idField.T_CSTR(), mLastInsertId.toLongLong() );
			break;
		}
	}

	return ret;
}

QVariant TDao::lastInsertId() const
{
	return mLastInsertId;
}

TSelectBuilderPtr TDao::selectBuilder()
{
	TSelectBuilder* sb = new TSelectBuilder();
	sb->composeWith( mTableInfo );	

	return TSelectBuilderPtr( sb );
}

TQueryPtr TDao::selectAll( const QSqlDatabase& db )
{
	TSelectBuilder sb;

	sb.composeWith( mTableInfo );

	TQueryPtr qry = TQueryPtr( new TQuery( db ) );
	qry->setForwardOnly( true );
	qry->prepare( sb.build() );
	qry->exec();
	
	return qry;
}

TQueryPtr TDao::selectOffset( const QSqlDatabase& db, int maximum, int offset)
{
	TSelectBuilder sb;

	sb.composeWith( mTableInfo );
	sb.limitN(maximum);
	if (offset > 0) sb.limitOffset(offset);
	TQueryPtr qry = TQueryPtr( new TQuery( db, sb.build() ) );
	qry->setForwardOnly( true );
	qry->prepare( sb.build() );
	qry->exec();

	return qry;
}


QVariant TDao::selectSingle( const QSqlDatabase& db, const QString& column, const QString& where )
{
	QVariant ret;

	QString stmt;

    stmt.append( QStringLiteral("select ") );
	stmt.append( column );
    stmt.append( QStringLiteral(" from ") );
	stmt.append( mTableInfo->tableName() );

	if ( !where.isEmpty() ) {
        stmt.append( QStringLiteral(" where ") );
		stmt.append( where );
	}

    stmt.append( QStringLiteral(" limit 1 ") );


	QSqlQuery qry;
    qry.setForwardOnly( true );

	if ( TOrm::instance()->enableLog() ) {
		T_DEBUG( stmt );
	}
	
    T_SCOPED_STOPWATCH( t1 ) {
		qry = db.exec( stmt );
		if ( qry.next() ) {
			return qry.value( 0 );
		}
	}

	if ( TOrm::instance()->enableLog() ) {
        T_DEBUGV( "query elapsed %.4f", t1 );
	}

	return ret;
}

TQueryPtr TDao::selectById( const QSqlDatabase& db, const QVariant& id )
{
	TSelectBuilder sb;

	sb.composeWith( mTableInfo );
    sb.where( QString( QStringLiteral("%1=%2") )
		.arg( mTableInfo->idField()->name() ) 
		.arg( id.toString() ) 
	);

	TQueryPtr qry = TQueryPtr( new TQuery( db, sb.build() ) );
    qry->setForwardOnly( true );
	qry->exec();

	return qry;
}

TQueryPtr TDao::selectByField( const QSqlDatabase& db, const QString& name, const QString& value )
{
	TSelectBuilder sb;

	sb.composeWith( mTableInfo );
    sb.where( QString( QStringLiteral("%1='%2'") )
		.arg( name ) 
		.arg( value ) 
		);

	TQueryPtr qry = TQueryPtr( new TQuery( db, sb.build() ) );
	qry->exec();

	return qry;
}

TDeleteBuilderPtr TDao::deleteBuilder()
{
	TDeleteBuilder* sb = new TDeleteBuilder();
	sb->composeWith( mTableInfo );

	return TDeleteBuilderPtr( sb );
}

int TDao::deleteById( const QSqlDatabase& db, const QVariant& id )
{
	if ( mDeleteByIdQry.isNull() ) {
		TDeleteBuilder sb;
		sb.composeWith( mTableInfo );
        sb.where( QString( QStringLiteral("%1=:%2") )
			.arg( mTableInfo->idField()->name() ) 
			.arg( mTableInfo->idField()->name() ) 
		);
		mDeleteByIdQry = sb.buildQuery( db);
		mDeleteByIdQry->prepare();
	}

    mDeleteByIdQry->bind( QStringLiteral(":") + mTableInfo->idField()->name(), id );
	mDeleteByIdQry->exec();

	return mDeleteByIdQry->numRowsAffected();
}

TUpdateBuilderPtr TDao::updateBuilder()
{
	TUpdateBuilder* sb = new TUpdateBuilder();
	sb->composeWith( mTableInfo );

	return TUpdateBuilderPtr( sb );
}

int TDao::updateById( const QSqlDatabase& db, QObject* dto )
{
	return updateById( db, *dto );
}

int TDao::updateById( const QSqlDatabase& db, QObject& dto )
{
	if ( mUpdateByIdQry.isNull() ) {
		TUpdateBuilder sb;
		sb.composeWith( mTableInfo );
        sb.where( QString( QStringLiteral("%1=:%2") )
			.arg( mTableInfo->idField()->name() ) 
			.arg( mTableInfo->idField()->name() ) 
			);
		mUpdateByIdQry = sb.buildQuery( db );
		mUpdateByIdQry->prepare();
	}

	auto idField = mTableInfo->idField()->name();
	auto id = dto.property( idField.T_CSTR() );

    mUpdateByIdQry->bind( QStringLiteral(":") + mTableInfo->idField()->name(), id );
	mUpdateByIdQry->bind( &dto );
	mUpdateByIdQry->exec();

	return mUpdateByIdQry->numRowsAffected();
}

int TDao::updateById( const QSqlDatabase& db, QObject& dto, const QList<TFieldInfoPtr>& fields)
{
	if ( mUpdateByIdQry.isNull() ) {
		TUpdateBuilder sb;
		auto tableInfo = TOrm::instance()->tableInfo( dto.metaObject() );
		sb.table(tableInfo->tableName());

		for ( int i = 0, pos = 0, count = fields.count(); i < count; i++ ) {
			const TFieldInfoPtr& fi = fields.at( i );
			if ( !fi->isFieldCreatable() ) {
				continue;
			}
			sb.column( fi->name() );
		}

        sb.where( QString( QStringLiteral("%1=:%2") )
			.arg( mTableInfo->idField()->name() )
			.arg( mTableInfo->idField()->name() )
			);
		mUpdateByIdQry = sb.buildQuery( db );
		mUpdateByIdQry->prepare();
	}

	auto idField = mTableInfo->idField()->name();
	auto id = dto.property( idField.T_CSTR() );

    mUpdateByIdQry->bind( QStringLiteral(":") + mTableInfo->idField()->name(), id );
	mUpdateByIdQry->bind( &dto );
	mUpdateByIdQry->exec();

	return mUpdateByIdQry->numRowsAffected();
}


TQueryPtr TDao::prepare( const QSqlDatabase& db, TStatementBuilder* stmt )
{
	auto qry = new TQuery( db, stmt->build() );
	qry->prepare();

	return TQueryPtr( qry );
}

TQueryPtr TDao::prepare( const QSqlDatabase& db, TStatementBuilder& stmt )
{	
	auto qry = new TQuery( db, stmt.build() );
	qry->prepare();

	return TQueryPtr( qry );
}

TQueryPtr TDao::exec( const QSqlDatabase& db, TStatementBuilder* stmt )
{
	auto s = stmt->build();

	auto qry = new TQuery( db, s );
	qry->exec();

	return TQueryPtr( qry );
}

TQueryPtr TDao::exec( const QSqlDatabase& db, TStatementBuilder& stmt )
{	
	auto qry = new TQuery( db, stmt.build() );
	qry->exec();

	return TQueryPtr( qry );
}

void TDao::setIdentityInsert( const QSqlDatabase& db, bool identityInsert )
{
	mIdentityInsert = identityInsert;

	QString stmt;	
	
    stmt = QString( QStringLiteral("Set Identity_Insert %1 %2") )
		.arg( mTableInfo->tableName() )
        .arg( identityInsert ? QStringLiteral("ON") : QStringLiteral("OFF") );

	db.exec( stmt );

	if ( db.lastError().isValid() ) {
        T_DEBUG( db.lastError().text() );
	}
}


bool TDao::find( const QSqlDatabase& db, QVariantMap fields, QObject* foundRecord)
{
	bool isExist = false;
	TSelectBuilder sb;
	sb.composeWith( mTableInfo );
	QVariantMap::const_iterator itr = fields.constBegin();
	QString where;
	while (itr != fields.constEnd())
	{
		const TFieldInfo* fi = mTableInfo->getFieldInfoByColumnName(itr.key());
		if (!fi)
		{
			T_DEBUGV("dao, not found field info:%s",itr.key().toLocal8Bit().constData());
			return false;
		}

		switch (fi->type())
		{
		case QVariant::String:
			where.append(QString("%1='%2'").arg(fi->name()).arg(itr.value().toString()));
			break;
		case QVariant::Bool:
		case QVariant::Int:
			where.append(QString("%1=%2").arg(fi->name()).arg(itr.value().toInt()));
			break;
		case QVariant::LongLong:
			where.append(QString("%1=%2").arg(fi->name()).arg(itr.value().toLongLong()));
			break;
		case QVariant::Double:
			where.append(QString("%1=%2").arg(fi->name()).arg(itr.value().toDouble()));
			break;
		default:
			T_DEBUGV("dao, not found field type info:%s",itr.key().toLocal8Bit().constData());
			return false;
		}

		itr++;
		if (itr != fields.constEnd())
			where.append(QStringLiteral(" AND "));
	}

	sb.where(where);

	TQueryPtr qry = TQueryPtr( new TQuery( db, sb.build() ) );
	qry->exec();
	if(qry->next())
	{
		isExist = true;
		if (foundRecord)
			qry->record(foundRecord);
	}
	return isExist;
}
