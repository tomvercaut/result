
find_package(Doxygen
        REQUIRED dot
        OPTIONAL_COMPONENTS mscgen dia
        )

set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_MAN YES)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_EXAMPLE_PATH ${PROJECT_SOURCE_DIR}/examples/src)
set(DOXYGEN_CALL_GRAPH YES)
set(DOXYGEN_CALLER_GRAPH NO)

doxygen_add_docs(result-doc
    doxygen
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generate documentation"
)