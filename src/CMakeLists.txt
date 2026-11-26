cmake_minimum_required(VERSION 3.16)

project(LayoutViewer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include("/home/hz/toolchain/cmake/toolchain.cmake")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
    Xml
    Network
)

find_package(Python3 REQUIRED COMPONENTS Development)
find_package(Boost REQUIRED COMPONENTS filesystem system)
find_package(ZLIB REQUIRED)
find_package(toolkit REQUIRED)

# 布局数据核心库（toolchain 预编译包），替代原先 src/db 里的 geo/io 等实现。
# 注意：layout-core 目前不含 netlist / rdb / gsi 实现，src/db 仍保留给它们使用。
find_package(layout-core REQUIRED)

add_definitions(-DHAVE_QT)

# ctest 支持（供 tl/test 下的单元测试使用）
enable_testing()

add_subdirectory(tl)
add_subdirectory(db)
add_subdirectory(lay)

add_executable(LayoutViewer
  main.cpp
)

target_include_directories(LayoutViewer PRIVATE
    ${CMAKE_SOURCE_DIR}
)

message(${CMAKE_SOURCE_DIR})

target_link_libraries(LayoutViewer PRIVATE
  lay
)
