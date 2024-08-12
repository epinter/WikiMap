#include "MapMenuHook.h"
#include "SelectedItem.h"
#include "Config.h"

namespace wmh {
    void MapMenuHook::installMapMenuHook(RE::GFxMovieView* movieView) {
        SelectedItem::getInstance().reset();

        RE::GFxValue mapMenu{};
        if (movieView->GetVariable(&mapMenu, "_global.Map.MapMenu.prototype") && mapMenu.IsObject()) {
            RE::GFxValue setSelectedMarkerOrig{};
            if (mapMenu.GetMember("SetSelectedMarker", &setSelectedMarkerOrig) && setSelectedMarkerOrig.IsObject()) {
                RE::GFxValue setSelectedMarkerNew{};
                RE::GPtr<MapSetSelectedMarker> funcSetSelectedMarker =
                    RE::make_gptr<MapMenuHook::MapSetSelectedMarker>(setSelectedMarkerOrig);
                movieView->CreateFunction(&setSelectedMarkerNew, funcSetSelectedMarker.get());
                mapMenu.SetMember("SetSelectedMarker", setSelectedMarkerNew);
            } else {
                logger::error("SetSelectedMarker not found");
            }
        } else {
            logger::error("_global.Map.MapMenu.prototype not found");
        }
    }

    void MapMenuHook::MapSetSelectedMarker::Call([[maybe_unused]] Params& params) {
        if (origFunction.IsObject()) {
            origFunction.Invoke("call", params.retVal, params.argsWithThisRef, params.argCount + 1);
        }

        if (params.argCount == 0 || !params.args[0].IsNumber()) {
            return;
        }

        int markerIdx = static_cast<int>(params.args[0].GetNumber());
        bool unsetMarker = false;
        if (markerIdx < 0) {
            unsetMarker = true;
        }

        RE::GPtr<RE::MapMenu> uiMapMenu = RE::UI::GetSingleton()->GetMenu<RE::MapMenu>(RE::MapMenu::MENU_NAME);
        bool localMap = uiMapMenu->localMapMenu.GetRuntimeData().showingMap;

        if (localMap && markerIdx != uiMapMenu->localMapMenu.GetRuntimeData().selectedMarker &&
            markerIdx == uiMapMenu->GetRuntimeData2().selectedMarker) {
            //local map is open and the called SetSelectedMarker received another idx, so it is the worldmap
            return;
        }

        if (unsetMarker) {
            SelectedItem::getInstance().setMapNone();
        } else {
            RE::BSTArray<RE::MapMenuMarker> markers = localMap ? uiMapMenu->localMapMenu.mapMarkers : uiMapMenu->mapMarkers;
            if (markers.size() > 0 && markerIdx >= 0 && markerIdx < static_cast<int>(markers.size())) {
                std::string markerName{};
                RE::TESFullName* fn = markers[markerIdx].fullName;
                const char* cm = markers[markerIdx].customMarker;

                if (fn && fn->GetFullName() && !std::string{fn->GetFullName()}.contains("<")) {
                    markerName = fn->GetFullName();
                } else if (cm && strlen(cm) > 0) {
                    markerName = cm;
                }

                if (markers[markerIdx].type == 3 || markers[markerIdx].type == 2) {
                    //2 = Custom Destination, 3 = Current Location
                    SelectedItem::getInstance().reset();
                    return;
                }

                logger::trace("markerIdx:{}; markerName:{}; type:{};", markerIdx, markerName, markers[markerIdx].type);
                SelectedItem::getInstance().setMap(markerName, markerIdx, markers[markerIdx].type);

                RE::TESForm* qf = markers[markerIdx].form;
                if (qf) {
                    if (fn && fn->GetFullName() && !std::string{fn->GetFullName()}.contains("<")) {
                        SelectedItem::getInstance().setMapQuest(fn->GetFullName(), qf->GetFormID());
                    } else if (cm && strlen(cm) > 0) {
                        SelectedItem::getInstance().setMapQuest(cm, qf->GetFormID());
                    }
                }
            }
        }

        if (!Config::get().isUpdateBottomBar()) {
            return;
        }

        RE::GFxValue bottomBar{};
        RE::GFxValue buttonPanel{};
        RE::GFxValue buttons{};
        RE::GFxValue locationFinder{};

        if (!(params.thisPtr->GetMember("_bottomBar", &bottomBar) && bottomBar.IsObject())) {
            logger::error("bottomBar NOT found");
            return;
        }

        if (!(bottomBar.GetMember("buttonPanel", &buttonPanel) && buttonPanel.IsDisplayObject())) {
            logger::error("buttonPanel NOT found");
            return;
        }

        if (!(buttonPanel.GetMember("buttons", &buttons) && buttons.IsArray())) {
            logger::error("buttons NOT found");
            return;
        }

        if (!buttonPanel.HasMember("updateButtons")) {
            logger::error("updateButtons NOT found");
            return;
        }

        bool locationFinderShown = false;
        if (params.thisPtr->GetMember("_locationFinder", &locationFinder) && locationFinder.IsObject()) {
            RE::GFxValue finderShown{};
            if (!(locationFinder.GetMember("_bShown", &finderShown) && finderShown.IsBool())) {
                logger::error("locationFinder NOT found");
                return;
            }
            locationFinderShown = finderShown.GetBool();
        }

        for (uint32_t i = 0; i < buttons.GetArraySize(); i++) {
            RE::GFxValue btn{};
            if (buttons.GetElement(i, &btn) && btn.IsDisplayObject()) {
                RE::GFxValue label{};
                if (!btn.GetMember("label", &label)) {
                    continue;
                }
                if (label.IsString() && (label.GetString() == "$Search"sv || label.GetString() == "$Wiki"sv)) {
                    RE::GFxValue control{};
                    if (!btn.GetMember("_keyCodes", &control)) {
                        continue;
                    }
                    if ((unsetMarker || locationFinderShown) && originalSet) {
                        control.SetArraySize(1);
                        control.SetElement(0, originalCode);
                        btn.SetMember("_visible", (locationFinderShown || origVisible));
                        btn.SetMember("label", originalLabel.c_str());
                    } else {
                        RE::GFxValue btnVis{};
                        if (!btn.GetMember("_visible", &btnVis)) {
                            continue;
                        }
                        if (!originalSet) {
                            originalSet = true;
                            originalLabel = label.GetString();
                            if (btnVis.IsBool()) {
                                origVisible = btnVis.GetBool();
                            }
                            RE::GFxValue origCode{};
                            if (control.IsArray() && control.GetArraySize() >= 1 && control.GetElement(0, &origCode)) {
                                originalCode = origCode.GetUInt();
                            }
                        }
                        int k = 0;
                        int buttonCode = 0;
                        if (RE::BSInputDeviceManager::GetSingleton()->IsGamepadEnabled()) {
                            buttonCode = Config::get().getGamepadButton();
                        } else {
                            buttonCode = Config::get().getKeyCode();
                            if (Config::get().getModCode() > 0) {
                                control.SetArraySize(2);
                                control.SetElement(k++, Config::get().getModCode());
                            }
                        }
                        control.SetElement(k, buttonCode);
                        if (btnVis.IsBool() && !btnVis.GetBool()) {
                            btn.SetMember("_visible", true);
                        }
                        if (label != "$Wiki") {
                            btn.SetMember("label", "$Wiki");
                        }
                    }
                }
            }
        }

        RE::GFxValue updateButtonArg(RE::GFxValue::ValueType::kBoolean);
        updateButtonArg.SetBoolean(true);
        buttonPanel.Invoke("updateButtons", &updateButtonArg);
    }

}  // namespace wmh
