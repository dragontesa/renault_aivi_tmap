#ifndef RESOURCERESOLVER_H
#define RESOURCERESOLVER_H

namespace SKT {

class ResourceResolver : public QObject
{
    Q_OBJECT
public:
    static ResourceResolver* instance();

    void initialize( const QString& resDirPath );

    Q_INVOKABLE QString imageUrl( const QString& path, const QString& path1 = QString(), const QString& path2 = QString(), const QString& path3 = QString() );
	
    void setLogicalDpi( qreal dpi );

    Q_INVOKABLE int pixelToDp( int pixelSize );
    Q_INVOKABLE int centerAlignOffset( int pixelSize );

    QString imagePath( const QString& path, const QString& path1 = QString(), const QString& path2 = QString(), const QString& path3 = QString() );

private:
    QString url( bool tryLocal, bool checkIfExists, const QString& protocol, const QString& path, const QString& path1 = QString(), const QString& path2 = QString(), const QString& path3 = QString(), const QString& path4 = QString(), const QString& path5 = QString() );

    ResourceResolver();

    QString mDirPath;

    qreal mLogicalDpi;
    qreal mPhysicalDpi;
};

}

#endif // RESOURCERESOLVER_H
