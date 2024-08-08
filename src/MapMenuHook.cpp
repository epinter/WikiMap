#include "MapMenuHook.h"
#include "SelectedItem.h"
#include "Config.h"

namespace wmh {
    void MapMenuHook::installMapMenuHook(RE::GFxMovieView* movieView) {
        SelectedItem::getInstance().reset();

        RE::GFxValue mapMenu{};
        movieView->GetVariable(&mapMenu, "_global.Map.MapMenu.prototype");
        if (mapMenu.IsObject()) {
            RE::GFxValue setSelectedMarkerOrig{};
            mapMenu.GetMember("SetSelectedMarker", &setSelectedMarkerOrig);
            if (setSelectedMarkerOrig.IsObject()) {
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
        params.thisPtr->GetMember("_bottomBar", &bottomBar);
        if (!bottomBar.IsObject()) {
            logger::error("bottomBar NOT found");
            return;
        }

        RE::GFxValue buttonPanel{};
        bottomBar.GetMember("buttonPanel", &buttonPanel);
        if (!buttonPanel.IsDisplayObject()) {
            logger::error("buttonPanel NOT found");
            return;
        }

        RE::GFxValue buttons{};
        buttonPanel.GetMember("buttons", &buttons);
        if (!buttons.IsArray()) {
            logger::error("buttons NOT found");
            return;
        }

        bool locationFinderShown = false;
        RE::GFxValue locationFinder{};
        params.thisPtr->GetMember("_locationFinder", &locationFinder);
        if (locationFinder.IsObject()) {
            RE::GFxValue finderShown{};
            locationFinder.GetMember("_bShown", &finderShown);
            if (!finderShown.IsBool()) {
                logger::error("locationFinder NOT found");
                return;
            }
            locationFinderShown = finderShown.GetBool();
        }

        for (uint32_t i = 0; i < buttons.GetArraySize(); i++) {
            RE::GFxValue btn{};
            buttons.GetElement(i, &btn);
            if (btn.IsDisplayObject()) {
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
                        btn.GetMember("_visible", &btnVis);
                        if (!originalSet) {
                            originalSet = true;
                            originalLabel = label.GetString();
                            if (btnVis.IsBool()) {
                                origVisible = btnVis.GetBool();
                            }
                            RE::GFxValue origCode{};
                            control.GetElement(0, &origCode);
                            originalCode = origCode.GetUInt();
                        }
                        int k = 0;
                        if (Config::get().getModCode() > 0) {
                            control.SetArraySize(2);
                            control.SetElement(k++, Config::get().getModCode());
                        }
                        control.SetElement(k, Config::get().getKeyCode());
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
