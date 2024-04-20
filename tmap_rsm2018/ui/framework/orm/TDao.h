#ifndef TDAO_H
#define TDAO_H

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT TDao
{
public:
    TDao( TTableInfo* tableInfo );

    ~TDao();

	void finish();

    const TTableInfo* tableInfo() const;

	// insert
    TInsertBuilderPtr insertBuilder( bool insertOrReplace = false );

	// slow
	int insert( const QSqlDatabase& db, QObject* dto );

	int insert( const QSqlDatabase& db, QObject& dto );

	// fast
    int insert( const QSqlDatabase& db, const TQueryPtr& qry, QObject* dto );

    int insert( const QSqlDatabase& db, const TQueryPtr& qry, QObject& dto );

	QVariant lastInsertId() const;

	// select

    TSelectBuilderPtr selectBuilder();

    TQueryPtr selectAll( const QSqlDatabase& db );

    TQueryPtr selectOffset( const QSqlDatabase& db, int maximum, int offset=0);

    TQueryPtr selectById( const QSqlDatabase& db, const QVariant& id );

    TQueryPtr selectByField( const QSqlDatabase& db, const QString& name, const QString& value );

	QVariant selectSingle( const QSqlDatabase& db, const QString& column, const QString& where );	

	// delete

    TDeleteBuilderPtr deleteBuilder();
		
	int deleteById( const QSqlDatabase& db, const QVariant& id );

	// update

    TUpdateBuilderPtr updateBuilder();

	int updateById( const QSqlDatabase& db, QObject* dto );

	int updateById( const QSqlDatabase& db, QObject& dto );

	int updateById( const QSqlDatabase& db, QObject& dto, const QList<TFieldInfoPtr>& fileds);

	// exec

    TQueryPtr prepare( const QSqlDatabase& db, TStatementBuilder* select );

    TQueryPtr prepare( const QSqlDatabase& db, TStatementBuilder& select );

    TQueryPtr exec( const QSqlDatabase& db, TStatementBuilder* select );

    TQueryPtr exec( const QSqlDatabase& db, TStatementBuilder& select );

	void setIdentityInsert( const QSqlDatabase& db, bool identityInsert );

	// check if a db record exist
	bool find( const QSqlDatabase& db, QVariantMap fields, QObject* foundRecord=0);

private:	
	QString idToString( const QVariant& id );

    TTableInfo* mTableInfo;
    TQueryPtr mDeleteByIdQry;
    TQueryPtr mUpdateByIdQry;
	QVariant mLastInsertId;
	bool mIdentityInsert;
};

// typed wrapper

template<typename T>
class TTypedDao {
public:
	typedef T Dto;
    typedef QSharedPointer<TTypedDao<T>> Ptr;
	typedef QSharedPointer<T> DtoPtr;
    typedef typename TTypedQuery<T>::Ptr QueryPtr;

    TTypedDao( TDaoPtr dao )
		: mDao( dao ) {
	}

    ~TTypedDao() {
	}

	void finish() {
		return mDao->finish();
	}

    TDaoPtr dao() const {
		return mDao;
	}

    const TTableInfo* tableInfo() const {
		return mDao->tableInfo();
	}
	
	// insert
	
    TInsertBuilderPtr insertBuilder( bool insertOrReplace ) {
        return mDao->insertBuilder( insertOrReplace );
	}

	// slow
	int insert( const QSqlDatabase& db, QObject* dto ) {
		return mDao->insert( db, dto );
	}

	int insert( const QSqlDatabase& db, QObject& dto ) {
		return mDao->insert( db, dto );
	}

	// fast
    int insert( const QSqlDatabase& db, const TQueryPtr& qry, QObject* dto ) {
		return mDao->insert( db, qry, dto );
	}

    int insert( const QSqlDatabase& db, const TQueryPtr& qry, QObject& dto ) {
		return mDao->insert( db, qry, dto );
	}

	QVariant lastInsertId() const {
		return mDao->lastInsertId();
	}

	// select

    TSelectBuilderPtr selectBuilder() {
		return mDao->selectBuilder();
	}

	QVariant selectSingle( const QSqlDatabase& db, const QString& column, const QString& where ) {
		return mDao->selectSingle( db, column, where );
	}

	QueryPtr selectAll( const QSqlDatabase& db ) {
        TQueryPtr qry = mDao->selectAll( db );
        QueryPtr ret( new TTypedQuery<T>( qry ) );
		return ret;
	}	

	QueryPtr selectOffset( const QSqlDatabase& db, int maximum, int offset=0)
	{
		TQueryPtr qry = mDao->selectOffset(db,maximum,offset);
		QueryPtr ret ( new TTypedQuery<T>( qry ) );
		return ret;
	}

	DtoPtr selectById( const QSqlDatabase& db, const QVariant& id ) {
		DtoPtr ret;

        TQueryPtr qry = mDao->selectById( db, id );
		if ( qry->next() ) {
			Dto* dto = new Dto();
			qry->record( dto );
			ret = DtoPtr( dto );
		}

		return ret;
	}

    QueryPtr selectByField( const QSqlDatabase& db, const QString& name, const QString& value ) {
        TQueryPtr qry = mDao->selectByField( db, name, value );
        QueryPtr ret( new TTypedQuery<T>( qry ) );
        return ret;
	}

	// delete

    TDeleteBuilderPtr deleteBuilder() {
		return mDao->deleteBuilder();
	}

	int deleteById( const QSqlDatabase& db, const QVariant& id ) {
		return mDao->deleteById( db, id );
	}

	// update

    TUpdateBuilderPtr updateBuilder() {
		return mDao->updateBuilder();
	}

	int updateById( const QSqlDatabase& db, QObject* dto ) {
		return mDao->updateById( db, dto );
	}

	int updateById( const QSqlDatabase& db, QObject& dto ) {
		return mDao->updateById( db, dto );
	}

	int updateById( const QSqlDatabase& db, QObject& dto, const QList<TFieldInfoPtr> fields) {
		return mDao->updateById(db, dto, fields);
	}

	// exec

	// select

    QueryPtr prepare( const QSqlDatabase& db, TSelectBuilderPtr& select ) {
        TQueryPtr qry = mDao->prepare( db, select.data() );
        QueryPtr ret( new TTypedQuery<T>( qry ) );
		return ret;
	}

    QueryPtr exec( const QSqlDatabase& db, TSelectBuilderPtr& select ) {
        TQueryPtr qry = mDao->exec( db, select.data() );
        QueryPtr ret( new TTypedQuery<T>( qry ) );
		return ret;
	}

	// delete
    QueryPtr prepare( const QSqlDatabase& db, TDeleteBuilderPtr& del ) {
        TQueryPtr qry = mDao->prepare( db, del.data() );
        QueryPtr ret( new TTypedQuery<T>( qry ) );
		return ret;
	}

    QueryPtr exec( const QSqlDatabase& db, TDeleteBuilderPtr& del ) {
        TQueryPtr qry = mDao->exec( db, del.data() );
        QueryPtr ret( new TTypedQuery<T>( qry ) );
		return ret;
	}

	void setIdentityInsert( const QSqlDatabase& db, bool identityInsert ) {
		mDao->setIdentityInsert( db, identityInsert );
	}
	
	bool find( const QSqlDatabase& db, QVariantMap fields, Dto* foundRecord=0) {
		return mDao->find(db,fields,foundRecord);
	}

private:
    TDaoPtr mDao;
};

}}

#endif
