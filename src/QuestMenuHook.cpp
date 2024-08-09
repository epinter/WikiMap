#include "QuestMenuHook.h"
#include "SelectedItem.h"
#include "Config.h"

namespace wmh {
    void QuestMenuHook::installJournalMenuHook(RE::GFxMovieView* movieView) {
        SelectedItem::getInstance().reset();

        RE::GFxValue journalMenu{};
        movieView->GetVariable(&journalMenu, "_global.Quest_Journal.prototype");
        if (!journalMenu.IsObject()) {
            return;
        }

        RE::GFxValue onTabClickOrig{};
        journalMenu.GetMember("onTabClick", &onTabClickOrig);
        if (onTabClickOrig.IsObject()) {
            RE::GFxValue onTabClickNew{};
            RE::GPtr<QuestOnTabClick> funcOnTabClick = RE::make_gptr<QuestMenuHook::QuestOnTabClick>(onTabClickOrig);
            movieView->CreateFunction(&onTabClickNew, funcOnTabClick.get());
            journalMenu.SetMember("onTabClick", onTabClickNew);
        }
    }

    void QuestMenuHook::installQuestMenuHook(RE::GFxMovieView* movieView) {
        SelectedItem::getInstance().reset();

        RE::GFxValue questsMenu{};
        movieView->GetVariable(&questsMenu, "_global.QuestsPage.prototype");
        if (!questsMenu.IsObject()) {
            return;
        }

        //on mouse over quest
        RE::GFxValue onQuestHighlightOrig{};
        questsMenu.GetMember("onTitleListMouseSelectionChange", &onQuestHighlightOrig);
        if (onQuestHighlightOrig.IsObject()) {
            RE::GFxValue onQuestHighlightNew{};
            RE::GPtr<QuestOnSelectedId> funcOnQuestHighlight = RE::make_gptr<QuestMenuHook::QuestOnSelectedId>(onQuestHighlightOrig);
            movieView->CreateFunction(&onQuestHighlightNew, funcOnQuestHighlight.get());
            questsMenu.SetMember("onTitleListMouseSelectionChange", onQuestHighlightNew);
        }

        //on keyboard
        RE::GFxValue onTitleMoveDownOrig{};
        questsMenu.GetMember("onTitleListMoveDown", &onTitleMoveDownOrig);
        if (onTitleMoveDownOrig.IsObject()) {
            RE::GFxValue onTitleMoveDownNew{};
            RE::GPtr<QuestOnSelectedId> funcOnTitleMoveDownNew = RE::make_gptr<QuestMenuHook::QuestOnSelectedId>(onTitleMoveDownOrig);
            movieView->CreateFunction(&onTitleMoveDownNew, funcOnTitleMoveDownNew.get());
            questsMenu.SetMember("onTitleListMoveDown", onTitleMoveDownNew);
        }

        //on keyboard
        RE::GFxValue onTitleMoveUpOrig{};
        questsMenu.GetMember("onTitleListMoveUp", &onTitleMoveUpOrig);
        if (onTitleMoveUpOrig.IsObject()) {
            RE::GFxValue onTitleMoveUpNew{};
            RE::GPtr<QuestOnSelectedId> funcOnTitleMoveUpNew = RE::make_gptr<QuestMenuHook::QuestOnSelectedId>(onTitleMoveUpOrig);
            movieView->CreateFunction(&onTitleMoveUpNew, funcOnTitleMoveUpNew.get());
            questsMenu.SetMember("onTitleListMoveUp", onTitleMoveUpNew);
        }

        if (Config::get().isUpdateBottomBar()) {
            //set wiki button during tab creation
            RE::GFxValue startPageOrig{};
            questsMenu.GetMember("startPage", &startPageOrig);
            if (startPageOrig.IsObject()) {
                RE::GFxValue startPageNew{};
                RE::GPtr<QuestStartPage> funcOnStartPage = RE::make_gptr<QuestMenuHook::QuestStartPage>(startPageOrig);
                movieView->CreateFunction(&startPageNew, funcOnStartPage.get());
                questsMenu.SetMember("startPage", startPageNew);
            }
        }
    }

