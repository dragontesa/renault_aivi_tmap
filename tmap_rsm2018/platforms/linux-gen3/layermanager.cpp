#include <QDebug>
#include <QQuickWindow>
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include "layermanager.h"

#ifdef BUILD_TARGET
#define USE_WAYLAND_INPUT
#endif

#ifdef USE_WAYLAND_INPUT
#include <qpa/qplatformnativeinterface.h>
#include <QGuiApplication>
#endif

namespace SKT {

SurfaceProperty::SurfaceProperty() :
    m_enabled(true),
    m_isSurfaceVisibile(false),
    m_width(0),
    m_height(0),
    m_x(0),
    m_y(0),
    m_layerId(0),
    m_surfaceId(0),
    m_surfaceOpacity(1.0F)
{

}

SurfaceProperty::SurfaceProperty(bool isSurfaceVisibile, uint width, uint height, int x, int y, uint layerId, uint surfaceId, float surfaceOpacity) :
    m_enabled(true),
    m_isSurfaceVisibile(isSurfaceVisibile),
    m_width(width),
    m_height(height),
    m_x(x),
    m_y(y),
    m_layerId(layerId),
    m_surfaceId(surfaceId),
    m_surfaceOpacity(surfaceOpacity)
{

}

SurfaceProperty::~SurfaceProperty()
{
}

LayerManager::LayerManager(QObject* parent)
    : QObject(parent),
    m_initialized(false)
{

}

LayerManager::~LayerManager()
{
    if (m_initialized) {
#ifdef BUILD_TARGET
        ilm_destroy();
#endif
        m_initialized = false;
    }
}

bool LayerManager::layerHasSurface(int layerId, int surfaceId)
{
#ifdef BUILD_TARGET
    t_ilm_int length;
    t_ilm_surface *surfaces;
    auto err = ilm_getSurfaceIDsOnLayer(layerId, &length, &surfaces);
    if (err == ILM_SUCCESS) {
        for (auto i=0; i<length; i++) {
            if (surfaces[i] == surfaceId) {
                qDebug() << "layer " << layerId << " has surface : " << surfaceId;
                return true;
            }
        }
    }
#endif
    return false;
}

void LayerManager::removeSurfaceFromLayer(int layerId, int surfaceId)
{
#ifdef BUILD_TARGET
    auto err = ilm_layerRemoveSurface(layerId, surfaceId);
    qDebug() << "surface " << surfaceId << " removed from layer " << layerId << (err == ILM_SUCCESS ? "successfully" : "failed");
#endif
}

void LayerManager::removeSurface(int surfaceId)
{
#ifdef BUILD_TARGET
    auto err = ilm_surfaceRemove(surfaceId);
    qDebug() << "surface " << surfaceId << " removed " << (err == ILM_SUCCESS ? "successfully" : "failed");
#endif
}

#ifdef USE_WAYLAND_INPUT
void pointerEventEnter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    int lX = (int) wl_fixed_to_double(surface_x);
    int lY = (int) wl_fixed_to_double(surface_x);

    qDebug() << "[NATIVE] pointerEventEnter(" << lX << ", " << lY << ")";
}

void pointerEventLeave(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface)
{
    qDebug() << "[NATIVE] pointerEventLeave";
}

void pointerEventMotion(void *data, struct wl_pointer *wl_pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    int lX = (int) wl_fixed_to_double(surface_x);
    int lY = (int) wl_fixed_to_double(surface_x);

    qDebug() << "[NATIVE] pointerEventMotion(" << lX << ", " << lY << ")";
}

void pointerEventButton(void *data, struct wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    qDebug() << "[NATIVE] pointerEventButton(button:" << button << ", state:" << state << ")";
}

