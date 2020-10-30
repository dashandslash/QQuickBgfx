#include "bgfx_item.h"

#include "bgfx_node/Metal/bgfx_node.h"
#include "bgfx_renderer.h"

#include <QtGui/QScreen>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtCore/QFile>


BgfxItem::BgfxItem()
{
    setFlag(ItemHasContents, true);
}

void BgfxItem::invalidateSceneGraph()
{
    m_node = nullptr;
}

void BgfxItem::releaseResources()
{
    m_node = nullptr;
}

QSGNode *BgfxItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    BgfxNode *node = static_cast<BgfxNode *>(oldNode);

    if (!node && (width() <= 0 || height() <= 0))
        return nullptr;

    if (!node) {
        m_node = new BgfxNode(m_viewId, m_backgroundColor, this);
        connect(this, &BgfxItem::backgroundColorChanged, [&]{
            m_node->backgroundColor = m_backgroundColor;
        });
    }
    
    m_node->setRect(boundingRect());

    if(!bgfxRenderer::initialized())
    {
        auto win = window();
        const auto dpr = win->effectiveDevicePixelRatio();
        bgfxRenderer::init(win, win->width() * dpr, win->height() * dpr, bgfx::RendererType::Metal);
    }

    m_node->sync();

    m_node->setTextureCoordinatesTransform(QSGSimpleTextureNode::NoTransform);
    m_node->setFiltering(QSGTexture::Linear);

    return m_node;
}

void BgfxItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);

    if (newGeometry.size() != oldGeometry.size())
        update();
}

void BgfxItem::setViewId(uint16_t viewId)
{
    if (viewId == m_viewId)
        return;

    m_viewId = viewId;
    emit viewIdChanged();

    update();
}

void BgfxItem::setBackgroundColor(QColor color)
{
    if (color == m_backgroundColor)
        return;

    m_backgroundColor = color;
    emit backgroundColorChanged();

    update();
}


//#include "bgfx_item.moc"
