#pragma once

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT TObjectUtils
{
public:
	TObjectUtils();

	~TObjectUtils();
		
	class BlockSignals
	{
	public:
		QVector<QObject*> objs;

		template<typename... Args>
		BlockSignals( Args... args ) {
			objs = {args...};
			for ( QObject* obj : objs ) {
				obj->blockSignals( true );
			}
		}

		~BlockSignals() {
			for ( QObject* obj : objs ) {
				obj->blockSignals( false );
			}
		}
	};

    static QString metaAttribute( const QMetaObject* mo, const QString& wantedName, const QString& wantedKey );

    static bool checkMetaAttribute( const QMetaObject* mo, const QString& wantedName, const QString& wantedKey, const QString& wantedValue );
};

}
