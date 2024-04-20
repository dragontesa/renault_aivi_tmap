#ifndef STRINGHELPER_H
#define STRINGHELPER_H

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT StringHelper
{
public:
    static StringHelper* instance();

    QString fromCp949( const char* cstr );
    QByteArray toCp949( const QString& cstr );

    QString normalized(QString query);

    static QChar extractChosung( QChar ch );

private:
    StringHelper();

    QTextCodec* mCp949Codec;
};

}

#endif // STRINGHELPER_H
