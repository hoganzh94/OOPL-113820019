#ifndef PEASHOOTER_HPP
#define PEASHOOTER_HPP

#include "Plants.hpp"
#include "Pea.hpp"
#include "Util/Animation.hpp"
#include "Config.hpp"
#include "AbilityInterfaces.hpp"
#include "GameWorld.hpp"

class Peashooter : public Plant, public IAttacker {
public:
    enum class AnimState { IDLE, ATTACKING };

    Peashooter(glm::vec2 pos) : Plant(PlantType::PEASHOOTER, pos, Config::PEASHOOTER_HP) {
        m_CurrentAnim = AnimState::IDLE;
        m_LastAppliedAnim = -1;
        m_FireTimer = 0.0f;
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

            // --- 修正 1：拉開狀態切換與發射的時間間隔 ---
            // 假設 1.2s 開始準備攻擊，1.5s 才發射
            if (m_FireTimer >= 1.2f && m_FireTimer < 1.5f) {
                m_CurrentAnim = AnimState::ATTACKING;
            }
            else if (m_FireTimer >= 1.5f) {
                // 執行發射
                auto newPea = std::make_shared<Pea>(m_Transform.translation + glm::vec2(30.0f, 20.0f));
                renderer.AddChild(newPea);
                world.AddPea(newPea);

                // 發射後重置
                m_FireTimer = 0.0f;
                m_CurrentAnim = AnimState::IDLE;
            } else {
                // 0.0s ~ 1.2s 之間保持 IDLE
                m_CurrentAnim = AnimState::IDLE;
            }
        } else {
            m_FireTimer = 0.0f;
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
    AnimState m_CurrentAnim;
    int m_LastAppliedAnim;

    void ApplyAnimation(AnimState state) {
        std::vector<std::string> paths;
        std::string base = std::string(RESOURCE_DIR) + "/Image/Plant/Peashooter/";
        int frames;
        std::string folder;

        if (state == AnimState::IDLE) {
            frames = 8;
            folder = "Idle/Peashooter - Idle ";
        } else {
            frames = 3;
            folder = "Attack/Peashooter - Attack ";
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