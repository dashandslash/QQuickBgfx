#include "renderer.h"

#include <components/base.h>
#include <qquick_bgfx.h>

#include <debugdraw/debugdraw.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/string_cast.hpp>


void renderer::init(const bgfx::Init &init)
{
    if (!QQuickBgfx::initialized())
    {
        bgfx::renderFrame();
        bgfx::init(init);
        bgfx::setDebug(BGFX_DEBUG_TEXT);
        ddInit();
    }
}

namespace registry::utils {
template<typename T>
const T& getUpdatedComponent(entt::registry &r, const entt::entity e, const T& c)
{
    if (r.has<components::Update<T>>(e))
    {
        return r.get<components::Update<T>>(e).component;
    }
    else
    {
        return c;
    }
}
}


void renderer::render(const entt::registry &registry)
{
    registry.view<const components::ViewId, const components::ViewPort, const components::Mouse, const components::Color, const CameraPersp>().each(
      [&registry](const auto e, const auto &viewId, const auto &viewport, const auto &mouse, const auto &color, const auto &cam) {

          const auto &w = viewport.value.z;
          const auto &h = viewport.value.w;
          if (w == 0 || h == 0)
            return;
        
          const uint32_t icolor = uint8_t(color.value.r * 255) << 24 | uint8_t(color.value.g * 255) << 16 | uint8_t(color.value.b * 255) << 8 | 255;

          bgfx::setViewClear(viewId.value, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, icolor, 1.0f, 0);
          bgfx::touch(viewId.value);

          static float time{0.0f};
          time += 0.003f;

          const auto &view = cam.getViewMatrix();
          const auto &proj = cam.getProjectionMatrix();

          bgfx::setViewTransform(viewId.value, glm::value_ptr(view), glm::value_ptr(proj));
          bgfx::setViewRect(viewId.value, 0, 0, uint16_t(w), uint16_t(h));
          float mtx[16];
          bx::mtxRotateXY(mtx, time, time);
          DebugDrawEncoder dde;
          dde.begin(viewId.value);
          dde.setState(true, true, false);
          dde.drawCapsule({-2.0f, -2.0f, 0.0f}, {-2.0f, 0.0f, 0.0f}, 1.0);
          dde.drawCone({3.0f, -2.0f, 0.0f}, {3.0f, 2.0f, 0.0f}, 1.0f);
          dde.drawAxis(0.0f, 0.0f, 0.0f);
          dde.setTransform(&mtx);
          dde.draw(Aabb{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}});
          dde.end();
      });
}
