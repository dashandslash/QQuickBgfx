#include "qquick_bgfx.h"

#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

#include <QSGTextureProvider>
#include <QQuickWindow>


bgfx::Init QQuickBgfx::initMetalBackend(void *windowHandler, void* context, const uint16_t width, const uint16_t height)
{
    if(!initialized())
    {
        bgfx::Init init;
        init.type = bgfx::RendererType::Metal;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.resolution.width = width;
        init.resolution.height = height;
        init.platformData.nwh = reinterpret_cast<CAMetalLayer *>(reinterpret_cast<NSView *>(windowHandler).layer);
        init.platformData.context = static_cast<id<MTLDevice>>(context);
        return init;
    }
    else
        return bgfx::Init();
}

QQuickBgfx::TextureHandles QQuickBgfx::CreateQSGMetalTexture(QQuickWindow *window, int w, int h)
{
    auto dev = (id<MTLDevice>)bgfx::getInternalData()->context;
    MTLTextureDescriptor *desc = [[MTLTextureDescriptor alloc] init];
    desc.textureType = MTLTextureType2D;
    desc.pixelFormat = MTLPixelFormatRGBA8Unorm;
    desc.width = w;
    desc.height = h;
    desc.mipmapLevelCount = 1;
    desc.resourceOptions = MTLResourceStorageModePrivate;
    desc.storageMode = MTLStorageModePrivate;
    desc.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
    id<MTLTexture> MTL_texture = [dev newTextureWithDescriptor: desc];
    if (MTL_texture == nil)
    {
        return {nullptr, reinterpret_cast<uintptr_t>(MTL_texture)};
    }
    return {QNativeInterface::QSGMetalTexture::fromNative(MTL_texture, window, {w, h}), reinterpret_cast<uintptr_t>(MTL_texture)};
}
