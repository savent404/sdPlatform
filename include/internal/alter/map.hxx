/**
 * @file map.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief alter for std::map
 * @version 0.1
 * @date 2020-11-19
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <functional>
#include <map>
#include <utility>

#include <internal/alter/allocator.hxx>

namespace platform {
namespace internal {
namespace alter {
template <typename _k, typename _p, typename _compare = std::less<_k>,
          typename _alloc = alter::allocator<std::pair<const _k, _p>>>
using map = std::map<_k, _p, _compare, _alloc>;
}  // namespace alter

}  // namespace internal
}  // namespace platform
