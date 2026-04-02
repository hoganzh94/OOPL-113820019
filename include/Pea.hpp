#ifndef PEA_HPP
#define PEA_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Config.hpp"

class Pea : public Util::GameObject {
public:
    Pea(glm::vec2 pos) : m_IsExploding(false), m_IsDead(false) {
        m_Transform.translation = pos;
        m_Transform.scale = {2.0f, 2.0f};
        SetZIndex(13.0f);
        SetDrawable(std::make_shared<Util::Image>("C:/Users/user/ptsd-template/Resources/Image/Plant/Peashooter/Pea/Peashooter - Pea 1.png"));
    }

    // 必須是 Update() 才能被 GameWorld 呼叫
    void Update() {
        if (m_IsDead) return;

        if (!m_IsExploding) {
            // 移動邏輯：x 座標增加
            m_Transform.translation.x += Config::PEA_SPEED * Util::Time::GetDeltaTime();

            // 邊界檢查：超過畫面就標記死亡
            if (m_Transform.translation.x > 500.0f) m_IsDead = true;
        } else {
            // 爆炸動畫邏輯
            m_ExplodeTimer += Util::Time::GetDeltaTime();
            if (m_ExplodeTimer >= 0.05f && m_ExplodeTimer < 0.1f) {
                SetDrawable(std::make_shared<Util::Image>("C:/Users/user/ptsd-template/Resources/Image/Plant/Peashooter/Pea/Peashooter - Pea 2.png"));
            } else if (m_ExplodeTimer >= 0.1f) {
                SetDrawable(std::make_shared<Util::Image>("C:/Users/user/ptsd-template/Resources/Image/Plant/Peashooter/Pea/Peashooter - Pea 3.png"));
                if (m_ExplodeTimer >= 0.15f) m_IsDead = true;
            }
        }
    }

    void Hit() {
        if (!m_IsExploding) {
            m_IsExploding = true;
            m_ExplodeTimer = 0.0f;
        }
    }

    bool IsDead() const { return m_IsDead; }
    bool IsExploding() const { return m_IsExploding; }

private:
    float m_ExplodeTimer = 0.0f;
    bool m_IsExploding;
    bool m_IsDead;
};

#endif