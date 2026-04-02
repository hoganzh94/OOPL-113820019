#ifndef LEVELCONTROLLER_HPP
#define LEVELCONTROLLER_HPP

#include <memory>
#include <vector>
#include "GameWorld.hpp"
#include "LevelLoader.hpp"
#include "Config.hpp"
#include "AllZombies.hpp"
#include "Grid.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include "Util/Renderer.hpp"

class LevelController {
public:
    void Initialize(int level, std::shared_ptr<Grid> grid) {
        m_CurrentLevel = level;
        m_Grid = grid;
        m_SpawnTimer = 0.0f;
        m_SpawnedCount = 0;
        m_IsGameOver = false;
        m_HugeWaveTriggered = false;

        // 設定關卡配置 (可依需求移動至 LevelLoader)
        if (level == 1)      m_Config = {10, 10, 2}; 
        else if (level < 4)  m_Config = {20, 10, 5};
        else                 m_Config = {40, 15, 8};
    }

    void Update(GameWorld& world, Util::Renderer& renderer) {
        if (m_IsGameOver) return;

        m_SpawnTimer += Util::Time::GetDeltaTime();

        // 1. 檢查是否需要生成殭屍
        if (m_SpawnedCount < m_Config.totalZombies && m_SpawnTimer >= Config::ZOMBIE_SPAWN_INTERVAL) {
            SpawnZombie(world, renderer);
            m_SpawnTimer = 0.0f;
        }

        // 2. 檢查敗北條件 (殭屍入屋)
        for (auto& z : world.GetZombies()) {
            if (z->GetX() < -300.0f) {
                m_IsGameOver = true;
                LOG_INFO("LevelController: Game Over detected.");
                break;
            }
        }
    }

    bool IsGameOver() const { return m_IsGameOver; }

    bool IsLevelComplete(GameWorld& world) const {
        // 條件 1：設定的總殭屍數已經全部產生完畢
        bool allSpawned = (m_SpawnedCount >= m_Config.totalZombies);

        // 條件 2：畫面上目前沒有任何活著的殭屍 (從 GameWorld 取得)
        bool noZombiesLeft = world.GetZombies().empty();

        return allSpawned && noZombiesLeft;
    }

    float GetProgress() const {
        if (m_Config.totalZombies == 0) return 0.0f;
        // 必須使用 float 強制轉型，否則 int 相除會永遠是 0 或 1
        return static_cast<float>(m_SpawnedCount) / static_cast<float>(m_Config.totalZombies);
    }

private:
    void SpawnZombie(GameWorld& world, Util::Renderer& renderer) {
        int rows = m_Grid->GetRows();
        int randomRow = rand() % rows;
        float spawnY = m_Grid->GetWorldPos(randomRow, 8).y;
        glm::vec2 spawnPos(500.0f, spawnY + 10.0f);

        std::shared_ptr<Zombie> newZombie = nullptr;

        // 根據關卡與進度決定殭屍種類 (OOP 邏輯封裝)
        if (m_CurrentLevel > 1 && m_SpawnedCount > 0 && m_SpawnedCount % m_Config.flagInterval == 0) {
            newZombie = std::make_shared<FlagZombie>(spawnPos);
        } else if (m_CurrentLevel >= 3 && (rand() % 100 < 30)) {
            newZombie = std::make_shared<ConeHeadZombie>(spawnPos);
            m_SpawnedCount++; // 路障殭屍強度較高，額外佔一格計數
        } else {
            newZombie = std::make_shared<NormalZombie>(spawnPos);
        }

        if (newZombie) {
            world.AddZombie(newZombie);
            renderer.AddChild(newZombie);
            m_SpawnedCount++;
        }

        // 處理 Huge Wave
        if (!m_HugeWaveTriggered && (m_Config.totalZombies - m_SpawnedCount) <= m_Config.hugeWaveAt) {
            m_HugeWaveTriggered = true;
            LOG_INFO("LevelController: HUGE WAVE!");
            for(int i = 0; i < 3; i++) {
                int hRow = rand() % rows;
                auto hz = std::make_shared<NormalZombie>(glm::vec2(500.0f, m_Grid->GetWorldPos(hRow, 8).y + 10.0f));
                world.AddZombie(hz);
                renderer.AddChild(hz);
            }
        }
    }

    struct ConfigData { int totalZombies; int flagInterval; int hugeWaveAt; };
    ConfigData m_Config;
    int m_CurrentLevel = 1;
    int m_SpawnedCount = 0;
    float m_SpawnTimer = 0.0f;
    bool m_IsGameOver = false;
    bool m_HugeWaveTriggered = false;
    std::shared_ptr<Grid> m_Grid;
};

#endif