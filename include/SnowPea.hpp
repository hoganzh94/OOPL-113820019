#ifndef SNOWPEA_HPP
#define SNOWPEA_HPP

#include "Plants.hpp"
#include "SnowPeaBullet.hpp"
#include "AbilityInterfaces.hpp"
#include "Config.hpp"
#include "Util/Animation.hpp"

class SnowPea : public Plant, public IAttacker {
public:
    enum class AnimState { IDLE, ATTACKING };

    SnowPea(glm::vec2 pos) : Plant(PlantType::SNOWPEA, pos, Config::SNOWPEA_HP) {
        m_CurrentAnim = AnimState::IDLE;
        m_LastAppliedAnim = -1; // 確保第一次載入
        m_FireTimer = 0.0f;
    }

    void Attack(std::vector<std::shared_ptr<Zombie>>& rowZombies, Util::Renderer& renderer, GameWorld& world) override {
        bool foundZombie = false;
        for (auto& z : rowZombies) {
            if (!z->IsDead() && z->GetX() > m_Transform.translation.x) {
                foundZombie = true;
                break;
            }
        }

        if (foundZombie) {
            m_FireTimer += Util::Time::GetDeltaTime();

            // 區間判定：1.2s~1.5s 顯示張嘴攻擊動畫
            if (m_FireTimer >= 1.2f && m_FireTimer < 1.5f) {
                m_CurrentAnim = AnimState::ATTACKING;
            }
            else if (m_FireTimer >= 1.5f) {
                // 發射寒冰豌豆
                auto bullet = std::make_shared<SnowPeaBullet>(m_Transform.translation + glm::vec2(30.0f, 20.0f));
                renderer.AddChild(bullet);
                world.AddPea(bullet);

                m_FireTimer = 0.0f;
                m_CurrentAnim = AnimState::IDLE;
            } else {
                m_CurrentAnim = AnimState::IDLE;
            }
        } else {
            m_FireTimer = 0.0f;
            m_CurrentAnim = AnimState::IDLE;
        }
    }

    void Update() override {
        // 狀態鎖機制：只有切換時才更換 Drawable
        if (static_cast<int>(m_CurrentAnim) != m_LastAppliedAnim) {
            ApplyAnimation(m_CurrentAnim);
            m_LastAppliedAnim = static_cast<int>(m_CurrentAnim);
        }
    }

private:
    float m_FireTimer;
    AnimState m_CurrentAnim;
    int m_LastAppliedAnim;

    void ApplyAnimation(AnimState state) {
        std::vector<std::string> paths;
        std::string base = "C:/Users/user/OOPL-113820019/Resources/Image/Plant/Snow Pea/";
        int frames = (state == AnimState::IDLE) ? 8 : 3;
        std::string folder = (state == AnimState::IDLE) ? "Idle/Snow Pea - Idle " : "Attack/Snow Pea - Attack ";

        for (int i = 1; i <= frames; ++i) {
            paths.push_back(base + folder + std::to_string(i) + ".png");
        }

        // 強制 loop = true 規避框架 Bug，攻擊動畫張嘴 0.3 秒非常合適
        auto anim = std::make_shared<Util::Animation>(paths, true, 100, true);
        SetDrawable(anim);
    }
};

#endif