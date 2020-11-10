#include "qquick_bgfx.h"

#include <d3d11.h>

template<>
bgfx::Init QQuickBgfx::init<bgfx::RendererType::Direct3D11>(void *windowHandler, void* context, const uint16_t width, const uint16_t height)
{
    if(!initialized())
    {
        bgfx::Init init;
        init.type = bgfx::RendererType::Direct3D11;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.resolution.width = width;
        init.resolution.height = height;
        //init.platformData.nwh = windowHandler;
        init.platformData.context = reinterpret_cast<ID3D11Device*>(context);
        return init;
    }
    else
        return bgfx::Init();

}
