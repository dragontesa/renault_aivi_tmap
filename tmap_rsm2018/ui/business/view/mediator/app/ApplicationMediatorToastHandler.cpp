#include "ApplicationMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ApplicationConstants.h"
#include "ViewRegistry.h"
#include "RgConstants.h"
#include "ToastsConstants.h"
#include <QQuickWindow>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QTimer>
#include "PlatformHelper.h"

using namespace SKT;

void handleToastForApp(QQmlEngine* engine, QObject* viewComponent, QObject* toastWrap, const QString &msg, int pos, int duration)
{
    auto root = qobject_cast<QQuickWindow*>( viewComponent );

    QQmlComponent component(engine, QUrl(QStringLiteral("qrc:/rsm_ui/qml/component/internal/Toast.qml")),QQmlComponent::PreferSynchronous);
    auto toast = qobject_cast<QQuickItem*>(component.create() );
    if ( toast == nullptr ) {
        T_DEBUG( "Could not create Toast" );
    }
    else {
        QQmlEngine::setObjectOwnership( toast, QQmlEngine::JavaScriptOwnership );
        auto rootWidth = root->width();
        auto rootHeight = root->height();

        toast->setParent( toastWrap );
        toast->setParentItem( qobject_cast<QQuickItem*>(toastWrap) );

        auto toastHeight = toast->height();

        QQmlProperty::write( toast, QStringLiteral("message"), msg );

        if (duration > 0) {
            QQmlProperty::write( toast, QStringLiteral("autoCloseTimer"), duration );
        }

        if ( pos == ToastsConstants::ToastUp ) {
#if defined(BUILD_TARGET)
            toast->setY( 0 );
#else
            toast->setY(-toastHeight);
#endif
        }
        else if ( pos == ToastsConstants::ToastCenter ) {
            auto y = rootHeight / 2 - toastHeight / 2;
            toast->setY( y );
        }
        else if ( pos == ToastsConstants::ToastDown ) {
            auto y = rootHeight - toastHeight;
            toast->setY( y );
        }
        QMetaObject::invokeMethod( toast, "show" );
        QQmlProperty::write(toast, "state", "visible");
    }
}

QQuickItem* handleToastForTarget(QQmlEngine* engine, QQuickItem* toast, const QString& msg)
{
    PlatformHelper::instance()->closeView("Toastpopup3");

    auto toastWindow = dynamic_cast<QQuickWindow*>(PlatformHelper::instance()->toastWindow());

    if (toast == nullptr) {
        auto root = qobject_cast<QObject*>(PlatformHelper::instance()->toastRoot());
        if (root == nullptr) {
            qDebug() << "toastRoot is nullptr";

            return nullptr;
        }
        QQmlComponent component(engine, QUrl(QStringLiteral("qrc:/rsm_ui/qml/component/internal/Toast.qml")),QQmlComponent::PreferSynchronous);
        toast = qobject_cast<QQuickItem*>(component.create() );
        if ( toast == nullptr ) {
            T_DEBUG( "Could not create Toast" );

            return nullptr;
        }

        toast->setParent(root);

        auto *quickItem = dynamic_cast<QQuickItem*>(root);
        quickItem->setWidth(820);
        toast->setParentItem(quickItem);

        QQmlProperty::write(toast, "enableAutoClose", false);

        toastWindow->show();
    } else {
        QQmlProperty::write(toast, "state", "invisible");
    }

    QQmlProperty::write( toast, QStringLiteral("message"), msg );

    QTimer::singleShot(33, [=](){
        PlatformHelper::instance()->showView("Toastpopup3");
        QQmlProperty::write(toast, "state", "visible");
    });

    return toast;
}

void ApplicationMediator::handleToast( const mvc::INotificationPtr& note )
{
    auto args = note->getBody().value<QVariantMap>();
    auto msg = args[QStringLiteral("message")].toString();

#ifdef BUILD_TARGET
    mToast = handleToastForTarget(mQmlEngine, qobject_cast<QQuickItem*>(mToast), msg);
#else
    auto pos = args[QStringLiteral("position")].toInt();
    auto duration = args[QStringLiteral("duration")].toInt();
    handleToastForApp(mQmlEngine, mViewComponent, mToastWrap, msg, pos, duration);
#endif
}
