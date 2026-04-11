#ifndef CHOMPER_HPP
#define CHOMPER_HPP

#include "Plants.hpp"
#include "AbilityInterfaces.hpp"
#include "Config.hpp"

class Chomper : public Plant, public IAttacker {
public:
    enum class State { IDLE, CHOMPING, CHEWING };

    Chomper(glm::vec2 pos) : Plant(PlantType::CHOMPER, pos, Config::CHOMPER_HP) {
        m_CurrentState = State::IDLE;
        m_LastAppliedState = -1;
        m_StateTimer = 0.0f;
    }

    void Attack(std::vector<std::shared_ptr<Zombie>>& rowZombies, Util::Renderer& renderer, GameWorld& world) override {
        // 只有 IDLE 狀態能觸發攻擊
        if (m_CurrentState != State::IDLE) return;

        float attackRange = 135.0f;
        for (auto& z : rowZombies) {
            float dist = z->GetX() - m_Transform.translation.x;
            if (!z->IsDead() && dist > 0 && dist <= attackRange) {
                // 觸發瞬間：切換狀態並秒殺殭屍
                m_CurrentState = State::CHOMPING;
                m_StateTimer = 0.0f;
                z->TakeDamage(9999);
                return;
            }
        }
    }

    void Update() override {
        float dt = Util::Time::GetDeltaTime();

        // 1. 處理狀態演進計時 (放在動畫更新之前)
        if (m_CurrentState == State::CHOMPING) {
            m_StateTimer += dt;
            // 這裡設定咬合動作持續的時間 (0.6秒)
            if (m_StateTimer >= 0.6f) {
                m_CurrentState = State::CHEWING;
                m_StateTimer = 0.0f;
            }
        }
        else if (m_CurrentState == State::CHEWING) {
            m_StateTimer += dt;
            // 消化 60 秒
            if (m_StateTimer >= Config::CHOMPER_DIGEST_TIME) {
                m_CurrentState = State::IDLE;
                m_StateTimer = 0.0f;
            }
        }

        // 2. 只有狀態發生「改變」時，才更換 Animation 物件
        if (static_cast<int>(m_CurrentState) != m_LastAppliedState) {
            ApplyAnimation(m_CurrentState);
            m_LastAppliedState = static_cast<int>(m_CurrentState);
        }
    }

private:
    State m_CurrentState;
    int m_LastAppliedState;
    float m_StateTimer;

    void ApplyAnimation(State state) {
        std::vector<std::string> paths;
        std::string base = "C:/Users/user/OOPL-113820019/Resources/Image/Plant/Chomper/";
        std::string folder;
        int frames = 0;

        // 全都使用 loop = true 避開框架 Bug
        if (state == State::IDLE) {
            folder = "Idle/Chomper - Idle "; frames = 8;
        } else if (state == State::CHOMPING) {
            folder = "Chomping/Chomper - Chomping "; frames = 6;
        } else {
            folder = "Chewing/Chomper - Chewing "; frames = 3;
        }

        for (int i = 1; i <= frames; ++i) {
            paths.push_back(base + folder + std::to_string(i) + ".png");
        }

        // 這裡固定用 loop = true，切換交給 Update 裡的 m_StateTimer 處理
        SetDrawable(std::make_shared<Util::Animation>(paths, true, 100, true));
    }
};

#endif