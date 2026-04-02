#ifndef LAWNMOWER_HPP
#define LAWNMOWER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Config.hpp"

class LawnMower : public Util::GameObject {
public:
    enum class State { IDLE, ACTIVATED, DEAD };

    LawnMower(glm::vec2 pos) : m_State(State::IDLE) {
        SetDrawable(std::make_shared<Util::Image>("C:/Users/user/ptsd-template/Resources/Image/Scene/Levels - weeding.png"));
        m_Transform.translation = pos;
        m_Transform.scale = {3.5f, 3.5f};
        SetZIndex(17.0f); // 介於草地 (10) 與植物 (11) 之間
    }

    void Update() {
        if (m_State == State::ACTIVATED) {
            // 高速衝向右側
            m_Transform.translation.x += 600.0f * Util::Time::GetDeltaTime();
            
            // 出鏡判定
            if (m_Transform.translation.x > 500.0f) {
                m_State = State::DEAD;
                m_Transform.translation.x = 500.0f;
            }
        }
    }

    void Activate() {
        if (m_State == State::IDLE) {
            m_State = State::ACTIVATED;
        }
    }

    [[nodiscard]] State GetState() const { return m_State; }
    [[nodiscard]] bool IsDead() const { return m_State == State::DEAD; }

private:
    State m_State;
};

#endif