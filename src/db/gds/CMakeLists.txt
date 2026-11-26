file(GLOB SOURCES
    "*.cc"
)

add_library(gds SHARED
    ${SOURCES}
)

set_target_properties(gds
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../"
)

target_include_directories(gds
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(gds
    PUBLIC
    tl
    db
)
