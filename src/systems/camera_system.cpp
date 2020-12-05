#include "camera_system.h"
#define GLM_SWIZZLE
#include <components/camera.h>
#include <components/base.h>

#include <utility>

void updateAspectRatio(entt::registry &r)
{
    using namespace components;
    r.view<Update<ViewPort>, CameraPersp>().each([](auto e, const auto &uV, auto &c){
        c.setAspectRatio(uV.component.value.z/static_cast<float>(uV.component.value.w));
    });
}

void zoom(const components::Mouse &m1, const components::Update<components::Mouse> &u, const components::ViewPort &v, components::CameraPersp &c)
{
    const auto &m2 = u.component;
//    auto &c = uC.component;
    const auto mouseDelta = (m2.pos.z - m1.pos.z) * 10.0f;
    const auto initialPivotDistance = c.getPivotDistance();

    const auto newPivotDistance = powf(2.71828183f, 2 * -mouseDelta / length(glm::vec2(v.value.z,v.value.w))) * initialPivotDistance;
    const auto oldTarget = c.getEyePoint() + c.getViewDirection() * initialPivotDistance;
    if (const auto eyePoint = oldTarget - c.getViewDirection() * newPivotDistance; !glm::any(glm::isnan(eyePoint)))
    {
        c.setEyePoint(eyePoint);
        c.setPivotDistance( std::max<float>( newPivotDistance, 1.0f ) );
    }
}

void pan(const components::Mouse &m1, const components::Update<components::Mouse> &u, const components::ViewPort &v, components::CameraPersp &c)
{
    const auto &m2 = u.component;
//    auto &c = uC.component;
    const auto initialPivotDistance = c.getPivotDistance();
    
    const auto mouseDelta = (m2.pos.xy() - m1.pos.xy()) / glm::vec2{v.value.zw()} * initialPivotDistance * 3.0f;

    glm::vec3 right, up;
    c.getBillboardVectors( &right, &up );
    if ( const auto eyePoint = c.getEyePoint() - right * mouseDelta.x + up * mouseDelta.y; !glm::any(glm::isnan(eyePoint)))
    {
        c.setEyePoint(eyePoint);
    }
}

void orbit(const components::Mouse &m1, const components::Update<components::Mouse> &u, const components::ViewPort &v, components::CameraPersp &c)
{
    const auto &m2 = u.component;
//    auto &c = uC.component;
    constexpr auto factor = glm::vec2{-100.0f, 100.0f};
    const auto initialPivotDistance = c.getPivotDistance();
    auto mouseDelta = ( m2.pos.xy() - m1.pos.xy() ) / factor;
    const auto mW = normalize(c.getViewDirection());
    const bool invertMotion = (c.getOrientation() * c.getWorldUp()).y < 0.0f;
		
    const auto mU = normalize( cross( c.getWorldUp(), mW ) );

    if(invertMotion)
    {
        mouseDelta.x = -mouseDelta.x;
        mouseDelta.y = -mouseDelta.y;
    }

    glm::vec3 rotatedVec = glm::angleAxis(mouseDelta.y, mU) * (-c.getViewDirection() * initialPivotDistance);
    rotatedVec = glm::angleAxis(mouseDelta.x, c.getWorldUp()) * rotatedVec;
    if (!glm::any(glm::isnan(rotatedVec)))
    {
        c.setEyePoint(c.getEyePoint() + c.getViewDirection() * initialPivotDistance + rotatedVec);
        c.setOrientation(glm::angleAxis(mouseDelta.x, c.getWorldUp()) * glm::angleAxis(mouseDelta.y, mU) * c.getOrientation());
    }
}

void controlCamera(entt::registry &r)
{
    r.view<components::Mouse, components::Update<components::Mouse>, components::ViewPort, components::CameraPersp>().each([](auto e, const auto &m, const auto &uM, const auto &v, auto &c){

//    auto &uC = registry.get_or_emplace<components::Update<CameraPersp>>(e, c);
//    const auto &vv = registry.has<components::Update<components::ViewPort>>(e) ? registry.get<components::Update<components::ViewPort>>(e).component : v;
        const auto leftDrag = m.buttons[components::Mouse::MouseButton::Left] && uM.component.buttons[components::Mouse::MouseButton::Left];
        const auto rightDrag = m.buttons[components::Mouse::MouseButton::Right] && uM.component.buttons[components::Mouse::MouseButton::Right];
        const auto scroll = m.pos.z != uM.component.pos.z;
        
        if (leftDrag)
        {
            orbit(m, uM, v, c);
        }
        else if (rightDrag)
        {
            pan(m, uM, v, c);
        }
        else if (scroll)
        {
            zoom(m, uM, v, c);
        }
    });
}

void systems::cameraSystem::update(entt::registry &registry)
{
    updateAspectRatio(registry);
    controlCamera(registry);
}
