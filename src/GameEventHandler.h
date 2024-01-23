#pragma once
#include "SkseMessagingListener.h"
#include "InputListener.h"
#include "UrlProvider.h"

namespace wmh {
    class GameEventHandler : public SkseMessagingListener<GameEventHandler> {
        private:
            GameEventHandler() {
                registerListener();
            };
            GameEventHandler(GameEventHandler&) = delete;
            GameEventHandler& operator=(GameEventHandler&&) = delete;
            void operator=(GameEventHandler&) = delete;
            UrlProvider urlProvider{};
            InputListener inputListener{};

        public:
            [[nodiscard]] static GameEventHandler& getInstance() {
                static GameEventHandler instance;
                return instance;
            }

            void onLoad() override;
            void onInputLoaded() override;
            void onDataLoaded() override;
            void onNewGame() override;
            void onPreLoadGame() override;
            void onPostLoadGame() override;
    };
}  // namespace wmh
