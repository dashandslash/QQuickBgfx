#pragma once
#include <QObject>

#include <vector>

class BgfxItem;
class QQmlEngine;
class QQuickWindow;

namespace qquick_bgfx
{
class Renderer : public QObject
{
    Q_OBJECT
public:
Renderer() = delete;
Renderer(QQuickWindow *, const QList<BgfxItem*>);

~Renderer();

private slots:

void render();
void shutdown();
void init();

private:
std::vector<BgfxItem*> bgfxItems;
QQuickWindow* window{nullptr};

};
}
