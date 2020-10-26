#include "bgfx_item.h"

#include "bgfx_renderer.h"

#include <QtGui/QScreen>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtCore/QFile>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

#include <QSGTexture>
#include <QtGui/QScreen>
#include <QtQuick/QSGTextureProvider>
#include <QSGSimpleTextureNode>

#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

class BgfxNode : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    BgfxNode(const uint16_t viewId, const QColor color, BgfxItem *item);
    ~BgfxNode();

    QSGTexture *texture() const override;

    void sync();
    private slots:
    void render();

private:

    BgfxItem *m_item;
    QQuickWindow *m_window;
    QSizeF m_size;
    qreal m_dpr;
    id<MTLDevice> m_device = nil;
    id<MTLTexture> m_texture = nil;
    id<MTLRenderPipelineState> m_pipeline;

    bgfx::FrameBufferHandle m_offscreenFB{bgfx::kInvalidHandle};
    bgfx::TextureHandle m_backBuffer{bgfx::kInvalidHandle};
    bgfx::TextureHandle m_depthBuffer{bgfx::kInvalidHandle};
    
    uint16_t m_viewId{0};
    QColor m_backgroundColor{Qt::GlobalColor::green};
    uint64_t m_frameCount{0};
};


BgfxNode::BgfxNode(const uint16_t viewId, const QColor color, BgfxItem *item)
: m_item(item), m_viewId(viewId), m_backgroundColor(color)
{
    m_window = m_item->window();
    connect(m_window, &QQuickWindow::beforeRenderPassRecording, this, &BgfxNode::render);
    connect(m_window, &QQuickWindow::screenChanged, this, [this]() {
        if (m_window->effectiveDevicePixelRatio() != m_dpr)
            m_item->update();
    });
    
    connect(m_item, &BgfxItem::backgroundColorChanged, this, [this]() {
        m_backgroundColor = m_item->backgroundColor();
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
    
    if(!bgfxRenderer::initialized())
    {
        bgfxRenderer::init(m_window, m_window->width() * m_dpr, m_window->height() * m_dpr, bgfx::RendererType::Metal);
    }

    if (!texture())
        needsNew = true;

    if (newSize != m_size) {
        needsNew = true;
        m_size = newSize;
    }

    if (needsNew) {
        delete texture();
        [m_texture release];

        QSGRendererInterface *rif = m_window->rendererInterface();
        m_device = (id<MTLDevice>) rif->getResource(m_window, QSGRendererInterface::DeviceResource);
        assert(m_device);

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
        const auto m_texture = [m_device newTextureWithDescriptor: desc];
        [desc release];
        
        // create a QSGTexture from the native texture
        const auto qsgtexture = QNativeInterface::QSGMetalTexture::fromNative(m_texture, m_window, m_window->size());
        
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
    m_backgroundColor.getRgbF(&r, &g, &b);

    const uint32_t color = uint8_t(r * 255) << 24 | uint8_t(g * 255) << 16 | uint8_t(b * 255) << 8 | 255;

    bgfx::setViewClear(m_viewId, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, color, 1.0f, 0);
    bgfx::touch(m_viewId);

    m_window->endExternalCommands();
}

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
    }
    
    m_node->setRect(boundingRect());

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


#include "bgfx_item.moc"
