#ifndef REPEATER_HPP
#define REPEATER_HPP

#include "Plants.hpp"
#include "Pea.hpp"
#include "Util/Animation.hpp"
#include "Config.hpp"
#include "AbilityInterfaces.hpp"
#include "GameWorld.hpp"
#include "TargetingStrategy.hpp" // 引入策略

class Repeater : public Plant, public IAttacker {
public:
    enum class AnimState { IDLE, ATTACKING };

    Repeater(glm::vec2 pos) : Plant(PlantType::REPEATER, pos, Config::REPEATER_HP) {
        m_CurrentAnim = AnimState::IDLE;
        m_LastAppliedAnim = -1;
        m_FireTimer = 0.0f;
        m_FiredFirstPea = false;

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

            if (m_FireTimer >= 1.2f && m_FireTimer < 1.4f) {
                m_CurrentAnim = AnimState::ATTACKING;
            }
            else if (m_FireTimer >= 1.4f && m_FireTimer < 1.6f) {
                if (!m_FiredFirstPea) {
                    Shoot(renderer, world);
                    m_FiredFirstPea = true;
                }
            }
            else if (m_FireTimer >= 1.6f) {
                Shoot(renderer, world);
                m_FireTimer = 0.0f;
                m_FiredFirstPea = false;
                m_CurrentAnim = AnimState::IDLE;
            } else {
                m_CurrentAnim = AnimState::IDLE;
            }
        } else {
            m_FireTimer = 0.0f;
            m_FiredFirstPea = false;
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
    bool m_FiredFirstPea;
    AnimState m_CurrentAnim;
    int m_LastAppliedAnim;
    std::shared_ptr<ITargetingStrategy> m_Targeting;

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
            frames = 8;
            folder = "Idle/Repeater - Idle ";
        } else {
            frames = 4;
            folder = "Attack/Repeater - Attack ";
        }

        for (int i = 1; i <= frames; ++i) {
            paths.push_back(base + folder + std::to_string(i) + ".png");
        }

        auto anim = std::make_shared<Util::Animation>(paths, true, 100, true);
        SetDrawable(anim);
    }
};

#endif