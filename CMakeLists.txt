cmake_minimum_required(VERSION 3.14)
project(BgfxQQuickItemExample LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INSTALL_EXAMPLEDIR "BgfxQQuickItemExample")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)

qt_add_executable(BgfxQQuickItemExample
    main.cpp
    bgfx_under_qml/bgfx_item/bgfx_item.h
    bgfx_under_qml/bgfx_item/bgfx_item.cpp
    bgfx_under_qml/bgfx_item/bgfx_node/Metal/bgfx_node.h
    bgfx_under_qml/bgfx_item/bgfx_node/Metal/bgfx_node.mm
    bgfx_under_qml/bgfx_renderer.h
    bgfx_under_qml/bgfx_renderer.mm
)

target_link_libraries(BgfxQQuickItemExample PUBLIC
    "-framework Metal"
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickPrivate
    bgfx
)

target_include_directories(BgfxQQuickItemExample
                            PUBLIC
                            ${CMAKE_SOURCE_DIR}/bgfx_under_qml
                            ${CMAKE_SOURCE_DIR}/bgfx_under_qml/bgfx_item
                            )

# Resources:
set(BgfxQQuickItemExample_resource_files
    "main.qml"
)

qt6_add_resources(BgfxQQuickItemExample "BgfxQQuickItemExample"
    PREFIX
        "/BgfxQQuickItemExample"
    FILES
        ${BgfxQQuickItemExample_resource_files}
)

if(MACOS)
    target_link_libraries(BgfxQQuickItemExample PUBLIC
        "-framework AppKit"
    )
endif()

install(TARGETS BgfxQQuickItemExample
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

set_target_properties(BgfxQQuickItemExample PROPERTIES
    QT_QML_MODULE_VERSION 1.0
    QT_QML_MODULE_URI BgfxQQuickItem
)

qt6_qml_type_registration(BgfxQQuickItemExample)
add_subdirectory(deps)

