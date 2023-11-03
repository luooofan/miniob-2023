/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "common/lang/lruk_cache.h"
#include <array>

using namespace common;
using ::testing::ElementsAre;

TEST(test_lruk_cache, test_lru2_cache)
{  
  using FrameLruKCache = common::LruKCache<2, int, int>;
  FrameLruKCache lru2_cache;

  lru2_cache.put(1, 1);
  lru2_cache.put(2, 2);
  lru2_cache.put(3, 3);
  lru2_cache.put(4, 4);
  lru2_cache.put(5, 5);
  lru2_cache.put(6, 6);
  ASSERT_EQ(6, lru2_cache.count());
  // lru2 order: 1 2 3 4 5 6

  std::vector<int> values;
  size_t count = 6;
  auto purge_finder = [&values, &count](const int& key, const int& val) {
    values.emplace_back(val);
    if (values.size() >= count) {
      return false;
    }
    return true;
  };
  values.clear();
  lru2_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(1, 2, 3, 4, 5, 6));

  int value = 0;
  lru2_cache.get(5, value);
  ASSERT_EQ(5, value);
  // lru2 order: 1 2 3 4 6 5
  values.clear();
  lru2_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(1, 2, 3, 4, 6, 5));

  lru2_cache.get(3, value);
  ASSERT_EQ(3, value);
  // lru2 order: 1 2 4 6 3 5
  values.clear();
  lru2_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(1, 2, 4, 6, 3, 5));

  lru2_cache.remove(4);
  ASSERT_EQ(5, lru2_cache.count());
  // lru2 order: 1 2 6 3 5
  values.clear();
  count = 5;
  lru2_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(1, 2, 6, 3, 5));

  lru2_cache.get(1, value);
  ASSERT_EQ(1, value);
  // lru2 order: 2 6 1 3 5
  values.clear();
  lru2_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(2, 6, 1, 3, 5));

  lru2_cache.get(1, value);
  ASSERT_EQ(1, value);
  // lru2 order: 2 6 3 5 1
  values.clear();
  lru2_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(2, 6, 3, 5, 1));

  lru2_cache.get(3, value);
  ASSERT_EQ(3, value);
  // lru2 order: 2 6 5 3 1
  values.clear();
  lru2_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(2, 6, 5, 3, 1));

  lru2_cache.get(3, value);
  ASSERT_EQ(3, value);
  // lru2 order: 2 6 5 1 3
  values.clear();
  lru2_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(2, 6, 5, 1, 3));

  lru2_cache.remove(3);
  count = 4;
  ASSERT_EQ(4, lru2_cache.count());
  values.clear();
  lru2_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(2, 6, 5, 1));

  lru2_cache.put(3, 7);
  count = 5;
  ASSERT_EQ(5, lru2_cache.count());
  values.clear();
  lru2_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(2, 6, 7, 5, 1));
}

TEST(test_lruk_cache, test_lru3_cache)
{
  // 这里主要测试对于访问次数不够 K 次的数据是否实现了 LRU
  using FrameLruKCache = common::LruKCache<3, int, int>;
  FrameLruKCache lru3_cache;

  lru3_cache.put(1, 1);
  lru3_cache.put(2, 2);
  lru3_cache.put(3, 3);
  ASSERT_EQ(3, lru3_cache.count());
  // lurk order: 1 2 3

  std::vector<int> values;
  size_t count = 3;
  auto purge_finder = [&values, &count](const int& key, const int& val) {
    values.emplace_back(val);
    if (values.size() >= count) {
      return false;
    }
    return true;
  };
  values.clear();
  lru3_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(1, 2, 3));

  int value = 0;
  lru3_cache.get(1, value);
  ASSERT_EQ(1, value);
  values.clear();
  lru3_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(2, 3, 1));

  lru3_cache.get(3, value);
  ASSERT_EQ(3, value);
  values.clear();
  lru3_cache.foreach_reverse(purge_finder);
  ASSERT_THAT(values, ElementsAre(2, 1, 3));
}

int main(int argc, char **argv)
{
  // 分析gtest程序的命令行参数
  testing::InitGoogleTest(&argc, argv);

  // 调用RUN_ALL_TESTS()运行所有测试用例
  // main函数返回RUN_ALL_TESTS()的运行结果
  return RUN_ALL_TESTS();
}