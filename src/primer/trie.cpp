#include "primer/trie.h"
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  std::shared_ptr<const TrieNode> node_ptr = root_;
  for (auto c : key) {
    if (node_ptr && node_ptr->children_.find(c) != node_ptr->children_.end()) {
      node_ptr = node_ptr->children_.at(c);
    } else {
      return nullptr;
    }
  }

  auto value_ptr = std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(node_ptr);
  if (value_ptr && value_ptr->is_value_node_) {
    return value_ptr->value_.get();
  }

  return nullptr;

  // throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  int size = key.size();
  auto dfs = [&](auto self, int idx, std::shared_ptr<const TrieNode> ptr) -> std::shared_ptr<const TrieNode> {
    if (idx == size) {
      auto value_ptr = std::make_shared<T>(std::move(value));
      if (ptr) {
        return std::make_shared<TrieNodeWithValue<T>>(ptr->children_, value_ptr);
      } else {
        return std::make_shared<TrieNodeWithValue<T>>(value_ptr);
      }
    }

    auto c = key[idx];
    auto cur = ptr;
    if (ptr && ptr->children_.find(c) != ptr->children_.end()) {
      ptr = ptr->children_.at(c);
    } else {
      ptr = nullptr;
    }

    auto next = self(self, idx + 1, ptr);
    std::shared_ptr<TrieNode> ret;
    if (cur) {
      ret = cur->Clone();
    } else {
      ret = std::make_shared<TrieNode>();
    }
    ret->children_[c] = next;
    return ret;
  };

  auto root = dfs(dfs, 0, root_);
  return Trie(root);

  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  // throw NotImplementedException("Trie::Put is not implemented.");

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
}

auto Trie::Remove(std::string_view key) const -> Trie {
  int size = key.size();
  auto dfs = [&](auto self, int idx, std::shared_ptr<const TrieNode> ptr) -> std::shared_ptr<const TrieNode> {
    if (ptr == nullptr) {
      return nullptr;
    }

    if (idx == size) {
      if (ptr->is_value_node_ && !ptr->children_.empty()) {
        return std::make_shared<TrieNode>(ptr->children_);
      } else {
        return nullptr;
      }
    }

    std::shared_ptr<TrieNode> ret = ptr->Clone();

    char c = key[idx];
    if (ptr->children_.find(c) != ptr->children_.end()) {
      ptr = ptr->children_.at(c);
    } else {
      return nullptr;
    }

    auto next = self(self, idx + 1, ptr);
    if (!next) {
      ret->children_.erase(c);
      if (ret->children_.empty() && !ret->is_value_node_) {
        return nullptr;
      }
    } else {
      ret->children_[c] = next;
    }

    return ret;
  };

  auto root = dfs(dfs, 0, root_);
  return Trie(root);
  // throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
