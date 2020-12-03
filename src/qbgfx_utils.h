#pragma once

#include <entt/entt.hpp>
#include <QList>
class QQuickBgfxItem;

namespace QQuickBgfx::utils
{
void connectItemsSignals(entt::registry &registry, const QList<QQuickBgfxItem *> &bgfxItems);
}    // namespace QQuickBgfx::utils
