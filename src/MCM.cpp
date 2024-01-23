#include "MCM.h"

#include "Config.h"

namespace wmh {
    bool MCM::registerFunctions(RE::BSScript::IVirtualMachine* vm) {
        logger::trace("registering OnConfigClose function");
        vm->RegisterFunction("OnConfigClose", "WikiMapIntegration_MCM", MCM::onConfigClose);
        return true;
    }

    void MCM::onConfigClose([[maybe_unused]] RE::TESQuest* quest) {
        Config::get().readConfig();
    }
}  // namespace wmh
