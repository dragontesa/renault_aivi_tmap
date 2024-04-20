#include "TranslatorManager.h"
#include <QTextCodec>

using namespace SKT;

namespace {

const QString QmPrefix = QStringLiteral("tmap_rsm_ui_");

}

TranslatorManager::TranslatorManager()
{
}

void TranslatorManager::setQmPath( const QString& path )
{
    mQmPath = path;
}

QString TranslatorManager::lang()
{
    return mLang;
}

void TranslatorManager::addLang( const QString& lang )
{
    auto translator = t_new_shared<QTranslator>();
    auto qm = QmPrefix + lang;
    if ( translator->load( qm, mQmPath ) ) {
        T_DEBUG( QString( "loaded lang:%1" ).arg( lang ) );
        mTranslators.insert( lang, translator );
    }
}

void TranslatorManager::setLang( const QString& lang )
{
    T_DEBUG( QString( "%1" ).arg( lang ) );

    auto translator = mTranslators[lang];
    if ( translator.isNull() ) {
        T_DEBUG( QString( "<B>Detected language '%1' not supported</B>`" ).arg( lang ) );
        return;
    }

    if ( qApp->installTranslator( translator.data() ) ) {
        mLang = lang;
    }
    else {
        T_DEBUG( QString( "<B>installTranslator Failed! lang:%1'</B>`" ).arg( lang ) );
    }
}
