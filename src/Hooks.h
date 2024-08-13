#pragma once

namespace wmh {
    class Hooks {
        private:
            //RE::Offset::BSScaleformManager::LoadMovie
            static constexpr int ADDR_LOADMOVIE_SE = 80302;
            static constexpr int ADDR_LOADMOVIE_AE = 82325;
            static constexpr int ADDR_LOADMOVIE_VR = 82325;
            static constexpr int OFFSET_LOADMOVIE_AE = 0x1DD;
            static constexpr int OFFSET_LOADMOVIE_SE = 0x1D9;
            static constexpr int OFFSET_LOADMOVIE_VR = 0x1D9;
            static constexpr SKSE::stl::nttp::string PATTERN_CHECK_LOADMOVIE = "FF 15";

            static void setViewScaleModeHook(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode);
            static inline REL::Relocation<decltype(setViewScaleModeHook)> setViewScaleMode;

            static void testDetours1(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode);
            static void testDetours2(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode);
            static void testDetours3(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode);
            static void testDetours4(RE::GFxMovieView* movieView, RE::GFxMovieView::ScaleModeType scaleMode);

            static void logTestDetours(int);

            using OrigTestDetour = decltype(&setViewScaleModeHook);
            static inline OrigTestDetour origSetViewScaleModeD1;
            static inline OrigTestDetour origSetViewScaleModeD2;
            static inline OrigTestDetour origSetViewScaleModeD3;
            static inline OrigTestDetour origSetViewScaleModeD4;

        public:
            static void installMovieHook();
    };
}  // namespace wmh
