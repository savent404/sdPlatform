/**
 * @file queue.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-26
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <queue>

#include <platform/alter/deque.hxx>

namespace platform::alter {

template <typename T>
using queue = std::queue<T, alter::deque<T>>;

}
