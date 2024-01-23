#pragma once

namespace fsteam {
    class SteamApi {
        private:
            static constexpr const char *SKYRIM_640_STEAMFRIENDS_INTERFACE_VERSION = "SteamFriends015";
            static constexpr const char *SKYRIM_640_STEAMUTILS_INTERFACE_VERSION = "SteamUtils007";
            static constexpr const char *LATEST_STEAMFRIENDS_INTERFACE_VERSION = "SteamFriends017";
            static constexpr const char *LATEST_STEAMUTILS_INTERFACE_VERSION = "SteamUtils010";
            static constexpr const char *STEAMAPI_DLL = "steam_api64.dll";

            template <typename T>
            T getPtr(std::string function) {
                return reinterpret_cast<T>(GetProcAddress(GetModuleHandle(STEAMAPI_DLL), function.c_str()));
            }

            using ISteamClient = uintptr_t;
            using ISteamUtils = uintptr_t;
            using ISteamFriends = uintptr_t;
            using HSteamPipe = uint32_t;
            using HSteamUser = uint32_t;
            using FuncSteamClient = std::add_pointer<ISteamClient()>::type;
            using FuncGetHSteamPipe = std::add_pointer<HSteamPipe()>::type;
            using FuncGetHSteamUser = std::add_pointer<HSteamUser()>::type;
            using FuncGetISteamFriends = std::add_pointer<ISteamFriends(ISteamClient, uint32_t, uint32_t, const char *)>::type;
            using FuncGetISteamUtils = std::add_pointer<ISteamUtils(ISteamClient, HSteamPipe, const char *)>::type;
            using FuncActivateGameOverlayToWebPage = std::add_pointer<void(ISteamFriends, const char *, int)>::type;
            using FuncIsOverlayEnabled = std::add_pointer<bool(ISteamUtils)>::type;
            using FuncSetRichPresence = std::add_pointer<bool(ISteamFriends, const char *, const char *)>::type;

            FuncSteamClient SteamAPI_SteamClient = getPtr<FuncSteamClient>("SteamClient");
            FuncGetHSteamPipe SteamAPI_GetHSteamPipe = getPtr<FuncGetHSteamPipe>("GetHSteamPipe");
            FuncGetHSteamUser SteamAPI_GetHSteamUser = getPtr<FuncGetHSteamUser>("GetHSteamUser");
            FuncGetISteamFriends SteamAPI_ISteamClient_GetISteamFriends =
                getPtr<FuncGetISteamFriends>("SteamAPI_ISteamClient_GetISteamFriends");
            FuncGetISteamUtils SteamAPI_ISteamClient_GetISteamUtils = getPtr<FuncGetISteamUtils>("SteamAPI_ISteamClient_GetISteamUtils");
            FuncActivateGameOverlayToWebPage SteamAPI_ISteamFriends_ActivateGameOverlayToWebPage =
                getPtr<FuncActivateGameOverlayToWebPage>("SteamAPI_ISteamFriends_ActivateGameOverlayToWebPage");
            FuncIsOverlayEnabled SteamAPI_ISteamUtils_IsOverlayEnabled =
                getPtr<FuncIsOverlayEnabled>("SteamAPI_ISteamUtils_IsOverlayEnabled");
            FuncSetRichPresence SteamAPI_ISteamFriends_SetRichPresence =
                getPtr<FuncSetRichPresence>("SteamAPI_ISteamFriends_SetRichPresence");

            // when false, use older api version
            bool useLatestApi = true;

        public:
            SteamApi() = default;
            SteamApi(bool b)
                : useLatestApi(b){};

            enum OverlayWebPageMode {  // ignored on SteamFriends015
                OverlayWebPageMode_Default = 0,
                OverlayWebPageMode_Modal = 1  //buggy since steam user interface update 2023, window don't close
            };

            [[maybe_unused]] inline bool isSteamApiLoaded() {
                return (GetModuleHandle(STEAMAPI_DLL) != NULL);
            }

            [[nodiscard]] inline const bool isUseLatestApi() {
                return useLatestApi;
            }

