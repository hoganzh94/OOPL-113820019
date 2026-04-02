#ifndef NORMALZOMBIE_HPP
#define NORMALZOMBIE_HPP

#include "Zombies.hpp"
#include "Config.hpp"

class NormalZombie : public Zombie {
public:
    NormalZombie(glm::vec2 pos) : Zombie(Config::NORMAL_ZOMBIE_HP, Config::ZOMBIE_WALK_SPEED, pos) {} // HP: 200, Speed: 30

protected:
    void UpdateAnimation(bool isHurt) override {
        // 狀態組合編號：0:正常Walk, 1:負傷Walk, 2:正常Eat, 3:負傷Eat
        int currentAnimState = 0;
        if (m_State == State::WALKING) {
            currentAnimState = isHurt ? 1 : 0;
        } else {
            currentAnimState = isHurt ? 3 : 2;
        }

        // 只有狀態改變時才更換 Animation 物件
        if (currentAnimState != m_LastAnimState) {
            std::vector<std::string> paths;
            std::string base = "C:/Users/user/ptsd-template/Resources/Image/Zombie/Normal Zombie/";
            std::string folder;
            int frames = 0;

            switch (currentAnimState) {
            case 0: folder = "Walk1/Zombie - Walk1 "; frames = 7; break;
            case 1: folder = "Walk2/Zombie - Walk2 "; frames = 7; break;
            case 2: folder = "Eat1/Zombie - Eat1 "; frames = 7; break;
            case 3: folder = "Eat2/Zombie - Eat2 "; frames = 7; break;
            }

            for (int i = 1; i <= frames; ++i) {
                paths.push_back(base + folder + std::to_string(i) + ".png");
            }

            SetDrawable(std::make_shared<Util::Animation>(paths, true, 150, true));
            m_LastAnimState = currentAnimState;
        }
    }
};

#endif