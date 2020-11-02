#pragma once
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>


namespace QQuickBgfx {
template<bgfx::RendererType::Enum T>
void init(void* windowHandle, void* context, const uint16_t width, const uint16_t height);

inline bool initialized()
{
    return bgfx::getInternalData()->context;
}

inline void frame()
{
    if(initialized())
    {
        bgfx::frame();
    }
}

}
