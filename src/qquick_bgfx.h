#pragma once
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

class QSGTexture;
class QQuickWindow;

namespace QQuickBgfx {

struct TextureHandles
{
    QSGTexture *qsgTextue{nullptr};
    uintptr_t nativeTextureHandle{};
};

#ifdef __APPLE__
bgfx::Init initMetalBackend(void *windowHandle, void *context, const uint16_t width, const uint16_t height);
TextureHandles CreateQSGMetalTexture(QQuickWindow *window, int w, int h);
#else
bgfx::Init initD3D11Backend(void *windowHandle, void *context, const uint16_t width, const uint16_t height);
TextureHandles CreateQSGD3D11Texture(QQuickWindow *window, int w, int h);
#endif

inline bgfx::Init initBackend(bgfx::RendererType::Enum graphicsApi, void *windowHandle, void *context,
                              const uint16_t width, const uint16_t height)
{
    switch (graphicsApi)
    {
        case bgfx::RendererType::Metal:
#ifdef __APPLE__
            return initMetalBackend(windowHandle, context, width, height);
#endif
            break;
        case bgfx::RendererType::Direct3D11:
#ifdef _WIN32
            return QQuickBgfx::initD3D11Backend(windowHandle, context, width, height);
#endif
            break;
        default:
            BX_ASSERT(false, "Invalid or not implemented Graphics Api");
            break;
    }
    return {};
}

inline TextureHandles CreateQSGTexture(QQuickWindow *window, int w, int h)
{
    switch (bgfx::getRendererType())
    {
        case bgfx::RendererType::Metal:
#ifdef __APPLE__
            return CreateQSGMetalTexture(window, w, h);
#endif
            break;
        case bgfx::RendererType::Direct3D11:
#ifdef _WIN32
            return CreateQSGD3D11Texture(window, w, h);
#endif
            break;
        default:
            BX_ASSERT(false, "Invalid or not implemented Graphics Api");
            break;
    }
    return {};
}

inline bool initialized()
{
    return bgfx::getInternalData()->context;
}

inline void frame()
{
    if (initialized())
    {
        bgfx::frame();
    }
}

}    // namespace QQuickBgfx
