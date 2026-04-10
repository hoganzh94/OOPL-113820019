#ifndef PEASHOOTER_HPP
#define PEASHOOTER_HPP

#include "Plants.hpp"
#include "Pea.hpp"
#include "Util/Animation.hpp"
#include "Config.hpp"
#include "PlantType.hpp"
#include "AbilityInterfaces.hpp"
#include "GameWorld.hpp"

class Peashooter : public Plant, public IAttacker {
public:
    Peashooter(glm::vec2 pos) : Plant(PlantType::PEASHOOTER, pos, Config::PEASHOOTER_HP) {
        std::vector<std::string> idlePaths;
        for (int i = 1; i <= 8; ++i)
            idlePaths.push_back("C:/Users/user/OOPL-113820019/Resources/Image/Plant/Peashooter/Idle/Peashooter - Idle " + std::to_string(i) + ".png");
        m_IdleAnim = std::make_shared<Util::Animation>(idlePaths, true, 120, true);

        std::vector<std::string> attackPaths;
        for (int i = 1; i <= 3; ++i)
            attackPaths.push_back("C:/Users/user/OOPL-113820019/Resources/Image/Plant/Peashooter/Attack/Peashooter - Attack " + std::to_string(i) + ".png");

        m_AttackAnim = std::make_shared<Util::Animation>(attackPaths, false, 100, true);
        SetDrawable(m_IdleAnim);
    }

    // 實作 IAttacker 介面 (確保參數為 3 個)
    void Attack(std::vector<std::shared_ptr<Zombie>>& rowZombies,
                Util::Renderer& renderer,
                GameWorld& world) override {

        bool foundZombie = false;
        for (auto& z : rowZombies) {
            // 判定殭屍是否在植物前方且活著
            if (!z->IsDead() && z->GetX() > m_Transform.translation.x) {
                foundZombie = true;
                break;
            }
        }

        if (foundZombie) {
            m_FireTimer += Util::Time::GetDeltaTime();

            // 準備發射時切換動畫 (例如在 1.2 秒時切換到攻擊幀)
            if (m_FireTimer >= 1.2f && m_FireTimer < 1.25f) {
                SetDrawable(m_AttackAnim);
            }

            // --- 關鍵修正：使用計時器取代 readyToFire ---
            if (m_FireTimer >= 1.5f) {
                // 生成豌豆
                auto newPea = std::make_shared<Pea>(m_Transform.translation + glm::vec2(30.0f, 20.0f));

                // 加入渲染系統 (顯示) 與 遊戲世界 (移動邏輯)
                renderer.AddChild(newPea);
                world.AddPea(newPea);

                // 重置狀態
                m_FireTimer = 0.0f;
                SetDrawable(m_IdleAnim);
            }
        } else {
            // 若前方沒殭屍，重置計時器(或停留在準備發射狀態)並恢復 Idle 動畫
            m_FireTimer = 0.0f;
            SetDrawable(m_IdleAnim);
        }
    }

    void Update() override {
        // 植物的視覺 Update 由繼承的 GameObject 自動處理渲染
    }

private:
    float m_FireTimer = 0.0f;
    std::shared_ptr<Util::Animation> m_IdleAnim, m_AttackAnim;
};

#endif