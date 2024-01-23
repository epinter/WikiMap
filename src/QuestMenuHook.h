#pragma once

namespace wmh {
    class QuestMenuHook {
        private:
            class QuestOnSelectedId : public RE::GFxFunctionHandler {
                private:
                    RE::GFxValue origFunction{};

                public:
                    explicit QuestOnSelectedId(RE::GFxValue& aOldFunc)
                        : origFunction(aOldFunc) {}
                    void Call(Params& a_params) override;
            };

            class QuestOnTabClick : public RE::GFxFunctionHandler {
                private:
                    RE::GFxValue origFunction{};

                public:
                    explicit QuestOnTabClick(RE::GFxValue& aOldFunc)
                        : origFunction(aOldFunc) {}
                    void Call(Params& a_params) override;
            };

            class QuestStartPage : public RE::GFxFunctionHandler {
                private:
                    RE::GFxValue origFunction{};

                public:
                    explicit QuestStartPage(RE::GFxValue& aOldFunc)
                        : origFunction(aOldFunc) {}
                    void Call(Params& a_params) override;
            };

        public:
            static void installJournalMenuHook(RE::GFxMovieView* movieView);
            static void installQuestMenuHook(RE::GFxMovieView* movieView);
    };
}  // namespace wmh