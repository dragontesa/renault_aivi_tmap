#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QSqlDatabase>

namespace SKT {
using namespace orm;
class DatabaseHelper
{
public:
    static DatabaseHelper* instance();
    ~DatabaseHelper();

    void initialize( const QString& dirPath );

    void terminate();

    QSqlDatabase connection();
    QSqlDatabase connection(int userProfileId);
    QSqlDatabase connectionOfCurrentUser(); // local current user db
    QList<QSqlDatabase> connectableConnections(); // connections connectable both cloud and local
    void disconnect(const QString& dbName);
    void disconnect(int userProfileId);
    QSqlDatabase connection(const QString& dbFileName);
    QSqlDatabase connectionForCloud(); // using server data by otp auth.
    void disconnectForCloud();

    void setCurrentUserProfileId(int profileId);
    QString getUserDBFilePath(int profileId) const;

    IDriver& getDriver() const;
    inline QString getDatabasePath() const { return mDirPath;}
    inline QString getLastUserFilePath() const {
        static QString LAST_USER_ID_FILE(QStringLiteral("%1/last_user").arg(mDirPath));
        return LAST_USER_ID_FILE;
    }

    bool isCloudDb() const;
    void setCloudDb();
    void deleteAllUserDB();

private:
    DatabaseHelper();

    QString mDirPath;
    QStringList mConnectionList;

    int mCurrentUserProfileId;
    bool mUsingCloudByOtp;
    bool mIsSessionConnected = false;
};

}

#endif // DATABASEHELPER_H
