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
#include <iostream>
namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) {
  clock_hand = 0;
  frame_size = 0;
  frames = std::vector<size_t>(num_pages, EMPTY);
}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) {
  frames_latch.lock();
  clock_hand_latch.lock();
  frame_size_latch.lock();

  if (frame_size == 0) {
    return false;
  }
  // std::cout << "size of frames is :" << frames.size() << '\n';

  int i = 0, len = 2 * frames.size();
  bool foundVictim = false;
  while (i++ <= len && !foundVictim) {
    if (clock_hand == frames.size()) {
      clock_hand = 0;
    }

    switch (frames[clock_hand]) {
      case EMPTY:
        break;
      case REF_TRUE:
        frames[clock_hand] = REF_FALSE;
        break;
      case REF_FALSE:
        *frame_id = (frame_id_t)clock_hand;
        std::cout << "result is :" << clock_hand << '\n';
        frames[clock_hand] = EMPTY;
        frame_size--;
        foundVictim = true;
        break;
      default:
        break;
    }

    clock_hand++;
  }
  frame_size_latch.unlock();
  frames_latch.unlock();
  clock_hand_latch.unlock();

  return true;
}
void ClockReplacer::Pin(frame_id_t frame_id) {
  frames_latch.lock();
  frame_size_latch.lock();

  if (frames[frame_id] != EMPTY) {
    frame_size--;
  }

  frames[frame_id] = EMPTY;

  frames_latch.unlock();
  frame_size_latch.unlock();
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
  frames_latch.lock();
  frame_size_latch.lock();
  if ((size_t)frame_id >= frames.size()) {
    return;
  }

  if (frames[frame_id] == EMPTY) {
    frame_size++;
  }
  frames[frame_id] = REF_TRUE;

  frame_size_latch.unlock();
  frames_latch.unlock();
}

size_t ClockReplacer::Size() { return frame_size; }

}  // namespace bustub
