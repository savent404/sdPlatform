/**
 * @file vector.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-26
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <vector>

#include <platform/alter/allocator.hxx>

namespace platfrom::alter {
template <typename T>
using vector = std::vector<T, alter::allocator<T>>;
}
