#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "packageinfo.h"
#include "packageutil.h"

namespace SKT {

int PackageContainer::error;
static PackageFile* parsePackageFile(const QJsonObject &obj)
{
    std::unique_ptr<PackageFile> pkgFile(new PackageFile);

    QJsonValue v = obj["path"]; if (!v.isString()) return 0;
    pkgFile->path = v.toString();

    v = obj["fileName"]; if (!v.isString()) return 0;
    pkgFile->fileName = v.toString();

    v = obj["fileSize"]; if (!v.toInt()) return 0;
    pkgFile->fileSize = v.toInt();

    v = obj["signature"]; if (!v.isString()) return 0;
    pkgFile->signature = QByteArray::fromBase64(v.toString().toLatin1());

    return pkgFile.release();
}

static PackageItem* parsePackageItem(const QJsonObject &obj)
{
    std::unique_ptr<PackageItem> pkgItem(new PackageItem);

    QJsonValue v = obj["name"]; if (!v.isString()) return 0;
    pkgItem->name = v.toString();

    v = obj["version"]; if (!v.isString()) return 0;
    pkgItem->version = v.toString();

    v = obj["sourcePath"]; if (!v.isString()) return 0;
    pkgItem->sourcePath = v.toString();

    v = obj["destinationPath"]; if (!v.isString()) return 0;
    pkgItem->destinationPath = v.toString();

    v = obj["files"]; if (!v.isArray()) return 0;
    QJsonArray files = v.toArray();
    for (QJsonValue file : files) {
        if (!file.isObject()) return 0;

        PackageFile* pkgFile = parsePackageFile(file.toObject());
        if (!pkgFile) return 0;

        pkgItem->files.emplace_back(pkgFile);
    }

    return pkgItem.release();
}

PackageInfo* PackageInfo::fromBuffer(const QByteArray &bytes)
{
    std::unique_ptr<PackageInfo> pkg(new PackageInfo);

    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (doc.isNull()) {
        return 0;
    }

    QJsonObject rootObj = doc.object();

    // check document version
    QJsonValue v = rootObj["version"];
    if (!v.isString() || v.toString() != QLatin1String(PACKAGE_VERSION)) {
        qDebug() << "invalid package version";
        return 0;
    }

    v = rootObj["package"];
    if (!v.isObject()) {
        return 0;
    }

    QJsonObject packageObj = v.toObject();

    v = packageObj["name"]; if (!v.isString()) return 0;
    pkg->name = v.toString();

    v = packageObj["description"]; if (!v.isString()) return 0;
    pkg->description = v.toString();

    v = packageObj["author"]; if (!v.isString()) return 0;
    pkg->author = v.toString();

    v = packageObj["date"]; if (!v.isString()) return 0;
    pkg->date = v.toString();

    v = packageObj["version"]; if (!v.isString()) return 0;
    pkg->version = v.toString();

    v = packageObj["items"]; if (!v.isArray()) return 0;
    QJsonArray items = v.toArray();
    for (QJsonValue value : items) {
        if (!value.isObject()) return 0;

        PackageItem* pkgItem = parsePackageItem(value.toObject());
        if (!pkgItem) return 0;

        pkg->items.emplace_back(pkgItem);
    }

    return pkg.release();
}

PackageContainer *PackageContainer::fromFile(const QString &fileName, RSAPublicKey* pubKey)
{
    error = E_ERROR_SUCCESS;
    std::unique_ptr<PackageContainer> container(new PackageContainer);
qDebug() << "pacman fromFile #1";
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << fileName;
        qDebug() << "!file.open(QIODevice::ReadOnly) - " << file.errorString();

        QFileInfo fi(file);
         if (fi.permission(QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther))
             qDebug() << "file is readable !!";
         else
             qDebug() << "file read permission denied!!";

         qDebug() << "owner: " << fi.owner() << ", group: " << fi.group();
        error = E_ERROR_FILEOPEN;
        return 0;
    }

    container->filePath = fileName;
    container->fileSize = file.size();

    QByteArray data = file.readAll();
qDebug() << "pacman fromFile #2";
    if (data.isEmpty()) {
        qDebug() << "file.readAll(), data.isEmpty()";
        error = E_ERROR_FILEMPTY;
        return 0;
    }

    file.close();
qDebug() << "pacman fromFile #3";
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "doc.isNull()";
        error = E_ERROR_JSONULL;
        return 0;
    }