void pointerEventAxis(void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    int lValue = (int) wl_fixed_to_double(value);

    qDebug() << "[NATIVE] pointerEventAxis(axis:" << axis << ", value:" << value << ", lValue:" << lValue << ", data:" << (int)data << ")";

    auto *layerManager = static_cast<LayerManager*>(data);

    layerManager->emitPointerEvent(axis, lValue);
}

static const struct wl_pointer_listener pointerEventHandler = {
    pointerEventEnter,
    pointerEventLeave,
    pointerEventMotion,
    pointerEventButton,
    pointerEventAxis,
};

void seatCapsHandle(void* data, struct wl_seat* seat, uint32_t caps) {
    // CSW, SWRC는 포인터로부터 데이터가 들어옴.
    if (caps & WL_SEAT_CAPABILITY_POINTER) {
        auto wlPointer = wl_seat_get_pointer(seat);

        wl_pointer_set_user_data(wlPointer, data);

        wl_pointer_add_listener(wlPointer, &pointerEventHandler, data);
    }
}

static struct wl_seat_listener seatListener = {
    seatCapsHandle,
};

void registryHandle(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
//    registryHandle(name: 1, interface: wl_compositor ,version: 4 )
//    registryHandle(name: 2, interface: wl_shm ,version: 1 )
//    registryHandle(name: 3, interface: serverinfo ,version: 1 )
//    registryHandle(name: 4, interface: wl_output ,version: 3 )
//    registryHandle(name: 5, interface: wl_viv ,version: 1 )
//    registryHandle(name: 6, interface: ilm_wl_drm ,version: 1 )
//    registryHandle(name: 7, interface: wl_seat ,version: 6 )

    if (0 == strcmp(interface, "wl_seat")) {
        struct wl_seat* wlSeat = (wl_seat*)wl_registry_bind(registry, name, &wl_seat_interface, 1);

        wl_seat_add_listener(wlSeat, &seatListener, data);
    }
}

static struct wl_registry_listener registryListener = {
    registryHandle,
    NULL
};
#endif //USE_WAYLAND_INPUT

void LayerManager::initWayland(QWindow *)
{
#ifdef USE_WAYLAND_INPUT
    qDebug() << "[NATIVE] initWayland() #1";

    QPlatformNativeInterface* native = QGuiApplication::platformNativeInterface();
    struct wl_display* display = (struct wl_display*)native->nativeResourceForWindow("display", NULL);
    if (display) {
        qDebug() << "[NATIVE] ws_display: " << (int)display << "this: " << (int)this;

        auto wlRegistry = wl_display_get_registry(display);

        wl_registry_add_listener(wlRegistry, &registryListener, this);

        wl_display_dispatch(display);

        wl_display_roundtrip(display);
    }
#endif
}

void LayerManager::emitPointerEvent(int axis, int value)
{
    emit pointerEvent(axis, value);
}

bool LayerManager::init()
{
    if (!m_initialized) {
#ifdef BUILD_TARGET
        if (ilm_init() != ILM_SUCCESS) { //only call once in one application
            qFatal("ilm_init failed");
            return false;
        }
#endif
        m_initialized = true;

        qDebug() << "LayerManager::init(" << m_initialized << ")";
    }

    return true;
}

