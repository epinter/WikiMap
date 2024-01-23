#include "SelectedItem.h"

namespace wmh {
    void SelectedItem::setMap(std::string aLabel, int aIdx, int aType) {
        label = aLabel;
        idx = aIdx;
        type = aType;
        mapQuestId = 0;
        mapQuestLabel = "";
        logger::trace("SelectedItem::setMap {}", aLabel);
    }

    void SelectedItem::setMapNone() {
        label = "";
        idx = -1;
        type = -1;
        mapQuestId = 0;
        mapQuestLabel = "";
        logger::trace("SelectedItem::setMapNone");
    }

    std::string SelectedItem::getMapLabel() {
        return label;
    }

    int SelectedItem::getMapIndex() {
        return idx;
    }

    int SelectedItem::getMapType() {
        return type;
    }

    uint32_t SelectedItem::getMapQuestId() {
        return mapQuestId;
    }

    std::string SelectedItem::getMapQuestLabel() {
        return mapQuestLabel;
    }

    std::string SelectedItem::toString() {
        return std::format("[label:'{}'; idx:'{}'; type:'{}'; mapQuestId:'{}'; mapQuestLabel:'{}'; questId:'{}'; questName:'{}']", label,
                           idx, type, mapQuestId, mapQuestLabel, questId, questName);
    }

    void SelectedItem::setMapQuest(std::string questLabel, uint32_t id) {
        mapQuestId = id;
        mapQuestLabel = questLabel;
        logger::trace("SelectedItem::setMapQuest {}", questLabel);
    }

    uint32_t SelectedItem::getQuestId() {
        return questId;
    }

    std::string SelectedItem::getQuestName() {
        return questName;
    }

    void SelectedItem::setQuest(std::string name, uint32_t id) {
        questName = name;
        questId = id;
        logger::trace("SelectedItem::setQuestId {}", id);
    }

    void SelectedItem::setQuestNone() {
        questName = "";
        questId = 0;
    }

    void SelectedItem::reset() {
        setMapNone();
        setQuest("", 0);
    }
}  // namespace wmh