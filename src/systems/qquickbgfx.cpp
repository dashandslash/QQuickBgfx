#include "qquickbgfx.h"

#include <components/base.h>
#include <components/camera.h>
#include <qquickbgfxitem/qquickbgfxitem.h>

#include <QList>

#include <iostream>
#include <thread>
#include <type_traits>

template<typename T>
void enqueue(rigtorp::MPMCQueue<systems::queueElement> &q, entt::entity e, T component)
{
    auto c = std::tuple<entt::entity, T>(e, component);
    q.try_push(c);
}

void systems::QQuickBgfx::update(entt::registry &registry)
{
    systems::queueElement element;
    while (queue.try_pop(element))
    {
        const auto [e, updatable] = element;
        std::visit(
          [&, e = e](const auto &component) {
              registry.emplace_or_replace<components::Update<std::decay_t<decltype(component)>>>(e, component);
          },
          updatable);
    }
}

systems::QQuickBgfx::QQuickBgfx(entt::registry &registry, const QList<::QQuickBgfxItem *> &bgfxItems): queue(1024)
{
    for (const auto &item : bgfxItems)
    {
        const auto viewEntity = registry.create();
        registry.emplace<components::ViewId>(viewEntity, item->viewId());
        registry.emplace<components::ViewPort>(viewEntity);
        registry.emplace<components::Color>(viewEntity);
        registry.emplace<components::Mouse>(viewEntity);
        auto &cam = registry.emplace<components::CameraPersp>(viewEntity);
        cam.setPerspective(40.0f, 1.0f, 0.1f, 100.0f);
        cam.lookAt({0.0f, 0.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

        QObject::connect(item, &QQuickBgfxItem::geometryChanged, [&]() {
            registry.view<components::ViewId>().each([&](const auto e, const auto viewId) {
                if (viewId.value == item->viewId())
                {
                    auto viewport = components::ViewPort{glm::ivec4{0, 0, item->dprWidth(), item->dprHeight()}};
                    enqueue(queue, e, std::move(viewport));
                }
            });
        });

        QObject::connect(item, &QQuickBgfxItem::backgroundColorChanged, [&]() {
            registry.view<components::ViewId>().each([&](const auto e, const auto viewId) {
                if (viewId.value == item->viewId())
                {
                    auto color = components::Color();
                    item->backgroundColor().getRgbF(&color.value[0], &color.value[1], &color.value[2]);
                    enqueue(queue, e, std::move(color));
                }
            });
        });

        QObject::connect(item, &QQuickBgfxItem::mouseChanged, [&]() {
            registry.view<components::ViewId>().each([&](const auto e, const auto viewId) {
                if (viewId.value == item->viewId())
                {
                    auto mouse =
                      components::Mouse{{item->mousePosition().x(), item->mousePosition().y(), item->mouseScroll()},
                                        {item->mouseButtons().testFlag(Qt::MouseButton::LeftButton),
                                         item->mouseButtons().testFlag(Qt::MouseButton::MiddleButton),
                                         item->mouseButtons().testFlag(Qt::MouseButton::RightButton)}};
                    enqueue(queue, e, std::move(mouse));
                }
            });
        });
    }
}