bool LayerManager::registerWindow(QQuickWindow* win, const SurfaceProperty* property)
{
#ifndef QT_NO_DEBUG_OUTPUT
    qDebug(sys) << "LayerManager::registerWindow" << win << property->GetSurfaceId();
#endif
    bool success = false;

    if (!win) {
        return false;
    }

    WId winId = win->winId();
    if (m_windows.contains(winId)) {
        qWarning() << "already registered window " << win;
        return false;
    }

    if (!init()) {
        return false;
    }

    t_ilm_surface surfaceId = property->GetSurfaceId();
    t_ilm_layer layerId = property->GetLayerId();

    // 등록하려는 surface가 이미 등록이 되어 있다면 제거 후 다시 등록한다.
    // 19.06.07 ymhong.
    if (layerHasSurface(layerId, surfaceId)) {
        removeSurfaceFromLayer(layerId, surfaceId);
        removeSurface(surfaceId);
    }

#ifdef BUILD_TARGET
    qDebug() << "Trying to register window \"" << win << "\" on surface" << surfaceId << ", layer" << layerId;
    qDebug() << "width=" << win->width() << " height=" << win->height();

    do {
        success = ilm_surfaceCreate((t_ilm_nativehandle)winId, property->GetWidth(), property->GetHeight(),
                                         ILM_PIXELFORMAT_RGBA_8888, &surfaceId) == ILM_SUCCESS;
        if (!success) {
            qDebug() << "registerWindow 1:" << "ilm_surfaceCreate";
            break;
        }
        success = ilm_layerAddSurface(layerId, surfaceId) == ILM_SUCCESS;
        if (!success) {
            qDebug() << "registerWindow 2:" << "ilm_layerAddSurface";
            break;
        }
        success = ilm_surfaceSetVisibility(surfaceId, property->IsSurfaceVisible()) == ILM_SUCCESS;
        if (!success) {
            qDebug() << "registerWindow 3:" << "ilm_surfaceSetVisibility";
            break;
        }
        success = ilm_surfaceSetDestinationRectangle(surfaceId, property->GetX(), property->GetY(), win->width(), win->height()) == ILM_SUCCESS;
        if (!success) {
            qDebug() << "registerWindow 4:" << "ilm_surfaceSetDestinationRectangle";
            break;
        }
        success = ilm_surfaceSetSourceRectangle(surfaceId, 0, 0, win->width(), win->height()) == ILM_SUCCESS;
        if (!success) {
            qDebug() << "registerWindow 5:" << "ilm_surfaceSetSourceRectangle";
            break;
        }
        success = ilm_commitChanges() == ILM_SUCCESS;
        if (!success) {
            qDebug() << "registerWindow 6:" << "ilm_commitChanges";
            break;
        }
        success = ilm_UpdateInputEventAcceptanceOn(surfaceId, ILM_INPUT_DEVICE_ALL, ILM_TRUE) == ILM_SUCCESS;
        if (!success) {
            qDebug() << "registerWindow 7:" << "ilm_UpdateInputEventAcceptanceOn";
        } else {
            qDebug() << "registerWindow 8: successed.";
        }
    } while (0);
#else
    success = true;
#endif

    if (success) {
        qDebug() << "Registered" << win << "[ Id:" << winId << "]" << "on surface" << surfaceId << ", layer" << layerId;

        WindowInfo winInfo;
        winInfo.winId = winId;
        winInfo.layerId = layerId;
        winInfo.surfaceId = surfaceId;

        m_windows.insert(win->winId(), winInfo);
    } else {
        qWarning() << "Could not register window" << win;
    }
    return success;
}

void LayerManager::onRegisterAppWindow(QQuickWindow* win, const SurfaceProperty* property)
{
    auto success = registerWindow(win, property);

    emit layerManagerInitialized(success);

    // CSW, SWRC 이벤트를 직접 처리하기 위한 Wayland 초기화
    // 19.09.17 ymhong
    initWayland(win);
}

void LayerManager::onSetupPopupWindow(QQmlEngine* qmlEngine)
{
#ifdef BUILD_TARGET
    qDebug() << "PlatformControllerGen3::setupPopupWindow(QQmlEngine* qmlEngine)";

    m_WindowEngine = qmlEngine;

    onSetupPopup();
#endif
}

void LayerManager::onSetPopupProperty(int layerId, int surfaceId, int width, int height)
{
#ifdef BUILD_TARGET
    mPopupProperty.m_LayerId = layerId;
    mPopupProperty.m_SurfaceId = surfaceId;
    mPopupProperty.m_Width = width;
    mPopupProperty.m_Height = height;
#endif
}

