#pragma once
#include <components/base.h>
#include <qquickbgfxitem/qquickbgfxitem.h>
#include <systems/qquickbgfx.h>

#include <entt/entt.hpp>
#include <QList>


namespace engine
{
struct engine
{
    engine() = delete;
    engine(entt::registry &registry, const QList<::QQuickBgfxItem *> &bgfxItems);
    void preUpdate(entt::registry &registry);
    void update(entt::registry &registry);
    void postUpdate(entt::registry &registry);

private:
    systems::QQuickBgfx m_bgfxSystem;
};
}    // namespace engine
