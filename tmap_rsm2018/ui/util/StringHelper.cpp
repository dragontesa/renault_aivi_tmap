#include "StringHelper.h"
#include <QTextCodec>

using namespace SKT;

StringHelper::StringHelper()
{
    mCp949Codec = QTextCodec::codecForName("windows-949");
    if (!mCp949Codec) {
        mCp949Codec = QTextCodec::codecForName("EUC-KR");
    }
}

StringHelper* StringHelper::instance()
{
    static StringHelper Instance;

    return &Instance;
}

QString StringHelper::fromCp949( const char* cstr )
{
    if (mCp949Codec) {
        return mCp949Codec->toUnicode( cstr );
    }
    return QString();
}

QByteArray StringHelper::toCp949( const QString& cstr )
{
    if (mCp949Codec) {
        return mCp949Codec->fromUnicode( cstr );
    }
    return QByteArray();
}

QString StringHelper::normalized(QString query) {
#ifdef BUILD_TARGET
    return query;
#endif
    if (query.isEmpty())
        return query;

    QChar source = query.at(query.length() - 1);
    query=query.normalized(QString::NormalizationForm_C);
    QChar target = query.at(query.length() - 1);
    if(source!=target){
        return query;
    }
    QString result="";
    QChar value = 0x00;
    for (int i = 0; i < query.length(); i++) {
        QChar currTxt = query.at(i);
        short a = currTxt.unicode();
        switch (a) {
            case 0x1100: value = (12593); break;
            case 0x1101: value = (12594); break;
            case 0x1102: value = (12596); break;
            case 0x1103: value = (12599); break;
            case 0x1104: value = (12600); break;
            case 0x1105: value = (12601); break;
            case 0x1106: value = (12609); break;
            case 0x1107: value = (12610); break;
            case 0x1108: value = (12611); break;
            case 0x1109: value = (12613); break;
            case 0x110a: value = (12614); break;
            case 0x110b: value = (12615); break;
            case 0x110c: value = (12616); break;
            case 0x110d: value = (12617); break;
            case 0x110e: value = (12618); break;
            case 0x110f: value = (12619); break;
            case 0x1110: value = (12620); break;
            case 0x1111: value = (12621); break;
            case 0x1112: value = (12622); break;
            default:     value = currTxt; break;
        }
        result +=value;
    }
    return result;
}

QChar StringHelper::extractChosung( QChar ch )
{
    QChar ret;

	ushort code = ch.unicode();
	bool isHangle = code >= 0xAC00 && code <= 0xD7AF;
	if ( !isHangle ) {
        ret = QChar( code );
	}
	else {
		int baseCode = 44032;
		int chosungCode = 588;
		//int jungsungCode = 28;

		static QString chosungSet = QStringLiteral( "ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ" );
		//static QString jungsungSet = QStringLiteral( "ㅏㅐㅑㅒㅓㅔㅕㅖㅗㅘㅙㅚㅛㅜㅝㅞㅟㅠㅡㅢㅣ" );
		//static QString jongsungSet = QStringLiteral( " ㄱㄲㄳㄴㄵㄶㄷㄹㄺㄻㄼㄽㄾㄿㅀㅁㅂㅄㅅㅆㅇㅈㅊㅋㅌㅍㅎ" );

		int cBase = code - baseCode;
		int cho = cBase / chosungCode;
		//int jung = (cBase - (chosungCode * cho)) / jungsungCode;
		//int jong = (cBase - (chosungCode * cho) - (jungsungCode * jung));

        ret = chosungSet[cho];
	}

	return ret;
}
