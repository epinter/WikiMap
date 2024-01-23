#include "PlayerLocation.h"

namespace wmh {
    const float& PlayerLocation::getX() const {
        return x;
    }

    void PlayerLocation::setX(float newX) {
        x = newX;
    }

    const float& PlayerLocation::getY() const {
        return y;
    }

    void PlayerLocation::setY(float newY) {
        y = newY;
    }

    const float& PlayerLocation::getZ() const {
        return z;
    }

    void PlayerLocation::setZ(float newZ) {
        z = newZ;
    }

    const std::string& PlayerLocation::getLocationName() const {
        return locationName;
    }

    void PlayerLocation::setLocationName(std::string newLocationName) {
        locationName = newLocationName;
    }

    const std::string& PlayerLocation::getLocationEditorId() const {
        return locationEditorId;
    }

    void PlayerLocation::setLocationEditorId(std::string newLocationEditorId) {
        locationEditorId = newLocationEditorId;
    }

    const std::string& PlayerLocation::getWorldName() const {
        return worldName;
    }

    void PlayerLocation::setWorldName(std::string newWorldName) {
        worldName = newWorldName;
    }

    const std::string& PlayerLocation::getWorldEditorId() const {
        return worldEditorId;
    }

    void PlayerLocation::setWorldEditorId(std::string newWorldEditorId) {
        worldEditorId = newWorldEditorId;
    }

    const std::string& PlayerLocation::getParentWorldName() const {
        return parentWorldName;
    }

    void PlayerLocation::setParentWorldName(std::string newParentWorldName) {
        parentWorldName = newParentWorldName;
    }

    const std::string& PlayerLocation::getParentWorldEditorId() const {
        return parentWorldEditorId;
    }

    void PlayerLocation::setParentWorldEditorId(std::string newParentWorldEditorId) {
        parentWorldEditorId = newParentWorldEditorId;
    }

    const bool& PlayerLocation::isLocationInterior() const {
        return locationInterior;
    }

    void PlayerLocation::setLocationInterior(bool newInterior) {
        locationInterior = newInterior;
    }

    const bool& PlayerLocation::isLocationHasMapData() const {
        return locationHasMapData;
    }

    void PlayerLocation::setLocationHasMapData(bool b) {
        locationHasMapData = b;
    }

    void PlayerLocation::reset() {
        x = 0.0;
        y = 0.0;
        z = 0.0;
        locationName = "";
        locationEditorId = "";
        worldName = "";
        worldEditorId = "";
        parentWorldName = "";
        parentWorldEditorId = "";
        locationHasMapData = "";
        locationInterior = "";
    }

    bool PlayerLocation::update() {
        RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

        reset();

        setX(player->GetPositionX());
        setY(player->GetPositionY());
        setZ(player->GetPositionZ());

        if (player->GetParentCell() && player->GetParentCell()->IsInteriorCell()) {
            logger::debug("interior: true; cell:'{}'; location:'{}';", player->GetParentCell()->GetFullName(),
                          player->GetCurrentLocation() ? player->GetCurrentLocation()->GetFullName() : "");
            setLocationInterior(true);
            if (player->GetParentCell()->GetFullName()) {
                setLocationName(player->GetParentCell()->GetFullName());
            }
            setLocationEditorId(player->GetParentCell()->GetFormEditorID());
        } else {
            logger::debug("interior: false;");
            setLocationInterior(false);
            if (player->GetCurrentLocation()) {
                if (player->GetCurrentLocation()->GetFullName()) {
                    setLocationName(player->GetCurrentLocation()->GetFullName());
                }
                setLocationEditorId(player->GetCurrentLocation()->GetFormEditorID());
            }
        }

        if (player->GetWorldspace()) {
            logger::debug("world:'{}'; location:'{}';", player->GetWorldspace()->GetFullName(), getLocationName());
            setWorldName(player->GetWorldspace()->GetFullName());
            setWorldEditorId(player->GetWorldspace()->GetFormEditorID());
            if (player->GetWorldspace()->parentWorld) {
                setParentWorldName(player->GetWorldspace()->parentWorld->GetFullName());
                setParentWorldEditorId(player->GetWorldspace()->parentWorld->GetFormEditorID());
            }

            RE::WORLD_MAP_DATA mapData = player->GetWorldspace()->worldMapData;
            if (!(mapData.cameraData.initialPitch == 0 && mapData.cameraData.maxHeight == 0 && mapData.cameraData.minHeight &&
                  mapData.nwCellX == 0 && mapData.nwCellY == 0 && mapData.seCellX == 0 && mapData.seCellY == 0 &&
                  mapData.usableHeight == 0 && mapData.usableWidth == 0)) {
                setLocationHasMapData(true);
                logger::debug("world has map data");
            }
        } else if (!isLocationInterior()) {
            // if !worldspace, player must be in an interior cell, or something is wrong
            return false;
        }
        return true;
    }
}  // namespace wmh