#pragma once

#include <memory>
#include <unordered_set>

#include "endstone_mipmap_cpp/DataStore.h"
#include "endstone_mipmap_cpp/HttpServer.h"

namespace endstone {
class Plugin;
}

namespace endstone_mipmap_cpp {

class MapPlugin : public endstone::Plugin {
public:
    void onEnable() override;
    void onDisable() override;
    void onLoad() override;

private:
    void onChunkLoad(/* endstone::event::ChunkLoadEvent& event */);
    void onPlayerJoin(/* endstone::event::PlayerJoinEvent& event */);
    void onPlayerQuit(/* endstone::event::PlayerQuitEvent& event */);
    void onPlayerMove(/* endstone::event::PlayerMoveEvent& event */);
    void onPlayerChangedWorld(/* endstone::event::PlayerChangedWorldEvent& event */);

    DataStore dataStore_;
    std::unique_ptr<HttpServer> httpServer_;
    HttpConfig httpConfig_{};

    std::unordered_set<std::string> blockBlacklist_{
        "air",
        "water",
        "lava",
        "flowing_water",
        "flowing_lava"
    };
};

} // namespace endstone_mipmap_cpp
