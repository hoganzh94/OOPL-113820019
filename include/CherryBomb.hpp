#ifndef CHERRYBOMB_HPP
#define CHERRYBOMB_HPP

#include "Plants.hpp"
#include "AbilityInterfaces.hpp"
#include "Util/Animation.hpp"
#include "Config.hpp"

class CherryBomb : public Plant, public IExplosive {
public:
    CherryBomb(glm::vec2 pos) : Plant(PlantType::CHERRYBOMB, pos, Config::CHERRYBOMB_HP) {
        std::vector<std::string> paths;
        for (int i = 1; i <= 4; ++i)
            paths.push_back("C:/Users/user/ptsd-template/Resources/Image/Plant/Cherrybomb/Exploding/Cherry Bomb - Exploding " + std::to_string(i) + ".png");
        SetDrawable(std::make_shared<Util::Animation>(paths, false, 150, true));
        m_Timer = 0.0f;
    }

    void Update() override {
        m_Timer += Util::Time::GetDeltaTime();

        if (!m_Exploded && m_Timer >= 0.6f) {
            m_Exploded = true;
            m_NeedsDamageApply = true; // 標記給 CombatSystem 執行一次性傷害
            SetDrawable(std::make_shared<Util::Image>("C:/Users/user/ptsd-template/Resources/Image/Plant/Cherrybomb/Cherry Bomb - Explosion.png"));
            m_Transform.scale = {4.0f, 4.0f};
        }

        if (m_Exploded && m_Timer >= 1.1f) m_IsDead = true;
    }

    // 實作 IExplosive
    bool IsExplodingNow() const override { return m_NeedsDamageApply; }
    float GetExplosionRange() const override { return Config::CHERRYBOMB_RANGE; }
    int GetExplosionDamage() const override { return Config::CHERRYBOMB_DAMAGE; }

    // 自定義：讓 CombatSystem 讀完傷害後重置
    void ResetExplosionFlag() { m_NeedsDamageApply = false; }

private:
    float m_Timer;
    bool m_Exploded = false;
    bool m_NeedsDamageApply = false;
};

#endif