#ifndef SUNMANAGER_HPP
#define SUNMANAGER_HPP

#include "Sun.hpp"
#include "Util/Renderer.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include <vector>

class SunManager {
public:
    SunManager(Util::Renderer& renderer) : m_Renderer(renderer) {}

    void Update() {
        // --- 1. 自然掉落邏輯 (確保計時器運作) ---
        m_SpawnTimer += Util::Time::GetDeltaTime();
        if (m_SpawnTimer >= 10.0f) {
            SpawnSkySun();
            m_SpawnTimer = 0.0f;
        }

        // --- 2. 更新與點擊判定 ---
        for (auto it = m_Suns.begin(); it != m_Suns.end(); ) {
            (*it)->Update();

            // 增加寬容度：檢查是否被點擊
            if ((*it)->IsClicked()) {
                m_SunCount += 25;
                m_Renderer.RemoveChild(*it); // 從渲染層移除
                it = m_Suns.erase(it);      // 從容器移除
                LOG_INFO("Sun collected! Total: {}", m_SunCount);
            } else {
                ++it;
            }
        }
    }

    void AddSunFromPlant(glm::vec2 pos) {
        // 向日葵產出的陽光：設定為彈跳模式 (true)
        auto sun = std::make_shared<Sun>(pos, pos, true);
        m_Suns.push_back(sun);
        m_Renderer.AddChild(sun);
    }

    void SpawnSkySun() {
        // 隨機生成天空掉落的陽光
        float randomX = static_cast<float>((rand() % 600) - 300);
        float targetY = static_cast<float>((rand() % 400) - 200);
        auto sun = std::make_shared<Sun>(glm::vec2(randomX, 400.0f), glm::vec2(randomX, targetY), false);
        m_Suns.push_back(sun);
        m_Renderer.AddChild(sun);
    }

    void ClearSunsFromRenderer() {
        for (auto& sun : m_Suns) m_Renderer.RemoveChild(sun);
        m_Suns.clear();
    }

    void Reset() {
        ClearSunsFromRenderer();
        m_SunCount = 50;
        m_SpawnTimer = 0.0f;
    }

    int GetSunCount() const { return m_SunCount; }
    void SpendSun(int amount) { m_SunCount -= amount; }

private:
    Util::Renderer& m_Renderer;
    std::vector<std::shared_ptr<Sun>> m_Suns;
    float m_SpawnTimer = 0.0f;
    int m_SunCount = 50;
};

#endif