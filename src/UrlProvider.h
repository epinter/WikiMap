#pragma once
#include "PlayerLocation.h"

namespace wmh {
    class UrlProvider {
        private:
            UrlProvider(UrlProvider&) = delete;
            UrlProvider& operator=(UrlProvider&&) = delete;
            void operator=(UrlProvider&) = delete;

            PlayerLocation playerLocation{};
            [[nodiscard]] std::string buildInteriorUrl(std::string templateLocation);
            [[nodiscard]] std::string buildExteriorUrl(std::string templateWorld);
            [[nodiscard]] const std::string buildLocationUrl();
            [[nodiscard]] const std::string buildLocationUrl(std::string locationName);
            [[nodiscard]] const std::string buildQuestUrl(std::string name, std::string editorId);
            [[nodiscard]] std::string validateUrl(std::string url);

        public:
            UrlProvider() = default;
            std::function<void(void)> inputHandler();
    };
}  // namespace wmh