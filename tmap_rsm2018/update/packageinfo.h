#ifndef PACKAGEINFO_H
#define PACKAGEINFO_H

#include <QString>
#include <QByteArray>
#include <QSharedPointer>
#include <vector>
#include <memory>

#define PACKAGE_CONTAINER_VERSION       "1.0"
#define PACKAGE_VERSION                 "1.0"
#define PACKAGE_NAME_VSM                "vsm"
#define PACKAGE_NAME_LOCAL              "TmapNavi"
#define PACKAGE_NAME_CROSSIMAGE         "crossimage"
#define PACKAGE_TAG_MAP_PREFIX          "${mapPathPrefix}"
#define PACKAGE_TAG_VR_PREFIX           "${vrPathPrefix}"

namespace SKT {

class RSAPublicKey;

struct PackageFile
{
    PackageFile() : fileSize(0) {}

    QString path;
    QString fileName;
    int fileSize;
    QByteArray signature;
};

struct PackageItem
{
    QString name;
    QString version;
    QString sourcePath;
    QString destinationPath;
    std::vector<std::unique_ptr<PackageFile>> files;
};

struct PackageInfo
{
    QString name;
    QString description;
    QString author;
    QString date;
    QString version;
    std::vector<std::unique_ptr<PackageItem>> items;

    static PackageInfo* fromBuffer(const QByteArray &bytes);
};

struct PackageContainer
{
    PackageContainer() : fileSize(0) {}

    QString filePath;
    int fileSize;

    std::unique_ptr<PackageInfo> packageInfo;

    // 에러 코드 정의
    enum ErrorCode
    {
        E_ERROR_SUCCESS = 0,
        E_ERROR_FILEOPEN,
        E_ERROR_FILEMPTY,
        E_ERROR_JSONULL,
        E_ERROR_INVALIDVERSION,
        E_ERROR_MESSAGE,
        E_ERROR_SIGNATURE,
        E_ERROR_SHA256,
        E_ERROR_VERIFYSIG,
        E_ERROR_PKGINFO,
    };
    static int error;
    static PackageContainer* fromFile(const QString &fileName, RSAPublicKey* pubKey);

    QString toString();
};

typedef QSharedPointer<PackageContainer> PackageContainerPtr;

}

#endif // PACKAGEINFO_H
