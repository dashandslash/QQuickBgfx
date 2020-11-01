#pragma once
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

class BgfxItem;
class QQuickWindow;

namespace bgfxRenderer
{
bool initialized();

void init(QQuickWindow *window, const uint16_t width, const uint16_t height, bgfx::RendererType::Enum type);
void addBgfxItem(BgfxItem* item);

void render();

void frame();

void shutdown();
}
