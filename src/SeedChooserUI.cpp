#include "SeedChooserUI.hpp"

SeedChooserUI::SeedChooserUI(Util::Renderer& renderer) : m_Renderer(renderer) {}

void SeedChooserUI::Initialize() {
    std::string base = RESOURCE_DIR;

    // 開始戰鬥按鈕 (使用文字做為按鈕)
    m_StartButton = std::make_shared<Util::GameObject>();
    m_StartButtonText = std::make_shared<Util::Text>(base + "/Smile Delight (Demo_Font).otf", 40, "LET'S ROCK!", Util::Color::FromRGB(128, 128, 128));
    m_StartButton->SetDrawable(m_StartButtonText);
    m_StartButton->m_Transform.translation = {400.0f, -250.0f}; // 右下角
    m_StartButton->SetZIndex(60.0f);
}

void SeedChooserUI::Show(const std::vector<PlantType>& unlockedPlants) {
    if (m_IsVisible) return;
    
    m_SelectedPlants.clear();
    m_Cards.clear();

    // 動態生成玩家擁有的植物卡牌
    float startX = -400.0f;
    float startY = -150.0f; // 備選區高度
    float offsetX = 110.0f; // 卡牌橫向間距
    float offsetY = -140.0f; // 換行間距

    for (size_t i = 0; i < unlockedPlants.size(); ++i) {
        ChooserCard card;
        card.type = unlockedPlants[i];
        card.isSelected = false;
        
        // 計算九宮格/多行排列位置
        float x = startX + (i % 8) * offsetX;
        float y = startY + (i / 8) * offsetY;
        card.inventoryPos = {x, y};

        card.obj = std::make_shared<Util::GameObject>();
        card.obj->SetDrawable(std::make_shared<Util::Image>(GetCardImagePath(card.type)));
        card.obj->m_Transform.translation = card.inventoryPos;
        card.obj->m_Transform.scale = {2.5f, 2.5f}; // 卡牌縮小一點
        card.obj->SetZIndex(12.0f);

        m_Cards.push_back(card);
        m_Renderer.AddChild(card.obj);
    }

    m_Renderer.AddChild(m_StartButton);
    m_IsVisible = true;
    UpdateCardPositions(); // 初始化按鈕顏色與位置
}

void SeedChooserUI::Hide() {
    if (!m_IsVisible) return;
    for (auto& card : m_Cards) {
        m_Renderer.RemoveChild(card.obj);
    }
    m_Renderer.RemoveChild(m_StartButton);
    m_IsVisible = false;
}

bool SeedChooserUI::Update() {
    if (!m_IsVisible) return false;

    // 檢查滑鼠左鍵點擊 (只在按下的瞬間觸發)
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        
        // 1. 檢查是否點擊「LET'S ROCK!」按鈕
        if (m_SelectedPlants.size() == MAX_SLOTS && IsMouseHovering(m_StartButton->m_Transform.translation, 200.0f, 60.0f)) {
            return true; // 準備開戰！
        }

        // 2. 檢查是否點擊卡牌
        for (auto& card : m_Cards) {
            // 卡牌大約 100x140 大小
            if (IsMouseHovering(card.obj->m_Transform.translation, 150.0f, 200.0f)) {
                if (card.isSelected) {
                    // 取消選取：移出清單
                    card.isSelected = false;
                    auto it = std::find(m_SelectedPlants.begin(), m_SelectedPlants.end(), card.type);
                    if (it != m_SelectedPlants.end()) m_SelectedPlants.erase(it);
                } else {
                    // 選取：加入清單 (前提是還沒滿)
                    if (m_SelectedPlants.size() < MAX_SLOTS) {
                        card.isSelected = true;
                        m_SelectedPlants.push_back(card.type);
                    }
                }
                UpdateCardPositions(); // 更新所有卡牌畫面
                break; // 點到一張就跳出迴圈
            }
        }
    }

    return false;
}

void SeedChooserUI::UpdateCardPositions() {
    // 定義上方「出戰槽」的起始位置
    float slotStartX = -300.0f;
    float slotY = 250.0f;
    float slotOffsetX = 100.0f;

    // 更新每一張卡牌的位置與透明度
    for (auto& card : m_Cards) {
        if (card.isSelected) {
            // 找到這張卡在 m_SelectedPlants 裡面的索引值，決定它在出戰槽的格子
            auto it = std::find(m_SelectedPlants.begin(), m_SelectedPlants.end(), card.type);
            int index = std::distance(m_SelectedPlants.begin(), it);
            card.obj->m_Transform.translation = {slotStartX + index * slotOffsetX, slotY};
        } else {
            // 放回備選區
            card.obj->m_Transform.translation = card.inventoryPos;
        }
    }

    // 更新按鈕顏色：選滿 6 張變綠色，未滿變灰色
    if (m_SelectedPlants.size() == MAX_SLOTS) {
        m_StartButtonText->SetColor(Util::Color::FromRGB(0, 255, 0)); // 綠色
    } else {
        m_StartButtonText->SetColor(Util::Color::FromRGB(128, 128, 128)); // 灰色
    }
}

std::vector<PlantType> SeedChooserUI::GetSelectedPlants() const {
    return m_SelectedPlants;
}

// 根據植物類型回傳對應的卡片圖片路徑
std::string SeedChooserUI::GetCardImagePath(PlantType type) {
    std::string base = RESOURCE_DIR;
    switch (type) {
        case PlantType::PEASHOOTER: return base + "/Image/Plant/Peashooter/Peashooter - Image.png";
        case PlantType::SUNFLOWER:  return base + "/Image/Plant/Sunflower/Sunflower - Image.png";
        case PlantType::CHERRYBOMB: return base + "/Image/Plant/Cherrybomb/Cherry Bomb - Image.png";
        case PlantType::WALLNUT:    return base + "/Image/Plant/Wallnut/Wall-Nut - Image.png";
        case PlantType::POTATOMINE: return base + "/Image/Plant/Potato mine/Potato Mine - Image.png";
        case PlantType::SNOWPEA:    return base + "/Image/Plant/Snow Pea/Snow Pea - Image.png";
        case PlantType::CHOMPER:    return base + "/Image/Plant/Chomper/Chomper - Image.png";
        default: return base + "/Image/UI/SeedPacket_Peashooter.png"; // 預設防呆
    }
}

// 簡單的滑鼠碰撞判定 (AABB)
bool SeedChooserUI::IsMouseHovering(const glm::vec2& pos, float width, float height) {
    auto mousePos = Util::Input::GetCursorPosition();
    return (mousePos.x >= pos.x - width / 2 && mousePos.x <= pos.x + width / 2 &&
            mousePos.y >= pos.y - height / 2 && mousePos.y <= pos.y + height / 2);
}