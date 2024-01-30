#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>

using std::cerr;
using std::cout;
using std::endl;
using std::make_unique;
using std::string;
using std::to_string;
using std::tuple;
using std::unique_ptr;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using SIZE = size_t;
using PID = u64;    // page ID
using LID = u64;    // log ID
using TTS = u64;    // transaction Time Stamp
using TREEID = u64; // data structure ID

using WORKERID = u16;
using TXID = u64;
using COMMANDID = u32;
#define TYPE_MSB(TYPE) (1ull << ((sizeof(TYPE) * 8) - 1))

using StringMap = std::unordered_map<std::string, std::string>;
using str = std::string_view;
using Slice = std::basic_string_view<u8>;

constexpr u64 kLsb = u64(1);

namespace leanstore {

inline std::string ToString(Slice slice) {
  return std::string(reinterpret_cast<const char*>(slice.data()), slice.size());
}

inline Slice ToSlice(const std::string& str) {
  return Slice(reinterpret_cast<const u8*>(str.data()), str.size());
}

enum class TxMode : u8 {
  kLongRunning = 0,
  kShortRunning = 1,
};

inline std::string ToString(TxMode txMode) {
  switch (txMode) {
  case TxMode::kLongRunning: {
    return "LongRunning";
  }
  case TxMode::kShortRunning: {
    return "ShortRunning";
  }
  }
  return "Unknown TxMode";
}

enum class IsolationLevel : u8 {
  // kReadUnCommitted = 0,
  // kReadCommitted = 1,
  kSnapshotIsolation = 2,
  kSerializable = 3,
};

inline IsolationLevel ParseIsolationLevel(std::string str) {
  if (str == "ser") {
    return leanstore::IsolationLevel::kSerializable;
  }
  if (str == "si") {
    return leanstore::IsolationLevel::kSnapshotIsolation;
  }
  return leanstore::IsolationLevel::kSnapshotIsolation;
}

} // namespace leanstore
