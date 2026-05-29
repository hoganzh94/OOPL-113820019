#ifndef SNOWPEA_HPP
#define SNOWPEA_HPP

#include "Plants.hpp"
#include "SnowPeaBullet.hpp" // 發射冰豌豆
#include "Util/Animation.hpp"
#include "Config.hpp"
#include "AbilityInterfaces.hpp"
#include "GameWorld.hpp"
#include "TargetingStrategy.hpp" // 引入策略

class SnowPea : public Plant, public IAttacker {
public:
    enum class AnimState { IDLE, ATTACKING };

    SnowPea(glm::vec2 pos) : Plant(PlantType::SNOWPEA, pos, Config::SNOWPEA_HP) {
        m_CurrentAnim = AnimState::IDLE;
        m_LastAppliedAnim = -1;
        m_FireTimer = 0.0f;

        // ★ 掛載全線射擊策略
        m_Targeting = std::make_shared<ForwardTargeting>();
    }

    void Attack(std::vector<std::shared_ptr<Zombie>>& rowZombies,
                Util::Renderer& renderer,
                GameWorld& world) override {

        // ★ 使用策略尋找目標
        auto target = m_Targeting->FindTarget(m_Transform.translation, rowZombies);

        if (target != nullptr) {
            m_FireTimer += Util::Time::GetDeltaTime();

            if (m_FireTimer >= 1.2f && m_FireTimer < 1.5f) {
                m_CurrentAnim = AnimState::ATTACKING;
            }
            else if (m_FireTimer >= 1.5f) {
                auto newPea = std::make_shared<SnowPeaBullet>(m_Transform.translation + glm::vec2(30.0f, 20.0f));
                renderer.AddChild(newPea);
                world.AddPea(newPea);

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
        if (static_cast<int>(m_CurrentAnim) != m_LastAppliedAnim) {
            ApplyAnimation(m_CurrentAnim);
            m_LastAppliedAnim = static_cast<int>(m_CurrentAnim);
        }
    }

private:
    float m_FireTimer;
    AnimState m_CurrentAnim;
    int m_LastAppliedAnim;
    std::shared_ptr<ITargetingStrategy> m_Targeting;

    void ApplyAnimation(AnimState state) {
        std::vector<std::string> paths;
        std::string base = std::string(RESOURCE_DIR) + "/Image/Plant/Snow Pea/";
        int frames;
        std::string folder;

        if (state == AnimState::IDLE) {
            frames = 8;
            folder = "Idle/Snow Pea - Idle ";
        } else {
            frames = 3;
            folder = "Attack/Snow Pea - Attack ";
        }

        for (int i = 1; i <= frames; ++i) {
            paths.push_back(base + folder + std::to_string(i) + ".png");
        }

        auto anim = std::make_shared<Util::Animation>(paths, true, 100, true);
        SetDrawable(anim);
    }
};

#endif