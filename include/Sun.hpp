#ifndef SUN_HPP
#define SUN_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Time.hpp"

class Sun : public Util::GameObject {
public:
    // startPos: 產生位置, targetPos: 掉落/彈跳目標位置
    // isFromPlant: 是否為向日葵產出
    Sun(glm::vec2 startPos, glm::vec2 targetPos, bool isFromPlant = false)
        : m_TargetPos(targetPos), m_IsFromPlant(isFromPlant) {

        // 修正：無論來源，產出的陽光物件統一使用躍動的 Sun.png
        SetDrawable(std::make_shared<Util::Image>("C:/Users/user/OOPL-113820019/Resources/Image/Scene/Sun.png"));

        m_Transform.translation = startPos;
        m_Transform.scale = {2.0f, 2.0f};
        SetZIndex(20.0f); // 確保在植物與草地之上

        if (m_IsFromPlant) {
            m_JumpVelocity = 250.0f; // 向上彈跳的初速度
        }
    }

    void Update() {
        if (m_IsFromPlant) {
            // --- 向日葵陽光邏輯：彈跳動畫 ---
            if (m_JumpTimer < 1.0f) { // 彈跳動畫持續約 1 秒
                m_JumpTimer += Util::Time::GetDeltaTime();

                // 更新位置
                m_Transform.translation.y += m_JumpVelocity * Util::Time::GetDeltaTime();
                m_Transform.translation.x += 30.0f * Util::Time::GetDeltaTime(); // 稍微向右偏移

                // 更新速度（重力模擬）
                m_JumpVelocity -= 500.0f * Util::Time::GetDeltaTime();
            }
        } else {
            // --- 自然掉落陽光邏輯：直線掉落 ---
            if (m_Transform.translation.y > m_TargetPos.y) {
                m_Transform.translation.y -= 150.0f * Util::Time::GetDeltaTime();
            }
        }
    }

    bool IsClicked() {
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            glm::vec2 mousePos = Util::Input::GetCursorPosition();
            float dist = glm::distance(mousePos, m_Transform.translation);
            return dist < 45.0f; // 點擊半徑
        }
        return false;
    }

private:
    glm::vec2 m_TargetPos;
    bool m_IsFromPlant;
    float m_JumpVelocity = 0.0f;
    float m_JumpTimer = 0.0f;
};

#endif