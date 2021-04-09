#include "input/mapper.hpp"

#include <exception>

namespace rp::input {
      InputMapper::MappingID InputMapper::mapKeyPressEvent(const std::string& mapName, Keyboard::Key key, InputMapper::Callback keyCallback) {
        throw std::exception("mapKeyPressEvent not implemented!"); 
        return 0;
      }
      bool InputMapper::unmapKeyPressEvent(InputMapper::MappingID mapId) {
        throw std::exception("unmapKeyPressEvent not implemented!"); 
        return false;
      }
      std::optional<InputMapper::KeyMapping> InputMapper::getKeyPressMapping(InputMapper::MappingID mapId) {
        throw std::exception("getKeyPressMapping not implemented!"); 
        return std::nullopt;
      }

      InputMapper::MappingID InputMapper::mapKeyReleaseEvent(const std::string& mapName, Keyboard::Key key, InputMapper::Callback keyCallback) {
        throw std::exception("mapKeyReleaseEvent not implemented!"); 
        return 0;
      }
      bool InputMapper::unmapKeyReleaseEvent(InputMapper::MappingID mapId) {
        throw std::exception("unmapKeyReleaseEvent not implemented!"); 
        return false;
      }
      std::optional<InputMapper::KeyMapping> InputMapper::getKeyReleaseMapping(InputMapper::MappingID mapId) {
        throw std::exception("getKeyReleaseMapping not implemented!"); 
        return std::nullopt;
      }

      void InputMapper::pushEvent(const rp::Event& e) {
        throw std::exception("pushEvent not implemented!");
      }

      InputMapper::MappingID generateMappingID() {
        throw std::exception("generateMappingID not implemented!");
        return 0;
      }
}