project(SimFy VERSION 1.0.0)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
find_package(assimp REQUIRED)

set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    assimp::assimp
)

# The .glb model is too large to embed via rcc; ship it next to the binary.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Copying SimFy assets"
    VERBATIM
)

# Builds target SimFy-docs (see cmake/XecuDocs.cmake)
xecu_add_component_docs(${PROJECT_NAME})
