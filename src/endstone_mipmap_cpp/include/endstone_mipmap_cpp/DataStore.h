#pragma once

#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <nlohmann/json.hpp>

namespace endstone_mipmap_cpp {

struct BlockInfo {
    std::string name;
    int x = 0;
    int y = 0;
    int z = 0;
};

struct ChunkInfo {
    std::string dimension;
    int chunkX = 0;
    int chunkZ = 0;
    std::vector<BlockInfo> blocks;
};

struct PlayerInfo {
    std::string name;
    std::string xuid;
    std::string dimension;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    std::vector<std::uint8_t> skinPng;
    std::chrono::steady_clock::time_point lastUpdated = std::chrono::steady_clock::now();
};

struct Bounds {
    std::optional<double> xMin;
    std::optional<double> xMax;
    std::optional<double> zMin;
    std::optional<double> zMax;
};

class DataStore {
public:
    void upsertChunk(const ChunkInfo& chunk);
    void upsertPlayer(const PlayerInfo& player);
    void removePlayer(const std::string& name);

    std::vector<PlayerInfo> getPlayers(const Bounds& bounds) const;
    std::vector<ChunkInfo> getChunks() const;
    std::vector<std::string> getWorlds() const;

    nlohmann::json playersToJson(const Bounds& bounds) const;
    nlohmann::json chunksToJson() const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, ChunkInfo> chunks_;
    std::unordered_map<std::string, PlayerInfo> players_;
    std::unordered_set<std::string> worlds_;

    static std::string chunkKey(const ChunkInfo& chunk);
};

} // namespace endstone_mipmap_cpp
