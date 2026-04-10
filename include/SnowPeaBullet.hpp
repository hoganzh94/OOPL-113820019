#ifndef SNOWPEABULLET_HPP
#define SNOWPEABULLET_HPP

#include "Pea.hpp"

class SnowPeaBullet : public Pea
{
public:
    SnowPeaBullet(glm::vec2 pos) : Pea(pos)
    {
        SetDrawable(std::make_shared<Util::Image>("C:/Users/user/OOPL-113820019/Resources/Image/Plant/Snow Pea/Pea/Snow Pea - Pea 1.png"));
    }

    bool IsIce() const { return true;}

    void Update() override {
        if (m_IsDead) return;

        if (!m_IsExploding) {
            // 移動邏輯 (同一般豌豆)
            m_Transform.translation.x += Config::PEA_SPEED * Util::Time::GetDeltaTime();
            if (m_Transform.translation.x > 500.0f) m_IsDead = true;
        }
        else {
            // --- 寒冰專屬爆炸動畫邏輯 ---
            m_ExplodeTimer += Util::Time::GetDeltaTime();

            // 這裡假設寒冰爆炸有 3 幀，每幀間隔 0.05s
            if (m_ExplodeTimer < 0.05f) {
                SetDrawable(std::make_shared<Util::Image>("C:/Users/user/OOPL-113820019/Resources/Image/Plant/Snow Pea/Pea/Snow Pea - Pea 2.png"));
            }
            else if (m_ExplodeTimer < 0.1f) {
                SetDrawable(std::make_shared<Util::Image>("C:/Users/user/OOPL-113820019/Resources/Image/Plant/Snow Pea/Pea/Snow Pea - Pea 3.png"));
            }
            else if (m_ExplodeTimer < 0.15f) {
                // 如果有第 4 幀可以使用，或是直接結束
                m_IsDead = true;
            }
        }
    }
};

#endif