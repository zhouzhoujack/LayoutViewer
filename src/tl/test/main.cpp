// ===========================================================================
// tl 单元测试入口
//
// 取代 gtest_main：所有 *_test.cc 都编译进同一个可执行文件，
// 由这里统一初始化并运行全部用例。
// ===========================================================================

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}
