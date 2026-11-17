project(compositor VERSION 1.0.0)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick WaylandCompositor)

set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

qt_add_resources(${PROJECT_NAME} "compositor_qml"
    PREFIX "/"
    FILES qml/main.qml
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::WaylandCompositor
)

# The compositor launches the client apps from their build locations
target_compile_definitions(${PROJECT_NAME} PRIVATE
    XECU_MCM_EXECUTABLE="$<TARGET_FILE:master-control-module>"
    XECU_SIMFY_EXECUTABLE="$<TARGET_FILE:SimFy>"
)
add_dependencies(${PROJECT_NAME} master-control-module SimFy)

# Builds target compositor-docs (see cmake/XecuDocs.cmake)
xecu_add_component_docs(${PROJECT_NAME})
