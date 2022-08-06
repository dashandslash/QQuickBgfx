#include "qsgbgfxnode.h"

#include "qbgfx.h"
#include <qquick_bgfx.h>

#include <bgfx/platform.h>

#include <stdexcept>


QSGBgfxNode::QSGBgfxNode(const uint16_t viewId, QQuickItem *item)
: m_item(item), m_viewId(viewId)
{
    m_window = m_item->window();
    connect(m_window, &QQuickWindow::screenChanged, this, [this]() {
        if (m_window->effectiveDevicePixelRatio() != m_dpr)
            m_item->update();
    });

    qDebug() << "BgfxNode created, view id: " << viewId;
}

QSGBgfxNode::~QSGBgfxNode()
{
    qDebug() << "BgfxNode destroyed, view id: " << m_viewId;
}

QSGTexture *QSGBgfxNode::texture() const
{
    return QSGSimpleTextureNode::texture();
}

void QSGBgfxNode::sync()
{
    m_dpr = m_window->effectiveDevicePixelRatio();
    const auto newSize = (rect().size() * m_dpr).toSize();
    assert(!newSize.isEmpty());

    // In case there is no qsgtexture attached to the node or the node has a new size
    if (!texture() || (newSize != m_size)) {
        m_size = newSize;
        const auto width = static_cast<uint16_t >(newSize.width());
        const auto height = static_cast<uint16_t >(newSize.height());

        auto &backBuffer = m_attachments[0];
        auto &depthBuffer = m_attachments[1];

        if (texture())
        {
            texture()->deleteLater();
        }
        if (bgfx::isValid(backBuffer))
        {
            bgfx::destroy(backBuffer);
        }

        if (bgfx::isValid(depthBuffer))
        {
            bgfx::destroy(depthBuffer);
        }

        backBuffer = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT, NULL);
        depthBuffer = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT, NULL);

        bgfx::frame();

        QQuickBgfx::TextureHandles textureHandles;
        textureHandles = QQuickBgfx::CreateQSGTexture(m_window, width, height);

        auto newBgfxInternalId = bgfx::overrideInternal(backBuffer, textureHandles.nativeTextureHandle);

		if (bgfx::isValid(m_offscreenFB))
		{
			bgfx::destroy(m_offscreenFB);
		}
		m_offscreenFB = bgfx::createFrameBuffer(2, m_attachments, false);
		bgfx::setViewFrameBuffer(m_viewId, m_offscreenFB);
		bgfx::setViewRect(m_viewId, 0, 0, width, height);

        // assign the QSGTexture to the node
        setTexture(textureHandles.qsgTextue);
    }
}
