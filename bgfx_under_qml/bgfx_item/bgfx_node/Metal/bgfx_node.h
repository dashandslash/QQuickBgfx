#pragma once
#include <QtQuick/QSGTextureProvider>
#include <QSGSimpleTextureNode>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

class BgfxNode : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    BgfxNode(const uint16_t viewId, const QColor color, QQuickItem *item);
    ~BgfxNode();

    QSGTexture *texture() const override;
    void sync();

    QColor backgroundColor{Qt::GlobalColor::green};

    private slots:
    void render();

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
