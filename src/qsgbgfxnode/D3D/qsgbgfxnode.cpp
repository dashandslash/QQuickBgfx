#include "qsgbgfxnode.h"

#include "qbgfx.h"

#include <QSGTexture>
#include <QQuickItem>

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

        QSGRendererInterface *rif = m_window->rendererInterface();

        if (texture())
        {
            texture()->deleteLater();
            //[static_cast<id<MTLTexture>>(m_texture) release];
        }
        if(bgfx::isValid(m_backBuffer))
        {
            bgfx::destroy(m_backBuffer);
        }

        if(bgfx::isValid(m_backBuffer))
        {
            bgfx::destroy(m_depthBuffer);
        }

        m_backBuffer = bgfx::createTexture2D(width, height, false, 2, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT, NULL);
        m_depthBuffer = bgfx::createTexture2D(width, height, false, 2, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT, NULL);
        
        // The call to frame() is necessary to actually construct the textures in GPU
        bgfx::frame();

        std::array<bgfx::Attachment, 2> m_attachments{};
        auto &[colorAttachment, depthAttachment] = m_attachments;
        colorAttachment.init(m_backBuffer, bgfx::Access::Write, 0);
        depthAttachment.init(m_depthBuffer, bgfx::Access::Write, 0);
        
        if(bgfx::isValid(m_offscreenFB))
        {
            bgfx::destroy(m_offscreenFB);
        }

        m_offscreenFB = bgfx::createFrameBuffer(m_attachments.size(), m_attachments.data(), false);
        
        bgfx::setViewFrameBuffer(m_viewId, m_offscreenFB);

        bgfx::setViewRect(m_viewId, 0, 0, width, height);
 
        m_texture = (void*)bgfx::getInternal(m_backBuffer);

        QSGTexture* qsgtexture{nullptr};
        switch (rif->graphicsApi())
        {
        case QSGRendererInterface::MetalRhi:
#ifdef __APPLE_
            qsgtexture = QNativeInterface::QSGMetalTexture::fromNative(m_texture, m_window, newSize);
#endif
            break;
        case QSGRendererInterface::Direct3D11Rhi:
            qsgtexture = QNativeInterface::QSGD3D11Texture::fromNative(m_texture, m_window, newSize);
            break;
        default:
            throw std::runtime_error("Invalid or not implemented Graphics Api");
            return;
        }

        // assign the QSGTexture to the node      
        setTexture(qsgtexture);
    }
}
