#ifndef TORM_H
#define TORM_H

namespace SKT { namespace orm {

T_SHAREABLE( TFieldInfo );
T_SHAREABLE( TTableInfo );
T_SHAREABLE( TOrm );
T_SHAREABLE( TQuery );
T_SHAREABLE( TDao );
T_SHAREABLE( TInsertBuilder );
T_SHAREABLE( TSelectBuilder );
T_SHAREABLE( TStatementBuilder );
T_SHAREABLE( TUpdateBuilder );
T_SHAREABLE( TDeleteBuilder );

}}

#include "IDriver.h"
#include "TQuery.h"
#include "TInsertBuilder.h"
#include "TSelectBuilder.h"
#include "TDeleteBuilder.h"
#include "TUpdateBuilder.h"
#include "TDao.h"

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT TOrm
{
public:
	// static

	static QSqlQuery query( const QSqlDatabase& db, const QString& stmt );

	static QSqlRecord querySingleRecord( const QSqlDatabase& db, const QString& stmt, bool throwNoResults = false, bool throwMultipleResults = false );

	static QVariant querySingleValue( const QSqlDatabase& db, const QString& stmt, int valueIndex = 0, bool throwNoResults = false, bool throwMultipleResults = false );		

    static TOrm* instance();

	bool enableLog();

	void setEnableLog( bool enable );

    void setDriver( IDriver* driver );

    template<typename T>
    void create( const QSqlDatabase& db ) {
        create( db, &T::staticMetaObject );
    }

	template<typename T>
    typename TTypedDao<T>::Ptr dao() {
		TDaoPtr ptr = dao( &T::staticMetaObject );
        return typename TTypedDao<T>::Ptr( new TTypedDao<T>( ptr ) );
	}

	TTableInfo* tableInfo( const QMetaObject* mo ) {
		return buildTableInfo( mo );
	}

private:        
    TOrm();

    ~TOrm();

    void create( const QSqlDatabase& db, const QMetaObject* mo );

	TDaoPtr dao( const QMetaObject* mo );

	TTableInfo* buildTableInfo( const QMetaObject* mo );	

	QMap<const QMetaObject*, TTableInfo*> mTableInfos;

	bool mEnableLog;

    IDriver* mDriver;
};

}}

#endif
