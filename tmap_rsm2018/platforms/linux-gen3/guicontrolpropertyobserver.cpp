#include "guicontrolpropertyobserver.h"
#include "guicontroltype.h"

#include <QDebug>

namespace SKT {


static const char* PropName_NaviPropertyFocusConfig           = "NaviPropertyFocusConfig";
static const char* PropName_NaviGadgetFocusStatus             = "NaviGadgetFocusStatus";

GuicontrolPropertyObserver::GuicontrolPropertyObserver(QObject *parent)
    : QObject(parent)
    , m_focusConfig(0)
{
    if (parent) {
        connect(parent, SIGNAL(propertyChanged(QString,QVariant)),
                this, SLOT(onPropertyChanged(QString,QVariant)));
        // set initial value
        m_focusConfig = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyFocusConfig));
    }
}

void GuicontrolPropertyObserver::emitFocusConfigChanged()
{
    emit focusConfigChanged(m_focusConfig);
}

void GuicontrolPropertyObserver::onPropertyChanged(const QString &propertyName, const QVariant &value)
{
#if 0 // Too verbose
    qDebug() << Q_FUNC_INFO << "name:" << propertyName << "value:" << value;
#endif
    if (propertyName == QLatin1String(PropName_NaviPropertyFocusConfig)) {
        char focusConfig = qvariant_cast<uchar>(value);
//        qDebug() << "NavigationGuiService > NaviPropertyFocusConfig :" << (int)focusConfig;
        if (m_focusConfig != focusConfig) {
            m_focusConfig = focusConfig;
            emit focusConfigChanged(m_focusConfig);
        }
    } else if (propertyName == QLatin1String(PropName_NaviGadgetFocusStatus)) {
        auto naviGadgetFocusInfo = value.value<GCNaviGadgetFocusInfo>();
        auto instanceId = naviGadgetFocusInfo.m_GadgetInstanceId;
        auto focus = naviGadgetFocusInfo.m_FocusStatus;
        emit naviGadgetFocusInfoChanged(instanceId, focus);
    }
}
}
