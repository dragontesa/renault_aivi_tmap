#ifndef TQUERY_H
#define TQUERY_H

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT TQuery
{
public:
	TQuery( const QSqlDatabase& db );

	TQuery( const QSqlDatabase& db, const QString& stmt );

	~TQuery();

	TQuery( const TQuery& other );

	TQuery& operator=( const TQuery& other );

	void finish();

	QSqlQuery& raw();

	void bind( const QString& holder, const QVariant& value );

	void bind( const QObject* dto );

	void bind( const QMap<QString,QVariant>& values);

	QString statement() const;

	void setStatement( const QString& stmt );

	void setForwardOnly( bool forward );

	void prepare();

	void prepare( const QString& stmt );

	bool isPrepared() const;

	void exec();

	int numRowsAffected() const;

	int insert();
	
	QVariant lastInsertId() const;

	bool next();

    void record( QObject* obj, QList<QMetaProperty>* propertyList = nullptr );

	QSqlError lastError() const;

private:
	QSqlDatabase mDb;
	QString mStmt;
	QSqlQuery mQry;
	bool mPrepared;
    bool mTryFastLookup = true;
    bool mIsValidPropertyList = false;
};

template<typename T>
class TTypedQuery
{
public:
	typedef QSharedPointer<TTypedQuery<T>> Ptr;
	typedef QSharedPointer<T> DtoPtr;

	TTypedQuery( const TQueryPtr& query ) 
		: mQuery( query ) {
	}

	TTypedQuery( const TTypedQuery& other ) 
		: mQuery( other.mQuery ) {
	}

	~TTypedQuery() {
	}

	TTypedQuery& operator=( const TTypedQuery& other ) {
        mQuery = other.mQuery;
		return *this;
	}

	void finish() {
		mQuery->finish();
	}

	QSqlQuery& raw() {
		return mQuery->raw();
	}

	void bind( const QString& holder, const QVariant& value ) {
		mQuery->bind( holder, value );
	}

	void bind( const QObject* dto ) {
		mQuery->bind( dto );
	}

	void bind( const QMap<QString,QVariant>& values) {
		mQuery->bind( values );
	}

	QString statement() const {
		return mQuery->statement();
	}

    void prepare( const QString& stmt ) {
        mQuery->prepare( stmt );
	}

	void exec() {
		mQuery->exec();
	}

	int numRowsAffected() const {
		return mQuery->numRowsAffected();
	}

	int insert() {
		return mQuery->insert();
	}

	QVariant lastInsertId() const {
		return mQuery->lastInsertId();
	}

	bool next() {
		return mQuery->next();
	}

    DtoPtr record() {
        T* dto = new T();
        mQuery->record( dto, &mPropertyList );
        return DtoPtr( dto );
    }

private:
    QList<QMetaProperty> mPropertyList;
	TQueryPtr mQuery;    
};

}}

#endif
