#include <bgfx_renderer.h>

#include <bgfx_item/bgfx_item.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

#include <QtQuick/QQuickWindow>
//#include <QQuickItem>
#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

namespace bgfxRenderer
{

static std::vector<BgfxItem*> bgfxItems;

bool initialized()
{
    return bgfx::getInternalData()->context;
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
        QObject::connect(window, &QQuickWindow::beforeRenderPassRecording, [win = window](){
            win->beginExternalCommands();
            bgfxRenderer::render();
            win->endExternalCommands();
        });
        QObject::connect(window, &QQuickWindow::afterRenderPassRecording, bgfxRenderer::frame);
        QObject::connect(window, &QQuickWindow::destroyed, bgfxRenderer::shutdown);

        bgfx::frame();
    }
}

void addBgfxItem(BgfxItem* item)
{
    bgfxItems.push_back(item);
}

void render()
{
    if (!bgfxRenderer::initialized())
        return;

    for(const auto item : bgfxItems)
    {
        float r{0.0f};
        float g{0.0f};
        float b{0.0f};
        item->backgroundColor().getRgbF(&r, &g, &b);

        const uint32_t color = uint8_t(r * 255) << 24 | uint8_t(g * 255) << 16 | uint8_t(b * 255) << 8 | 255;

        bgfx::setViewClear(item->viewId(), BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, color, 1.0f, 0);
        bgfx::touch(item->viewId());
    }
}

void frame()
{
    if(initialized())
    {
        bgfx::frame();
    }
}

void shutdown()
{
    if(initialized())
    {
        bgfx::shutdown();
    }
    bgfxItems.clear();
}
}
