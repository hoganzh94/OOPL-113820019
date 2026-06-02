#ifndef PEASHOOTER_HPP
#define PEASHOOTER_HPP

#include "Plants.hpp"
#include "Pea.hpp"
#include "Util/Animation.hpp"
#include "Config.hpp"
#include "AbilityInterfaces.hpp"
#include "GameWorld.hpp"
#include "TargetingStrategy.hpp" // 已引入策略介面

class Peashooter : public Plant, public IAttacker {
public:
    enum class AnimState { IDLE, ATTACKING };

    Peashooter(glm::vec2 pos) : Plant(PlantType::PEASHOOTER, pos, Config::PEASHOOTER_HP) {
        m_CurrentAnim = AnimState::IDLE;
        m_LastAppliedAnim = -1;
        m_FireTimer = 0.0f;

        // ★ 修改重點 1：在建構子中綁定「全線射擊」策略
        m_Targeting = std::make_shared<ForwardTargeting>();
    }

    void Attack(std::vector<std::shared_ptr<Zombie>>& rowZombies,
                Util::Renderer& renderer,
                GameWorld& world) override {

        // ★ 修改重點 2：刪除原本的 for 迴圈，改用策略來尋找目標
        auto target = m_Targeting->FindTarget(m_Transform.translation, rowZombies);

        // ★ 修改重點 3：原本的 if (foundZombie) 改為判斷 target 是否存在
        if (target != nullptr) {
            m_FireTimer += Util::Time::GetDeltaTime();

            // --- 狀態切換與發射的時間間隔 ---
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
            // 如果沒看到殭屍 (target == nullptr)，重置狀態
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

    // ★ 修改重點 4：宣告策略指標
    std::shared_ptr<ITargetingStrategy> m_Targeting;

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

        // ★ 修正：退回獨立建立 Animation 的寫法，讓每棵植物擁有自己的播放進度
        auto anim = std::make_shared<Util::Animation>(paths, true, 100, true);
        SetDrawable(anim);
    }
};

#endif