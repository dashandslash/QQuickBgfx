#pragma once
#include <QtQuick/QSGTextureProvider>
#include <QSGSimpleTextureNode>

#include <bgfx/bgfx.h>


class BgfxNode : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    BgfxNode(const uint16_t viewId, QQuickItem *item);
    ~BgfxNode();

    QSGTexture *texture() const override;
    void sync();

private:

    QQuickItem *m_item;
    QQuickWindow *m_window;
    QSizeF m_size;
    qreal m_dpr;

    bgfx::FrameBufferHandle m_offscreenFB{bgfx::kInvalidHandle};
    bgfx::TextureHandle m_backBuffer{bgfx::kInvalidHandle};
    bgfx::TextureHandle m_depthBuffer{bgfx::kInvalidHandle};
    
    uint16_t m_viewId{0};
    uint64_t m_frameCount{0};
    void* m_texture{nullptr};
};
