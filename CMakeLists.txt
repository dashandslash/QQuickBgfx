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

set(SOURCES

    main.cpp
    src/qquickbgfxitem/qquickbgfxitem.h
    src/qquickbgfxitem/qquickbgfxitem.cpp
    src/qbgfx.h
    src/qbgfx.cpp
    src/qquick_bgfx.h
    src/qsgbgfxnode/qsgbgfxnode.h
    src/qsgbgfxnode/qsgbgfxnode.cpp
)
if(APPLE)
    set(SOURCES
        ${SOURCES}
        src/qquick_bgfx.mm
    )
endif()

if(WIN32)
    set(SOURCES
        ${SOURCES}
        src/qquick_bgfx.cpp
    )
endif()

if(UNIX)
    set(SOURCES
        ${SOURCES}
        src/qquick_bgfx_opengl.cpp
    )
endif()

qt_add_executable(BgfxQQuickItemExample
    ${SOURCES}
)


target_link_libraries(BgfxQQuickItemExample PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickPrivate
    bgfx
    bimg
    example-common
)

target_include_directories(BgfxQQuickItemExample
                            PUBLIC
                            ${CMAKE_SOURCE_DIR}/src
                            ${CMAKE_SOURCE_DIR}/src/qquickbgfxitem
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
        "-framework Metal"
    )
endif()

install(TARGETS BgfxQQuickItemExample
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

set_target_properties(BgfxQQuickItemExample PROPERTIES
    QT_QML_MODULE_VERSION 1.0
    QT_QML_MODULE_URI QQuickBgfx
)

qt6_qml_type_registration(BgfxQQuickItemExample)
add_subdirectory(deps)