    void QuestMenuHook::QuestOnTabClick::Call([[maybe_unused]] Params& params) {
        if (origFunction.IsObject()) {
            logger::trace("calling QuestOnTabClick::origFunction function");
            origFunction.Invoke("call", params.retVal, params.argsWithThisRef, params.argCount + 1);
        }

        SelectedItem::getInstance().setQuestNone();
    }

    void QuestMenuHook::QuestStartPage::Call([[maybe_unused]] Params& params) {
        if (origFunction.IsObject()) {
            logger::trace("calling QuestStartPage::origFunction function");
            origFunction.Invoke("call", params.retVal, params.argsWithThisRef, params.argCount + 1);
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

        // the third button of QuestsPages bottomBar is unused on PC
        RE::GFxValue updateButtonArg(RE::GFxValue::ValueType::kBoolean);
        updateButtonArg.SetBoolean(true);
        RE::GFxValue third{};
        RE::GFxValue labelThird{};
        buttons.GetElement(2, &third);
        RE::GFxValue control{};
        if (third.IsObject() && third.GetMember("label", &labelThird) && third.GetMember("_keyCodes", &control) &&
            labelThird.GetString() == ""sv) {
            third.SetMember("_visible", false);

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

            third.SetMember("label", "$Wiki");
            buttonPanel.Invoke("updateButtons", &updateButtonArg);
        }
    }

    void QuestMenuHook::QuestOnSelectedId::Call([[maybe_unused]] Params& params) {
        if (origFunction.IsObject()) {
            logger::trace("calling QuestOnSelectedId::origFunction function");
            origFunction.Invoke("call", params.retVal, params.argsWithThisRef, params.argCount + 1);
        }

        if (params.argCount == 0 || !params.args[0].IsObject()) {
            logger::info("QuestOnSelectedId: invalid argument");
            SelectedItem::getInstance().setQuestNone();
            return;
        }

        RE::GFxValue event = params.args[0];
        RE::GFxValue eventIndex{};

        bool isQuestHighLight = false;
        bool isValidQuest = false;
        if (event.GetMember("index", &eventIndex) && eventIndex.IsNumber() && eventIndex.GetNumber() >= 0) {
            logger::trace("eventIndex: {}", eventIndex.GetNumber());
            isQuestHighLight = true;
        } else {
            SelectedItem::getInstance().setQuestNone();
        }

        RE::GFxValue titleList{};
        params.thisPtr->GetMember("TitleList", &titleList);
        if (!titleList.IsObject()) {
            logger::error("titleList not found");
            return;
        }

        RE::GFxValue selectedEntry{};
        titleList.GetMember("selectedEntry", &selectedEntry);
        if (!selectedEntry.IsObject()) {
            logger::error("selectedEntry not found");
            return;
        }

        RE::GFxValue questName{};
        selectedEntry.GetMember("text", &questName);
        if (!questName.IsString()) {
            logger::error("formId not found");
            return;
        }

        RE::GFxValue formId{};
        selectedEntry.GetMember("formID", &formId);
        if (!formId.IsObject() && !formId.IsNumber()) {
            logger::error("formId not found");
            return;
        }

        if (formId.GetSInt() >= 0) {
            SelectedItem::getInstance().setQuest(questName.GetString(), static_cast<uint32_t>(formId.GetSInt()));
        }

        if (formId.GetSInt() > 0) {
            isValidQuest = true;
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

        for (uint32_t i = 0; i < buttons.GetArraySize(); i++) {
            RE::GFxValue btn{};
            RE::GFxValue label{};
            buttons.GetElement(i, &btn);
            if (btn.IsObject() && btn.GetMember("label", &label)) {
                if (label.GetString() == "$Toggle Active"sv || label.GetString() == "$Show on Map"sv) {
                    btn.SetMember("_visible", true);
                } else if (label.GetString() == "$Wiki"sv) {
                    btn.SetMember("_visible", isValidQuest);
                }
            }
        }

        RE::GFxValue updateButtonArg(RE::GFxValue::ValueType::kBoolean);
        updateButtonArg.SetBoolean(true);
        buttonPanel.Invoke("updateButtons", &updateButtonArg);
    }

}  // namespace wmh
