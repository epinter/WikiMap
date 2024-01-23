#pragma once

#include <SimpleIni.h>

#include <boost/algorithm/string.hpp>

namespace wmh {
    class Config {
        private:
            Config();
            Config(Config&) = delete;
            Config& operator=(Config&&) = delete;
            void operator=(Config&) = delete;

            bool enableSteam = true;
            bool useSteamModal = false;
            bool updateBottomBar = true;
            std::string urlInterior = "https://en.uesp.net/w/index.php?title=Special:Search&search=Skyrim:%L";
            std::string urlSelected = "https://en.uesp.net/w/index.php?title=Special:Search&search=Skyrim:%L";
            std::multimap<std::string, std::string> worldUrl{};
            std::string urlQuest = "https://en.uesp.net/w/index.php?title=Special:Search&search=Skyrim:%Q";
            int keyCode = 53;
            int modCode = 0;
            std::function<void(void)> configLoadFunc = []() {};

            bool debug = false;

        public:
            [[nodiscard]] static Config& get() {
                static Config instance;
                return instance;
            }

            void readConfig();

            [[nodiscard]] const std::multimap<std::string, std::string>& getMapUrlWorld();
            [[nodiscard]] const std::string getUrlInterior();
            [[nodiscard]] const std::string getUrlSelected();
            [[nodiscard]] const std::string getUrlQuest();

            [[nodiscard]] bool isEnableSteam();
            void setEnableSteam(bool b);

            [[nodiscard]] bool isUseSteamModal();
            void setUseSteamModal(bool b);

            [[nodiscard]] int getKeyCode();
            void setKeyCode(int newCode);

            [[nodiscard]] int getModCode();
            void setModCode(int newMod);

            [[nodiscard]] int isUpdateBottomBar();

            [[nodiscard]] bool isDebug() const;
            void setConfigLoadHandler(std::function<void(void)> func);
    };
}  // namespace wmh

