#pragma once

namespace wmh {
    class SelectedItem {
        private:
            SelectedItem() = default;
            SelectedItem(SelectedItem&) = delete;
            SelectedItem& operator=(SelectedItem&&) = delete;
            void operator=(SelectedItem&) = delete;
            std::string label{};
            int idx = -1;
            int type = -1;
            uint32_t questId = 0;
            std::string questName{};
            uint32_t mapQuestId = 0;
            std::string mapQuestLabel{};

        public:
            [[nodiscard]] static SelectedItem& getInstance() {
                static SelectedItem instance;
                return instance;
            }

            void setMap(std::string aLabel, int aIdx, int aType);
            void setMapNone();
            void reset();
            void setQuest(std::string name, uint32_t id);
            void setQuestNone();
            [[nodiscard]] uint32_t getQuestId();
            [[nodiscard]] std::string getQuestName();
            [[nodiscard]] std::string getMapLabel();
            [[nodiscard]] int getMapIndex();
            [[nodiscard]] int getMapType();
            void setMapQuest(std::string label, uint32_t id);
            [[nodiscard]] uint32_t getMapQuestId();
            [[nodiscard]] std::string getMapQuestLabel();
            [[nodiscard]] std::string toString();
    };
}  // namespace wmh