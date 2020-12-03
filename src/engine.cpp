#include <engine.h>


namespace {
template<class... Vs>
void consolidate(entt::registry &r, const std::variant<Vs...>& ) {
  (..., (
      r.view<components::Update<Vs>>().each([&](const auto  e, const auto &updateC){
        r.emplace_or_replace<Vs>(e, updateC.component);
        })
  ));
}
}

void engine::preUpdate(entt::registry &registry)
{}

void engine::update(entt::registry &registry)
{
}

void engine::postUpdate(entt::registry &registry)
{
    consolidate(registry, components::updatables);
}
