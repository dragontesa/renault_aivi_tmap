#ifndef GUICONTROLPROPERTYOBSERVER_H
#define GUICONTROLPROPERTYOBSERVER_H

#include <QObject>

namespace SKT {

class GuicontrolPropertyObserver : public QObject
{
    Q_OBJECT

    Q_PROPERTY(char m_focusConfig READ focusConfig NOTIFY focusConfigChanged)


public:
    explicit GuicontrolPropertyObserver(QObject *parent = 0);

    char focusConfig()                    const { return m_focusConfig; }

    void emitFocusConfigChanged();

signals:
    void focusConfigChanged(char focusConfig);
    void naviGadgetFocusInfoChanged(unsigned int instanceId, bool focus);

public slots:

private slots:
    void onPropertyChanged(const QString &propertyName, const QVariant &value);

private:

    char m_focusConfig;
};

}




#endif // GUICONTROLPROPERTYOBSERVER_H
