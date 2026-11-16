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
    FlipSwitch.cpp
    ImageToggleSwitch.cpp
    LedIndicator.cpp
    RegisterMap.cpp
    CanBus.cpp
    ${RESOURCES}
)

target_include_directories(xecu-pro PRIVATE src/)

target_link_libraries(xecu-pro PRIVATE
    Qt6::Core
    Qt6::Widgets
)

find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)

    configure_file(
        ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in
        ${CMAKE_BINARY_DIR}/Doxyfile
        @ONLY
    )

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating documentation"
        VERBATIM
    )
else()
    message(STATUS "Doxygen not found — docs target unavailable")
endif()
