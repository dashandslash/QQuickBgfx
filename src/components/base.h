#pragma once
#include <components/camera.h>

#include <glm/glm.hpp>

#include <array>
#include <stdio.h>
#include <variant>

namespace components {
struct ViewId
{
    uint16_t value;
};

struct ViewPort
{
    glm::ivec4 value;
};

struct Mouse
{
    struct MouseButton
    {
        enum Enum
        {
            Left,
            Middle,
            Right,

            Count
        };
    };

    glm::vec3 pos{0.0f, 0.0f, 0.0f};
    std::array<bool, MouseButton::Count> buttons{false};
};

struct Color
{
    glm::vec4 value;
};

template <typename T>
struct Update
{
    T component;
};

using Updatables = std::variant<Color, Mouse, ViewPort, CameraPersp> ;

inline Updatables updatables;

}    // namespace components
