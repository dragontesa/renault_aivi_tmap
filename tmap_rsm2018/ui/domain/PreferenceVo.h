#ifndef PREFERENCEITEMVO_H
#define PREFERENCEITEMVO_H

namespace SKT {

class PreferenceVo
{
    Q_GADGET
    Q_PROPERTY(QString section READ section WRITE setSection)
    Q_PROPERTY(QString key READ key WRITE setKey)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    PreferenceVo();

    PreferenceVo( const QString& section,
        const QString& key,
        const QVariant& value );

    PreferenceVo( const PreferenceVo& other );

    PreferenceVo& operator=( const PreferenceVo& other );

    QString section() const;
    void setSection( const QString& value );

    QString key() const;
    void setKey( const QString& value );

    QVariant value() const;
    void setValue( const QVariant& value );

private:
    QString mSection;
    QString mKey;
    QVariant mValue;
};

}

Q_DECLARE_METATYPE(SKT::PreferenceVo)

#endif // PREFERENCEITEMVO_H
