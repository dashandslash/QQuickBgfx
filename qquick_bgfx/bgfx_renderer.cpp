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
    connect(window, &QQuickWindow::beforeRenderPassRecording, this, &Renderer::renderFrame, Qt::DirectConnection);
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

void Renderer::renderFrame()
{
    if (!qquick_bgfx::initialized())
        return;
    
    window->beginExternalCommands();
    emit render(bgfxItems);
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
