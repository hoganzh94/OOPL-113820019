#ifndef CHOMPER_HPP
#define CHOMPER_HPP

#include "Plants.hpp"
#include "AbilityInterfaces.hpp"
#include "Config.hpp"
#include "TargetingStrategy.hpp" // 引入策略

class Chomper : public Plant, public IAttacker {
public:
    enum class State { IDLE, CHOMPING, CHEWING };

    Chomper(glm::vec2 pos) : Plant(PlantType::CHOMPER, pos, Config::CHOMPER_HP) {
        m_CurrentState = State::IDLE;
        m_LastAppliedState = -1;
        m_StateTimer = 0.0f;

        // ★ 掛載近戰策略，攻擊範圍設為 135.0f
        m_Targeting = std::make_shared<MeleeTargeting>(135.0f);
    }

    void Attack(std::vector<std::shared_ptr<Zombie>>& rowZombies,
                Util::Renderer& renderer,
                GameWorld& world) override {
        // 只有 IDLE 狀態能觸發攻擊
        if (m_CurrentState != State::IDLE) return;

        // ★ 使用策略尋找目標
        auto target = m_Targeting->FindTarget(m_Transform.translation, rowZombies);

        if (target != nullptr) {
            m_CurrentState = State::CHOMPING;
            m_StateTimer = 0.0f;
            target->TakeDamage(9999); // 觸發瞬間秒殺殭屍
        }
    }

    void Update() override {
        float dt = Util::Time::GetDeltaTime();

        if (m_CurrentState == State::CHOMPING) {
            m_StateTimer += dt;
            if (m_StateTimer >= 0.6f) { // 咬合動作持續的時間
                m_CurrentState = State::CHEWING;
                m_StateTimer = 0.0f;
            }
        } else if (m_CurrentState == State::CHEWING) {
            m_StateTimer += dt;
            if (m_StateTimer >= Config::CHOMPER_DIGEST_TIME) {
                m_CurrentState = State::IDLE;
                m_StateTimer = 0.0f;
            }
        }

        if (static_cast<int>(m_CurrentState) != m_LastAppliedState) {
            ApplyAnimation(m_CurrentState);
            m_LastAppliedState = static_cast<int>(m_CurrentState);
        }
    }

private:
    State m_CurrentState;
    int m_LastAppliedState;
    float m_StateTimer;
    std::shared_ptr<ITargetingStrategy> m_Targeting;

    void ApplyAnimation(State state) {
        std::vector<std::string> paths;
        std::string base = std::string(RESOURCE_DIR) + "/Image/Plant/Chomper/";
        std::string folder;
        int frames = 0;

        if (state == State::IDLE) {
            folder = "Idle/Chomper - Idle ";
            frames = 8;
        } else if (state == State::CHOMPING) {
            folder = "Chomping/Chomper - Chomping ";
            frames = 6;
        } else {
            folder = "Chewing/Chomper - Chewing ";
            frames = 3;
        }

        for (int i = 1; i <= frames; ++i) {
            paths.push_back(base + folder + std::to_string(i) + ".png");
        }

        SetDrawable(std::make_shared<Util::Animation>(paths, true, 100, true));
    }
};

#endif