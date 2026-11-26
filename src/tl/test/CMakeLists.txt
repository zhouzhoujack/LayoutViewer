# ---------------------------------------------------------------------------
# tl 单元测试（gtest）
# ---------------------------------------------------------------------------
find_package(GTest REQUIRED)

add_executable(ut_tl
    main.cpp              # 测试入口，替代 gtest_main
    classRegistryTest.cc
)

target_link_libraries(ut_tl PRIVATE
    tl
    GTest::gtest
)

add_test(NAME ut_tl COMMAND ut_tl)
