#ifndef WALLNUT_HPP
#define WALLNUT_HPP

#include "Plants.hpp"
#include "Config.hpp"
#include "Util/Animation.hpp"

class Wallnut : public Plant {
public:
    Wallnut(glm::vec2 pos) : Plant(PlantType::WALLNUT, pos, Config::WALLNUT_HP) {
        UpdateAnimation();
    }

    void Update() override {
        UpdateAnimation();
    }

private:
    int m_LastState = -1;

    void UpdateAnimation() {
        int state = (m_Hp > m_MaxHp * 0.66f) ? 1 : (m_Hp > m_MaxHp * 0.33f ? 2 : 3);
        if (state != m_LastState) {
            std::vector<std::string> paths;
            for (int i = 1; i <= 5; ++i)
                paths.push_back("C:/Users/user/ptsd-template/Resources/Image/Plant/Wallnut/Idle" + std::to_string(state) +
                                "/Wall-Nut - Idle" + std::to_string(state) + " " + std::to_string(i) + ".png");
            SetDrawable(std::make_shared<Util::Animation>(paths, true, 120, true));
            m_LastState = state;
        }
    }
};

#endif