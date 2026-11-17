# Per-component Doxygen documentation helpers.
#
# Output layout (relative to ${DOXYGEN_OUTPUT_DIR}):
#
#   html/                     <- root landing page (docs/architecture.md)
#   <component>/html/         <- component API docs
#   <component>/<component>.tag
#
# The root docs consume every component tag file, so class/file names written
# in the root pages auto-link into the component docs. Page-level navigation
# between the sites uses relative HTML links based on the layout above.

# xecu_add_component_docs(<component>)
#
# Expects <current source dir>/docs/Doxyfile.in. Creates the target
# <component>-docs and registers its tag file with the root `docs` target.
function(xecu_add_component_docs COMPONENT)
    if(NOT DOXYGEN_FOUND)
        return()
    endif()

    # Variables consumed by the component's Doxyfile.in
    set(COMPONENT_NAME          ${COMPONENT})
    set(COMPONENT_DOCS_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/docs)
    set(COMPONENT_OUTPUT_DIR    ${DOXYGEN_OUTPUT_DIR}/${COMPONENT})
    set(COMPONENT_TAGFILE       ${COMPONENT_OUTPUT_DIR}/${COMPONENT}.tag)

    configure_file(
        ${COMPONENT_DOCS_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )

    add_custom_target(${COMPONENT}-docs
        COMMAND ${CMAKE_COMMAND} -E make_directory ${COMPONENT_OUTPUT_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating ${COMPONENT} documentation"
        VERBATIM
    )

    # Tag file entry format: <tagfile>=<html location relative to root html/>
    set_property(GLOBAL APPEND PROPERTY XECU_DOC_TARGETS ${COMPONENT}-docs)
    set_property(GLOBAL APPEND PROPERTY XECU_DOC_TAGFILES
        "${COMPONENT_TAGFILE}=../${COMPONENT}/html")
endfunction()
