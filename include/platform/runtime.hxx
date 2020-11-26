/**
 * @file runtime.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <memory>

#include <consthash/crc32.hxx>
#include <platform/cJSON.hxx>

namespace platform {

#define RUNTIME_INIT(name)                                                   \
  static constexpr uint32_t __hash = consthash::crc32(#name, sizeof(#name)); \
  constexpr static uint32_t __get_hash() { return __hash; }                  \
  void __init() override { hash_ = __hash; }

namespace dtls {
struct runtime_base {
 public:

  // runtime() = default;
  explicit runtime_base(int hash = 0) : hash_(hash) {}
  ~runtime_base() = default;

  /**
   * @brief convert this instance to cJSON obj
   * @return cJSON*
   */
  virtual platform::cJSON* to_json() const { return nullptr; }
  virtual void from_json(platform::cJSON* obj) const {}

  virtual void __init() { hash_ = 0; }
  static uint32_t __get_hash() { return 0; }

 protected:
  uint32_t hash_;
};

}  // namespace dtls

/**
 * @brief 运行时数据
 * @note 支持from_json/to_json方法，需要子类继承方法后使用
 *
 * @note 在struct开头使用魔法 "RUNTIME_INIT(<自定义名字>)", 需要确保自定义名字全局唯一
 * @note 为新的runtime创建一个新的默认构造函数， 调用 __init() 函数
 *
 * @code
struct myruntime : public platform::runtime {
  RUNTIME_INIT(test-runtime);
  myruntime() { __init(); }
  int a;
  int b;
  int c;
};
 * @endcode
 * 
 * @note 使用promote将父类的指针转换为其子类
 */
struct runtime : public dtls::runtime_base {
  using value_ref = runtime&;
  using value_ptr = std::unique_ptr<runtime>;

  RUNTIME_INIT(runtimebase);

  runtime() : dtls::runtime_base() { __init(); }
  /**
   * @brief 将基础类的this指针提升为子类的指针
   * 
   * @tparam T platform::runtime的子类
   * @param hash 用于检查是否可以转换的哈希唯一标识符
   * @code
   * platform::runtime* base_ptr = new myruntime;
   * myruntime* = base_ptr->promote<myruntime>();
   * @endcode
   * @warning 仅用于指针间的类型强转，无法将实际的父类转换为子类
   * @note 具有类型的安全检查
   * @return T* 子类指针
   */
  template <typename T = runtime>
  T* promote(uint32_t hash = T::__get_hash()) {
    while (hash != hash_) {
    }
    return reinterpret_cast<T*>(this);
  }
};

}  // namespace platform
