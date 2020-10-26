#pragma once
#include <QtQuick/QQuickItem>

class BgfxNode;

class BgfxItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int viewId READ viewId WRITE setViewId NOTIFY viewIdChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    QML_ELEMENT

public:
    BgfxItem();

    uint16_t viewId() const { return m_viewId; }
    void setViewId(uint16_t viewId);

    QColor backgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(QColor color);

signals:
    void viewIdChanged();
    void backgroundColorChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    private slots:
    void invalidateSceneGraph();

private:
    void releaseResources() override;

    BgfxNode *m_node = nullptr;
    uint16_t m_viewId{0};
    QColor m_backgroundColor;
};
