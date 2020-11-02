#pragma once
#include <bgfx/platform.h>

#include <QObject>

#include <vector>


class BgfxItem;
class QQmlEngine;
class QQuickWindow;

namespace QQuickBgfx
{
class QBgfx : public QObject
{
    Q_OBJECT
public:
QBgfx() = delete;
QBgfx(QQuickWindow *, const QList<BgfxItem*>);

~QBgfx();

private slots:

void renderFrame();
void shutdown();
void init();

signals:
void render(const std::vector<BgfxItem*>&);
void initialized(bgfx::Init&);

private:
std::vector<BgfxItem*> bgfxItems;
QQuickWindow* window{nullptr};
bgfx::Init m_bgfxInit;
};
}
