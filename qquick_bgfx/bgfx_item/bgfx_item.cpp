#include "bgfx_item.h"

#include "bgfx_node/Metal/bgfx_node.h"
#include "qquick_bgfx.h"


BgfxItem::BgfxItem()
{
    setFlag(ItemHasContents, true);
}

BgfxItem::~BgfxItem() = default;

void BgfxItem::invalidateSceneGraph()
{
    m_node->deleteLater();
    m_node.reset();
}

void BgfxItem::releaseResources()
{
    m_node->deleteLater();
    m_node.reset();
}

QSGNode* BgfxItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    BgfxNode *node = static_cast<BgfxNode *>(oldNode);
    const auto size = boundingRect().size().toSize();
    if (!QQuickBgfx::initialized() || (!node && (size.width() <= 0 || size.height() <= 0)))
    {
        return node;
    }

    if (!node)
    {
        m_node = std::make_unique<BgfxNode>(m_viewId, this);
    }
     
    m_node->setRect(boundingRect());

    m_node->sync();

    m_node->setTextureCoordinatesTransform(QSGSimpleTextureNode::NoTransform);
    m_node->setFiltering(QSGTexture::Linear);

    return m_node.get();
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
