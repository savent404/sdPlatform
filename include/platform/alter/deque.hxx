/**
 * @file deque.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-26
 * 
 * Copyright 2020 jrlc
 * 
 */
#include <deque>
#include <platform/alter/allocator.hxx>
namespace platform::alter {

template<typename T>
using deque = std::deque<T, alter::allocator<T>>;

}
