#ifndef BUCKETHEADZOMBIE_HPP
#define BUCKETHEADZOMBIE_HPP

#include "NormalZombie.hpp"
#include "Config.hpp"

class BucketHeadZombie : public NormalZombie {
public:
    BucketHeadZombie(glm::vec2 pos) : NormalZombie(pos) {
        m_Hp = Config::NORMAL_ZOMBIE_HP;
        m_ArmorHp = Config::BUCKETHEAD_ARMOR_HP;
        m_MaxArmorHp = Config::BUCKETHEAD_ARMOR_HP;
        m_Speed = Config::BUCKETHEAD_SPEED;
    }

    // 傷害判定邏輯與路障殭屍一致：優先扣除鐵桶血量
    void TakeDamage(int damage) override {
        if (m_ArmorHp > 0) {
            m_ArmorHp -= damage;
            m_HurtTimer = 0.15f; 

            if (m_ArmorHp <= 0) {
                int overflow = -m_ArmorHp;
                m_ArmorHp = 0;
                // 鐵桶掉落，剩餘傷害由殭屍本體吸收
                NormalZombie::TakeDamage(overflow);
            }
        } else {
            NormalZombie::TakeDamage(damage);
        }
        UpdateAnimation(m_HurtTimer > 0, m_SlowTimer > 0);
    }

    void Update() override {
        if (m_HurtTimer > 0) {
            m_HurtTimer -= Util::Time::GetDeltaTime();
            if (m_HurtTimer <= 0) UpdateAnimation(false, m_SlowTimer > 0);
        }
        NormalZombie::Update();
    }

protected:
    int m_ArmorHp;
    int m_MaxArmorHp;
    float m_HurtTimer = 0.0f;
    int m_LastAnimState = -1;

    void UpdateAnimation(bool isHurt, bool isSlowed) override {
        // 如果鐵桶已經消失，使用普通殭屍動畫
        if (m_ArmorHp <= 0) {
            NormalZombie::UpdateAnimation(isHurt, isSlowed);
            return;
        }

        // 鐵桶損壞階段判定 (分三階段：完整、輕微受損、嚴重受損)
        int armorStage = 1;
        float ratio = (float)m_ArmorHp / m_MaxArmorHp;
        if (ratio > 0.7f) armorStage = 1;
        else if (ratio > 0.3f) armorStage = 2;
        else armorStage = 3;

        int action = (m_State == State::WALKING) ? 0 : 1;
        int combined = action * 10 + armorStage + (isHurt ? 100 : 0);

        if (combined != m_LastAnimState) {
            std::string actStr = (action == 0) ? "Walk" : "Eat";
            // 改為 BucketHead Zombie 資料夾
            std::string base = std::string(RESOURCE_DIR) + "/Image/Zombie/BucketHead Zombie/";

            // 拼接路徑，例如：.../BucketHead Zombie/Walk1/Buckethead Zombie - Walk1 1.png
            std::string pathPrefix = base + actStr + std::to_string(armorStage) +
                                     "/Buckethead Zombie - " + actStr + std::to_string(armorStage) + " ";

            std::vector<std::string> paths;
            for (int i = 1; i <= Config::BUCKETHEAD_ANIM_FRAMES; ++i) {
                paths.push_back(pathPrefix + std::to_string(i) + ".png");
            }

            SetDrawable(std::make_shared<Util::Animation>(paths, true, 150, true));
            m_LastAnimState = combined;
        }
    }
};

#endif