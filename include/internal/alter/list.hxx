/**
 * @file list.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief based on std::list
 * @version 0.1
 * @date 2020-11-19
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <list>

#include <internal/alter/allocator.hxx>

namespace platform {
namespace internal {
namespace alter {

template <typename T>
using list = std::list<T, alter::allocator<T>>;

}  // namespace alter
}  // namespace internal
}  // namespace platform
