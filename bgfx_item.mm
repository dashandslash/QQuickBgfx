#include "bgfx_item.h"
#include <QtGui/QScreen>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtCore/QFile>

#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

#include <QSGTexture>
#include <QtGui/QScreen>
#include <QtQuick/QSGTextureProvider>
#include <QSGSimpleTextureNode>

class BgfxNode : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    BgfxNode(const uint16_t viewId, QQuickItem *item);
    ~BgfxNode();

    QSGTexture *texture() const override;

    void sync();
private slots:
    void render();

private:

    QQuickItem *m_item;
    QQuickWindow *m_window;
    QSizeF m_size;
    qreal m_dpr;
    id<MTLDevice> m_device = nil;
    id<MTLTexture> m_texture = nil;
    bool m_initialized{false};
    id<MTLRenderPipelineState> m_pipeline;

    float m_t;
    
    bgfx::FrameBufferHandle m_offscreenFB{bgfx::kInvalidHandle};
    bgfx::TextureHandle m_backBuffer{bgfx::kInvalidHandle};
    bgfx::TextureHandle m_depthBuffer{bgfx::kInvalidHandle};
    
    uint16_t m_viewId{0};
};


BgfxNode::BgfxNode(const uint16_t viewId, QQuickItem *item)
    : m_item(item), m_viewId(viewId)
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
    
    if(!m_initialized)
    {
        bgfx::Init init;
        init.type = bgfx::RendererType::Metal;
        init.platformData.context = m_device;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.resolution.width = width;
        init.resolution.height = height;
        init.platformData.nwh = reinterpret_cast<CAMetalLayer *>(reinterpret_cast<NSView *>(m_window->winId()).layer);
        bgfx::renderFrame();

        bgfx::init(init);
        bgfx::setDebug(BGFX_DEBUG_TEXT);
        bgfx::frame();
        m_initialized = true;
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
        Q_ASSERT(m_device);

        m_backBuffer = bgfx::createTexture2D(width, height, false, 2, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT, NULL);
        m_depthBuffer = bgfx::createTexture2D(width, height, false, 2, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT, NULL);
        
        // The call to frame() is necessary to actually construct the textures in GPU
        bgfx::frame();

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
        
        const auto qsgtexture = QNativeInterface::QSGMetalTexture::fromNative(m_texture, m_window, m_window->size());
        
        [[maybe_unused]] const auto newBgfxInternalId =
        bgfx::overrideInternal(m_backBuffer, uintptr_t(m_texture));
        assert(newBgfxInternalId != bgfx::kInvalidHandle);

        std::array<bgfx::Attachment, 2> m_attachments{};
        auto &[colorAttachment, depthAttachment] = m_attachments;
        colorAttachment.init(m_backBuffer, bgfx::Access::Write, 0);
        depthAttachment.init(m_depthBuffer, bgfx::Access::Write, 1);
        
        m_offscreenFB = bgfx::createFrameBuffer(m_attachments.size(), m_attachments.data(), false);
        
        bgfx::setViewFrameBuffer(m_viewId, m_offscreenFB);

        bgfx::setViewRect(m_viewId, 0, 0, width, height);
        bgfx::setViewClear(m_viewId, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, 0x0030ffff, 1.0f, 0);
        
        setTexture(qsgtexture);
    }

    m_t = float(static_cast<BgfxItem *>(m_item)->t());
}

void BgfxNode::render()
{
    if (!m_initialized)
        return;
    m_window->beginExternalCommands();
    
    static float r2{0.0f};
    static float g2{0.0f};
    static float b2{0.0f};
    g2 = fmod(g2 + 0.01f, 1.0f);
    uint32_t color  = uint8_t(r2 * 255) << 24 | uint8_t(g2 * 255) << 16 | uint8_t(b2 * 255) << 8 | 255;
    
    bgfx::setViewClear(m_viewId, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, color, 1.0f, 0);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
    bgfx::touch(m_viewId);

    bgfx::frame();
    
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
        m_node = new BgfxNode(m_viewId, this);
    }
    
    m_node->setRect(x(), y(), width(), height());

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

void BgfxItem::setT(qreal t)
{
    if (t == m_t)
        return;

    m_t = t;
    emit tChanged();

//    update();
}

void BgfxItem::setViewId(uint16_t viewId)
{
    if (viewId == m_viewId)
        return;

    m_viewId = viewId;
    emit viewIdChanged();

    update();
}


#include "bgfx_item.moc"
