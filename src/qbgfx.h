#pragma once
#include "qquickbgfxitem/qquickbgfxitem.h"

#include <bgfx/platform.h>

#include <QObject>

#include <vector>


class QQmlEngine;
class QQuickWindow;

namespace QQuickBgfx
{
class QBgfx : public QObject
{
    Q_OBJECT
public:
QBgfx() = delete;
QBgfx(QQuickWindow *, const QList<QQuickBgfxItem*>);

~QBgfx();

private slots:

void renderFrame();
void shutdown();
void init();

signals:
void render(const std::vector<QQuickBgfxItem*>&);
void initialized(bgfx::Init&);

private:
std::vector<QQuickBgfxItem*> bgfxItems;
QQuickWindow* window{nullptr};
bgfx::Init m_bgfxInit;
};
}
