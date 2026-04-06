#ifndef SUNFLOWER_HPP
#define SUNFLOWER_HPP

#include "Plants.hpp"
#include "SunManager.hpp"
#include "Util/Animation.hpp"
#include "Config.hpp"

class Sunflower : public Plant {
public:
    Sunflower(glm::vec2 pos, std::shared_ptr<SunManager> sunMgr)
        : Plant(PlantType::SUNFLOWER, pos, Config::SUNFLOWER_HP), m_SunManager(sunMgr) {

        std::vector<std::string> idlePaths;
        for (int i = 1; i <= 6; ++i)
            idlePaths.push_back("C:/Users/user/ptsd-template/Resources/Image/Plant/Sunflower/Idle/Sunflower - Idle " + std::to_string(i) + ".png");
        m_IdleAnimation = std::make_shared<Util::Animation>(idlePaths, true, 120, true);
        SetDrawable(m_IdleAnimation);

        m_ProducingDrawable = std::make_shared<Util::Image>("C:/Users/user/ptsd-template/Resources/Image/Plant/Sunflower/Sunflower - Produced Sun.png");
        m_SunTimer = static_cast<float>(rand() % 10);
    }

    void Update() override {
        m_SunTimer += Util::Time::GetDeltaTime();
        if (m_SunTimer >= Config::SUN_PRODUCE_TIME) {
            if (m_SunManager) m_SunManager->AddSunFromPlant(m_Transform.translation);
            m_SunTimer = 0.0f;
            SetDrawable(m_IdleAnimation);
        }
        else if (m_SunTimer >= 20.0f && m_SunTimer < 20.1f) {
            SetDrawable(m_ProducingDrawable);
        }
    }

private:
    float m_SunTimer = 0.0f;
    std::shared_ptr<SunManager> m_SunManager;
    std::shared_ptr<Util::Animation> m_IdleAnimation;
    std::shared_ptr<Util::Image> m_ProducingDrawable;
};

#endif