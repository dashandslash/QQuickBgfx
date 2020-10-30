#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

#include <QtQuick/QQuickWindow>
#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

namespace bgfxRenderer
{
bool initialized()
{
    return bgfx::getInternalData()->context;
}

void frame()
{
    if(initialized())
    {
        bgfx::frame();
    }
}

void init(QQuickWindow *window, const uint16_t width, const uint16_t height, bgfx::RendererType::Enum type)
{
    if(!initialized())
    {
        QSGRendererInterface *rif = window->rendererInterface();
        bgfx::Init init;
        init.type = type;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.resolution.width = width;
        init.resolution.height = height;
        switch (type) {
            case bgfx::RendererType::Metal:
                init.platformData.nwh = reinterpret_cast<CAMetalLayer *>(reinterpret_cast<NSView *>(window->winId()).layer);
                init.platformData.context = (id<MTLDevice>) rif->getResource(window, QSGRendererInterface::DeviceResource);
                break;
            default:
                break;
        }
        bgfx::renderFrame();
        
        bgfx::init(init);
        bgfx::setDebug(BGFX_DEBUG_TEXT);
        QObject::connect(window, &QQuickWindow::afterRenderPassRecording, bgfxRenderer::frame);

        bgfx::frame();
    }
}
}
