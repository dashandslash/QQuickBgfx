# bgfx
set( BGFX_BUILD_TOOLS     ON  CACHE INTERNAL "" )
set( BGFX_BUILD_EXAMPLES  OFF CACHE INTERNAL "" )
set( BGFX_CUSTOM_TARGETS  OFF CACHE INTERNAL "" )
# add_compile_definitions(BGFX_CONFIG_MULTITHREADED=0)
add_subdirectory( bgfx.cmake )