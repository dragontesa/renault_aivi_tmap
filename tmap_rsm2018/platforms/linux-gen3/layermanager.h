#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include <QMap>
#include <QWindow>
#ifdef BUILD_TARGET
#include <ilm/ilm_client.h>
#include <ilm/ilm_control.h>
#include "guicontroltype.h"
#include <wayland-client-protocol.h>
#else
typedef unsigned int t_ilm_surface;
typedef unsigned int t_ilm_layer;
#endif


QT_BEGIN_NAMESPACE
class QQmlEngine;
class QQuickWindow;
class QQuickView;
QT_END_NAMESPACE

namespace SKT {

class SurfaceProperty
{
public:

    /**
     * Constructor
     */
    SurfaceProperty();
    SurfaceProperty(bool isSurfaceVisibile, uint width, uint height, int x, int y, uint layerId, uint surfaceId, float surfaceOpacity=1.0F);

    /**
     * Destructor
     */
    virtual ~SurfaceProperty();

    // Properties ----------------------------------------------------------------------------------------

    /**
     * Set the width of the window associated to this surface.
     * @param width value to use as width or negative value to use display width.
     */
    void SetWidth(uint width) { m_width = width; }
    /**
     * Get the width of the window associated to this surface.
     * @return value set by SetWidth or the default -1.
     */
    uint GetWidth() const { return m_width; }

    /**
     * Set the height of the window associated to this surface.
     * @param height value to use as height or negative value to use display height.
     */
    void SetHeight(uint height) { m_height = height; }
    /**
     * Get the height of the window associated to this surface.
     * @return value set by SetHeight or the default -1.
     */
    uint GetHeight() const { return m_height; }

    /**
     * Set the position in the X direction of the window associated to this surface.
     * @param x value to use as X position.
     */
    void SetX(int x) { m_x = x; }
    /**
     * Get the position in the X direction of the window associated to this surface.
     * @return value set by SetX or the default 0.
     */
    int GetX() const { return m_x; }

    /**
     * Set the position in the Y direction of the window associated to this surface.
     * @param y value to use as Y position.
     */
    void SetY(int y) { m_y = y; }
    /**
     * Get the position in the Y direction of the window associated to this surface.
     * @return value set by SetY or the default 0.
     */
    int GetY() const { return m_y; }

    /**
     * Set the ILM layer index to associate this surface to.
     * @param The ILM layer index to associate this surface to.
     */
    void SetLayerId(t_ilm_layer layerId) { m_layerId = layerId; }
    /**
     * Gets the index of the ILM layer this surface is associated to.
     * @return the index of the ILM layer this surface is associated to.
     */
    t_ilm_layer GetLayerId() const { return m_layerId; }

    /**
     * Set the ILM surface index to associate this surface to.
     * @param The ILM surface index to associate this surface to.
     */
    void SetSurfaceId(t_ilm_surface surfaceId){ m_surfaceId = surfaceId; }
    /**
     * Gets the index of the ILM surface this surface is associated to.
     * @return the index of the ILM surface this surface is associated to.
     */
    t_ilm_surface GetSurfaceId() const { return m_surfaceId; }

    /**
     * Sets the opacity of the ILM surface.
     * @param opacity Opacity of the ILM surface.
     */
    void SetSurfaceOpacity(float opacity) { m_surfaceOpacity = opacity; }
    /**
     * Gets the opacity of the ILM surface.
     * @return  Opacity of the ILM surface.
     */
    float GetSurfaceOpacity() const { return m_surfaceOpacity; }

    /**
     * Sets if ILM surface is visible or not.
     * @param visibility Visibility of the ILM surface.
     */
    void SetSurfaceVisibility(bool visibility) { m_isSurfaceVisibile = visibility; }
    /**
     * Gets if ILM surface is visible or not.
     * @return visibility Visibility of the ILM surface.
     */
    bool IsSurfaceVisible() const { return m_isSurfaceVisibile; }

    /**
     * Enable or disable this surface. The surface is enabled after information
     * on the attached render target was updated (i.e. when RenderTarget::SwapBuffers is called).
     * The surface is disabled the first time the display is updated. (i.e. as soon as possible.)
     * Note GraphicDeviceUnit::ApplyChanges must be called for this change to take effect.
     * @param enabled true to enable, false to disable.
     */
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    /**
     * Check if surface is enabled.
     * @return value set by SetEnabled or the default true.
     */
    bool IsEnabled() const { return m_enabled; }

private:
    bool m_enabled;
    bool m_isSurfaceVisibile;

    uint m_width;
    uint m_height;
    int m_x;
    int m_y;

    t_ilm_layer m_layerId;
    t_ilm_surface m_surfaceId;

    float m_surfaceOpacity;
};

class LayerManager: public QObject
{
    Q_OBJECT
public:
    LayerManager(QObject* parent=nullptr);
    ~LayerManager();

    enum SurfaceId {
        SurfaceId_Zone3 = 94,
    };

    bool layerHasSurface(int layerId, int surfaceId);
    void removeSurfaceFromLayer(int layerId, int surfaceId);
    void removeSurface(int surfaceId);

    void initWayland(QWindow* win);
    void emitPointerEvent(int axis, int value);

public slots:
    void onRegisterAppWindow(QQuickWindow* window, const SurfaceProperty* property);
    void onSetupPopupWindow(QQmlEngine* qmlEngine);
    void onSetPopupProperty(int layerId, int surfaceId, int width, int height);
    void onSetToastPopupProperty(int layerId, int surfaceId, int width, int height);
    void onSetupPopup();

signals:
    void layerManagerInitialized(bool success);
    void surfaceRegistered(int surfaceId);
    void surfaceRegisterFailed(int surfaceId);
    void pointerEvent(int axis, int value);

private:
    bool init();
    bool registerWindow(QQuickWindow* window, const SurfaceProperty* property);
    QQuickView* createPopupWindow(QQmlEngine* engine, const SurfaceProperty& surfaceProperty);

private:
    bool m_initialized;

    struct WindowInfo {
        WId winId;
        t_ilm_layer layerId;
        t_ilm_surface surfaceId;
    };

    QMap<WId, WindowInfo> m_windows;

    QQmlEngine* m_WindowEngine = nullptr;
#ifdef BUILD_TARGET
    PopupProperty mPopupProperty;
    PopupProperty mToastPopupProperty;
#endif

    bool mIsPopupWindowRegistered = false;
    bool mIsToastWindowRegistered = false;
};

}

#endif // LAYERMANAGER_H
