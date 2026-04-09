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
    struct LevelData
    {
        int totalZombies;
        int hugeWaveCount;
        int wave1Zombies;
        int wave2Zombies;
    };

    void Initialize(int level, std::shared_ptr<Grid> grid) {
        m_CurrentLevel = level;
        m_Grid = grid;
        m_SpawnTimer = 0.0f;
        m_SpawnedCount = 0;
        m_IsGameOver = false;

        m_Wave1Triggered = false;
        m_Wave2Triggered = false;

        LoadLevelConfig(level);
        ResetSpawnInterval();
    }

    void Update(GameWorld& world, Util::Renderer& renderer) {
        if (m_IsGameOver) return;

        m_SpawnTimer += Util::Time::GetDeltaTime();

        // 1. 檢查是否需要生成殭屍
        if (m_SpawnedCount < m_LevelData.totalZombies && m_SpawnTimer >= m_CurrentInterval) {
            SpawnZombie(world, renderer, false);
            m_SpawnTimer = 0.0f;
            ResetSpawnInterval();
        }

        CheckHugeWave(world, renderer);

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
        bool allSpawned = (m_SpawnedCount >= m_LevelData.totalZombies);

        // 條件 2：畫面上目前沒有任何活著的殭屍 (從 GameWorld 取得)
        bool noZombiesLeft = world.GetZombies().empty();

        return allSpawned && noZombiesLeft;
    }

    float GetProgress() const {
        if (m_LevelData.totalZombies == 0) return 0.0f;
        // 必須使用 float 強制轉型，否則 int 相除會永遠是 0 或 1
        return static_cast<float>(m_SpawnedCount) / static_cast<float>(m_LevelData.totalZombies);
    }

private:
    void LoadLevelConfig(int level) {
        static std::map<int, LevelData> allLevels = {
            {1,  {8,  1, 3,  0}},
            {2,  {12, 1, 5,  0}},
            {3,  {15, 1, 5,  0}},
            {4,  {18, 1, 7,  0}},
            {5,  {30, 2, 8,  9}},
            {6,  {30, 1, 8,  0}},
            {7,  {35, 1, 9,  0}},
            {8,  {40, 1, 9,  0}},
            {9,  {50, 1, 10, 0}},
            {10, {60, 2, 10, 12}}
        };
        m_LevelData = allLevels[level];
    }

    void CheckHugeWave(GameWorld& world, Util::Renderer& renderer) {
        // 觸發條件：當剩下的殭屍數剛好等於一大波要出的數量時 (例如 Level 1 剩下 3 隻時)
        int remainingToSpawn = m_LevelData.totalZombies - m_SpawnedCount;

        // 第一波觸發
        if (!m_Wave1Triggered && remainingToSpawn <= m_LevelData.wave1Zombies) {
            m_Wave1Triggered = true;
            TriggerHugeWave(world, renderer, m_LevelData.wave1Zombies);
        }
        // 第二波觸發 (若該關卡有 2 次一大波)
        else if (m_LevelData.hugeWaveCount == 2 && !m_Wave2Triggered &&
                 m_Wave1Triggered && remainingToSpawn <= m_LevelData.wave2Zombies) {
            m_Wave2Triggered = true;
            TriggerHugeWave(world, renderer, m_LevelData.wave2Zombies);
                 }
    }

    void TriggerHugeWave(GameWorld& world, Util::Renderer& renderer, int count) {
        LOG_INFO("A HUGE WAVE OF ZOMBIES IS COMING!");

        // 1. 生成一個旗幟殭屍作為領隊 (你的註解要求)
        SpawnZombie(world, renderer, true);

        // 2. 生成其餘的一大波殭屍
        for (int i = 0; i < count - 1; ++i) {
            SpawnZombie(world, renderer, false);
        }
    }

    void SpawnZombie(GameWorld& world, Util::Renderer& renderer, bool forceFlag) {
        int rows = m_Grid->GetRows();
        int randomRow = rand() % rows;
        float spawnY = m_Grid->GetWorldPos(randomRow, 8).y;
        glm::vec2 spawnPos(500.0f, spawnY + 10.0f);

        std::shared_ptr<Zombie> newZombie = nullptr;

        // 根據關卡與進度決定殭屍種類 (OOP 邏輯封裝)
        if (forceFlag) {
            newZombie = std::make_shared<FlagZombie>(spawnPos);
        } else {
            // 隨機決定是路障還是普通 (Level 3 以上才有路障)
            if (m_CurrentLevel >= 3 && (rand() % 100 < 25)) {
                newZombie = std::make_shared<ConeHeadZombie>(spawnPos);
            } else {
                newZombie = std::make_shared<NormalZombie>(spawnPos);
            }
        }

        if (newZombie) {
            world.AddZombie(newZombie);
            renderer.AddChild(newZombie);
            m_SpawnedCount++;
        }
    }

    LevelData m_LevelData;
    int m_CurrentLevel = 1;
    int m_SpawnedCount = 0;
    float m_SpawnTimer = 0.0f;
    float m_CurrentInterval = 10.0f;
    bool m_IsGameOver;
    bool m_Wave1Triggered, m_Wave2Triggered;
    std::shared_ptr<Grid> m_Grid;

    void ResetSpawnInterval()
    {
        float min = 8.0f;
        float max = 12.0f;
        m_CurrentInterval = min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));

        LOG_INFO("Next zombie will spawn in {:.2f} seconds", m_CurrentInterval);
    }
};

#endif