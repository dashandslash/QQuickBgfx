#pragma once

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

#include <QtQuick/QQuickWindow>
#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

namespace BgfxRenderer
{
static bool initialized()
{
    return bgfx::getInternalData()->context;
}

static void init(QQuickWindow *window, const uint16_t width, const uint16_t height, bgfx::RendererType::Enum type)
{
    if(!initialized())
    {
        QSGRendererInterface *rif = window->rendererInterface();
        auto device = (id<MTLDevice>) rif->getResource(window, QSGRendererInterface::DeviceResource);
        assert(device);
        bgfx::Init init;
        init.type = type;
        init.platformData.context = device;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.resolution.width = width;
        init.resolution.height = height;
        switch (type) {
            case bgfx::RendererType::Metal:
                init.platformData.nwh = reinterpret_cast<CAMetalLayer *>(reinterpret_cast<NSView *>(window->winId()).layer);
                break;
            default:
                break;
        }
        bgfx::renderFrame();
        
        bgfx::init(init);
        bgfx::setDebug(BGFX_DEBUG_TEXT);
        bgfx::frame();
    }
}
}
