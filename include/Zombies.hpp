#ifndef ZOMBIES_HPP
#define ZOMBIES_HPP

#include "Config.hpp"
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Time.hpp"
#include "Plants.hpp"

class Zombie : public Util::GameObject {
public:
    enum class State { WALKING, EATING };

    Zombie(int hp, float speed, glm::vec2 pos)
        : m_Hp(hp), m_MaxHp(hp), m_Speed(speed) {
        m_Transform.translation = pos;
        m_Transform.scale = {2.5f, 2.5f};
        SetZIndex(12.0f);
    }

    ~Zombie() override = default;

    // --- 修正：新增公有介面供 SceneManager 存取座標 ---
    [[nodiscard]] float GetX() const { return m_Transform.translation.x; }
    [[nodiscard]] float GetY() const { return m_Transform.translation.y; }

    virtual void Update()  {
        bool isHurt = (static_cast<float>(m_Hp) / m_MaxHp) <= 0.4f;

        if (m_State == State::WALKING) {
            m_Transform.translation.x -= m_Speed * Util::Time::GetDeltaTime();
            m_EatTimer = 0.0f;
        }
        else if (m_State == State::EATING && m_TargetPlant) {
            m_EatTimer += Util::Time::GetDeltaTime();
            if (m_EatTimer >= 1.0f) {
                m_TargetPlant->TakeDamage(Config::ZOMBIE_EAT_DAMAGE);
                m_EatTimer = 0.0f;
            }

            if (m_TargetPlant->IsDead()) {
                m_TargetPlant = nullptr;
                SetEating(false);
            }
        }
        UpdateAnimation(isHurt);
    }

    void SetTargetPlant(std::shared_ptr<Plant> plant) {
        m_TargetPlant = plant;
        SetEating(plant != nullptr);
    }

    void SetEating(bool eating) { m_State = eating ? State::EATING : State::WALKING; }
    // Zombie.hpp 的 TakeDamage
    virtual void TakeDamage(int d) {
        m_Hp -= d;
        if (m_Hp <= 0) {
            m_Hp = 0;
            m_State = State::WALKING; // 炸死了就不再是 EATING
            m_TargetPlant = nullptr;  // 清除目標
        }
    }
    bool IsDead() const { return m_Hp <= 0; }

protected:
    virtual void UpdateAnimation(bool isHurt) = 0;
    int m_Hp, m_MaxHp;
    float m_Speed;
    State m_State = State::WALKING;
    float m_EatTimer = 0.0f;
    std::shared_ptr<Plant> m_TargetPlant = nullptr;
    int m_LastAnimState = -1;
};

#endif