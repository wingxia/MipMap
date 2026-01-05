#include "endstone_mipmap_cpp/DataStore.h"

#include <algorithm>

namespace endstone_mipmap_cpp {

void DataStore::upsertChunk(const ChunkInfo& chunk) {
    std::lock_guard<std::mutex> lock(mutex_);
    chunks_[chunkKey(chunk)] = chunk;
    worlds_.insert(chunk.dimension);
}

void DataStore::upsertPlayer(const PlayerInfo& player) {
    std::lock_guard<std::mutex> lock(mutex_);
    players_[player.name] = player;
    worlds_.insert(player.dimension);
}

void DataStore::removePlayer(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    players_.erase(name);
}

std::vector<PlayerInfo> DataStore::getPlayers(const Bounds& bounds) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PlayerInfo> result;
    result.reserve(players_.size());

    for (const auto& [name, player] : players_) {
        if (bounds.xMin && player.x < *bounds.xMin) {
            continue;
        }
        if (bounds.xMax && player.x > *bounds.xMax) {
            continue;
        }
        if (bounds.zMin && player.z < *bounds.zMin) {
            continue;
        }
        if (bounds.zMax && player.z > *bounds.zMax) {
            continue;
        }

        result.push_back(player);
    }

    std::sort(result.begin(), result.end(), [](const PlayerInfo& a, const PlayerInfo& b) {
        return a.name < b.name;
    });

    return result;
}

std::vector<ChunkInfo> DataStore::getChunks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ChunkInfo> result;
    result.reserve(chunks_.size());

    for (const auto& [key, chunk] : chunks_) {
        result.push_back(chunk);
    }

    return result;
}

std::vector<std::string> DataStore::getWorlds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> result(worlds_.begin(), worlds_.end());
    std::sort(result.begin(), result.end());
    return result;
}

nlohmann::json DataStore::playersToJson(const Bounds& bounds) const {
    nlohmann::json payload;
    payload["status"] = "success";
    payload["players"] = nlohmann::json::array();

    for (const auto& player : getPlayers(bounds)) {
        payload["players"].push_back({
            {"name", player.name},
            {"xuid", player.xuid},
            {"dimension", player.dimension},
            {"x", player.x},
            {"y", player.y},
            {"z", player.z}
        });
    }

    return payload;
}

nlohmann::json DataStore::chunksToJson() const {
    nlohmann::json payload;
    payload["status"] = "success";
    payload["chunks"] = nlohmann::json::array();

    for (const auto& chunk : getChunks()) {
        nlohmann::json chunkJson;
        chunkJson["dimension"] = chunk.dimension;
        chunkJson["chunkX"] = chunk.chunkX;
        chunkJson["chunkZ"] = chunk.chunkZ;
        chunkJson["blocks"] = nlohmann::json::array();

        for (const auto& block : chunk.blocks) {
            chunkJson["blocks"].push_back({
                {"name", block.name},
                {"coordinates", {block.x, block.y, block.z}}
            });
        }

        payload["chunks"].push_back(chunkJson);
    }

    return payload;
}

std::string DataStore::chunkKey(const ChunkInfo& chunk) {
    return chunk.dimension + ":" + std::to_string(chunk.chunkX) + ":" + std::to_string(chunk.chunkZ);
}

} // namespace endstone_mipmap_cpp
