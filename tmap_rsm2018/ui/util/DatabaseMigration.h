#ifndef UI_UTIL_DATABASEMIGRATION_H_
#define UI_UTIL_DATABASEMIGRATION_H_
#include <QSqlDatabase>

namespace SKT {
using namespace orm;

/*! 
 *  현재 DB 마그레이션은 테이블의 최소한의 변경사항에 대해서만 수행된다. 예를 들면, 테이블의 컬럼이 추가되거나 삭제 또는 그 스키마가 변경될 경우 한에서.
 * ! 1. 동일한 데이터에 대해서 컬럼명이 바뀌는 것은 다른 데이터 타입으로 간주되어 이전 데이터는 복구되지 않는다.
*/
class DatabaseMigration
{

  public:
  	enum MigrationJobType {
  		DB2DB = 0, // migrate from a db to another db
  		TB2TB, // migrate from a table to another one in same DB
  		MAX_JOBTYPE,
  	};
  	struct ColumnInfo {
  		QString name;
  		QString type;
  		QString fk;
  		QString refTable;
  		QString refCol;
  		QString onDelete;
  		QString onUpdate;
  		QString onConflict;
  		bool notNull = false;
  		bool pk = false;
  		bool ai = false;
  		bool unique = false;
  	};
  	struct IndexInfo {
  		QString name;
  		QString columns;
  		bool unique = false;
  	};
         DatabaseMigration(QSqlDatabase conDB);
         ~DatabaseMigration();
         void migrateDatabase(QSqlDatabase dst);
         void migrateTable(const IDriver& driver, const QMetaObject& dto);
         // void renameColumnName(const QString& tableName, const QString& oldName, const QString& newName);

  protected:
  	// jobs following should be run in other threads
     	class MigrateTableJob {
     	public:
     		MigrateTableJob(QSqlDatabase db, const IDriver& driver, const QMetaObject& dto);
     		void runJob();

     	private:
     		QString getNewTableName() const;
     		QStringList getNewColumnNameList() const;
     		QList<DatabaseMigration::ColumnInfo> getNewTableColumnList() const;
     		QList<DatabaseMigration::IndexInfo> getNewIndexList() const;
     		QString buildTableSchema();
     		QStringList buildIndexSchema();

     		QSqlDatabase mConDB;
     		const IDriver& mDriver;
     		const QMetaObject& mDto;
     	};

     	class MigrateDbJob {
     	public:
     		void runJob(QSqlDatabase src, QSqlDatabase dst);
     	};

    // worker for migration
     	class MigrateWorker: public QThread {
     	   public:
     		MigrateWorker(MigrationJobType jobType,
     		 QSqlDatabase db,
     		 const IDriver& driver,
     		 const QMetaObject& dto);
     	protected:
     		void run() override;

     	private:
     		MigrationJobType mJobType;
     		QSqlDatabase mWorkDB;
     		const IDriver& mWorkDriver;
     		const QMetaObject& mWorkDto;

     	};

     	QSqlDatabase mConDB;

};

}



#endif // UI_UTIL_DATABASEMIGRATION_H_
