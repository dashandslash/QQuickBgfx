#pragma once
#include <components/base.h>
#include <qquickbgfxitem/qquickbgfxitem.h>

#include <entt/entt.hpp>
#include <QList>

namespace engine
{
void preUpdate(entt::registry &registry);
void update(entt::registry &registry);
void postUpdate(entt::registry &registry);
}    // namespace engine
