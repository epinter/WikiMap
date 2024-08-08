#pragma once

namespace wmh {
    class Hooks {
        private:
            static constexpr int ADDR_LOADMOVIE_SE = 80302;
            static constexpr int ADDR_LOADMOVIE_AE = 82325;
            static constexpr int OFFSET_LOADMOVIE_AE = 0x1DD;
            static constexpr int OFFSET_LOADMOVIE_SE = 0x1D9;
            static constexpr int OFFSET_LOADMOVIE_VR = 0x1D9;
            static constexpr SKSE::stl::nttp::string PATTERN_CHECK_LOADMOVIE = "FF 15";

            static void setViewScaleModeHook(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode);
            static inline REL::Relocation<decltype(setViewScaleModeHook)> setViewScaleMode;

        public:
            static void installMovieHook();
    };
}  // namespace wmh
