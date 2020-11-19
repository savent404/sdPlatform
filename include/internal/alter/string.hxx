/**
 * @file string.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief  alter for std::string(std::basic_string<char>)
 * @version 0.1
 * @date 2020-11-19
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <string>
#include <bits/basic_string.tcc> // NOLINT

#include <internal/alter/exception.hxx>
#include <internal/alter/allocator.hxx>

namespace platform {
namespace internal {
namespace alter {

using string = std::basic_string<char, std::char_traits<char>, alter::allocator<char>>;

}  // namespace alter
}  // namespace internal
}  // namespace platform
