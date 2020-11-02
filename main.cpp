#include <qquick_bgfx/bgfx_renderer.h>
#include <bgfx_item/bgfx_item.h>

#include <QGuiApplication>
#include <QQuickView>

#include <bgfx/bgfx.h>


//This function runs the rendering code
void renderExample(const std::vector<BgfxItem*>& bgfxItems)
{
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
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QQuickWindow::setGraphicsApi(QSGRendererInterface::MetalRhi);

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///BgfxQQuickItemExample/main.qml"));
    view.show();

    const auto qbgfx = qquick_bgfx::QBgfx(static_cast<QQuickWindow*>(&view), view.rootObject()->findChildren<BgfxItem*>());
    
    //Connection to the emitted signal allows to decouple the rendering code from the qquick_bgfx::Renderer wrapper
    QObject::connect(&qbgfx, &qquick_bgfx::QBgfx::render, renderExample);
    
    return app.exec();
}
