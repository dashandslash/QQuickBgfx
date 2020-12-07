#include "engine.h"

#include <systems/camera_system.h>

#include <iostream>
#include <functional>
#include <utility>


namespace {
template<typename T>
void updateAndClear(entt::registry &r)
{
    auto v = r.view<components::Update<T>>();
      v.each([&](const auto  e, const auto &updateC){
        r.emplace_or_replace<T>(e, updateC.component);
        });
    r.remove<components::Update<T>>(v.begin() , v.end());
}
template<typename... Ts>
void consolidate(entt::registry &r, const std::variant<Ts...>&)
{
  (..., ( updateAndClear<Ts>(r) ));
}
}

engine::engine::engine(entt::registry &registry, const QList<::QQuickBgfxItem *> &bgfxItems) : m_bgfxSystem(registry, bgfxItems)
{

}

void engine::engine::preUpdate(entt::registry &registry)
{}


void engine::engine::update(entt::registry &registry)
{
    engine::preUpdate(registry);
    m_bgfxSystem.update(registry);
    systems::cameraSystem::update(registry);
    postUpdate(registry);
}

void engine::engine::postUpdate(entt::registry &registry)
{
    consolidate(registry, components::updatables);
}
