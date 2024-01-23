#include "GameEventHandler.h"

#include "Config.h"
#include "MCM.h"
#include "Hooks.h"

namespace wmh {
    void GameEventHandler::onLoad() {
        if (Config::get().isDebug()) {
            spdlog::set_level(spdlog::level::debug);
            spdlog::flush_on(spdlog::level::debug);
            spdlog::set_pattern(PLUGIN_LOGPATTERN_DEBUG);
        }

        logger::trace("registering papyrus callback");
        SKSE::GetPapyrusInterface()->Register(MCM::registerFunctions);

        Config::get().setConfigLoadHandler([this]() {
            logger::debug("setting inputhandler key-codes: modKey:'{}'; key:'{}';", Config::get().getModCode(), Config::get().getKeyCode());
            inputListener.setKey(Config::get().getKeyCode());
            inputListener.setModifier(Config::get().getModCode());
        });
    }

    void GameEventHandler::onInputLoaded() {
        logger::debug("configuring input handler: modKey:'{}'; key:'{}';", Config::get().getModCode(), Config::get().getKeyCode());
        inputListener.setHandler(urlProvider.inputHandler(), Config::get().getModCode(), Config::get().getKeyCode());

        Hooks::installMovieHook();
    }

    void GameEventHandler::onDataLoaded() {}

    void GameEventHandler::onNewGame() {
        inputListener.setEnabled(true);
    }

    void GameEventHandler::onPreLoadGame() {
        inputListener.setEnabled(false);
    }

    void GameEventHandler::onPostLoadGame() {
        inputListener.setEnabled(true);
    }
}  // namespace wmh