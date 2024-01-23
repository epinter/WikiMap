#pragma once

namespace wmh {
    class PlayerLocation {
        private:
            float x = 0.0;
            float y = 0.0;
            float z = 0.0;
            std::string locationName{};
            std::string locationEditorId{};
            std::string worldName{};
            std::string worldEditorId{};
            std::string parentWorldName{};
            std::string parentWorldEditorId{};
            bool locationHasMapData = false;
            bool locationInterior = false;

        public:
            PlayerLocation() = default;
            PlayerLocation(const PlayerLocation&) = delete;
            [[nodiscard]] const float& getX() const;
            void setX(float newX);
            [[nodiscard]] const float& getY() const;
            void setY(float newY);
            [[nodiscard]] const float& getZ() const;
            void setZ(float newZ);
            [[nodiscard]] const std::string& getLocationName() const;
            void setLocationName(std::string newLocationName);
            [[nodiscard]] const std::string& getLocationEditorId() const;
            void setLocationEditorId(std::string newLocationEditorId);
            [[nodiscard]] const std::string& getWorldName() const;
            void setWorldName(std::string newWorldName);
            [[nodiscard]] const std::string& getWorldEditorId() const;
            void setWorldEditorId(std::string newWorldEditorId);
            [[nodiscard]] const std::string& getParentWorldName() const;
            void setParentWorldName(std::string newParentWorldName);
            [[nodiscard]] const std::string& getParentWorldEditorId() const;
            void setParentWorldEditorId(std::string newParentWorldEditorId);
            [[nodiscard]] const bool& isLocationInterior() const;
            void setLocationInterior(bool interior);
            [[nodiscard]] const bool& isLocationHasMapData() const;
            void setLocationHasMapData(bool b);
            void reset();
            bool update();
    };
}  // namespace wmh