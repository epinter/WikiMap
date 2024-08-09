#pragma once

namespace wmh {
    class InputListener : public RE::BSTEventSink<RE::InputEvent*> {
        private:
            bool enabled = false;
            int modifier = 0;
            int key = 0;
            int gameButton = 0;
            bool modifierHeld = false;
            bool handlerSet = false;
            std::function<void(void)> keyPressEventFunc = []() {};

        public:
            InputListener() = default;
            InputListener(InputListener&) = delete;
            InputListener& operator=(InputListener&&) = delete;
            void operator=(InputListener&) = delete;

            void setHandler(std::function<void(void)> function, int modifier, int key, int button);
            [[nodiscard]] bool isEnabled() const;
            void setEnabled(bool b);
            void setModifier(int modifier);
            void setKey(int key);
            void setGameButton(int button);
            RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* eventPtr, RE::BSTEventSource<RE::InputEvent*>*);
    };
}  // namespace wmh