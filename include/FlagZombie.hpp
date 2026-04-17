#ifndef FLAGZOMBIE_HPP
#define FLAGZOMBIE_HPP

#include "NormalZombie.hpp"

class FlagZombie : public NormalZombie {
public:
    // 旗幟殭屍血量與普通一樣，但速度稍微快一點 (例如 40.0f)
    FlagZombie(glm::vec2 pos) : NormalZombie(pos) {
        m_Speed = 30.0f;
        m_Hp = 200;
        m_MaxHp = 200;
    }

protected:
    // 改寫動畫路徑，指向 Flag Zombie 的資料夾
    void UpdateAnimation(bool isHurt, bool isSlowed) override {
        int currentAnimState = 0;
        if (m_State == State::WALKING) {
            currentAnimState = isHurt ? 1 : 0;
        } else {
            currentAnimState = isHurt ? 3 : 2;
        }

        if (currentAnimState != m_LastAnimState) {
            std::vector<std::string> paths;
            std::string base = std::string(RESOURCE_DIR) + "/Image/Zombie/Flag Zombie/";
            std::string folder;
            int frames = 0;

            switch (currentAnimState) {
            case 0: folder = "Walk1/Flag Zombie - Walk1 "; frames = 7; break; // 假設格數
            case 1: folder = "Walk2/Flag Zombie - Walk2 "; frames = 7; break;
            case 2: folder = "Eat1/Flag Zombie - Eat1 "; frames = 7; break;
            case 3: folder = "Eat2/Flag Zombie - Eat2 "; frames = 7; break;
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