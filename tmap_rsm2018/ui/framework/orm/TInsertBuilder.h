#ifndef TINSERTBUILDER_H
#define TINSERTBUILDER_H

#include "TStatementBuilder.h"

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT TInsertBuilder : public TStatementBuilder
{
public:	
    TInsertBuilder( bool insertOrReplace );

	~TInsertBuilder();

	TInsertBuilder& table( const QString& table );

	TInsertBuilder& column( const QString& column );		

	QString build() const;

	void setIdentityInsert( bool identityInsert );

private:
	void onComposeWith( const TTableInfo* tableInfo );

    bool mInsertOrReplace;
	QString mTable;
	QStringList mColumns;
	QStringList mHolders;
	bool mIdentityInsert;
};

}}

#endif
