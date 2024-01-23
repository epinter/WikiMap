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
            }
        }
    }

    void MapMenuHook::MapSetSelectedMarker::Call([[maybe_unused]] Params& params) {
        if (origFunction.IsObject()) {
            logger::trace("calling MapSetSelectedMarker::origFunction function");
            origFunction.Invoke("call", params.retVal, params.argsWithThisRef, params.argCount + 1);
        }

        bool unsetMarker = false;
        if (params.argCount == 0 || !params.args[0].IsNumber() || params.args[0].GetNumber() < 0) {
            unsetMarker = true;
        }

        int localMap = -1;
        RE::GFxValue localMapMenu{};
        params.thisPtr->GetMember("LocalMapMenu", &localMapMenu);
        if (localMapMenu.IsObject()) {
            RE::GFxValue localMapState{};
            localMapMenu.GetMember("_state", &localMapState);  //0 = hidden, 1 = localMap, 2=locationFinder
            if (localMapState.IsNumber()) {
                localMap = static_cast<int>(localMapState.GetNumber());
            } else {
                logger::error("LocalMap._state not found");
            }
            if (isLocalMapOpen && localMap == 1) {
                //stop if local map is open and current is world map
                return;
            } else if (localMap == 0) {
                isLocalMapOpen = false;
            }
        } else if (params.thisPtr->HasMember("LocalMapMenu")) {
            //LocalMapMenu member has no value when local map is open
            localMap = 1;
            isLocalMapOpen = true;
        }

        if (unsetMarker && localMap != 1) {
            //reset SelectedItem when we are in MapMenu and LocalMap is not open
            logger::trace("resetting SelectedItem: unsetMarker:'{}'; localMap:'{}'", unsetMarker, localMap);
            SelectedItem::getInstance().setMapNone();
        }

        if (!unsetMarker) {
            int markerIdx = static_cast<int>(params.args[0].GetNumber());
            RE::GFxValue markerList{};
            params.thisPtr->GetMember("_markerList", &markerList);
            if (!markerList.IsArray()) {
                logger::trace("markerList not found, idx:{};", markerIdx);
                return;
            }

            RE::GFxValue marker{};
            markerList.GetElement(markerIdx, &marker);
            if (!marker.IsObject()) {
                logger::trace("marker not found: idx:{};", markerIdx);
                return;
            }

            RE::GFxValue markerName{};
            marker.GetMember("label", &markerName);

            RE::GFxValue createIconType{};
            params.movie->GetVariable(&createIconType, "_global.Map.MapMenu.CREATE_ICONTYPE");
            if (!createIconType.IsNumber()) {
                logger::error("CREATE_ICONTYPE not found");
                return;
            }

            RE::GFxValue markerType{};
            marker.GetMember("markerType", &markerType);
            if (!markerType.IsNumber()) {
                logger::error("markerType not found");
                return;
            }

            if (markerType.GetNumber() > 64 && markerType.GetNumber() <= 67) {
                logger::trace("marker type = {}, ignoring", markerType.GetNumber());
                SelectedItem::getInstance().setMapNone();
                return;
            }

            logger::trace("markerIdx:{}; markerName:{}", markerIdx, markerName.GetString());
            SelectedItem::getInstance().setMap(std::string{markerName.GetString()}, markerIdx, static_cast<int>(markerType.GetNumber()));

            if (localMap == 0 || localMap == 1) {
                RE::GPtr<RE::MapMenu> uiMapMenu = RE::UI::GetSingleton()->GetMenu<RE::MapMenu>(RE::MapMenu::MENU_NAME);
                RE::BSTArray<RE::MapMenuMarker> markers = localMap == 0 ? uiMapMenu->mapMarkers : uiMapMenu->localMapMenu.mapMarkers;

                if (markers.size() > 0 && markerIdx >= 0 && markerIdx < static_cast<int>(markers.size())) {
                    RE::TESForm* questForm = markers[markerIdx].form;
                    RE::TESFullName* fn = markers[markerIdx].fullName;
                    if (questForm && fn && fn->GetFullName() && !std::string{fn->GetFullName()}.contains("<")) {
                        SelectedItem::getInstance().setMapQuest(fn->GetFullName(), questForm->GetFormID());
                    } else if (questForm && markers[markerIdx].customMarker && strlen(markers[markerIdx].customMarker) > 0) {
                        SelectedItem::getInstance().setMapQuest(markers[markerIdx].customMarker, questForm->GetFormID());
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
