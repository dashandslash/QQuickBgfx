#pragma once
#include <QQuickItem>

class BgfxNode;

using MousePosition = std::array<int, 2>;

class BgfxItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int viewId READ viewId WRITE setViewId NOTIFY viewIdChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    QML_ELEMENT

public:
    BgfxItem();
    ~BgfxItem();

    uint16_t viewId() const { return m_viewId; }
    void setViewId(uint16_t viewId);

    QColor backgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(QColor color);

    MousePosition mousePosition()
    {
        return m_mousePos;
    }

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

    std::unique_ptr<BgfxNode> m_node;
    uint16_t m_viewId{0};
    QColor m_backgroundColor;
    MousePosition m_mousePos{0, 0};
};
