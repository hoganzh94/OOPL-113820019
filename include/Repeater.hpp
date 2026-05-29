#ifndef REPEATER_HPP
#define REPEATER_HPP

#include "Plants.hpp"
#include "Pea.hpp" // 雙發豌豆一樣使用一般的豌豆子彈
#include "Util/Animation.hpp"
#include "Config.hpp"
#include "AbilityInterfaces.hpp"
#include "GameWorld.hpp"

class Repeater : public Plant, public IAttacker {
public:
    enum class AnimState { IDLE, ATTACKING };

    Repeater(glm::vec2 pos) : Plant(PlantType::REPEATER, pos, Config::REPEATER_HP) {
        m_CurrentAnim = AnimState::IDLE;
        m_LastAppliedAnim = -1;
        m_FireTimer = 0.0f;
        m_FiredFirstPea = false; // ★ 紀錄是否已發射本輪的第一顆豌豆
    }

    void Attack(std::vector<std::shared_ptr<Zombie>>& rowZombies,
                Util::Renderer& renderer,
                GameWorld& world) override {

        bool foundZombie = false;
        for (auto& z : rowZombies) {
            if (!z->IsDead() && z->GetX() > m_Transform.translation.x) {
                foundZombie = true;
                break;
            }
        }

        if (foundZombie) {
            m_FireTimer += Util::Time::GetDeltaTime();

            // 1.2s 開始準備攻擊並切換動畫
            if (m_FireTimer >= 1.2f && m_FireTimer < 1.4f) {
                m_CurrentAnim = AnimState::ATTACKING;
            }
            // 1.4s 抵達第一顆豌豆的發射時間
            else if (m_FireTimer >= 1.4f && m_FireTimer < 1.6f) {
                if (!m_FiredFirstPea) {
                    Shoot(renderer, world);
                    m_FiredFirstPea = true; // 鎖定，避免在這 0.2 秒內重複發射
                }
            }
            // 1.6s 抵達第二顆豌豆的發射時間，並重置週期
            else if (m_FireTimer >= 1.6f) {
                Shoot(renderer, world); // 發射第二顆

                // 攻擊結束，全部重置
                m_FireTimer = 0.0f;
                m_FiredFirstPea = false;
                m_CurrentAnim = AnimState::IDLE;
            } else {
                // 0.0s ~ 1.2s 之間保持 IDLE
                m_CurrentAnim = AnimState::IDLE;
            }
        } else {
            // 沒有敵人時，重置所有狀態
            m_FireTimer = 0.0f;
            m_FiredFirstPea = false;
            m_CurrentAnim = AnimState::IDLE;
        }
    }

    void Update() override {
        // 狀態鎖定機制
        if (static_cast<int>(m_CurrentAnim) != m_LastAppliedAnim) {
            ApplyAnimation(m_CurrentAnim);
            m_LastAppliedAnim = static_cast<int>(m_CurrentAnim);
        }
    }

private:
    float m_FireTimer;
    bool m_FiredFirstPea;
    AnimState m_CurrentAnim;
    int m_LastAppliedAnim;

    // ★ 抽出發射邏輯，方便呼叫兩次
    void Shoot(Util::Renderer& renderer, GameWorld& world) {
        auto newPea = std::make_shared<Pea>(m_Transform.translation + glm::vec2(30.0f, 20.0f));
        renderer.AddChild(newPea);
        world.AddPea(newPea);
    }

    void ApplyAnimation(AnimState state) {
        std::vector<std::string> paths;
        std::string base = std::string(RESOURCE_DIR) + "/Image/Plant/Repeater/";
        int frames;
        std::string folder;

        if (state == AnimState::IDLE) {
            frames = 8; // Repeater Idle 是 8 張
            folder = "Idle/Repeater - Idle ";
        } else {
            frames = 4; // Repeater Attack 是 4 張
            folder = "Attack/Repeater - Attack ";
        }

        for (int i = 1; i <= frames; ++i) {
            paths.push_back(base + folder + std::to_string(i) + ".png");
        }

        // 強制 loop = true 規避 Bug
        auto anim = std::make_shared<Util::Animation>(paths, true, 100, true);
        SetDrawable(anim);
    }
};

#endif