qDebug() << "pacman fromFile #4";
    QJsonObject rootObj = doc.object();

    // check container version
    QJsonValue v = rootObj["version"];
    if (!v.isString() || v.toString() != QLatin1String(PACKAGE_CONTAINER_VERSION)) {
        qDebug() << "invalid package container version";
        error = E_ERROR_INVALIDVERSION;
        return 0;
    }
qDebug() << "pacman fromFile #5";
    v = rootObj["message"];
    if (!v.isString()) {
        error = E_ERROR_MESSAGE;
        return 0;
    }

    QString str = v.toString();
    QByteArray message = QByteArray::fromBase64(str.toLatin1());
qDebug() << "pacman fromFile #6";
    v = rootObj["signature"];
    if (!v.isString()) {
        qDebug() << "!v.isString(), signature";
        error = E_ERROR_SIGNATURE;
        return 0;
    }

    str = v.toString();
    QByteArray sig = QByteArray::fromBase64(str.toLatin1());
qDebug() << "pacman fromFile #7";
    // verify signature
    bool ok = false;
    MessageDigestSHA256 sha256;
    sha256.update(message.constData(), message.count());
    QByteArray m = sha256.final(&ok);
    if (!ok || m.isEmpty()) {
        qDebug() << "MessageDigestSHA256, !ok || m.isEmpty()";
        error = E_ERROR_SHA256;
        return 0;
    }
qDebug() << "pacman fromFile #8";
    if (!PackageUtil::verifySignature(m, sig, pubKey)) {
        qDebug() << "!PackageUtil::verifySignature(m, sig, pubKey)";
        error = E_ERROR_VERIFYSIG;
        return 0;
    }

    PackageInfo* pkgInfo = PackageInfo::fromBuffer(message);
    if (!pkgInfo) {
        qDebug() << "!pkgInfo";
        error = E_ERROR_PKGINFO;
        return 0;
    }
qDebug() << "pacman fromFile #9";
    container->packageInfo.reset(pkgInfo);

    return container.release();
}


QString PackageContainer::toString()
{
    typedef QSharedPointer<QJsonObject> QJsonObjectPtr;
    typedef QSharedPointer<QJsonArray> QJsonArrayPtr;

    struct Internal {
        QMap<QJsonObject*,int> mIndex;

        QJsonObjectPtr newObject() {
                return QJsonObjectPtr(new QJsonObject);
        }

        QJsonArrayPtr newArray() {
                return QJsonArrayPtr(new QJsonArray);
        }

        void insert(QJsonObjectPtr& jsonObj, const QString& key, const QJsonValue& value) {
            auto ptr = jsonObj.data();
            if (!mIndex.contains(ptr)) {
                mIndex[ptr] = 0;
            }
            auto index = mIndex[ptr];
            jsonObj->insert(QString("&&%1&&%2").arg(index).arg(key), value);
            mIndex[ptr] = mIndex[ptr] + 1;
        }

        QString toString(const QJsonObject& root) {
            QJsonDocument doc(root);
            auto json =  QString(doc.toJson(QJsonDocument::Indented));
            json.replace(QRegExp("&&([0-9]+)&&"), "");
            return json;
        }
    };

    Internal internal;

    auto pkg = internal.newObject();
    internal.insert(pkg, "name", packageInfo->name);
    internal.insert(pkg, "description", packageInfo->description);
    internal.insert(pkg, "author", packageInfo->author);
    internal.insert(pkg, "date", packageInfo->date);
    internal.insert(pkg, "version", packageInfo->version);

    auto pkgItems = internal.newArray();
    for (int i = 0; i < packageInfo->items.size(); i++) {
        auto& item = packageInfo->items[i];

        auto itemJson = internal.newObject();
        internal.insert(itemJson, "name", item->name);
        internal.insert(itemJson, "version", item->version);
        internal.insert(itemJson, "sourcePath", item->sourcePath);
        internal.insert(itemJson, "destinationPath", item->destinationPath);

        auto itemFiles = internal.newArray();
        for (int j = 0; j < item->files.size(); j++) {
            auto& file = item->files[j];

            auto fileJson = internal.newObject();
            internal.insert(fileJson, "path", file->path);
            internal.insert(fileJson, "fileName", file->fileName);
            internal.insert(fileJson, "name", file->fileSize);
            //jsonUtil.insert(fileJson, "signature", QString(file->signature));
            itemFiles->append(*fileJson);
        }
        //jsonUtil.insert(itemJson, "files", itemFiles);

        pkgItems->append(*itemJson);
    }

    internal.insert(pkg, "items", *pkgItems);
    auto json = internal.toString(*pkg);

    return json;
}

}
