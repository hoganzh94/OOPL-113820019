#ifndef SEEDPACKETMANAGER_HPP
#define SEEDPACKETMANAGER_HPP

#include "SeedPacket.hpp"
#include <vector>

class SeedPacketManager {
public:
    SeedPacketManager(Util::Renderer& renderer) : m_Renderer(renderer) {}

    void Initialize() {
        m_Packets.clear();
        m_SelectedType = PlantType::NONE;

        // 傳入對應的冷卻時間
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::PEASHOOTER, "C:/Users/user/OOPL-113820019/Resources/Image/Plant/Peashooter/Peashooter - Image.png", Config::PEASHOOTER_COOLDOWN));
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::SUNFLOWER, "C:/Users/user/OOPL-113820019/Resources/Image/Plant/Sunflower/Sunflower - Image.png",   Config::SUNFLOWER_COOLDOWN));
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::CHERRYBOMB, "C:/Users/user/OOPL-113820019/Resources/Image/Plant/Cherrybomb/Cherry Bomb - Image.png",Config::CHERRYBOMB_COOLDOWN));
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::WALLNUT, "C:/Users/user/OOPL-113820019/Resources/Image/Plant/Wallnut/Wall-Nut - Image.png",     Config::WALLNUT_COOLDOWN));
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::POTATOMINE, "C:/Users/user/OOPL-113820019/Resources/Image/Plant/Potato mine/Potato Mine - Image.png", Config::POTATOMINE_COOLDOWN));
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::SNOWPEA, "C:/Users/user/OOPL-113820019/Resources/Image/Plant/Snow Pea/Snow Pea - Image.png", Config::SNOWPEA_COOLDOWN));

        float startY = 250.0f;
        for (auto& p : m_Packets) {
            p->SetPosition({-450.0f, startY});
            startY -= 75.0f;
        }
    }

    void Update() {
        // 這個 Update 現在只負責偵測滑鼠點擊卡片的那一瞬間
        if (!m_IsVisible) return;

        // 注意：不要在這裡跑 p->Update()，因為我們在 App.cpp 統一跑了
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            for (auto& p : m_Packets) {
                if (p->IsMouseHovering() && p->IsReady()) {
                    m_SelectedType = p->GetType();
                    return;
                }
            }
        }
    }

    // 當成功種植植物後，觸發特定卡牌的冷卻
    void StartPacketCooldown(PlantType type) {
        for (auto& p : m_Packets) {
            if (p->GetType() == type) {
                p->StartCooldown();
                break;
            }
        }
    }

    [[nodiscard]] const std::vector<std::shared_ptr<SeedPacket>>& GetPackets() const { return m_Packets; }
    void SetVisibleStatus(bool visible) { m_IsVisible = visible; }
    PlantType GetSelectedType() const { return m_SelectedType; }
    void ClearSelection() { m_SelectedType = PlantType::NONE; }

private:
    Util::Renderer& m_Renderer;
    std::vector<std::shared_ptr<SeedPacket>> m_Packets;
    PlantType m_SelectedType = PlantType::NONE;
    bool m_IsVisible = false;
};

#endif