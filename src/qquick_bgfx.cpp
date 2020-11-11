#include "qquick_bgfx.h"

#include <QSGTextureProvider>
#include <QQuickWindow>

#ifdef _WIN32
#include <d3d11.h>

template<>
bgfx::Init QQuickBgfx::init<bgfx::RendererType::Direct3D11>(void* windowHandler, void* context, const uint16_t width, const uint16_t height)
{
    if (!initialized())
    {
        bgfx::Init init;
        init.type = bgfx::RendererType::Direct3D11;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.resolution.width = width;
        init.resolution.height = height;
        init.platformData.context = reinterpret_cast<ID3D11Device*>(context);
        return init;
    }
    else
        return bgfx::Init();

}

template<>
QSGTexture* QQuickBgfx::qsgTexture<bgfx::RendererType::Direct3D11>(void* texture, QQuickWindow* window, int w, int h)
{
    return QNativeInterface::QSGD3D11Texture::fromNative(texture, window, { w, h });
}
#endif
