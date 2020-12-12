//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.cpp
//
// Identification: src/buffer/clock_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/clock_replacer.h"
namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) {
  clock_hand = 0;
  frame_size = 0;
  frames = std::vector<size_t>(num_pages, 0);
}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) {
  frame_size_latch.lock();
  clock_hand_latch.lock();
  frame_size_latch.lock();

  if (frame_size == 0) {
    return false;
  }

  while (true) {
    if (clock_hand == frames.size()) {
      clock_hand = 0;
    }
    if (frames[clock_hand++] == EMPTY) {
      continue;
    }

    if (frames[clock_hand] == REF_TRUE) {
      frames[clock_hand++] = 0;
    }

    if (frames[clock_hand] == REF_FALSE) {
      *frame_id = (frame_id_t)clock_hand;
      break;
    }
  }
  frame_size_latch.unlock();
  clock_hand_latch.unlock();
  frame_size_latch.unlock();

  return true;
}
void ClockReplacer::Pin(frame_id_t frame_id) {
  frame_size_latch.lock();
  frame_size_latch.lock();

  if (frames[frame_id] != EMPTY) {
    frame_size--;
  }

  frames[frame_id] = EMPTY;

  frame_size_latch.unlock();
  frame_size_latch.unlock();
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
  frames_latch.lock();
  frame_size_latch.lock();
  if (frame_size >= frames.size()) {
    return;
  }

  if (frames[frame_id] == 2) {
    frame_size++;
  }
  frames[frame_id] = REF_TRUE;

  frame_size_latch.unlock();
  frames_latch.unlock();
}

size_t ClockReplacer::Size() { return frame_size; }

}  // namespace bustub
