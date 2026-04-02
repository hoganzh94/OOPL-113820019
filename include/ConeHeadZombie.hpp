#ifndef CONEHEADZOMBIE_HPP
#define CONEHEADZOMBIE_HPP

#include "NormalZombie.hpp"
#include "Config.hpp"

class ConeHeadZombie : public NormalZombie {
public:
    ConeHeadZombie(glm::vec2 pos) : NormalZombie(pos) {
        m_Hp = Config::NORMAL_ZOMBIE_HP;
        m_ArmorHp = Config::CONEHEAD_ARMOR_HP;
        m_MaxArmorHp = Config::CONEHEAD_ARMOR_HP;
        m_Speed = Config::CONEHEAD_SPEED;
    }

    // 關鍵修正：必須有 override！否則系統只會執行基類的 TakeDamage
    void TakeDamage(int damage) override {
        if (m_ArmorHp > 0) {
            m_ArmorHp -= damage;
            m_HurtTimer = 0.15f; // 觸發受傷閃爍計時

            if (m_ArmorHp <= 0) {
                int overflow = -m_ArmorHp;
                m_ArmorHp = 0;
                // 路障破裂，剩下的打本體
                NormalZombie::TakeDamage(overflow);
            }
        } else {
            NormalZombie::TakeDamage(damage);
        }
        // 每次受傷強制檢查一次動畫切換
        UpdateAnimation(m_HurtTimer > 0);
    }

    void Update() override {
        // 更新受傷計時器，時間到就切換回正常動畫
        if (m_HurtTimer > 0) {
            m_HurtTimer -= Util::Time::GetDeltaTime();
            if (m_HurtTimer <= 0) UpdateAnimation(false);
        }
        NormalZombie::Update();
    }

protected:
    int m_ArmorHp;
    int m_MaxArmorHp;
    float m_HurtTimer = 0.0f;
    int m_LastAnimState = -1;

    void UpdateAnimation(bool isHurt) override {
        if (m_ArmorHp <= 0) {
            NormalZombie::UpdateAnimation(isHurt);
            return;
        }

        // 階段判定
        int armorStage = 1;
        float ratio = (float)m_ArmorHp / m_MaxArmorHp;
        if (ratio > 0.7f) armorStage = 1;
        else if (ratio > 0.3f) armorStage = 2;
        else armorStage = 3;

        int action = (m_State == State::WALKING) ? 0 : 1;
        // 狀態編碼：Action(10s) + Stage(1s) + Hurt(100s)
        int combined = action * 10 + armorStage + (isHurt ? 100 : 0);

        if (combined != m_LastAnimState) {
            std::string actStr = (action == 0) ? "Walk" : "Eat";
            std::string base = "C:/Users/user/ptsd-template/Resources/Image/Zombie/ConeHead Zombie/";

            // 注意：請確認資料夾名稱是否完全匹配 "Walk1", "Walk2", "Walk3"
            std::string pathPrefix = base + actStr + std::to_string(armorStage) +
                                     "/Conehead Zombie - " + actStr + std::to_string(armorStage) + " ";

            std::vector<std::string> paths;
            // 根據你的資源格數修正，假設是 7 格
            for (int i = 1; i <= 7; ++i) {
                paths.push_back(pathPrefix + std::to_string(i) + ".png");
            }

            SetDrawable(std::make_shared<Util::Animation>(paths, true, 150, true));
            m_LastAnimState = combined;
        }
    }
};

#endif