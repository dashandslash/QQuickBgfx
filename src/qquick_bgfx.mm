#include "qquick_bgfx.h"

#include <Cocoa/Cocoa.h>
#include <QuartzCore/CAMetalLayer.h>


template<>
bgfx::Init QQuickBgfx::init<bgfx::RendererType::Metal>(void *windowHandler, void* context, const uint16_t width, const uint16_t height)
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
