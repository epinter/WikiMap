#include "Hooks.h"
#include "MapMenuHook.h"
#include "QuestMenuHook.h"
#include <detours/detours.h>

namespace wmh {
    void Hooks::setViewScaleModeHook(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode) {
        setViewScaleMode(movieView, scaleMode);
        logger::info("setViewScaleMode called");

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

    void Hooks::testDetours1(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode) {
        origSetViewScaleModeD1(movieView, scaleMode);
        logTestDetours(1);
    }

    void Hooks::testDetours2(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode) {
        origSetViewScaleModeD2(movieView, scaleMode);
        logTestDetours(2);
    }

    void Hooks::testDetours3(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode) {
        origSetViewScaleModeD3(movieView, scaleMode);
        logTestDetours(3);
    }

    void Hooks::testDetours4(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode) {
        origSetViewScaleModeD4(movieView, scaleMode);
        logTestDetours(4);
    }

    void Hooks::logTestDetours([[maybe_unused]] int n) {
        logger::info("test detours {}", n);
    }

    template <typename T>
    bool installDetour(T& origPtr, PVOID detour, REL::RelocationID func, std::string name) {
        origPtr = reinterpret_cast<T>(func.address());
        if (DetourTransactionBegin() == NO_ERROR && DetourUpdateThread(GetCurrentThread()) == NO_ERROR &&
            DetourAttach(&reinterpret_cast<PVOID&>(origPtr), detour) == NO_ERROR && DetourTransactionCommit() == NO_ERROR) {
            logger::info("{} hook installed at address 0x{:08X}", name, func.address());
            return true;
        } else {
            logger::error("Failed to install {} hook", name);
            return false;
        }
    }

    void Hooks::installMovieHook() {
        REL::RelocationID hookFuncAddr = REL::RelocationID(ADDR_LOADMOVIE_SE, ADDR_LOADMOVIE_AE, ADDR_LOADMOVIE_VR);
        REL::RelocationID target = REL::RelocationID(0, 83988);  // target function called by hookFuncAddr + offset

        installDetour<OrigTestDetour>(origSetViewScaleModeD1, testDetours1, target, "SetViewScaleModeD1");
        installDetour<OrigTestDetour>(origSetViewScaleModeD2, testDetours2, target, "SetViewScaleModeD2");

        int hookFuncOffset = REL::Relocate(OFFSET_LOADMOVIE_SE, OFFSET_LOADMOVIE_AE, OFFSET_LOADMOVIE_VR);
        std::uintptr_t hook = hookFuncAddr.address() + hookFuncOffset;
        if (REL::make_pattern<PATTERN_CHECK_LOADMOVIE>().match(hook)) {
            auto& trampoline = SKSE::GetTrampoline();
            trampoline.create(64);
            setViewScaleMode = *reinterpret_cast<std::uintptr_t*>(trampoline.write_call<6>(hook, setViewScaleModeHook));
            logger::info("Hook installed at address 0x{:X} (id {})", hookFuncAddr.offset(), hookFuncAddr.id());
        } else {
            logger::error("Hook NOT installed, address 0x{:X} (id {}, offset 0x{:X}) doesn't match. {:X} {:X}", hookFuncAddr.offset(),
                          hookFuncAddr.id(), hookFuncOffset, (uint8_t) hook, (uint8_t) hook + 1);
        }

        installDetour<OrigTestDetour>(origSetViewScaleModeD3, testDetours3, target, "SetViewScaleModeD3");
        installDetour<OrigTestDetour>(origSetViewScaleModeD4, testDetours4, target, "SetViewScaleModeD4");
    }

}  // namespace wmh