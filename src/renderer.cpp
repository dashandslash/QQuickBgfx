#include <renderer.h>

#include <components/base.h>
#include <qquick_bgfx.h>

#include <debugdraw/debugdraw.h>
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

void renderer::render(entt::registry &registry)
{
    registry.view<components::ViewId, components::ViewPort, components::Mouse, components::Color>().each(
      [](const auto &viewId, const auto &viewport, const auto &mouse, const auto &color) {
          const auto w = viewport.value[2];
          const auto h = viewport.value[3];
          auto hsv = glm::hsvColor(glm::vec3{color.value});
          hsv.z *= std::clamp(mouse.pos.y / (float)h, 0.0f, 1.0f);
          auto c = glm::rgbColor(hsv);
          float r{c.x};
          float g{c.y};
          float b{c.z};

          const uint32_t icolor = uint8_t(r * 255) << 24 | uint8_t(g * 255) << 16 | uint8_t(b * 255) << 8 | 255;

          bgfx::setViewClear(viewId.value, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, icolor, 1.0f, 0);
          bgfx::touch(viewId.value);

          static float time{0.0f};
          time += 0.003f;

          const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
          const bx::Vec3 eye = {std::clamp(mouse.pos.x / (float)w - 0.5f, -0.5f, 0.5f) * 15.0f, 0.0f,
                                std::clamp(mouse.pos.y / (float)h - 0.5f, -0.5f, 0.5f) * 15.0f};

          float view[16];
          bx::mtxLookAt(view, eye, at);

          float proj[16];
          bx::mtxProj(proj, 60.0f, float(w) / float(h), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
          bgfx::setViewTransform(viewId.value, view, proj);
          bgfx::setViewRect(viewId.value, 0, 0, uint16_t(w), uint16_t(h));

          float mtx[16];
          bx::mtxRotateXY(mtx, time, time);
          DebugDrawEncoder dde;
          dde.begin(viewId.value);
          dde.drawCapsule({-2.0f, -2.0f, 0.0f}, {-2.0f, 0.0f, 0.0f}, 1.0);
          dde.drawCone({3.0f, -2.0f, 0.0f}, {3.0f, 2.0f, 0.0f}, 1.0f);
          dde.drawAxis(0.0f, 0.0f, 0.0f);
          dde.setTransform(&mtx);
          dde.draw(Aabb{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}});
          dde.end();
      });
}
