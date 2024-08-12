#include "Hooks.h"
#include "MapMenuHook.h"
#include "QuestMenuHook.h"

namespace wmh {
    void Hooks::setViewScaleModeHook(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode) {
        setViewScaleMode(movieView, scaleMode);

        RE::GFxValue mapMenu{};
        if (movieView->GetVariable(&mapMenu, "_global.Map.MapMenu.prototype") && mapMenu.IsObject()) {
            MapMenuHook::installMapMenuHook(movieView);
        }

        RE::GFxValue questsMenu{};
        if (movieView->GetVariable(&questsMenu, "_global.QuestsPage.prototype") && questsMenu.IsObject()) {
            QuestMenuHook::installQuestMenuHook(movieView);
        }

        RE::GFxValue journalMenu{};
        if (movieView->GetVariable(&journalMenu, "_global.Quest_Journal.prototype") && journalMenu.IsObject()) {
            QuestMenuHook::installJournalMenuHook(movieView);
        }
    }

    void Hooks::installMovieHook() {
        REL::RelocationID hookFuncAddr = REL::RelocationID(ADDR_LOADMOVIE_SE, ADDR_LOADMOVIE_AE, ADDR_LOADMOVIE_VR);
        int hookFuncOffset = REL::Relocate(OFFSET_LOADMOVIE_SE, OFFSET_LOADMOVIE_AE, OFFSET_LOADMOVIE_VR);
        std::uintptr_t hook = hookFuncAddr.address() + hookFuncOffset;
        if (REL::make_pattern<PATTERN_CHECK_LOADMOVIE>().match(hook)) {
            auto& trampoline = SKSE::GetTrampoline();
            trampoline.create(64);
            setViewScaleMode = *reinterpret_cast<std::uintptr_t*>(trampoline.write_call<6>(hook, setViewScaleModeHook));
            logger::debug("Hook installed at address 0x{:X} (id {})", hookFuncAddr.offset(), hookFuncAddr.id());
        } else {
            logger::error("Hook NOT installed, address 0x{:X} (id {}, offset 0x{:X}) doesn't match.", hookFuncAddr.offset(),
                          hookFuncAddr.id(), hookFuncOffset);
        }
    }

}  // namespace wmh