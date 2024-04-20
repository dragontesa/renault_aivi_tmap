#ifndef VSMNAVIGATIONPROTOCOL_H
#define VSMNAVIGATIONPROTOCOL_H

namespace SKT {

class VsmNaviPositionMarkType
{
public:
	enum MarkType {
		POSITION_MARK_TYPE_TRACK = 0,
		POSITION_MARK_TYPE_COMPASS,
		POSITION_MARK_TYPE_CAVATAR_NORMAL,
		POSITION_MARK_TYPE_CAVATAR_WEAK_SIGNAL,
		POSITION_MARK_TYPE_CAVATAR_BIRDVIEW,
		POSITION_MARK_TYPE_CAVATAR_WEAK_SIGNAL_BIRDVIEW,
		POSITION_MARK_TYPE_MAX,
		POSITION_MARK_TYPE_MIN = POSITION_MARK_TYPE_TRACK
	};

	inline static QString markTypeKey ( int idx)	{
		QString keys[POSITION_MARK_TYPE_MAX] = {
				QStringLiteral("TRACK"),
				QStringLiteral("COMPASS"),
				QStringLiteral("CAVATAR"),
				QStringLiteral("WEAKSIGNAL"),
				QStringLiteral("CAVATARBIRD"),
				QStringLiteral("WEAKSIGNALBIRD")
		};
		return keys[idx];
	};

	inline static QString sectionName( MarkType e) {
		return QString(QString(QStringLiteral("CavataMark_%1")).arg(markTypeKey(e)));
	}

};


}
#endif // VSMNAVIGATIONPROTOCOL_H
