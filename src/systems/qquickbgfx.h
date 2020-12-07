#pragma once

#include <components/base.h>

#include <entt/entt.hpp>
#include <rigtorp/MPMCQueue.h>

#include <QList>

class QQuickBgfxItem;

namespace systems
{

using queueElement = std::tuple<entt::entity, components::Updatables>;

struct QQuickBgfx
{
QQuickBgfx() = delete;

QQuickBgfx(entt::registry &registry, const QList<::QQuickBgfxItem *> &bgfxItems);

void update(entt::registry &registry);

rigtorp::MPMCQueue<queueElement> queue;
};



}   // namespace systems
