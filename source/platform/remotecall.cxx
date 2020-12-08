/**
 * @file remotecall.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-07
 *
 * Copyright 2020 jrlc
 *
 */
#include <rpc_api.h>

#include <platform/remotecall.hxx>

namespace platform {

rcall_client::rcall_client() : ch_(nullptr) {}
rcall_client::rcall_client(val_t* ch) { ch_ = std::unique_ptr<val_t>(ch); }

rcall_client::rcall_client(cJSON* json) { from_json(json); }

void rcall_client::from_json(cJSON* json) {
  auto pointer = cJSON_GetObjectItem(json, "pointer");
  if (!pointer) return;
  int val = static_cast<int>(cJSON_GetNumberValue(pointer));
  val_t* ptr = reinterpret_cast<val_t*>(val);
  ch_ = std::unique_ptr<val_t>(ptr);
}

cJSON* rcall_client::to_json() {
  auto root = cJSON_CreateObject();
  cJSON_AddNumberToObject(root, "pointer", (int)(ch_.get()));  // NOLINT
  return root;
}

template <typename ReturnType>
int rcall_client::call(int msg_id, const void* tx_data, size_t tx_len, ReturnType* rx_data, size_t* rx_len) {
  return _call(msg_id, tx_data, tx_len, rx_data, rx_len);
}

int rcall_client::_call(int msg_id, const void* tx_data, size_t tx_len, void* rx_data, size_t* rx_len) {
  bool hasTx = tx_data != nullptr && tx_len != 0;
  bool hasRx = rx_data != nullptr && rx_len != 0;

  u8* txdata = static_cast<u8*>(const_cast<void*>(tx_data));
  u8* rxdata = static_cast<u8*>(rx_data);
  u16 txlen = tx_len;
  u16 rxlen = *rx_len;

  int res = 0;

  if (hasTx && hasRx) {
    res = rpc_call(ch_.get(), msg_id, rxdata, txlen, rxdata, &rxlen);
    *rx_len = rxlen;
  } else if (hasTx && !hasRx) {
    res = rpc_call_no_rx(ch_.get(), msg_id, txdata, txlen);
  } else if (!hasRx && hasRx) {
    res = rpc_call_no_tx(ch_.get(), msg_id, rxdata, &rxlen);
    *rx_len = rxlen;
  } else {
    res = rpc_call_no_txrx(ch_.get(), msg_id);
  }
  return res;
}
}  // namespace platform
