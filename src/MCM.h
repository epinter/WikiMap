#pragma once

namespace wmh {
    class MCM {
        private:
            static void onConfigClose(RE::TESQuest* quest);

        public:
            static bool registerFunctions(RE::BSScript::IVirtualMachine* vm);
    };
}  // namespace wmh
