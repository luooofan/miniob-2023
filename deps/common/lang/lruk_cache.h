/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include <cassert>
#include <functional>
#include <set>
#include <array>
#include <unordered_set>

namespace common {

template <size_t K, typename Key, typename Value, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>>
class LruKCache {

  class ListNode {
  public:
    Key key_;
    Value value_;
    size_t times_{0}; // 访问次数
    std::array<size_t, K> history_; // 访问时间

  public:
    ListNode(const Key &key, const Value &value) : key_(key), value_(value)
    {}
    void access(size_t cur_time)
    {
      history_[times_++ % K] = cur_time;
    }
    // 符合 LRU-K 序
    bool operator< (const ListNode& other) const
    {
      if (this->times_ < K && other.times_ < K) {
        // 都没有访问够 K 次
        // LRU
        return this->get_last_access() < other.get_last_access();
        // FIFO
        // return this->get_first_access() < other.get_first_access();
      } else if (this->times_ < K) {
        return true;
      } else if (other.times_ < K) {
        return false;
      } else {
        // 都访问够 K 次
        return this->get_kth_prev_access() < other.get_kth_prev_access();
      }
    }
  private:
    size_t get_last_access() const
    {
      assert(times_ > 0 && times_ < K); // 只能对访问次数不足 K 次的节点调用此函数
      return history_[times_ - 1];
    }
    size_t get_first_access() const
    {
      assert(times_ > 0 && times_ < K); // 只能对访问次数不足 K 次的节点调用此函数
      return history_[0];
    }
    size_t get_kth_prev_access() const
    {
      assert(times_ >= K); // 只能对访问次数为 K 次的节点调用此函数
      return history_[times_ % K];
    }
  };

  class PListNodeHasher {
  public:
    size_t operator()(ListNode *node) const
    {
      if (node == nullptr) {
        return 0;
      }
      return hasher_(node->key_);
    }

  private:
    Hash hasher_;
  };

  class PListNodePredicator {
  public:
    bool operator()(ListNode *const node1, ListNode *const node2) const
    {
      if (node1 == node2) {
        return true;
      }

      if (node1 == nullptr || node2 == nullptr) {
        return false;
      }

      return pred_(node1->key_, node2->key_);
    }

  private:
    Pred pred_;
  };

  class PListNodeComparator {
  public:
    bool operator()(ListNode *const node1, ListNode *const node2) const
    {
      return *node1 < *node2;
    }
  };

public:
  LruKCache(size_t reserve = 0)
  {
    if (reserve > 0) {
      searcher_.reserve(reserve);
    }
  }

  ~LruKCache()
  {
    destroy();
  }

  void destroy()
  {
    for (ListNode *node : searcher_) {
      delete node;
    }
    searcher_.clear();
    cache_.clear();
  }

  size_t count() const
  {
    return searcher_.size();
  }

  bool get(const Key &key, Value &value)
  {
    auto iter = searcher_.find((ListNode *)&key);
    if (iter == searcher_.end()) {
      return false;
    }

    lruk_touch(*iter);
    value = (*iter)->value_;
    return true;
  }

  void put(const Key &key, const Value &value)
  {
    auto iter = searcher_.find((ListNode *)&key);
    if (iter != searcher_.end()) {
      ListNode *ln = *iter;
      ln->value_ = value;
      lruk_touch(ln);
      return;
    }

    ListNode *ln = new ListNode(key, value);
    lruk_push(ln);
  }

  void remove(const Key &key)
  {
    auto iter = searcher_.find((ListNode *)&key);
    if (iter != searcher_.end()) {
      lruk_remove(*iter);
    }
  }

  // 只要遍历到每一个 <k,v> 即可，没有任何顺序要求
  void foreach(std::function<bool(const Key &, const Value &)> func)
  {
    foreach_reverse(func);
  }

  // 以 LRU-K 序遍历每一个 <k, v>
  // NOTE: 这里的函数名字并不合理，只是为了与 LruCache 保持一致
  void foreach_reverse(std::function<bool(const Key &, const Value &)> func)
  {
    for (auto iter = cache_.begin(); iter != cache_.end(); ++iter) {
      bool ret = func((*iter)->key_, (*iter)->value_);
      if (!ret) {
        break;
      }
    }
  }

private:
  void lruk_touch(ListNode* node)
  {
    cache_.erase(node);
    node->access(cur_time_++);
    cache_.insert(node);
  }
  void lruk_push(ListNode* node)
  {
    searcher_.insert(node);
    node->access(cur_time_++);
    cache_.insert(node);
  }
  void lruk_remove(ListNode *node)
  {
    cache_.erase(node);
    searcher_.erase(node);
    delete node;
  }

private:
  using CacheType = std::set<ListNode *, PListNodeComparator>;
  using SearchType = std::unordered_set<ListNode *, PListNodeHasher, PListNodePredicator>;

private:
  size_t cur_time_{0};
  CacheType cache_; // real cache. maintain LRU-K order
  SearchType searcher_; // impl O(1) find
};

}  // namespace common
