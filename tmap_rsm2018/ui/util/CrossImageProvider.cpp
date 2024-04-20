#include "CrossImageProvider.h"

#define DATABASE_TYPE           "QSQLITE"
#define DATABASE_NAME_DAY       "crossimage_day_480x540.db"
#define DATABASE_NAME_NIGHT     "crossimage_night_480x540.db"
#define CONNECTION_NAME_DAY     "crossimage_day"
#define CONNECTION_NAME_NIGHT   "crossimage_night"

namespace SKT {

CrossImageProvider::CrossImageProvider()
{
}

CrossImageProvider* CrossImageProvider::instance()
{
    static CrossImageProvider Instance;

    return &Instance;
}

void CrossImageProvider::initialize(const QString &dbPath)
{
    mDbPath = dbPath;
}

CrossImageProvider::~CrossImageProvider()
{
//    if (mDayDB.isOpen()) {
//        mDayDB.close();
//    }
//    if (mNightDB.isOpen()) {
//        mNightDB.close();
//    }
}

QSqlDatabase CrossImageProvider::database(bool day)
{
    QSqlDatabase db;

    auto connName = QString( "%1_%2" )
        .arg( day ? CONNECTION_NAME_DAY : CONNECTION_NAME_NIGHT )
        .arg( reinterpret_cast<std::uintptr_t>(QThread::currentThreadId()) );

    if ( !QSqlDatabase::contains( connName ) ) {
        db = QSqlDatabase::addDatabase( "QSQLITE", connName );
        auto dbName = QString( "%1/%2" ).arg( mDbPath )
            .arg( day ? DATABASE_NAME_DAY : DATABASE_NAME_NIGHT );
        db.setDatabaseName( dbName );
        T_DEBUG( QString( "Created database connection '%1 %2'" ).arg( connName ).arg( dbName ) );
    }
    else {
        db = QSqlDatabase::database( connName, false );
    }

    if (!db.isOpen()) {
        T_DEBUG( QString( "Open database '%1'" ).arg( connName ) );
        db.setConnectOptions(QLatin1String("QSQLITE_OPEN_READONLY"));
        if (!db.open()) {
            auto lastError = db.lastError().text();
            T_DEBUG(QString("<B>Could not open database(%1) Error:%2</B>").arg(connName).arg(lastError));
        }
    }

    return db;
}

QByteArray CrossImageProvider::getImage(quint32 id, bool day)
{
    QByteArray image;

    T_DEBUG(QString("#1 id: %1 day: %2").arg(id).arg(day));

    auto db = database(day);
    if (!db.isOpen()) {
        return image;
    }    

    T_DEBUG(QString("#2 id: %1 day: %2").arg(id).arg(day));

    QSqlQuery query(db);
    auto sql = QString("SELECT image FROM crossImages WHERE id=%1").arg(id);
    if (query.exec( sql )) {
        query.next();
        if (query.isValid()) {
            image = query.value(0).toByteArray();
        }
        else {
            T_DEBUG(QString("#3 Could not find cross image id: %1 day: %2 sql: %3").arg(id).arg(day).arg(sql));
        }
    }
    else {
        T_DEBUG(QString("#3 Could not find cross image id: %1 day: %2 sql: %3").arg(id).arg(day).arg(sql));
    }

    return image;
}

}