namespace constants {
    static constexpr const char* CONFIG_FILE = "WikiMap.ini";
    static constexpr const char* MCM_CONFIG_FILE = "WikiMapIntegration.ini";
    static constexpr const char* INI_SECTION_GENERAL = "General";
    static constexpr const char* INI_SECTION_USER = "User";
    static constexpr const char* INI_SECTION_WORLD = "Worldspaces";
    static constexpr const char* INI_OPT_DEBUG = "Debug";
    static constexpr const char* INI_OPTENABLESTEAM = "EnableSteam";
    static constexpr const char* INI_OPTUSESTEAMMODAL = "UseSteamModal";
    static constexpr const char* INI_OPTSELECTEDURL = "UrlSelected";
    static constexpr const char* INI_OPTINTERIORURL = "UrlInterior";
    static constexpr const char* INI_OPTQUESTURL = "UrlQuest";
    static constexpr const char* INI_OPTPREFIX_WORLD = "World_";
    static constexpr const char* INI_OPTKEYCODE = "KeyCode";
    static constexpr const char* INI_OPTMODCODE = "ModifierKeyCode";
    static constexpr const char* INI_OPTBOTTOMBAR = "UpdateBottomBar";
    static constexpr const char* INI_DEF_URL_TAMRIEL = "https://gamemap.uesp.net/sr?world=skyrim&zoom=17&x=%X&y=%Y";
    static constexpr const char* INI_DEF_URL_SOLSTHEIM = "https://gamemap.uesp.net/sr/?world=solstheim&zoom=17&x=%X&y=%Y";
    static constexpr const char* INI_COMMENT_OPTBOTTOMBAR =
        ";; Set to false to disable the 'Wiki' button on Map and Quests menus. Useful if an incompatible menu is installed.\n"
        ";; Keyboard shortcut will still work";
    static constexpr const char* INI_COMMENT_OPTKEYCODE =
        ";; KeyCode\n"
        ";; Slash = 53\n"
        ";; Period = 52\n"
        ";; Comma = 51\n"
        ";; https://learn.microsoft.com/en-us/previous-versions/windows/desktop/bb321074(v=vs.85)\n"
        ";; This option is overwritten by MCM settings, configure in-game if you have MCM Helper installed";
    static constexpr const char* INI_COMMENT_OPTMODCODE =
        ";; Modifier key (CTRL,ALT,SHIFT)\n"
        ";; Set to 0 if you don't want to use a modifier key\n"
        ";; RightAlt = 184\n"
        ";; LeftAlt = 56\n"
        ";; RightControl = 157\n"
        ";; LeftControl = 29\n"
        ";; LeftShift = 42\n"
        ";; RightShift = 54\n"
        ";; This option is overwritten by MCM settings, configure in-game if you have MCM Helper installed";
    static constexpr const char* INI_COMMENT_OPTENABLESTEAM =
        ";; Use Steam overlay to open the web page. When false, will open the default browser over the game.\n"
        ";; This option is overwritten by MCM settings, configure in-game if you have MCM Helper installed";
    static constexpr const char* INI_COMMENT_OPTUSESTEAMMODAL =
        ";; When true, opens the web page on a dedicated Steam browser window without the other overlay options.\n"
        ";; Steam has a bug and don't close the browser tab properly sometimes. Disable if you have issues on Steam overlay.";
    static constexpr const char* INI_COMMENT_OPTSELECTEDURL =
        ";; Url template for selected locations (using mouse cursor and hotkey).\n"
        ";; %L is replaced by the location name, like \"Dead Man's Drink\"\n"
        ";; The player location name is URL-encoded when replaced.\n"
        ";UrlSelected=\"https://en.uesp.net/w/index.php?title=Special:Search&search=Skyrim:%L\"\n"
        ";UrlSelected=\"https://en.uesp.net/wiki/Skyrim:%L\"\n"
        ";UrlSelected=\"https://elderscrolls.fandom.com/wiki/Special:Search?query=%L\"";
    static constexpr const char* INI_COMMENT_OPTINTERIORURL =
        ";; Url template for interiors (CURRENT PLAYER LOCATION).\n"
        ";; %L is replaced by the location name, like \"Dead Man's Drink\"\n"
        ";; %I is replaced by the interior location editorid, like \"FalkreathDeadMansDrink\"\n"
        ";; Current Player Position: %X is replaced by the x coordinate, %Y by the y coordinate, and %Z by z coordinate.\n"
        ";; The player location name is URL-encoded when replaced.\n"
        ";UrlInterior=\"https://en.uesp.net/w/index.php?title=Special:Search&search=Skyrim:%L\"\n"
        ";UrlInterior=\"https://en.uesp.net/wiki/Skyrim:%L\"\n"
        ";UrlInterior=\"https://elderscrolls.fandom.com/wiki/Special:Search?query=%L\"\n"
        ";UrlInterior=\"https://elderscrolls.fandom.com/wiki/%L\"";
    static constexpr const char* INI_COMMENT_OPTQUESTURL =
        ";; Url template for quests.\n"
        ";; %Q is replaced by the quest name, like \"Before the Storm\"\n"
        ";; %I is replaced by the quest editorid, like \"MQ102\"\n"
        ";; The quest name is URL-encoded when replaced.\n"
        ";UrlQuest=\"https://en.uesp.net/w/index.php?title=Special:Search&search=Skyrim:%Q\"\n"
        ";UrlQuest=\"https://en.uesp.net/wiki/Skyrim:%Q\"\n"
        ";UrlQuest=\"https://en.uesp.net/w/index.php?title=Special:Search&search=Skyrim:%I\"\n"
        ";UrlQuest=\"https://en.uesp.net/wiki/Skyrim:%I\"\n"
        ";UrlQuest=\"https://elderscrolls.fandom.com/wiki/Special:Search?query=%I\"\n"
        ";UrlQuest=\"https://elderscrolls.fandom.com/wiki/%Q\"";
    static constexpr const char* INI_COMMENT_WORLD =
        ";; Url templates for exteriors (CURRENT PLAYER LOCATION)\n"
        ";; Each option has the format \"World_\" with the Worldspace EditorID appended.\n"
        ";; The key for Skyrim worldspace is \"World_Tamriel\". For Solstheim, is \"World_DLC2SolstheimWorld\".\n"
        ";; %L is replaced by the location name, like \"Dead Man's Drink\"\n"
        ";; %W is replaced by the worldspace name, like \"Skyrim\" and \"Solstheim\"\n"
        ";; Current Player Position: %X is replaced by the x coordinate, %Y by the y coordinate, and %Z by z coordinate.\n"
        ";; The player location name and worldspace name are URL-encoded when replaced.";
}  // namespace constants