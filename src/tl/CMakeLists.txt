# ---------------------------------------------------------------------------
# tl —— 主工程的通用库（启用 Qt）
# ---------------------------------------------------------------------------
file(GLOB SOURCES
    "core/*.cc"
    "math/*.cc"
    "object/*.cc"
    "exception/*.cc"
    "io/*.cc"
    "thread/*.cc"
    "registrar/*.cc"
    "text/*.cc"
)

add_library(tl SHARED
    ${SOURCES}
)

target_include_directories(tl PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/math
    ${CMAKE_CURRENT_SOURCE_DIR}/object
    ${CMAKE_CURRENT_SOURCE_DIR}/exception
    ${CMAKE_CURRENT_SOURCE_DIR}/io
    ${CMAKE_CURRENT_SOURCE_DIR}/thread
    ${CMAKE_CURRENT_SOURCE_DIR}/registrar
    ${CMAKE_CURRENT_SOURCE_DIR}/text
    ${CMAKE_CURRENT_SOURCE_DIR}/atomic
)

target_link_libraries(tl PUBLIC
    Qt5::Core
    Qt5::Widgets
    Qt5::Xml
    Qt5::Network
    ZLIB::ZLIB
    toolkit::logkit
)

add_subdirectory(test)
