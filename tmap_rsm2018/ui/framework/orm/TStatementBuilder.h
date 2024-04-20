#ifndef TSTATEMENTBUILDER_H
#define TSTATEMENTBUILDER_H

#include "TQuery.h"

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT TStatementBuilder
{
public:
	TStatementBuilder();

	~TStatementBuilder();

	void composeWith( const TTableInfo* tableInfo );

	void composeWith( const QObject* dto );
	
	void composeWith( const QObject& dto );
	
	virtual QString build() const = 0;

	TQueryPtr buildQuery( const QSqlDatabase& db );

    /*
	template<typename T>
	typename TTypedQuery<T>::Ptr buildTypedQuery( const QSqlDatabase& db ) {
        //return typename TTypedQuery<T>::Ptr( new TTypedQuery( db, build() ) );
        return TTypedQuery<T>::Ptr( new TTypedQuery( db, build() ) );
	}
    */

protected:
	virtual void onComposeWith ( const TTableInfo* tableInfo ) {}

	virtual void onComposeWith ( const QObject& dto ) {}

	const TTableInfo* mTableInfo;
	QStringList mBindColumns;
};

}}

#endif
