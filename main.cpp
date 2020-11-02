#include <qquick_bgfx/bgfx_renderer.h>
#include <bgfx_item/bgfx_item.h>

#include <QGuiApplication>
#include <QQuickView>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QQuickWindow::setGraphicsApi(QSGRendererInterface::MetalRhi);

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///BgfxQQuickItemExample/main.qml"));
    view.show();

    auto r = qquick_bgfx::Renderer(static_cast<QQuickWindow*>(&view), view.rootObject()->findChildren<BgfxItem*>());

    return app.exec();
}