            inline void setUseLatestApi(bool b) {
                useLatestApi = b;
            }

            [[maybe_unused]] inline bool isSteamOverlayEnabled() {
                if (!isSteamApiLoaded()) {
                    return false;
                }
                ISteamClient steamClient = SteamAPI_SteamClient();
                HSteamPipe steamPipe = SteamAPI_GetHSteamPipe();

                if (!steamClient || !steamPipe) {
                    return false;
                }

                ISteamUtils steamUtils = SteamAPI_ISteamClient_GetISteamUtils(steamClient, steamPipe, LATEST_STEAMUTILS_INTERFACE_VERSION);
                if (!steamUtils) {
                    return false;
                }

                return SteamAPI_ISteamUtils_IsOverlayEnabled(steamUtils);
            }

            [[maybe_unused]] inline ISteamFriends getSteamFriends(ISteamClient client, HSteamUser user, HSteamPipe pipe) {
                std::string apiVersion = LATEST_STEAMFRIENDS_INTERFACE_VERSION;

                if (!useLatestApi) {
                    apiVersion = SKYRIM_640_STEAMFRIENDS_INTERFACE_VERSION;
                }

                return SteamAPI_ISteamClient_GetISteamFriends(client, user, pipe, apiVersion.c_str());
            }

            [[maybe_unused]] inline ISteamFriends getSteamFriends() {
                std::string apiVersion = LATEST_STEAMFRIENDS_INTERFACE_VERSION;

                if (!useLatestApi) {
                    apiVersion = SKYRIM_640_STEAMFRIENDS_INTERFACE_VERSION;
                }

                ISteamClient steamClient = SteamAPI_SteamClient();
                HSteamPipe steamPipe = SteamAPI_GetHSteamPipe();
                HSteamUser steamUser = SteamAPI_GetHSteamUser();
                if (!steamClient || !steamPipe || !steamUser) {
                    logger::error("can't get SteamClient instance");
                    return 0;
                }

                return SteamAPI_ISteamClient_GetISteamFriends(steamClient, steamUser, steamPipe, apiVersion.c_str());
            }

            [[maybe_unused]] inline ISteamUtils getSteamUtils(ISteamClient client, HSteamPipe pipe) {
                std::string apiVersion = LATEST_STEAMUTILS_INTERFACE_VERSION;

                if (!useLatestApi) {
                    apiVersion = SKYRIM_640_STEAMUTILS_INTERFACE_VERSION;
                }

                return SteamAPI_ISteamClient_GetISteamUtils(client, pipe, apiVersion.c_str());
            }

            [[maybe_unused]] [[nodiscard]] inline void openUrlOnSteamOverlay(std::string url, bool modal) {
                openUrlOnSteamOverlay(url, modal ? OverlayWebPageMode_Modal : OverlayWebPageMode_Default);
            }

            [[maybe_unused]] [[nodiscard]] inline void openUrlOnSteamOverlay(std::string url,
                                                                             OverlayWebPageMode modal = OverlayWebPageMode_Default) {
                if (url.empty() || !isSteamApiLoaded()) {
                    return;
                }

                ISteamClient steamClient = SteamAPI_SteamClient();
                HSteamPipe steamPipe = SteamAPI_GetHSteamPipe();

                HSteamUser steamUser = SteamAPI_GetHSteamUser();
                if (!steamClient || !steamPipe || !steamUser) {
                    logger::error("can't get SteamClient instance");
                    return;
                }

                ISteamUtils steamUtils = getSteamUtils(steamClient, steamPipe);
                if (!steamUtils || !(SteamAPI_ISteamUtils_IsOverlayEnabled(steamUtils))) {
                    logger::error("can't get ISteamUtils instance");
                    return;
                }

                ISteamFriends steamFriends = getSteamFriends(steamClient, steamUser, steamPipe);
                if (steamFriends) {
                    SteamAPI_ISteamFriends_ActivateGameOverlayToWebPage(steamFriends, url.c_str(), modal);
                } else {
                    logger::error("can't get ISteamFriends instance");
                }
            }
    };

}  // namespace fsteam
