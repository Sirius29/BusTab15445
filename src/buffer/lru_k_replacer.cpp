//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include "common/exception.h"

namespace bustub {

// TODO : thread safe
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k)
    : replacer_size_(num_frames), k_(k), lru_head_(new LRUKNode(-1)), lru_k_head_(new LRUKNode(-1)) {
  lru_head_->prev = lru_head_;
  lru_head_->next = lru_head_;
  lru_k_head_->prev = lru_k_head_;
  lru_k_head_->next = lru_k_head_;
}

LRUKReplacer::~LRUKReplacer() {
  LRUKNode *head = lru_head_->next;
  while (head != lru_head_) {
    LRUKNode *tmp = head->next;
    delete head;
    head = tmp;
  }
  delete lru_head_;
  lru_head_ = nullptr;

  head = lru_k_head_->next;
  while (head != lru_k_head_) {
    LRUKNode *tmp = head->next;
    delete head;
    head = tmp;
  }
  delete lru_k_head_;
  lru_k_head_ = nullptr;
}

auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool {
  LRUKNode *tail = lru_head_->prev;
  while (tail != lru_head_) {
    if (tail->IsEvictable()) {
      frame_id_t fid = tail->GetFrameId();
      node_store_.erase(fid);
      *frame_id = fid;
      RemoveNode(tail);
      delete tail;
      --evictable_size_;
      return true;
    }
    tail = tail->prev;
  }
  tail = lru_k_head_->prev;
  while (tail != lru_k_head_) {
    if (tail->IsEvictable()) {
      frame_id_t fid = tail->GetFrameId();
      node_store_.erase(fid);
      *frame_id = fid;
      RemoveNode(tail);
      delete tail;
      --evictable_size_;
      return true;
    }
    tail = tail->prev;
  }
  return false;
}

void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
  LRUKNode *node = nullptr;
  if (node_store_.find(frame_id) != node_store_.end()) {
    node = node_store_.at(frame_id);
    RemoveNode(node);
  } else {
    node = new LRUKNode(frame_id);
    node_store_[frame_id] = node;
  }
  if (++node->k_ >= k_) {
    PutFront(lru_k_head_, node);
  } else {
    PutFront(lru_head_, node);
  }
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  if (node_store_.find(frame_id) == node_store_.end()) {
    throw ExecutionException("LRUKReplacer::Frame id is invalid.");
  }
  LRUKNode *node = node_store_.at(frame_id);
  auto is_evictable = node->IsEvictable();
  if (is_evictable != set_evictable) {
    if (is_evictable) {
      --evictable_size_;
    } else {
      ++evictable_size_;
    }
    node->SetEvictable(set_evictable);
  }
}

void LRUKReplacer::Remove(frame_id_t frame_id) {
  if (node_store_.find(frame_id) == node_store_.end()) {
    return;
  }
  LRUKNode *node = node_store_.at(frame_id);
  if (node->IsEvictable()) {
    node_store_.erase(frame_id);
    RemoveNode(node);
    delete node;
  } else {
    throw ExecutionException("LRUKReplacer::Frame is not evictable.");
  }
}

auto LRUKReplacer::Size() -> size_t { return evictable_size_; }

void LRUKReplacer::RemoveNode(LRUKNode *node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;
}

void LRUKReplacer::PutFront(LRUKNode *head, LRUKNode *node) {
  node->prev = head;
  node->next = head->next;
  node->prev->next = node;
  node->next->prev = node;
}

}  // namespace bustub
