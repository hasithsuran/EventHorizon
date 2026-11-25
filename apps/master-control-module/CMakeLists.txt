project(master-control-module VERSION 1.0.0)

# Qt 6.4 is the oldest supported version (Debian 12 dev container)
find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Qml Quick)

set(CMAKE_AUTOMOC ON)

# Put both QML modules under one tree in the build dir, laid out by URI
# (EvH/MasterControl, EvH/MasterControl/UI) so qmllint/qmlls can resolve them.
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/qml)

# EvH.MasterControl.UI: reusable controls (FlipSwitch, ImageToggleSwitch, LedIndicator)
add_subdirectory(ui)

qt_add_executable(${PROJECT_NAME}
    src/main.cpp
)

# EvH.MasterControl: C++ backend + the control panel window
set_source_files_properties(qml/Main.qml PROPERTIES QT_RESOURCE_ALIAS Main.qml)

qt_add_qml_module(${PROJECT_NAME}
    URI EvH.MasterControl
    VERSION 1.0
    # Explicit so the resource path is identical on Qt 6.4 and 6.5+ (QTP0001)
    RESOURCE_PREFIX /qt/qml
    QML_FILES
        qml/Main.qml
    RESOURCES
        assets/switch_off.png
        assets/switch_on.png
    SOURCES
        include/CanBus.h            src/CanBus.cpp
        include/ControlPanel.h      src/ControlPanel.cpp
        include/Protocol.h
        include/RegisterMap.h       src/RegisterMap.cpp
    # Tooling only (qmllint, qmlcachegen); runtime resolution is via import paths
    DEPENDENCIES
        QtQuick
        EvH.MasterControl.UI
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Build date in UTC; honours SOURCE_DATE_EPOCH for reproducible builds
string(TIMESTAMP XECU_MCM_BUILD_DATE "%Y-%m-%d" UTC)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    XECU_MCM_VERSION="${PROJECT_VERSION}"
    XECU_MCM_BUILD_DATE="${XECU_MCM_BUILD_DATE}"
    QT_NO_CAST_FROM_ASCII
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    master-control-module-uiplugin
)

# Builds target master-control-module-docs (see cmake/XecuDocs.cmake)
xecu_add_component_docs(${PROJECT_NAME})
