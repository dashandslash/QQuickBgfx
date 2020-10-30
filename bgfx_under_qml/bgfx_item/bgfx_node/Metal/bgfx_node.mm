#include "bgfx_node.h"

#include "bgfx_renderer.h"

#include <QtGui/QScreen>
#include <QQuickItem>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

#include <QSGTexture>

#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>


BgfxNode::BgfxNode(const uint16_t viewId, const QColor color, QQuickItem *item)
: m_item(item), m_viewId(viewId), backgroundColor(color)
{
    m_window = m_item->window();
    connect(m_window, &QQuickWindow::beforeRenderPassRecording, this, &BgfxNode::render);
    connect(m_window, &QQuickWindow::screenChanged, this, [this]() {
        if (m_window->effectiveDevicePixelRatio() != m_dpr)
            m_item->update();
    });

    qDebug("renderer created");
}

BgfxNode::~BgfxNode()
{
    qDebug("renderer destroyed");
}

QSGTexture *BgfxNode::texture() const
{
    return QSGSimpleTextureNode::texture();
}

void BgfxNode::sync()
{
    m_dpr = m_window->effectiveDevicePixelRatio();
    const auto newSize = rect().size() * m_dpr;
    if(newSize.isEmpty())
        return;
    const auto width = static_cast<uint16_t >(newSize.width());
    const auto height = static_cast<uint16_t >(newSize.height());
    bool needsNew = false;

    if (!texture())
        needsNew = true;

    if (newSize != m_size) {
        needsNew = true;
        m_size = newSize;
    }

    if (needsNew) {
        QSGRendererInterface *rif = m_window->rendererInterface();

        if (texture())
        {
            texture()->deleteLater();
            [static_cast<id<MTLTexture>>(m_texture) release];
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

        // create a native render target texture
        MTLTextureDescriptor *desc = [[MTLTextureDescriptor alloc] init];
        desc.textureType = MTLTextureType2D;
        desc.pixelFormat = MTLPixelFormatRGBA8Unorm;
        desc.width = static_cast<NSUInteger>(width);
        desc.height = static_cast<NSUInteger>(height);
        desc.mipmapLevelCount = 1;
        desc.resourceOptions = MTLResourceStorageModePrivate;
        desc.storageMode = MTLStorageModePrivate;
        desc.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
        const auto device = (id<MTLDevice>) rif->getResource(m_window, QSGRendererInterface::DeviceResource);
        assert(device);
        m_texture = [device newTextureWithDescriptor: desc];
        [desc release];
        
        // create a QSGTexture from the native texture
        const auto qsgtexture = QNativeInterface::QSGMetalTexture::fromNative(static_cast<id<MTLTexture>>(m_texture), m_window, m_window->size());
        
        // override the internal bgfx native texture handler which will be the color attachement of the framebuffer
        [[maybe_unused]] const auto newBgfxInternalId =
        bgfx::overrideInternal(m_backBuffer, uintptr_t(m_texture));
        assert(newBgfxInternalId != bgfx::kInvalidHandle);

        std::array<bgfx::Attachment, 2> m_attachments{};
        auto &[colorAttachment, depthAttachment] = m_attachments;
        colorAttachment.init(m_backBuffer, bgfx::Access::Write, 0);
        depthAttachment.init(m_depthBuffer, bgfx::Access::Write, 1);
        
        if(bgfx::isValid(m_offscreenFB))
        {
            bgfx::destroy(m_offscreenFB);
        }

        m_offscreenFB = bgfx::createFrameBuffer(m_attachments.size(), m_attachments.data(), false);
        
        bgfx::setViewFrameBuffer(m_viewId, m_offscreenFB);

        bgfx::setViewRect(m_viewId, 0, 0, width, height);
        
        // assign the QSGTexture to the node
        setTexture(qsgtexture);
    }
}

void BgfxNode::render()
{
    if (!bgfxRenderer::initialized())
        return;

    m_window->beginExternalCommands();

    float r{0.0f};
    float g{0.0f};
    float b{0.0f};
    backgroundColor.getRgbF(&r, &g, &b);

    const uint32_t color = uint8_t(r * 255) << 24 | uint8_t(g * 255) << 16 | uint8_t(b * 255) << 8 | 255;

    bgfx::setViewClear(m_viewId, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, color, 1.0f, 0);
    bgfx::touch(m_viewId);

    m_window->endExternalCommands();
}
