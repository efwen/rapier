#pragma once

#include "input/keyboard.hpp"

#include "core/event.hpp"

#include <unordered_map>
#include <functional>
#include <optional>

namespace rp::input {
  class InputMapper {
    public:
      using MappingID = uint64_t;
      using Callback = std::function<void(const MappingID& mapping)>;

      struct KeyMapping {
        std::string name;
        Keyboard::Key key;
        Callback callback;
      };

      MappingID mapKeyPressEvent(const std::string& mapName, Keyboard::Key key, Callback keyCallback);
      bool unmapKeyPressEvent(MappingID mapId);
      std::optional<KeyMapping> getKeyPressMapping(MappingID mapId);

      MappingID mapKeyReleaseEvent(const std::string& mapName, Keyboard::Key key, Callback keyCallback);
      bool unmapKeyReleaseEvent(MappingID mapId);
      std::optional<KeyMapping> getKeyReleaseMapping(MappingID mapId);

      void pushEvent(const rp::Event& e);

    private:
      std::unordered_map<uint64_t, KeyMapping> keyPressMap;
      std::unordered_map<uint64_t, KeyMapping> keyReleaseMap;

      MappingID generateMappingID();
  };
}