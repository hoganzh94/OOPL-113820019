#ifndef SEEDCHOOSERUI_HPP
#define SEEDCHOOSERUI_HPP

#include "pch.hpp"
#include "PlantType.hpp"
#include "Util/Renderer.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

class SeedChooserUI {
public:
    explicit SeedChooserUI(Util::Renderer& renderer);
    void Initialize();
    void Show(const std::vector<PlantType>& unlockedPlants);
    void Hide();
    bool Update(); // 回傳 true 代表玩家點擊了「開始戰鬥」
    std::vector<PlantType> GetSelectedPlants() const;

private:
    struct ChooserCard {
        PlantType type;
        std::shared_ptr<Util::GameObject> obj;
        bool isSelected = false;
        glm::vec2 inventoryPos; // 在下方備選區的原始位置
    };

    Util::Renderer& m_Renderer;
    bool m_IsVisible = false;
    const int MAX_SLOTS = 6;

    // UI 元素
    std::shared_ptr<Util::GameObject> m_PanelBG;
    std::shared_ptr<Util::GameObject> m_StartButton;
    std::shared_ptr<Util::Text> m_StartButtonText;

    std::vector<ChooserCard> m_Cards;
    std::vector<PlantType> m_SelectedPlants;

    // 輔助函式
    std::string GetCardImagePath(PlantType type);
    bool IsMouseHovering(const glm::vec2& pos, float width, float height);
    void UpdateCardPositions();
};

#endif