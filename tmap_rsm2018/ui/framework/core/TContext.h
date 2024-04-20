#pragma once

namespace SKT
{

class TMAP_RSM_UI_DLLSHARED_EXPORT TContext
{
public:
	TContext();
	
	virtual ~TContext();

	TContext( const TContext& other );

	TContext& operator=( const TContext& other );

	inline QVariant value( const QString& name, const QVariant& defValue = QVariant() ) const {
		return mValues.value( name, defValue );
	}

	bool hasValue( const QString& name ) {
		return mValues.contains( name );
	}

	inline TContext& setValue( const QString& name, const QVariant& value ) {
		mValues[name] = value;
		return *this;
	}

	inline TContext& setValue( const QString& name, const char* value ) {
		mValues[name] = qVariantFromValue<QString>( QString::fromLocal8Bit( value ) );
		return *this;
	}

	template<typename T>
	inline T enumValue( const QString& name ) const {
		return static_cast<T>(mValues[name].value<int>());
	}

	template<typename T>
	inline TContext& setEnumValue( const QString& name, const T& value ) {
		mValues[name] = qVariantFromValue<int>( static_cast<int>( value ) );
		return *this;
	}

	template<typename T>
	inline T* pointerValue( const QString& name ) const {
		return static_cast<T*>(mValues[name].value<void*>());
	}

	template<typename T>
	inline TContext& setPointerValue( const QString& name, const T& value ) {
		mValues[name] = qVariantFromValue<void*>( static_cast<void*>( value ) );

		return *this;
	}

	bool isEmpty() {
		return mValues.isEmpty();
	}
	
private:
	QMap<QString,QVariant> mValues;
};

}

Q_DECLARE_METATYPE( SKT::TContext* )

