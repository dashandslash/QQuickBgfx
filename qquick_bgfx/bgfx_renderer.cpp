#include <bgfx_renderer.h>

#include <qquick_bgfx.h>
#include <bgfx_item/bgfx_item.h>

#include <bgfx/bgfx.h>

#include <QList>
#include <QQuickWindow>
//#include <QGuiApplication>


using namespace qquick_bgfx;

Renderer::Renderer(QQuickWindow* w, const QList<BgfxItem*> items) : window(w)
{
    //Qt::DirectConnection needs to be specified in order to call the slot from the signal thread
    connect(window, &QQuickWindow::sceneGraphInitialized, this, &Renderer::init, Qt::DirectConnection);
    connect(window, &QQuickWindow::beforeRenderPassRecording, this, &Renderer::render, Qt::DirectConnection);
    //Free standing function instead will always be called from the signal thread
    connect(window, &QQuickWindow::afterRenderPassRecording, qquick_bgfx::frame);
//    connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit, this, &Renderer::shutdown, Qt::QueuedConnection);

    bgfxItems.reserve(bgfxItems.size());
    bgfxItems.insert(bgfxItems.end(), items.begin(), items.end());
}

Renderer::~Renderer()
{
//    shutdown();
}

void Renderer::init()
{
    QSGRendererInterface *rif = window->rendererInterface();
    const auto dpr = window->effectiveDevicePixelRatio();
    auto layer = reinterpret_cast<void*>(window->winId());
    auto device = static_cast<void*>(rif->getResource(window, QSGRendererInterface::DeviceResource));
    switch (rif->graphicsApi()) {
        case QSGRendererInterface::MetalRhi:
            qquick_bgfx::init<bgfx::RendererType::Metal>(layer, device, window->width() * dpr, window->height() * dpr);
            break;
        default:
            throw std::invalid_argument("Invalid or not implemented Graphics Api");
            break;
    }
}

void Renderer::render()
{
    if (!qquick_bgfx::initialized())
        return;
    
    window->beginExternalCommands();
    
    for(const auto item : bgfxItems)
    {
        if (item->viewId() < 256)
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
    window->endExternalCommands();
}

void Renderer::shutdown()
{
    if(initialized())
    {
        bgfx::shutdown();
    }
    bgfxItems.clear();
}
