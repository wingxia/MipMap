#pragma once

#include <atomic>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>

#include <httplib.h>

#include "endstone_mipmap_cpp/DataStore.h"

namespace endstone_mipmap_cpp {

struct HttpConfig {
    std::string host = "0.0.0.0";
    int port = 8100;
    std::filesystem::path webRoot = "webmap/web";
    std::filesystem::path dataRoot = "webmap/data";
    int updateIntervalMs = 5000;
    int mapSize = 1000;
    std::string defaultWorld = "Overworld";
};

class HttpServer {
public:
    HttpServer(DataStore& dataStore, HttpConfig config);
    ~HttpServer();

    void start();
    void stop();

private:
    void registerRoutes();
    static std::optional<Bounds> parseBounds(const httplib::Request& request);

    DataStore& dataStore_;
    HttpConfig config_;
    httplib::Server server_;
    std::thread serverThread_;
    std::atomic<bool> running_{false};
};

} // namespace endstone_mipmap_cpp
