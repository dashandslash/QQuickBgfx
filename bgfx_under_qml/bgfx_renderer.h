#pragma once
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

class QQuickWindow;

namespace bgfxRenderer
{
bool initialized();

void init(QQuickWindow *window, const uint16_t width, const uint16_t height, bgfx::RendererType::Enum type);

void frame();
}
