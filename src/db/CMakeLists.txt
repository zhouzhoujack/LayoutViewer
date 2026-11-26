file(GLOB SOURCES
    "geo/*.cc"
    "core/*.cc"
    "collections/*.cc"
    "ops/*.cc"
    "hier/*.cc"
    "io/*.cc"
    "gsi/*.cc"
    "netlist/*.cc"
    "rdb/*.cc"
    "manager/*.cc"
)

add_library(db SHARED
    ${SOURCES}
)

target_include_directories(db PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/geo
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/collections
    ${CMAKE_CURRENT_SOURCE_DIR}/ops
    ${CMAKE_CURRENT_SOURCE_DIR}/hier
    ${CMAKE_CURRENT_SOURCE_DIR}/io
    ${CMAKE_CURRENT_SOURCE_DIR}/gsi
    ${CMAKE_CURRENT_SOURCE_DIR}/netlist
    ${CMAKE_CURRENT_SOURCE_DIR}/rdb
    ${CMAKE_CURRENT_SOURCE_DIR}/manager
)

target_link_libraries(db PUBLIC
    tl
)

add_subdirectory(gds)