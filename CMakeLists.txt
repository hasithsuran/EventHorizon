cmake_minimum_required(VERSION 3.20)
project(xECU-pro VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(apps)
add_subdirectory(services)

find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)

    configure_file(
        ${CMAKE_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_BINARY_DIR}/Doxyfile.in
        @ONLY
    )

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating xECU-pro documentation"
        VERBATIM
    )

    add_custom_target(docs-serve
        COMMAND python3 -m http.server 8080
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/docs/html
        COMMENT "Serving docs at http://localhost:8080"
    )
else()
    message(STATUS "Doxygen not found — docs target unavailable")
endif()
