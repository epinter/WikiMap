#include "UrlProvider.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/exception/all.hpp>
#include <boost/url.hpp>
#include <boost/url/grammar/alnum_chars.hpp>

#include "Config.h"
#include "SteamApi.h"
#include "SelectedItem.h"

namespace wmh {
    std::function<void(void)> UrlProvider::inputHandler() {
        return [this]() {
            using namespace fsteam;

            if (!playerLocation.update()) {
                logger::error("unable to update player location");
                return;
            }

            std::string url{};

            if (RE::UI::GetSingleton()->IsMenuOpen(RE::MapMenu::MENU_NAME) && !RE::UI::GetSingleton()->IsMenuOpen(RE::Console::MENU_NAME) &&
                !RE::UI::GetSingleton()->IsMenuOpen(RE::JournalMenu::MENU_NAME)) {
                try {
                    logger::debug("SelectedItem: {}", SelectedItem::getInstance().toString());
                    if (SelectedItem::getInstance().getMapIndex() >= 0 && !SelectedItem::getInstance().getMapLabel().empty() &&
                        SelectedItem::getInstance().getMapQuestId() <= 0) {
                        url = buildLocationUrl(SelectedItem::getInstance().getMapLabel());
                    } else if (SelectedItem::getInstance().getMapQuestId() > 0) {
                        RE::TESQuest* quest = RE::TESForm::LookupByID<RE::TESQuest>(SelectedItem::getInstance().getMapQuestId());
                        if (quest) {
                            logger::debug("quest selected: '{}' ({})", quest->GetFullName(), quest->GetFormEditorID());
                            if (quest && std::string{quest->GetFullName()}.contains("<")) {
                                url = buildQuestUrl(SelectedItem::getInstance().getMapQuestLabel(), quest->GetFormEditorID());
                            } else if (quest) {
                                url = buildQuestUrl(quest->GetFullName(), quest->GetFormEditorID());
                            }
                        } else {
                            url = buildLocationUrl();
                        }
                    } else {
                        url = buildLocationUrl();
                    }

                    if (url.empty()) {
                        return;
                    }
                } catch (const std::exception& e) {
                    logger::error("Exception building map url: '{}'", e.what());
                    return;
                } catch (...) {
                    logger::error("Unknown error while building map url");
                    return;
                }
            } else if (RE::UI::GetSingleton()->IsMenuOpen(RE::JournalMenu::MENU_NAME)) {
                try {
                    if (SelectedItem::getInstance().getQuestId() > 0) {
                        RE::TESQuest* quest = RE::TESForm::LookupByID<RE::TESQuest>(SelectedItem::getInstance().getQuestId());
                        if (quest && std::string{quest->GetFullName()}.contains("<")) {
                            url = buildQuestUrl(SelectedItem::getInstance().getQuestName(), quest->GetFormEditorID());
                        } else if (quest) {
                            url = buildQuestUrl(quest->GetFullName(), quest->GetFormEditorID());
                        }
                    }
                    if (url.empty()) {
                        return;
                    }
                } catch (const std::exception& e) {
                    logger::error("Exception building quest url: '{}'", e.what());
                    return;
                } catch (...) {
                    logger::error("Unknown error while building quest url");
                    return;
                }
            } else {
                return;
            }

            SteamApi api{};

            if (Config::get().isEnableSteam() && api.isSteamOverlayEnabled()) {
                api.openUrlOnSteamOverlay(url, Config::get().isUseSteamModal());
            } else {
                logger::info("opening url using shell: '{}'", url);
                ShellExecute(0, "open", url.c_str(), 0, 0, 3);
            }
        };
    }

    const std::string UrlProvider::buildQuestUrl(std::string name, std::string editorId) {
        std::string templateQuest{Config::get().getUrlQuest()};
        std::string encodedQuest{};
        std::string encodedQuestId{};

        try {
            encodedQuest = boost::urls::encode(name, boost::urls::grammar::alnum_chars);
            encodedQuestId = boost::urls::encode(editorId, boost::urls::grammar::alnum_chars);
            logger::debug("encodedQuest: '{}'", encodedQuest);
            boost::algorithm::replace_all(templateQuest, "%Q", encodedQuest);
            boost::algorithm::replace_all(templateQuest, "%I", encodedQuestId);
            logger::debug("building url quest: '{}'", templateQuest);
            return validateUrl(boost::url{boost::url_view{templateQuest}}.c_str());
        } catch (const std::exception& e) {
            logger::error("Exception building quest url: '{}'", e.what());
            return std::string{};
        } catch (const boost::exception& e) {
            logger::error("Exception building quest url: '{}';", boost::diagnostic_information(e));
            return std::string{};
        } catch (...) {
            logger::error("Unknown error while building quest url");
            return std::string{};
        }
    }

    const std::string UrlProvider::buildLocationUrl(std::string locationName) {
        std::string templateLocation{Config::get().getUrlSelected()};
        std::string encodedLocation{};

        try {
            encodedLocation = boost::urls::encode(locationName, boost::urls::grammar::alnum_chars);
            logger::debug("encodedLocation: '{}'", encodedLocation);
            boost::algorithm::replace_all(templateLocation, "%L", encodedLocation);
            logger::debug("building url for selected location: '{}'", templateLocation);
            return validateUrl(boost::url{boost::url_view{templateLocation}}.c_str());
        } catch (const std::exception& e) {
            logger::error("Exception building selected url: '{}'", e.what());
            return std::string{};
        } catch (const boost::exception& e) {
            logger::error("Exception building selected url: '{}';", boost::diagnostic_information(e));
            return std::string{};
        } catch (...) {
            logger::error("Unknown error while building selected url");
            return std::string{};
        }
    }

