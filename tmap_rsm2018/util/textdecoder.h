#ifndef TEXTCODEC_H
#define TEXTCODEC_H

#include <QByteArray>
#include <QString>

class QTextCodec;

namespace SKT {

class TextDecoder
{
public:
    TextDecoder();

    static TextDecoder *instance();

    QByteArray fromUnicode(const QString &str) const;
    QString fromCP949(const char *chars) const;
    QString fromCP949(const QByteArray &a) const;

private:
    QTextCodec *m_cp949;
};

}

#endif // TEXTCODEC_H
