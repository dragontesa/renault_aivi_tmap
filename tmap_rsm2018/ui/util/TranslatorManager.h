#ifndef TRANSLATORMANAGER_H
#define TRANSLATORMANAGER_H

#include <QTranslator>

T_SHAREABLE( QTranslator )

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT TranslatorManager
{
public:
    TranslatorManager();

    void setQmPath( const QString& path );

    QString lang();

    void addLang( const QString& lang );

    void setLang( const QString& lang );

private:
    QString mQmPath;
    QString mDefaultLang;
    QMap<QString,QTranslatorPtr> mTranslators;
    QString mLang;
};

}

#endif // TRANSLATORMANAGER_H
