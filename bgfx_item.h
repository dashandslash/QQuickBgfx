#pragma once
#include <QtQuick/QQuickItem>

class BgfxNode;

class BgfxItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(qreal viewId READ viewId WRITE setViewId NOTIFY viewIdChanged)
    QML_ELEMENT

public:
    BgfxItem();

    qreal t() const { return m_t; }
    void setT(qreal t);
    
    qreal viewId() const { return m_viewId; }
    void setViewId(uint16_t t);

signals:
    void tChanged();
    void viewIdChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private slots:
    void invalidateSceneGraph();

private:
    void releaseResources() override;

    BgfxNode *m_node = nullptr;
    qreal m_t = 0;
    uint16_t m_viewId{0};
};
