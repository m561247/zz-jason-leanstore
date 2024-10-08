#include "telemetry/MetricsHttpExposer.hpp"

#include "leanstore/LeanStore.hpp"

namespace leanstore::telemetry {

MetricsHttpExposer::MetricsHttpExposer(LeanStore* store)
    : leanstore::telemetry::MetricsHttpExposer(store, store->mStoreOption->mMetricsPort) {
}

MetricsHttpExposer::MetricsHttpExposer(LeanStore* store, int32_t port)
    : UserThread(store, "MetricsExposer"),
      mPort(port) {
  mServer.new_task_queue = [] { return new httplib::ThreadPool(1); };
  mServer.Get("/metrics", [&](const httplib::Request& req, httplib::Response& res) {
    handleMetrics(req, res);
  });

  mServer.Get("/heap",
              [&](const httplib::Request& req, httplib::Response& res) { handleHeap(req, res); });

  mServer.Get("/profile", [&](const httplib::Request& req, httplib::Response& res) {
    handleProfile(req, res);
  });
}

} // namespace leanstore::telemetry