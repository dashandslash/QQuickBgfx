#include <qbgfx_utils.h>

#include <components/base.h>
#include <qquickbgfxitem/qquickbgfxitem.h>

#include <QList>


void QQuickBgfx::utils::connectItemsSignals(entt::registry &registry, const QList<QQuickBgfxItem *> &bgfxItems)
{
    for (const auto &item : bgfxItems)
    {
        const auto viewEntity = registry.create();
        registry.emplace<components::ViewId>(viewEntity, item->viewId());
        registry.emplace<components::ViewPort>(viewEntity);
        registry.emplace<components::Color>(viewEntity);
        registry.emplace<components::Mouse>(viewEntity);

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
                    registry.emplace_or_replace<components::Update<components::Mouse>>(e, components::Mouse{{item->mousePosition().x(), item->mousePosition().y(), 0.0f}, {item->mouseButtons().testFlag(Qt::MouseButton::LeftButton), item->mouseButtons().testFlag(Qt::MouseButton::MiddleButton), item->mouseButtons().testFlag(Qt::MouseButton::RightButton)}});
                }
              });
        });
    }
}