void LayerManager::onSetToastPopupProperty(int layerId, int surfaceId, int width, int height)
{
#ifdef BUILD_TARGET
    mToastPopupProperty.m_LayerId = layerId;
    mToastPopupProperty.m_SurfaceId = surfaceId;
    mToastPopupProperty.m_Width = width;
    mToastPopupProperty.m_Height = height;
#endif
}

void LayerManager::onSetupPopup()
{
    qDebug() << "LayerManager::onSetupPopup()";

#ifdef BUILD_TARGET
    if (m_WindowEngine) {
        if (!mIsPopupWindowRegistered) {
            const int popupLayerId = mPopupProperty.m_LayerId;
            const int popupSurfaceId = mPopupProperty.m_SurfaceId;
            auto width = mPopupProperty.m_Width;
            auto height = mPopupProperty.m_Height;
            auto popupProperty = SurfaceProperty(false, width, height, 0, 0, popupLayerId, popupSurfaceId);
            auto popupWindow = createPopupWindow(m_WindowEngine, popupProperty);

            mIsPopupWindowRegistered = registerWindow(popupWindow, &popupProperty);
            if (mIsPopupWindowRegistered) {
                auto popupContentItem = popupWindow->contentItem();

                m_WindowEngine->rootContext()->setProperty("popupRoot", QVariant::fromValue(popupContentItem));
                m_WindowEngine->rootContext()->setProperty("popupWindow", QVariant::fromValue(popupWindow));

                emit surfaceRegistered(popupSurfaceId);
            } else {
                qDebug() << "m_layerManager->registerWindow(popupWindow, &popupProperty)) - Fail";
                delete popupWindow;
                emit surfaceRegisterFailed(popupSurfaceId);
            }
        }

        if (!mIsToastWindowRegistered) {
            const int toastLayerId = mToastPopupProperty.m_LayerId;;
            const int toastSurfaceId = mToastPopupProperty.m_SurfaceId;;
            auto width = mToastPopupProperty.m_Width;
            auto height = mToastPopupProperty.m_Height;
            auto toastProperty = SurfaceProperty(false, width, height, 0, 0, toastLayerId, toastSurfaceId);
            auto toastWindow = createPopupWindow(m_WindowEngine, toastProperty);

            mIsToastWindowRegistered = registerWindow(toastWindow, &toastProperty);
            if (mIsToastWindowRegistered) {
                auto toastContentItem = toastWindow->contentItem();

                auto rootContext = m_WindowEngine->rootContext();
                rootContext->setProperty("toastRoot", QVariant::fromValue(toastContentItem));
                rootContext->setProperty("toastWindow", QVariant::fromValue(toastWindow));

                emit surfaceRegistered(toastSurfaceId);
            } else {
                qDebug() << "m_layerManager->registerWindow(toastWindow, &toastProperty)) - Fail";
                delete toastWindow;
                emit surfaceRegisterFailed(toastSurfaceId);
            }
        }
    } else {
        qDebug() << "LayerManager::onSetupPopup() : m_WindowEngine is null.";
    }
#endif
}

QQuickView* LayerManager::createPopupWindow(QQmlEngine* engine, const SurfaceProperty& surfaceProperty)
{
    QQuickView* quickView = new QQuickView(engine, nullptr);
#ifdef BUILD_TARGET
    qDebug() << "[popup] window = " << reinterpret_cast<int>(quickView);
#endif

    auto w = surfaceProperty.GetWidth();
    auto h = surfaceProperty.GetHeight();
    quickView->setWidth(w);
    quickView->setHeight(h);
    quickView->setSurfaceType(QSurface::OpenGLSurface);

    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    format.setRenderableType(QSurfaceFormat::OpenGLES);

    quickView->setFormat(format);
    quickView->setClearBeforeRendering(true);
    quickView->setFlags(Qt::FramelessWindowHint);
    quickView->setColor(QColor(0,0,0,0.95));

    return quickView;
}

}
