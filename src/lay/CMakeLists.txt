add_definitions(-DHAVE_QT)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    "${CMAKE_CURRENT_SOURCE_DIR}/ui"
)

# netlist / RDB 界面仍在本地 src/db（db::）上，而视图数据已换成 layout-core 的 geo::，
# 两者类型不通，界面层还需 port。默认先不编译这一块，源码保留。
option(LAY_WITH_NETLIST "Build the netlist/RDB browser UI (not yet ported to layout-core)" OFF)

file(GLOB_RECURSE SOURCES
    "*.cc"
    "view/*.cc"
    "tree/*.cc"
    "mainwindow/*.cc"
    "plugins/*.cc"
    "ruler/*.cc"
    "common/*.cc"
    "edit/*.cc"
    "panels/*.cc"
    "dialogs/*.cc"
    "macro/*.cc"
    "netlist/*.cc"
    "salt/*.cc"
    "tech/*.cc"
    "misc/*.cc"
)

if(NOT LAY_WITH_NETLIST)
    list(FILTER SOURCES EXCLUDE REGEX "/netlist/")
endif()

file(GLOB_RECURSE UI_HEADERS
    "ui/*.ui"
)

add_library(lay SHARED
    ${UI_HEADERS}
    ${SOURCES}
    layResources.qrc
)


# 让 tl 头文件在 lay 里复用 layout-core 提供的全局 tr()
target_compile_definitions(lay PUBLIC LAY_USES_LAYOUT_CORE)

if(LAY_WITH_NETLIST)
    target_compile_definitions(lay PRIVATE LAY_WITH_NETLIST)
endif()

target_include_directories(lay PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/view
    ${CMAKE_CURRENT_SOURCE_DIR}/tree
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins
    ${CMAKE_CURRENT_SOURCE_DIR}/ruler
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/edit
    ${CMAKE_CURRENT_SOURCE_DIR}/panels
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/macro
    ${CMAKE_CURRENT_SOURCE_DIR}/netlist
    ${CMAKE_CURRENT_SOURCE_DIR}/salt
    ${CMAKE_CURRENT_SOURCE_DIR}/tech
    ${CMAKE_CURRENT_SOURCE_DIR}/misc
)

target_link_libraries(lay
    PUBLIC
    # db 放前面：仍需它提供 netlist/rdb/gsi（layout-core 里没有这些），
    # 且它的 include 目录里有真正的 gsi.h，必须优先于 layout-core 的同名 stub。
    db
    layout-core::data
    layout-core::db-io
    layout-core::io
    layout-core::utils
    tl
    Python3::Python
    Boost::filesystem
    Boost::system
    Qt5::Core
    Qt5::Widgets
    Qt5::Xml
)
