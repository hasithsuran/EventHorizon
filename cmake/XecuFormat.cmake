# clang-format integration (style rules live in /.clang-format).
#
#   cmake --build . --target format         reformat all project sources in place
#   cmake --build . --target format-check   fail if any file is not formatted

find_program(CLANG_FORMAT_EXE NAMES clang-format clang-format-16)

if(NOT CLANG_FORMAT_EXE)
    message(STATUS "clang-format not found — format targets unavailable")
    return()
endif()

file(GLOB_RECURSE XECU_FORMAT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/apps/*.h
    ${CMAKE_SOURCE_DIR}/apps/*.hpp
    ${CMAKE_SOURCE_DIR}/apps/*.cpp
    ${CMAKE_SOURCE_DIR}/services/*.h
    ${CMAKE_SOURCE_DIR}/services/*.hpp
    ${CMAKE_SOURCE_DIR}/services/*.cpp
)

if(NOT XECU_FORMAT_SOURCES)
    return()
endif()

add_custom_target(format
    COMMAND ${CLANG_FORMAT_EXE} -i --style=file ${XECU_FORMAT_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting xECU-pro sources with clang-format"
    VERBATIM
)

add_custom_target(format-check
    COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror --style=file ${XECU_FORMAT_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking xECU-pro source formatting"
    VERBATIM
)
