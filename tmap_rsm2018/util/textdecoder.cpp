#include <QTextCodec>

#include "textdecoder.h"

namespace SKT {

TextDecoder* TextDecoder::instance()
{
    static TextDecoder textCodec;

    return &textCodec;
}

TextDecoder::TextDecoder()
{
    m_cp949 = QTextCodec::codecForName("windows-949");
    if (!m_cp949) {
        m_cp949 = QTextCodec::codecForName("EUC-KR");
    }
}

QByteArray TextDecoder::fromUnicode(const QString &str) const {
    if (m_cp949) {
        return m_cp949->fromUnicode(str);
    }
    return QByteArray();
}

QString TextDecoder::fromCP949(const char *chars) const
{
    if (m_cp949) {
        return m_cp949->toUnicode(chars);
    }
    return QString();
}

QString TextDecoder::fromCP949(const QByteArray &a) const
{
    if (m_cp949) {
        return m_cp949->toUnicode(a);
    }
    return QString();
}

}