    const std::string UrlProvider::buildLocationUrl() {
        std::string url{};

        std::string templateLocation{Config::get().getUrlInterior()};
        std::string templateWorld{};

        bool worldWithoutMapConfig = false;

        auto worldUrlMap = Config::get().getMapUrlWorld();
        if (auto search = worldUrlMap.find(playerLocation.getWorldEditorId()); search != worldUrlMap.end()) {
            templateWorld = search->second;
        } else if (auto searchParent = worldUrlMap.find(playerLocation.getParentWorldEditorId()); searchParent != worldUrlMap.end()) {
            templateWorld = searchParent->second;
        } else if (!playerLocation.isLocationInterior()) {
            worldWithoutMapConfig = true;
        }

        logger::debug("templateWorld: '{}'", templateWorld);

        bool worldWithoutMap = (!playerLocation.isLocationInterior() && worldWithoutMapConfig &&
                                playerLocation.getParentWorldEditorId().empty() && playerLocation.isLocationHasMapData()) ||
                               (!playerLocation.isLocationInterior() && worldWithoutMapConfig &&
                                !playerLocation.getParentWorldEditorId().empty() && !playerLocation.isLocationHasMapData());

        if (playerLocation.isLocationInterior() || worldWithoutMap) {
            url = buildInteriorUrl(templateLocation);
        } else {
            url = buildExteriorUrl(templateWorld);
        }

        logger::debug("url:'{}'; isInterior:'{}'; world:'{}'; location:'{}'; x:'{}'; y:'{}'; z:'{}';", url,
                      playerLocation.isLocationInterior(), playerLocation.getWorldName(), playerLocation.getLocationName(),
                      playerLocation.getX(), playerLocation.getY(), playerLocation.getZ());
        return validateUrl(url);
    }

    std::string UrlProvider::buildInteriorUrl(std::string templateLocation) {
        using namespace boost::algorithm;
        std::string encodedLocation{};

        try {
            replace_all(templateLocation, "%X", std::to_string(static_cast<int>(playerLocation.getX())));
            replace_all(templateLocation, "%Y", std::to_string(static_cast<int>(playerLocation.getY())));
            replace_all(templateLocation, "%Z", std::to_string(static_cast<int>(playerLocation.getZ())));
            replace_all(templateLocation, "%I", playerLocation.getLocationEditorId());
            encodedLocation = boost::urls::encode(playerLocation.getLocationName(), boost::urls::grammar::alnum_chars);
            logger::debug("encodedLocation: '{}'", encodedLocation);
            replace_all(templateLocation, "%L", encodedLocation);
            logger::debug("building url for interior: '{}'", templateLocation);
            return std::string{boost::url{boost::url_view{templateLocation}}.c_str()};
        } catch (const std::exception& e) {
            logger::error("Exception building interior url: '{}'", e.what());
            return std::string{};
        } catch (const boost::exception& e) {
            logger::error("Exception building interior url: '{}';", boost::diagnostic_information(e));
            return std::string{};
        } catch (...) {
            logger::error("Unknown error while building interior url");
            return std::string{};
        }
    }

    std::string UrlProvider::buildExteriorUrl(std::string templateWorld) {
        using namespace boost::algorithm;
        std::string encodedWorldName{};
        std::string encodedLocation{};

        try {
            replace_all(templateWorld, "%X", std::to_string(static_cast<int>(playerLocation.getX())));
            replace_all(templateWorld, "%Y", std::to_string(static_cast<int>(playerLocation.getY())));
            replace_all(templateWorld, "%Z", std::to_string(static_cast<int>(playerLocation.getZ())));
            replace_all(templateWorld, "%I", playerLocation.getWorldEditorId());
            encodedLocation = boost::urls::encode(playerLocation.getLocationName(), boost::urls::grammar::alnum_chars);
            encodedWorldName = boost::urls::encode(playerLocation.getWorldName(), boost::urls::grammar::alnum_chars);
            logger::debug("encodedLocation: '{}'", encodedLocation);
            logger::debug("encodedWorldName: '{}'", encodedWorldName);
            replace_all(templateWorld, "%W", encodedWorldName);
            replace_all(templateWorld, "%L", encodedLocation);
            logger::debug("building url for exterior: '{}'", templateWorld);
            return std::string{boost::url{boost::url_view{templateWorld}}.c_str()};
        } catch (const std::exception& e) {
            logger::error("Exception building exterior url: '{}'", e.what());
            return std::string{};
        } catch (const boost::exception& e) {
            logger::error("Exception building exterior url: '{}';", boost::diagnostic_information(e));
            return std::string{};
        } catch (...) {
            logger::error("Unknown error while building exterior url");
            return std::string{};
        }
    }

    std::string UrlProvider::validateUrl(std::string url) {
        try {
            if (url.empty() ||
                (!boost::algorithm::istarts_with(url.c_str(), "http:") && !boost::algorithm::istarts_with(url.c_str(), "https:"))) {
                logger::error("Invalid url: '{}'", url.c_str());
                return std::string{};
            }
        } catch (const std::exception& e) {
            logger::error("Exception validating url: '{}'", e.what());
            return std::string{};
        } catch (const boost::exception& e) {
            logger::error("Exception validating url: '{}';", boost::diagnostic_information(e));
            return std::string{};
        } catch (...) {
            logger::error("Unknown error while validating url");
            return std::string{};
        }

        return url;
    }
}  // namespace wmh