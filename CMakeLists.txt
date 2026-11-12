cmake_minimum_required(VERSION 3.20)
project(xecu-pro VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt6_add_resources(RESOURCES assets/switches.qrc)

add_executable(xecu-pro
    main.cpp
    MainWindow.cpp
    ImageToggleSwitch.cpp
    FlipSwitch.cpp
    LedIndicator.cpp
    ${RESOURCES}
)

target_include_directories(xecu-pro PRIVATE src/)

target_link_libraries(xecu-pro PRIVATE
    Qt6::Core
    Qt6::Widgets
)
