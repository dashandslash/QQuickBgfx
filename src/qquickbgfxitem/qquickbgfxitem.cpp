#include "qquickbgfxitem.h"

#include "qsgbgfxnode/Metal/qsgbgfxnode.h"
#include "qquick_bgfx.h"


QQuickBgfxItem::QQuickBgfxItem()
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(QQuickItem::ItemAcceptsInputMethod);
    setFlag(QQuickItem::ItemHasContents);
}

QQuickBgfxItem::~QQuickBgfxItem() = default;

void QQuickBgfxItem::invalidateSceneGraph()
{
    m_node->deleteLater();
    m_node.reset();
}

void QQuickBgfxItem::releaseResources()
{
    m_node->deleteLater();
    m_node.reset();
}

QSGNode* QQuickBgfxItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QSGBgfxNode *node = static_cast<QSGBgfxNode *>(oldNode);
    const auto size = boundingRect().size().toSize();
    if (!QQuickBgfx::initialized() || (!node && (size.width() <= 0 || size.height() <= 0)))
    {
        return node;
    }

    if (!node)
    {
        m_node = std::make_unique<QSGBgfxNode>(m_viewId, this);
    }
     
    m_node->setRect(boundingRect());

    m_node->sync();

    m_node->setTextureCoordinatesTransform(QSGSimpleTextureNode::NoTransform);
    m_node->setFiltering(QSGTexture::Linear);

    return m_node.get();
}

void QQuickBgfxItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);

    if (newGeometry.size() != oldGeometry.size())
        update();
}

void QQuickBgfxItem::mouseMoveEvent(QMouseEvent *event)
{
    QQuickItem::mouseMoveEvent(event);
    m_mousePos = {event->position().toPoint().x(), event->position().toPoint().y()};
}

void QQuickBgfxItem::mousePressEvent(QMouseEvent *event)
{
    if(!(event->button() & acceptedMouseButtons()))
    {
        QQuickItem::mousePressEvent(event);
        return;
    }
    m_mousePos = {event->position().toPoint().x(), event->position().toPoint().y()};
    event->setAccepted(true);
}

void QQuickBgfxItem::mouseReleaseEvent(QMouseEvent *event)
{
    if(!(event->button() & acceptedMouseButtons()))
    {
        QQuickItem::mouseReleaseEvent(event);
        return;
    }
    m_mousePos = {event->position().toPoint().x(), event->position().toPoint().y()};
    event->setAccepted(true);
}

void QQuickBgfxItem::setViewId(uint16_t viewId)
{
    if (viewId == m_viewId)
        return;

    m_viewId = viewId;
    emit viewIdChanged();

    update();
}

void QQuickBgfxItem::setBackgroundColor(QColor color)
{
    if (color == m_backgroundColor)
        return;

    m_backgroundColor = color;
    emit backgroundColorChanged();

    update();
}
