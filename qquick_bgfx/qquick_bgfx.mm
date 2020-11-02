#include <qquick_bgfx.h>

#include <bgfx_item/bgfx_item.h>

#include <Cocoa/Cocoa.h>
#include <QuartzCore/CAMetalLayer.h>


template<>
void qquick_bgfx::init<bgfx::RendererType::Metal>(void *windowHandler, void* context, const uint16_t width, const uint16_t height)
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
        bgfx::renderFrame();
        bgfx::init(init);
        bgfx::setDebug(BGFX_DEBUG_TEXT);
    }
}
