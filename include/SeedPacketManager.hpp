#ifndef SEEDPACKETMANAGER_HPP
#define SEEDPACKETMANAGER_HPP

#include "SeedPacket.hpp"
#include "PlantType.hpp"
#include <vector>
#include <memory>

class SeedPacketManager {
public:
    SeedPacketManager(Util::Renderer& renderer) : m_Renderer(renderer) {}

    void Initialize() {
        m_Packets.clear();
        m_SelectedType = PlantType::NONE;

        // 確保路徑與全域 PlantType 對應
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::PEASHOOTER, "C:/Users/user/ptsd-template/Resources/Image/Plant/Peashooter/Peashooter - Image.png"));
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::SUNFLOWER,  "C:/Users/user/ptsd-template/Resources/Image/Plant/Sunflower/Sunflower - Image.png"));
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::CHERRYBOMB, "C:/Users/user/ptsd-template/Resources/Image/Plant/Cherrybomb/Cherry Bomb - Image.png"));
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::WALLNUT,    "C:/Users/user/ptsd-template/Resources/Image/Plant/Wallnut/Wall-Nut - Image.png"));
        m_Packets.push_back(std::make_shared<SeedPacket>(PlantType::POTATOMINE, "C:/Users/user/ptsd-template/Resources/Image/Plant/Potato mine/Potato Mine - Image.png"));

        float startY = 250.0f;
        for (auto& p : m_Packets) {
            p->SetPosition({-450.0f, startY});
            startY -= 75.0f;
        }
    }

    void Update() {
        if (!m_IsVisible) return;
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            for (auto& p : m_Packets) {
                if (p->IsMouseHovering()) {
                    m_SelectedType = p->GetType();
                    return;
                }
            }
        }
    }

    // 關鍵修正：確保這兩個公有函式定義正確
    [[nodiscard]] const std::vector<std::shared_ptr<SeedPacket>>& GetPackets() const {
        return m_Packets;
    }

    void SetVisibleStatus(bool visible) {
        m_IsVisible = visible;
    }

    PlantType GetSelectedType() const { return m_SelectedType; }
    void ClearSelection() { m_SelectedType = PlantType::NONE; }

private:
    Util::Renderer& m_Renderer;
    std::vector<std::shared_ptr<SeedPacket>> m_Packets;
    PlantType m_SelectedType = PlantType::NONE;
    bool m_IsVisible = false;
};

#endif