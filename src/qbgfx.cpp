#include <qbgfx.h>

#include "qquick_bgfx.h"
#include "qquickbgfxitem/qquickbgfxitem.h"

#include <bgfx/bgfx.h>

#include <QList>
#include <QQuickWindow>
//#include <QGuiApplication>


using namespace QQuickBgfx;

QBgfx::QBgfx(QQuickWindow* w, const QList<QQuickBgfxItem*> items) : m_window(w)
{
    //Qt::DirectConnection needs to be specified in order to call the slot from the signal thread
    connect(m_window, &QQuickWindow::sceneGraphInitialized, this, &QBgfx::init, Qt::DirectConnection);
    connect(m_window, &QQuickWindow::beforeRenderPassRecording, this, &QBgfx::renderFrame, Qt::DirectConnection);
    //Free standing function instead will always be called from the signal thread
    connect(m_window, &QQuickWindow::afterRenderPassRecording, QQuickBgfx::frame);
//    connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit, this, &Renderer::shutdown, Qt::QueuedConnection);

    m_bgfxItems.reserve(m_bgfxItems.size());
    m_bgfxItems.insert(m_bgfxItems.end(), items.begin(), items.end());
}

QBgfx::~QBgfx()
{
//    shutdown();
}

void QBgfx::init()
{
    QSGRendererInterface *rif = m_window->rendererInterface();
    const auto dpr = m_window->effectiveDevicePixelRatio();
    auto layer = reinterpret_cast<void*>(m_window->winId());
    auto device = static_cast<void*>(rif->getResource(m_window, QSGRendererInterface::DeviceResource));
    switch (rif->graphicsApi()) {
        case QSGRendererInterface::MetalRhi:
            m_bgfxInit = QQuickBgfx::init<bgfx::RendererType::Metal>(layer, device, m_window->width() * dpr, m_window->height() * dpr);
            break;
        default:
            throw std::invalid_argument("Invalid or not implemented Graphics Api");
            return;
    }

    emit initialized(m_bgfxInit);
}

void QBgfx::renderFrame()
{
    if (!QQuickBgfx::initialized())
        return;
    
    m_window->beginExternalCommands();
    emit render(m_bgfxItems);
    m_window->endExternalCommands();
}

void QBgfx::shutdown()
{
    if(QQuickBgfx::initialized())
    {
        bgfx::shutdown();
    }
    m_bgfxItems.clear();
}
