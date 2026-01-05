#include "endstone_mipmap_cpp/HttpServer.h"

#include <fstream>
#include <sstream>

namespace endstone_mipmap_cpp {

HttpServer::HttpServer(DataStore& dataStore, HttpConfig config)
    : dataStore_(dataStore), config_(std::move(config)) {}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::start() {
    if (running_.exchange(true)) {
        return;
    }

    registerRoutes();

    serverThread_ = std::thread([this]() {
        server_.listen(config_.host.c_str(), config_.port);
    });
}

void HttpServer::stop() {
    if (!running_.exchange(false)) {
        return;
    }

    server_.stop();

    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

void HttpServer::registerRoutes() {
    if (std::filesystem::exists(config_.webRoot)) {
        server_.set_mount_point("/", config_.webRoot.string().c_str());
    }

    server_.Get("/api/config", [this](const httplib::Request&, httplib::Response& res) {
        nlohmann::json payload;
        payload["mapSize"] = config_.mapSize;
        payload["updateInterval"] = config_.updateIntervalMs;
        payload["defaultWorld"] = config_.defaultWorld;
        payload["worlds"] = dataStore_.getWorlds();
        res.set_content(payload.dump(), "application/json");
    });

    server_.Get("/api/players", [this](const httplib::Request& req, httplib::Response& res) {
        auto bounds = parseBounds(req).value_or(Bounds{});
        res.set_content(dataStore_.playersToJson(bounds).dump(), "application/json");
    });

    server_.Get(R"(/api/players/(.+)/skin\.png)", [this](const httplib::Request& req, httplib::Response& res) {
        const auto& playerName = req.matches[1];
        auto players = dataStore_.getPlayers(Bounds{});
        auto it = std::find_if(players.begin(), players.end(), [&](const PlayerInfo& player) {
            return player.name == playerName;
        });

        if (it != players.end() && !it->skinPng.empty()) {
            res.set_content(reinterpret_cast<const char*>(it->skinPng.data()), it->skinPng.size(), "image/png");
            res.set_header("Cache-Control", "public, max-age=3600");
            return;
        }

        std::filesystem::path fallback = config_.dataRoot / "skins" / "default.png";
        std::ifstream file(fallback, std::ios::binary);
        if (!file) {
            res.status = 404;
            res.set_content("skin not found", "text/plain");
            return;
        }

        std::ostringstream buffer;
        buffer << file.rdbuf();
        auto content = buffer.str();
        res.set_content(content, "image/png");
        res.set_header("Cache-Control", "public, max-age=3600");
    });

    server_.Get(R"(/api/tiles/([^/]+)/(\d+)/(\-?\d+)/(\-?\d+))",
                [this](const httplib::Request& req, httplib::Response& res) {
        const std::string world = req.matches[1];
        const std::string zoom = req.matches[2];
        const std::string tileX = req.matches[3];
        const std::string tileY = req.matches[4];

        std::filesystem::path tilePath = config_.dataRoot / "worlds" / world / "tiles" /
                                         ("zoom-" + zoom) /
                                         ("(" + tileX + ")-(" + tileY + ").png");

        if (!std::filesystem::exists(tilePath)) {
            res.status = 404;
            res.set_content("tile not found", "text/plain");
            return;
        }

        std::ifstream file(tilePath, std::ios::binary);
        std::ostringstream buffer;
        buffer << file.rdbuf();
        auto content = buffer.str();
        res.set_content(content, "image/png");
        res.set_header("Cache-Control", "public, max-age=3600");
    });

    server_.Get("/api/chunks", [this](const httplib::Request&, httplib::Response& res) {
        res.set_content(dataStore_.chunksToJson().dump(), "application/json");
    });
}

std::optional<Bounds> HttpServer::parseBounds(const httplib::Request& request) {
    if (request.params.empty()) {
        return std::nullopt;
    }

    Bounds bounds;
    if (request.has_param("x_min")) {
        bounds.xMin = std::stod(request.get_param_value("x_min"));
    }
    if (request.has_param("x_max")) {
        bounds.xMax = std::stod(request.get_param_value("x_max"));
    }
    if (request.has_param("z_min")) {
        bounds.zMin = std::stod(request.get_param_value("z_min"));
    }
    if (request.has_param("z_max")) {
        bounds.zMax = std::stod(request.get_param_value("z_max"));
    }

    return bounds;
}

} // namespace endstone_mipmap_cpp
