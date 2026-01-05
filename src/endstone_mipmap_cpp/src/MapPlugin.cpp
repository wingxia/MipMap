#include "endstone_mipmap_cpp/MapPlugin.h"

#include <chrono>

// #include <endstone/event/player/PlayerChangedWorldEvent.h>
// #include <endstone/event/player/PlayerJoinEvent.h>
// #include <endstone/event/player/PlayerMoveEvent.h>
// #include <endstone/event/player/PlayerQuitEvent.h>
// #include <endstone/event/server/ChunkLoadEvent.h>
// #include <endstone/plugin/PluginManager.h>

namespace endstone_mipmap_cpp {

void MapPlugin::onLoad() {
    // Optional: load configuration when Endstone exposes config in C++.
}

void MapPlugin::onEnable() {
    // TODO: Register event listeners with the Endstone plugin manager once the C++ API is wired.
    // getServer().getPluginManager().registerEvents(*this, *this);

    httpServer_ = std::make_unique<HttpServer>(dataStore_, httpConfig_);
    httpServer_->start();
}

void MapPlugin::onDisable() {
    if (httpServer_) {
        httpServer_->stop();
    }
}

void MapPlugin::onChunkLoad(/* endstone::event::ChunkLoadEvent& event */) {
    // TODO: Replace placeholder logic with Endstone C++ APIs.
    // auto& chunk = event.getChunk();
    // auto& world = chunk.getDimension();
    // int chunkX = chunk.getX();
    // int chunkZ = chunk.getZ();

    // ChunkInfo chunkInfo;
    // chunkInfo.dimension = world.getName();
    // chunkInfo.chunkX = chunkX;
    // chunkInfo.chunkZ = chunkZ;

    // int startX = chunkX * 16;
    // int startZ = chunkZ * 16;
    // int endX = startX + 16;
    // int endZ = startZ + 16;

    // for (int x = startX; x < endX; ++x) {
    //     for (int z = startZ; z < endZ; ++z) {
    //         auto block = world.getHighestBlockAt(x, z);
    //         auto blockType = block.getType();

    //         while (blockBlacklist_.contains(blockType) && block.getY() > -64) {
    //             block = block.getRelative(endstone::block::BlockFace::DOWN, 1);
    //             blockType = block.getType();
    //         }

    //         chunkInfo.blocks.push_back({blockType, block.getX(), block.getY(), block.getZ()});
    //     }
    // }

    // dataStore_.upsertChunk(chunkInfo);
}

void MapPlugin::onPlayerJoin(/* endstone::event::PlayerJoinEvent& event */) {
    // TODO: pull player data from the event and call dataStore_.upsertPlayer.
}

void MapPlugin::onPlayerQuit(/* endstone::event::PlayerQuitEvent& event */) {
    // TODO: pull player name from the event and call dataStore_.removePlayer.
}

void MapPlugin::onPlayerMove(/* endstone::event::PlayerMoveEvent& event */) {
    // TODO: update player position and dimension.
}

void MapPlugin::onPlayerChangedWorld(/* endstone::event::PlayerChangedWorldEvent& event */) {
    // TODO: update player dimension in dataStore_.
}

} // namespace endstone_mipmap_cpp
