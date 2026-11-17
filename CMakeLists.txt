cmake_minimum_required(VERSION 3.20)
project(xECU-pro VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/XecuFormat.cmake)

# Doxygen must be found before the subdirectories so components can
# register their own docs targets via xecu_add_component_docs().
find_package(Doxygen OPTIONAL_COMPONENTS dot)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)
include(${CMAKE_SOURCE_DIR}/cmake/XecuDocs.cmake)

add_subdirectory(apps)
add_subdirectory(services)

if(DOXYGEN_FOUND)
    # Collect component tag files so the root pages link into component docs
    get_property(XECU_DOC_TARGETS  GLOBAL PROPERTY XECU_DOC_TARGETS)
    get_property(XECU_DOC_TAGFILES GLOBAL PROPERTY XECU_DOC_TAGFILES)
    list(JOIN XECU_DOC_TAGFILES " " XECU_DOC_TAGFILES)

    configure_file(
        ${CMAKE_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_BINARY_DIR}/Doxyfile
        @ONLY
    )

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating xECU-pro documentation"
        VERBATIM
    )
    if(XECU_DOC_TARGETS)
        add_dependencies(docs ${XECU_DOC_TARGETS})
    endif()

    # Serve the whole docs tree so relative links between components work
    add_custom_target(docs-serve
        COMMAND python3 -m http.server 8080
        WORKING_DIRECTORY ${DOXYGEN_OUTPUT_DIR}
        COMMENT "Serving docs at http://localhost:8080/html/"
    )
else()
    message(STATUS "Doxygen not found — docs target unavailable")
endif()
