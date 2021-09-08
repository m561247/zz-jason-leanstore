#include "VersionsSpace.hpp"
#include "Units.hpp"
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
#include <atomic>
#include <condition_variable>
#include <functional>
#include <map>
#include <thread>
#include <unordered_map>
#include <vector>
// -------------------------------------------------------------------------------------
namespace leanstore
{
namespace cr
{
// -------------------------------------------------------------------------------------
void VersionsSpace::insertVersion(WORKERID session_id, TXID tx_id, DTID dt_id, u64 command_id, u64 payload_length, std::function<void(u8*)> cb)
{
   u64 key_length = sizeof(tx_id) + sizeof(dt_id) + sizeof(command_id);
   u8 key[key_length];
   u64 offset = 0;
   offset += utils::fold(key + offset, tx_id);
   offset += utils::fold(key + offset, dt_id);
   offset += utils::fold(key + offset, command_id);
   // -------------------------------------------------------------------------------------
   u8 payload[payload_length];
   cb(payload);
   btree->insert(key, key_length, payload, payload_length);
}
// -------------------------------------------------------------------------------------
bool VersionsSpace::retrieveVersion(WORKERID session_id, TXID tx_id, DTID dt_id, u64 command_id, std::function<void(const u8*, u64 payload_length)> cb)
{
   u64 key_length = sizeof(tx_id) + sizeof(dt_id) + sizeof(command_id);
   u8 key[key_length];
   u64 offset = 0;
   offset += utils::fold(key + offset, tx_id);
   offset += utils::fold(key + offset, dt_id);
   offset += utils::fold(key + offset, command_id);
   // -------------------------------------------------------------------------------------
   OP_RESULT ret = btree->lookup(key, key_length, [&](const u8* payload, u16 payload_length) { cb(payload, payload_length); });
   if (ret == OP_RESULT::OK) {
      return true;
   } else {
      return false;
   }
}
// -------------------------------------------------------------------------------------
void VersionsSpace::purgeTXIDRange(TXID from_tx_id, TXID to_tx_id)
{  // [from, to]
   // std::basic_string<u8> begin, end;
   // begin.resize(sizeof(from_tx_id));
   // utils::fold(begin.data(), from_tx_id);
   // end.resize(sizeof(from_tx_id));
   // utils::fold(end.data(), to_tx_id);
   // std::shared_lock guard(mutex);
   // auto range = map.range(begin)
}
// -------------------------------------------------------------------------------------
}  // namespace cr
}  // namespace leanstore
