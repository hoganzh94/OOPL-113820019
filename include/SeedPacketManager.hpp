#ifndef SEEDPACKETMANAGER_HPP
#define SEEDPACKETMANAGER_HPP

#include "SeedPacket.hpp"
#include <vector>
#include <memory>

class SeedPacketManager {
public:
    SeedPacketManager(Util::Renderer& renderer) : m_Renderer(renderer) {}

    // 預設前六關的初始化 (直接呼叫 InitializeWith)
    void Initialize() {
        InitializeWith({
            PlantType::PEASHOOTER, PlantType::SUNFLOWER, PlantType::CHERRYBOMB,
            PlantType::WALLNUT, PlantType::POTATOMINE, PlantType::SNOWPEA
        });
    }

    // ★ 全新功能：根據玩家選好的清單生成卡槽 ★
    void InitializeWith(const std::vector<PlantType>& selectedTypes) {
        m_Packets.clear();
        m_SelectedType = PlantType::NONE;

        float startY = 250.0f; // 左側卡槽的最高點
        for (PlantType type : selectedTypes) {
            std::string path = std::string(RESOURCE_DIR) + "/Image/Plant/";
            float cooldown = 5.0f;

            // 自動對應圖片與冷卻時間
            switch(type) {
                case PlantType::PEASHOOTER: path += "Peashooter/Peashooter - Image.png"; cooldown = Config::PEASHOOTER_COOLDOWN; break;
                case PlantType::SUNFLOWER:  path += "Sunflower/Sunflower - Image.png"; cooldown = Config::SUNFLOWER_COOLDOWN; break;
                case PlantType::CHERRYBOMB: path += "Cherrybomb/Cherry Bomb - Image.png"; cooldown = Config::CHERRYBOMB_COOLDOWN; break;
                case PlantType::WALLNUT:    path += "Wallnut/Wall-Nut - Image.png"; cooldown = Config::WALLNUT_COOLDOWN; break;
                case PlantType::POTATOMINE: path += "Potato mine/Potato Mine - Image.png"; cooldown = Config::POTATOMINE_COOLDOWN; break;
                case PlantType::SNOWPEA:    path += "Snow Pea/Snow Pea - Image.png"; cooldown = Config::SNOWPEA_COOLDOWN; break;
                case PlantType::CHOMPER:    path += "Chomper/Chomper - Image.png"; cooldown = Config::CHOMPER_COOLDOWN; break;
                default: path += "Peashooter/Peashooter - Image.png"; cooldown = Config::PEASHOOTER_COOLDOWN; break;
            }

            auto packet = std::make_shared<SeedPacket>(type, path, cooldown);
            packet->SetPosition({-450.0f, startY}); // 靠左排列
            startY -= 75.0f; // 往下排
            m_Packets.push_back(packet);
        }
    }

    void Update() {
        if (!m_IsVisible) return;

        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            for (auto& p : m_Packets) {
                if (p->IsMouseHovering() && p->IsReady()) {
                    m_SelectedType = p->GetType();
                    return;
                }
            }
        }
    }

    void StartPacketCooldown(PlantType type) {
        for (auto& p : m_Packets) {
            if (p->GetType() == type) {
                p->StartCooldown();
                break;
            }
        }
    }

    [[nodiscard]] const std::vector<std::shared_ptr<SeedPacket>>& GetPackets() const { return m_Packets; }

    // ★ 關鍵修正：同時改變內部變數與遊戲物件的顯示狀態 ★
    void SetVisibleStatus(bool visible) {
        m_IsVisible = visible;
        for (auto& p : m_Packets) {
            if (p) p->SetVisible(visible);
        }
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