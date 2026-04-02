#ifndef POTATOMINE_HPP
#define POTATOMINE_HPP

#include "Plants.hpp"
#include "AbilityInterfaces.hpp"
#include "Config.hpp"
#include "Util/Animation.hpp" // 必須引入動畫類別
#include "Util/Time.hpp"      // 必須引入時間類別

class PotatoMine : public Plant, public ICollisionReactive {
public:
    enum class State { UNDERGROUND, POPPING_UP, READY, EXPLODED };

    PotatoMine(glm::vec2 pos) : Plant(PlantType::POTATOMINE, pos, Config::POTATOMINE_HP) {
        m_State = State::UNDERGROUND;
        SetDrawable(std::make_shared<Util::Image>("C:/Users/user/ptsd-template/Resources/Image/Plant/Potato mine/Pop up/Potato Mine - Pop up 1.png"));
    }

    void Update() override {
        if (m_IsDead) return;

        m_Timer += Util::Time::GetDeltaTime();

        // --- 核心修正：根據狀態播放對應動畫 ---
        if (m_State == State::UNDERGROUND && m_Timer >= 15.0f) {
            // 進入 Pop up 階段
            m_State = State::POPPING_UP;

            // 載入 Pop up 動畫 (不循環播放)
            std::vector<std::string> popUpPaths;
            popUpPaths.push_back("C:/Users/user/ptsd-template/Resources/Image/Plant/Potato mine/Pop up/Potato Mine - Pop up 2.png");
            popUpPaths.push_back("C:/Users/user/ptsd-template/Resources/Image/Plant/Potato mine/Pop up/Potato Mine - Pop up 3.png");
            SetDrawable(std::make_shared<Util::Animation>(popUpPaths, false, 200, true));
            m_Timer = 0.0f; // 重置計時器
        }
        else if (m_State == State::POPPING_UP && m_Timer >= 0.4f) {
            // Pop up 動畫播放完畢，進入 Ready 階段
            m_State = State::READY;

            // 載入 Idle 動畫 (循環播放)
            std::vector<std::string> idlePaths;
            idlePaths.push_back("C:/Users/user/ptsd-template/Resources/Image/Plant/Potato mine/Idle/Potato mine - Idle 1.png");
            idlePaths.push_back("C:/Users/user/ptsd-template/Resources/Image/Plant/Potato mine/Idle/Potato mine - Idle 2.png");
            SetDrawable(std::make_shared<Util::Animation>(idlePaths, true, 200, true));
            m_Timer = 0.0f;
        }
        else if (m_State == State::EXPLODED && m_Timer >= 0.5f) {
            // 爆炸動畫播放完畢，地雷死亡
            m_IsDead = true;
        }
    }

    // 實作 ICollisionReactive
    // PotatoMine.hpp 片段
    void OnCollision(std::shared_ptr<Zombie> zombie) override {
        // 只有在準備好(Idle)的情況下，碰到殭屍才會爆炸
        if (m_State == State::READY) {
            m_State = State::EXPLODED;
            m_Timer = 0.0f;

            // 視覺：換成爆炸圖
            SetDrawable(std::make_shared<Util::Image>("C:/Users/user/ptsd-template/Resources/Image/Plant/Potato mine/Potato Mine - Explosion.png"));
            m_Transform.scale = {3.5f, 3.5f}; // 爆炸圖片通常較大

            // 效果：對該殭屍造成致命傷害
            zombie->TakeDamage(1800);
            LOG_INFO("Potato Mine: SPUDOW!");
        }
    }

    bool IsReadyToExplode() const { return m_State == State::READY; }

private:
    State m_State;
    float m_Timer = 0.0f;
};

#endif