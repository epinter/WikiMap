#include "InputListener.h"

namespace wmh {
    void InputListener::setHandler(std::function<void(void)> function, int newModifier, int newKey, int newButton) {
        if (handlerSet) {
            logger::warn("InputListener already configured!");
            return;
        }
        keyPressEventFunc = function;
        setModifier(newModifier);
        setKey(newKey);
        setGameButton(newButton);
        RE::BSInputDeviceManager::GetSingleton()->AddEventSink(this);
        handlerSet = true;
    }

    bool InputListener::isEnabled() const {
        return enabled;
    }

    void InputListener::setEnabled(bool b) {
        enabled = b;
    }

    void InputListener::setModifier(int newMod) {
        modifier = newMod < 0 ? 0 : newMod;
    }

    void InputListener::setKey(int newKey) {
        key = newKey < 0 ? 0 : newKey;
    }

    void InputListener::setGameButton(int newButton) {
        gameButton = newButton < 0 ? 0 : newButton;
    }

    RE::BSEventNotifyControl InputListener::ProcessEvent(RE::InputEvent* const* evtPtr, RE::BSTEventSource<RE::InputEvent*>*) {
        auto ui = RE::UI::GetSingleton();

        if (!evtPtr || !*evtPtr || !enabled || key <= 0 || !ui) {
            return RE::BSEventNotifyControl::kContinue;
        }
        auto* evt = *evtPtr;

        if ((!ui->IsMenuOpen(RE::MapMenu::MENU_NAME) && !ui->IsMenuOpen(RE::JournalMenu::MENU_NAME)) ||
            ui->IsMenuOpen(RE::Console::MENU_NAME) || !RE::PlayerCharacter::GetSingleton()->Is3DLoaded()) {
            return RE::BSEventNotifyControl::kContinue;
        }

        if (evt->GetEventType() == RE::INPUT_EVENT_TYPE::kButton) {
            const RE::ButtonEvent* bEvent = evt->AsButtonEvent();
            int code = bEvent->GetIDCode();
            int gamepadCode = SKSE::InputMap::GamepadMaskToKeycode(code);

            bool gamepadEnabled = RE::BSInputDeviceManager::GetSingleton()->IsGamepadEnabled();

            if (!gamepadEnabled && modifier > 0 && code == modifier) {
                modifierHeld = bEvent->IsHeld();
            }

            if (bEvent->IsUp()) {
                if ((gamepadEnabled && gamepadCode == gameButton) || (code == key &&
                           ((modifier > 0 && modifierHeld) ||
                            (modifier <= 0 && !((GetAsyncKeyState(VK_MENU) & 0x8000) || (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
                                                (GetAsyncKeyState(VK_CONTROL) & 0x8000)))))) {
                    modifierHeld = false;
                    keyPressEventFunc();
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }
}  // namespace wmh