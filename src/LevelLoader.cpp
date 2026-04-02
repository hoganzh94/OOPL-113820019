#include "LevelLoader.hpp"
#include "Util/Logger.hpp"

// 靜態成員初始化
std::map<int, LevelInfo> LevelLoader::m_Data;
bool LevelLoader::m_IsInitialized = false;

void LevelLoader::Initialize() {
    if (m_IsInitialized) return;

    // --- 版型 A：第 1 關 (1 行草坪) ---
    LevelInfo lv1;
    lv1.bgPath = "C:/Users/user/ptsd-template/Resources/Image/Scene/Levels - Day.png";
    lv1.lawnPath = "C:/Users/user/ptsd-template/Resources/Image/Scene/Levels - Day1.png";
    lv1.lawnPos = {120.0f, -5.0f};
    lv1.lawnScale = {3.1f, 3.1f};
    lv1.gridCoords = {
        {{-200, 0}, {-128, 0}, {-50, 0}, {40, 0}, {115, 0}, {195, 0}, {275, 0}, {345, 0}, {440, 0}}
    };
    m_Data[1] = lv1;

    // --- 版型 B：第 2 & 3 關 (3 行草坪) ---
    LevelInfo lv3;
    lv3.bgPath = "C:/Users/user/ptsd-template/Resources/Image/Scene/Levels - Day.png";
    lv3.lawnPath = "C:/Users/user/ptsd-template/Resources/Image/Scene/Levels - Day2Lawn.png";
    lv3.lawnPos = {120.0f, -5.0f};
    lv3.lawnScale = {3.1f, 3.1f};
    lv3.gridCoords = {
        {{-200, 100}, {-128, 100}, {-50, 100}, {40, 100}, {115, 100}, {195, 100}, {275, 100}, {345, 100}, {440, 100}},
        {{-200, 0},   {-128, 0},   {-50, 0},   {40, 0},   {115, 0},   {195, 0},   {275, 0},   {345, 0},   {440, 0}},
        {{-200, -100},{-128, -100},{-50, -100},{40, -100},{115, -100},{195, -100},{275, -100},{345, -100},{440, -100}}
    };
    m_Data[2] = lv3;
    m_Data[3] = lv3;

    // --- 版型 C：第 4 關到第 10 關 (標準 5 行草坪) ---
    LevelInfo lv5;
    lv5.bgPath = "C:/Users/user/ptsd-template/Resources/Image/Scene/Levels - Day.png";
    lv5.lawnPath = "C:/Users/user/ptsd-template/Resources/Image/Scene/Levels - DayLawn.png";
    lv5.lawnPos = {120.0f, -15.0f};
    lv5.lawnScale = {3.1f, 3.1f};
    lv5.gridCoords = {
        {{-200, 175}, {-128, 175}, {-50, 175}, {40, 175}, {115, 175}, {195, 175}, {275, 175}, {345, 175}, {440, 175}},
        {{-200, 70},  {-128, 70},  {-50, 70},  {40, 70},  {115, 70},  {195, 70},  {275, 70},  {345, 70},  {440, 70}},
        {{-200, -35}, {-128, -35}, {-50, -35}, {40, -35}, {115, -35}, {195, -35}, {275, -35}, {345, -35}, {440, -35}},
        {{-200, -125},{-128, -125},{-50, -125},{40, -125},{115, -125},{195, -125},{275, -125},{345, -125},{440, -125}},
        {{-200, -230},{-128, -230},{-50, -230},{40, -230},{115, -230},{195, -230},{275, -230},{345, -230},{440, -230}}
    };

    // 使用迴圈填充 4 到 10 關，避免重複撰寫相同的資料
    for (int i = 4; i <= 10; ++i) {
        m_Data[i] = lv5;
    }

    m_IsInitialized = true;
    LOG_INFO("LevelLoader: All 10 levels data initialized successfully.");
}

LevelInfo LevelLoader::GetLevel(int level) {
    // 雙重保險：如果資料庫是空的，先初始化
    if (!m_IsInitialized) Initialize();

    // 如果請求的關卡不存在，回傳第一關
    if (m_Data.find(level) == m_Data.end()) {
        LOG_WARN("LevelLoader: Level {} not found, returning Level 1.", level);
        return m_Data[1];
    }
    return m_Data[level];
}