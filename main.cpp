#define ENTT_USE_ATOMIC

#include <engine.h>
#include <qquickbgfxitem/qquickbgfxitem.h>
#include <qbgfx.h>
#include <qbgfx_utils.h>
#include <qquick_bgfx.h>
#include <renderer.h>

#include <QGuiApplication>
#include <QQuickView>
#include <QTimer>

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <debugdraw/debugdraw.h>
#include <entt/entt.hpp>

#include <iostream>

#include <thread>
#include <mutex>

entt::registry registry;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

#if (WIN32)
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Direct3D11Rhi);
#else
    QQuickWindow::setGraphicsApi(QSGRendererInterface::MetalRhi);
#endif

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///BgfxQQuickItemExample/main.qml"));
    view.show();
    auto items = view.rootObject()->findChildren<QQuickBgfxItem *>();
    const auto qbgfx = QQuickBgfx::QBgfx(static_cast<QQuickWindow *>(&view), items);
    QQuickBgfx::utils::connectItemsSignals(registry, items);

    //Connection to initialized signal allows to decouple the bgfx initialization from the qquick_bgfx::QBgfx wrapper
    QObject::connect(&qbgfx, &QQuickBgfx::QBgfx::initialized, renderer::init);
    //Connection to render signal allows to decouple the rendering code from the qquick_bgfx::QBgfx wrapper
    QObject::connect(&qbgfx, &QQuickBgfx::QBgfx::render, [](){
//        engine::update(registry);
        renderer::render(registry);
    });

    QTimer timer;
    timer.setInterval(16);
    timer.setSingleShot(false);
    timer.setTimerType(Qt::PreciseTimer);
    QObject::connect(&timer, &QTimer::timeout, [&]()
    {
        engine::update(registry);
    });
    timer.start();
    return app.exec();
}
