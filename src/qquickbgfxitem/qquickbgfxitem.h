#pragma once
#include <QQuickItem>

class QSGBgfxNode;

using MousePosition = std::array<int, 2>;

class QQuickBgfxItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int viewId READ viewId WRITE setViewId NOTIFY viewIdChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    QML_NAMED_ELEMENT(BgfxItem)

public:
    QQuickBgfxItem();
    ~QQuickBgfxItem();

    uint16_t viewId() const { return m_viewId; }
    void setViewId(uint16_t viewId);

    QColor backgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(QColor color);

    MousePosition mousePosition() { return m_mousePos; }

    uint16_t dprWidth() const { return m_dprWidth; }

    uint16_t dprHeight() const { return m_dprHeight; }

signals:
    void viewIdChanged();
    void backgroundColorChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void invalidateSceneGraph();

private:
    void releaseResources() override;

    std::unique_ptr<QSGBgfxNode> m_node;
    uint16_t m_viewId{0};
    QColor m_backgroundColor;
    MousePosition m_mousePos{0, 0};
    uint16_t m_dprWidth{0u};
    uint16_t m_dprHeight{0u};
};
