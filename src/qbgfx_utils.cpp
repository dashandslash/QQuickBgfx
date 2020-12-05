#include <qbgfx_utils.h>

#include <camera.h>
#include <components/base.h>
#include <qquickbgfxitem/qquickbgfxitem.h>

#include <QList>

#include <thread>
#include <iostream>

void QQuickBgfx::utils::connectItemsSignals(entt::registry &registry, const QList<QQuickBgfxItem *> &bgfxItems)
{
    for (const auto &item : bgfxItems)
    {
        const auto viewEntity = registry.create();
        registry.emplace<components::ViewId>(viewEntity, item->viewId());
        registry.emplace<components::ViewPort>(viewEntity);
        registry.emplace<components::Color>(viewEntity);
        registry.emplace<components::Mouse>(viewEntity);
        auto &cam = registry.emplace<CameraPersp>(viewEntity);
        cam.setPerspective(40.0f, 1.0f, 0.1f, 100.0f);
        cam.lookAt(glm::vec3{0.0f, 0.0f, 5.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});

        QObject::connect(item, &QQuickBgfxItem::geometryChanged, [&]() {
            registry.view<components::ViewId>().each([&item, &registry](const auto e, const auto viewId){
                if(viewId.value == item->viewId())
                {
                    registry.emplace_or_replace<components::Update<components::ViewPort>>(e, glm::ivec4{0, 0, item->dprWidth(), item->dprHeight()});
                }
            });
        });

        QObject::connect(item, &QQuickBgfxItem::backgroundColorChanged, [&]() {
            registry.view<components::ViewId>().each([&item, &registry](const auto e, const auto viewId){
                if(viewId.value == item->viewId())
                {
                    auto color = components::Color();
                    item->backgroundColor().getRgbF(&color.value[0], &color.value[1], &color.value[2]);
                    registry.emplace_or_replace<components::Update<components::Color>>(e, std::move(color));
                }
            });
        });

        QObject::connect(item, &QQuickBgfxItem::mouseChanged, [&]() {
              registry.view<components::ViewId>().each([&item, &registry](const auto e, const auto viewId){
                if(viewId.value == item->viewId())
                {
                    registry.emplace_or_replace<components::Update<components::Mouse>>(e, components::Mouse{{item->mousePosition().x(), item->mousePosition().y(), item->mouseScroll()}, {item->mouseButtons().testFlag(Qt::MouseButton::LeftButton), item->mouseButtons().testFlag(Qt::MouseButton::MiddleButton), item->mouseButtons().testFlag(Qt::MouseButton::RightButton)}});
                }
              });
        });
    }
}
