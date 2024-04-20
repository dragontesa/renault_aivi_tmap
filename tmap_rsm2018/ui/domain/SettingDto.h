#ifndef SETTINGDTO_H
#define SETTINGDTO_H

namespace SKT {

T_SHAREABLE( SettingDto );

class SettingDto : public QObject
{
    Q_OBJECT
    Q_CLASSINFO( "table", "name=setting" )
    Q_CLASSINFO( "column", "name=id,type=integer,pk=true,ai=true,not_null=true" )
    Q_CLASSINFO( "column", "name=type,type=integer,not_null=true" )
    Q_CLASSINFO( "column", "name=section,type=text" )
    Q_CLASSINFO( "column", "name=key,type=text" )
    Q_CLASSINFO( "column", "name=value,type=text" )
    Q_CLASSINFO( "unique_index", "name=section_key_unique_index,columns=section|key" )

    Q_PROPERTY(QVariant id READ id WRITE setId)
    Q_PROPERTY(QVariant type READ type WRITE setType)
    Q_PROPERTY(QVariant section READ section WRITE setSection)
    Q_PROPERTY(QVariant key READ key WRITE setKey)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    SettingDto();

    SettingDto( const QVariant& type,
        const QVariant& section,
        const QVariant& key,
        const QVariant& value );

    QVariant id() const;
    void setId( const QVariant& value );

    QVariant type() const;
    void setType( const QVariant& value );

    QVariant section() const;
    void setSection( const QVariant& value );

    QVariant key() const;
    void setKey( const QVariant& value );

    QVariant value() const;
    void setValue( const QVariant& value );

private:
    QVariant mId;
    QVariant mType;
    QVariant mSection;
    QVariant mKey;
    QVariant mValue;
};

}

#endif // SETTINGDTO_H
