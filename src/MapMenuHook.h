#pragma once

namespace wmh {
    class MapMenuHook {
        private:
            class MapSetSelectedMarker : public RE::GFxFunctionHandler {
                private:
                    RE::GFxValue origFunction{};
                    std::atomic<bool> originalSet = false;
                    std::atomic<bool> isLocalMapOpen = false;
                    bool origVisible = false;
                    std::string originalLabel{};
                    size_t originalCode{};

                public:
                    explicit MapSetSelectedMarker(RE::GFxValue& origFunc)
                        : origFunction(origFunc) {}
                    void Call(Params& params) override;
            };

        public:
            static void installMapMenuHook(RE::GFxMovieView* movieView);
    };
}  // namespace wmh