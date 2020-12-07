#pragma once
#include <bgfx/bgfx.h>
#include <entt/entt.hpp>

namespace renderer {
//init_example initialize bgfx from a gien bgfx::Init
void init(const bgfx::Init &init);

//render_example runs the rendering code
void render(const entt::registry &);
}    // namespace renderer
