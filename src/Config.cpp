#include <Config.h>

namespace wmh {
    Config::Config() {
        readConfig();
    }

    void Config::readConfig() {
        using namespace constants;

        // main ini file
        CSimpleIniA ini{};
        ini.SetQuotes(true);
        ini.SetAllowKeyOnly(false);
        ini.SetUnicode(true);
        ini.SetMultiKey(false);
        std::string fileName = std::string{"Data\\SKSE\\Plugins\\"}.append(CONFIG_FILE);

        logger::debug("loading config from: '{}';", fileName);
        bool save = false;
        if (ini.LoadFile(fileName.c_str()) == 0) {
            debug = ini.GetBoolValue(INI_SECTION_GENERAL, INI_OPT_DEBUG, debug);
            urlInterior = ini.GetValue(INI_SECTION_GENERAL, INI_OPTINTERIORURL, urlInterior.c_str());
            urlSelected = ini.GetValue(INI_SECTION_GENERAL, INI_OPTSELECTEDURL, urlSelected.c_str());
            urlQuest = ini.GetValue(INI_SECTION_GENERAL, INI_OPTQUESTURL, urlQuest.c_str());
            enableSteam = ini.GetBoolValue(INI_SECTION_USER, INI_OPTENABLESTEAM, enableSteam);
            useSteamModal = ini.GetBoolValue(INI_SECTION_USER, INI_OPTUSESTEAMMODAL, useSteamModal);
            keyCode = ini.GetLongValue(INI_SECTION_USER, INI_OPTKEYCODE, keyCode);
            modCode = ini.GetLongValue(INI_SECTION_USER, INI_OPTMODCODE, modCode);
            updateBottomBar = ini.GetBoolValue(INI_SECTION_GENERAL, INI_OPTBOTTOMBAR, updateBottomBar);

            if (ini.SectionExists(INI_SECTION_WORLD)) {
                for (auto const& e: *ini.GetSection(INI_SECTION_WORLD)) {
                    if (e.first.pItem && e.second && boost::algorithm::istarts_with(e.first.pItem, INI_OPTPREFIX_WORLD)) {
                        std::string worldId{e.first.pItem};
                        boost::algorithm::replace_first(worldId, INI_OPTPREFIX_WORLD, "");
                        if (!worldId.empty()) {
                            worldUrl.emplace(std::make_pair(worldId, e.second));
                            logger::debug("world found: editorID:'{}'; url:'{}';", worldId, e.second);
                        }
                    }
                }
            }
        } else {
            logger::error("Can't open config file '{}'. Trying to create it.", CONFIG_FILE);
        }

        //create default config
        if (!ini.KeyExists(INI_SECTION_GENERAL, INI_OPT_DEBUG)) {
            ini.SetBoolValue(INI_SECTION_GENERAL, INI_OPT_DEBUG, debug, nullptr, false);
            save = true;
        }
        if (!ini.KeyExists(INI_SECTION_GENERAL, INI_OPTBOTTOMBAR)) {
            ini.SetBoolValue(INI_SECTION_GENERAL, INI_OPTBOTTOMBAR, updateBottomBar, INI_COMMENT_OPTBOTTOMBAR, false);
            save = true;
        }
        if (!ini.KeyExists(INI_SECTION_USER, INI_OPTKEYCODE)) {
            ini.SetLongValue(INI_SECTION_USER, INI_OPTKEYCODE, keyCode, INI_COMMENT_OPTKEYCODE, false);
            save = true;
        }
        if (!ini.KeyExists(INI_SECTION_USER, INI_OPTMODCODE)) {
            ini.SetLongValue(INI_SECTION_USER, INI_OPTMODCODE, modCode, INI_COMMENT_OPTMODCODE, false);
            save = true;
        }
        if (!ini.KeyExists(INI_SECTION_USER, INI_OPTENABLESTEAM)) {
            ini.SetBoolValue(INI_SECTION_USER, INI_OPTENABLESTEAM, enableSteam, INI_COMMENT_OPTENABLESTEAM, false);
            save = true;
        }
        if (!ini.KeyExists(INI_SECTION_USER, INI_OPTUSESTEAMMODAL)) {
            ini.SetBoolValue(INI_SECTION_USER, INI_OPTUSESTEAMMODAL, useSteamModal, INI_COMMENT_OPTUSESTEAMMODAL, false);
            save = true;
        }
        if (!ini.KeyExists(INI_SECTION_GENERAL, INI_OPTSELECTEDURL)) {
            ini.SetValue(INI_SECTION_GENERAL, INI_OPTSELECTEDURL, urlSelected.c_str(), INI_COMMENT_OPTSELECTEDURL, false);
            save = true;
        }
        if (!ini.KeyExists(INI_SECTION_GENERAL, INI_OPTINTERIORURL)) {
            ini.SetValue(INI_SECTION_GENERAL, INI_OPTINTERIORURL, urlInterior.c_str(), INI_COMMENT_OPTINTERIORURL, false);
            save = true;
        }
        if (!ini.KeyExists(INI_SECTION_GENERAL, INI_OPTQUESTURL)) {
            ini.SetValue(INI_SECTION_GENERAL, INI_OPTQUESTURL, urlQuest.c_str(), INI_COMMENT_OPTQUESTURL, false);
            save = true;
        }
        if (!ini.SectionExists(INI_SECTION_WORLD)) {
            ini.SetValue(INI_SECTION_WORLD, nullptr, nullptr, INI_COMMENT_WORLD);
            save = true;
        }
        if (!ini.KeyExists(INI_SECTION_WORLD, std::format("{}Tamriel", INI_OPTPREFIX_WORLD).c_str())) {
            ini.SetValue(INI_SECTION_WORLD, std::format("{}Tamriel", INI_OPTPREFIX_WORLD).c_str(), INI_DEF_URL_TAMRIEL, nullptr, false);
            save = true;
        }
        if (!ini.KeyExists(INI_SECTION_WORLD, std::format("{}DLC2SolstheimWorld", INI_OPTPREFIX_WORLD).c_str())) {
            ini.SetValue(INI_SECTION_WORLD, std::format("{}DLC2SolstheimWorld", INI_OPTPREFIX_WORLD).c_str(), INI_DEF_URL_SOLSTHEIM,
                         nullptr, false);
            save = true;
        }

        if (save) {
            ini.SaveFile(fileName.c_str());
        }

        CSimpleIniA settings{};
        settings.SetQuotes(true);
        settings.SetAllowKeyOnly(false);
        settings.SetUnicode(true);
        settings.SetMultiKey(false);

        std::string mcmConfig = std::string{"Data\\MCM\\Settings\\"}.append(MCM_CONFIG_FILE);
        if (ini.LoadFile(mcmConfig.c_str()) == 0) {
            logger::debug("loading config from:'{}';", mcmConfig);
            enableSteam = ini.GetBoolValue(INI_SECTION_GENERAL, std::string("b").append(INI_OPTENABLESTEAM).c_str(), enableSteam);
            useSteamModal = ini.GetBoolValue(INI_SECTION_GENERAL, std::string("b").append(INI_OPTUSESTEAMMODAL).c_str(), useSteamModal);
            keyCode = ini.GetLongValue(INI_SECTION_GENERAL, std::string("i").append(INI_OPTKEYCODE).c_str(), keyCode);
            modCode = ini.GetLongValue(INI_SECTION_GENERAL, std::string("i").append(INI_OPTMODCODE).c_str(), modCode);
        }

        logger::info("config enableSteam:'{}'; useSteamModal:'{}'; keyCode:'{}'; modCode:'{}';", enableSteam, useSteamModal, keyCode,
                     modCode);

        try {
            logger::debug("calling onConfigLoad event handler");
            configLoadFunc();
        } catch (const std::exception& e) {
            logger::error("Exception calling onConfigLoad handler: '{}'", e.what());
        }
    }

    const std::multimap<std::string, std::string>& Config::getMapUrlWorld() {
        return worldUrl;
    }

    const std::string Config::getUrlInterior() {
        return urlInterior;
    }

    const std::string Config::getUrlSelected() {
        return urlSelected;
    }

    const std::string Config::getUrlQuest() {
        return urlQuest;
    }

    bool Config::isEnableSteam() {
        return enableSteam;
    }

    void Config::setEnableSteam(bool b) {
        enableSteam = b;
    }

    bool Config::isUseSteamModal() {
        return useSteamModal;
    }

    void Config::setUseSteamModal(bool b) {
        useSteamModal = b;
    }

    int Config::getKeyCode() {
        return keyCode;
    }

    void Config::setKeyCode(int newCode) {
        keyCode = newCode;
    }

    int Config::getModCode() {
        return modCode;
    }

    void Config::setModCode(int newMod) {
        modCode = newMod;
    }

    [[nodiscard]] bool Config::isDebug() const {
        return debug;
    }

    int Config::isUpdateBottomBar() {
        return updateBottomBar;
    }

    void Config::setConfigLoadHandler(std::function<void(void)> func) {
        configLoadFunc = func;
        configLoadFunc();
    }
}  // namespace wmh
