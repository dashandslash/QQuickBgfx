#include "qquick_bgfx.h"

#include <QSGTextureProvider>
#include <QQuickWindow>
#include <QOpenGLContext>

#ifdef __linux__
template<>
bgfx::Init QQuickBgfx::init<bgfx::RendererType::OpenGL>(void* windowHandler, void* context, const uint16_t width, const uint16_t height)
{
    if (!initialized())
    {
        bgfx::Init init;
        init.type = bgfx::RendererType::OpenGL;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.resolution.width = width;
        init.resolution.height = height;
        init.platformData.context = QOpenGLContext::currentContext();
        return init;
    }
    else
        return bgfx::Init();

}

template<>
QSGTexture* QQuickBgfx::qsgTexture<bgfx::RendererType::OpenGL>(void* texture, QQuickWindow* window, int w, int h)
{
    return QNativeInterface::QSGOpenGLTexture::fromNative(reinterpret_cast<uintptr_t>(texture), window, QSize(w, h), QQuickWindow::TextureHasAlphaChannel);
}
#endif
