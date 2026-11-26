// ===========================================================================
// myRegistrar 单元测试
//
// 覆盖 RegisteredClass（注册表） / Registry（注册器）的以下行为：
//   1. 注册后可按优先级（小的在前）、同优先级按名字升序枚举
// ===========================================================================

#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <vector>

#include "myRegistrar.h"

namespace {

//! 被测类型：live 用于观察对象何时被释放
struct Item {
    static int live;  //!< 当前存活的对象数
    int value;

    explicit Item(int v) : value(v) { ++live; }

    ~Item() { --live; }
};

int Item::live = 0;

using ItemRegistry = my::Registrar<Item>;
using ItemEntry = std::pair<int, std::string>;  //!< (priority, name)

//! 按注册表当前顺序拍快照，用于断言顺序
std::vector<ItemEntry> snapshot()
{
    std::vector<ItemEntry> result;
    for (auto it = ItemRegistry::begin(); it != ItemRegistry::end(); ++it)
        result.emplace_back(it.current_position(), it.current_name());
    return result;
}

}  // namespace

TEST(ClassRegistryTest, RegistersAndIteratesInPriorityOrder)
{
    ASSERT_TRUE(snapshot().empty());  //!< 其他用例不应残留条目

    {
        ItemRegistry alpha(new Item(1), 10, "order_alpha");
        ItemRegistry beta(new Item(2), 10, "order_beta");
        ItemRegistry gamma(new Item(3), 20, "order_gamma");

        // 优先级小的在前；同优先级按名字升序
        const std::vector<ItemEntry> expected = {
            {10, "order_alpha"},
            {10, "order_beta"},
            {20, "order_gamma"},
        };
        EXPECT_EQ(snapshot(), expected);
        EXPECT_EQ(Item::live, 3);
    }

    // 记录随作用域结束全部注销，对象一并释放
    EXPECT_TRUE(snapshot().empty());
    EXPECT_EQ(Item::live, 0);
}

