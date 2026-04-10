#ifndef SNOWPEA_HPP
#define SNOWPEA_HPP

#include "Plants.hpp"
#include "SnowPeaBullet.hpp"
#include "AbilityInterfaces.hpp"
#include "Config.hpp"

class SnowPea : public Plant, public IAttacker {
public:
    SnowPea(glm::vec2 pos) : Plant(PlantType::SNOWPEA, pos, Config::SNOWPEA_HP) {
        std::vector<std::string> idlePaths;
        for (int i = 1; i <= 8; ++i)
            idlePaths.push_back("C:/Users/user/OOPL-113820019/Resources/Image/Plant/Snow Pea/Idle/Snow Pea - Idle " + std::to_string(i) + ".png");
        m_IdleAnim = std::make_shared<Util::Animation>(idlePaths, true, 120, true);
        SetDrawable(m_IdleAnim);
    }

    void Attack(std::vector<std::shared_ptr<Zombie>>& rowZombies, Util::Renderer& renderer, GameWorld& world) override {
        bool foundZombie = false;
        for (auto& z : rowZombies) {
            if (!z->IsDead() && z->GetX() > m_Transform.translation.x) {
                foundZombie = true; break;
            }
        }

        if (foundZombie) {
            m_FireTimer += Util::Time::GetDeltaTime();
            if (m_FireTimer >= 1.5f) {
                // 關鍵：發射的是 SnowPeaBullet
                auto bullet = std::make_shared<SnowPeaBullet>(m_Transform.translation + glm::vec2(30.0f, 20.0f));
                renderer.AddChild(bullet);
                world.AddPea(bullet); // 同樣加入 Peas 容器
                m_FireTimer = 0.0f;
            }
        } else {
            m_FireTimer = 0.0f;
        }
    }

    void Update() override {}

private:
    float m_FireTimer = 0.0f;
    std::shared_ptr<Util::Animation> m_IdleAnim;
};

#endif