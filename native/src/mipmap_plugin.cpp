#include <httplib.h>
#include <nlohmann/json.hpp>

extern "C" const char *mipmap_plugin_name() {
  static const char name[] = "MipMap";
  return name;
}

void mipmap_plugin_stub() {
  httplib::Headers headers;
  headers.emplace("User-Agent", "MipMap-Plugin");

  nlohmann::json payload = nlohmann::json::object();
  payload["status"] = "initialized";
  payload["headers"] = headers.size();
}
