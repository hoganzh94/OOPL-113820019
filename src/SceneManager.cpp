#include "SceneManager.hpp"
#include "CombatSystem.hpp"
#include "PlantFactory.hpp"
#include "LevelLoader.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "AllPlants.hpp"
#include "AllZombies.hpp"
#include "spdlog/spdlog.h"

SceneManager::SceneManager(Util::Renderer& renderer)
    : m_Renderer(renderer), m_Grid(std::make_shared<Grid>()), m_SeedChooserUI(renderer) {
    // 這裡只做簡單初始化，資源載入交給 InitializeResources
}

void SceneManager::InitializeResources() {
    // --- 選單資源 ---
    float menuScale = 1.75f;
    float offsetX = 224.0f, offsetY = 168.0f;
    auto loadImage = [](const std::string& path, float scale, glm::vec2 pos) {
        auto obj = std::make_shared<Util::GameObject>();
        obj->SetDrawable(std::make_shared<Util::Image>(path));
        obj->m_Transform.scale = {scale, scale};
        obj->m_Transform.translation = pos;
        return obj;
    };

    std::string base = RESOURCE_DIR;

    m_MenuTopLeft     = loadImage(base + "/Image/Scene/Menu1.png", menuScale, {-offsetX, offsetY});
    m_MenuTopRight    = loadImage(base + "/Image/Scene/Menu2.png", menuScale, {offsetX, offsetY});
    m_MenuBottomLeft  = loadImage(base + "/Image/Scene/Menu3.png", menuScale, {-offsetX, -offsetY});
    m_MenuBottomRight = loadImage(base + "/Image/Scene/Menu4.png", menuScale, {offsetX, -offsetY});

    // --- 遊戲背景 ---
    m_GameSceneBG = std::make_shared<Util::GameObject>();
    m_Lawn = std::make_shared<Util::GameObject>();

    // --- UI 與 陽光管理 ---
    m_PacketManager = std::make_shared<SeedPacketManager>(m_Renderer);
    m_PacketManager->Initialize();
    m_SunManager = std::make_shared<SunManager>(m_Renderer);

    m_SunTextObj = std::make_shared<Util::GameObject>();
    m_SunTextDrawable = std::make_shared<Util::Text>(base + "/Smile Delight (Demo_Font).otf", 30, "50");
    m_SunTextObj->SetDrawable(m_SunTextDrawable);
    m_SunTextObj->m_Transform.translation = {500.0f, 320.0f};
    m_SunTextObj->SetZIndex(12.0f);

    // --- 狀態物件 ---
    m_GameOver = std::make_shared<Util::GameObject>();
    m_GameOver->SetDrawable(std::make_shared<Util::Image>(base + "/Image/Scene/Game Over Scene.png"));
    m_GameOver->SetZIndex(50.0f);

    m_ProgressBarBG = std::make_shared<Util::GameObject>();
    m_ProgressBarBG->SetDrawable(std::make_shared<Util::Image>(base + "/Image/Scene/Progress Bar - Bar.png"));
    m_ProgressBarBG->m_Transform.translation = {-450.0f, -320.0f}; // 底部中心
    m_ProgressBarBG->SetZIndex(18.0f);

    m_ProgressBarFill = std::make_shared<Util::GameObject>();
    // 這裡的圖片如果是那片「小葉子」或「進度頭」
    m_ProgressBarFill->SetDrawable(std::make_shared<Util::Image>(base + "/Image/Scene/Progress Bar - tag.png"));
    m_ProgressBarFill->m_Transform.translation = {-580.0f, -320.0f}; // 初始在最左邊
    m_ProgressBarFill->SetZIndex(19.0f); // 層級較高，確保看得到

    m_WinTextObj = std::make_shared<Util::GameObject>();
    auto winText = std::make_shared<Util::Text>(base + "/Smile Delight (Demo_Font).otf", 80, "YOU WIN", Util::Color::FromRGB(255, 255, 0));
    m_WinTextObj->SetDrawable(winText);
    m_WinTextObj->m_Transform.translation = {0.0f, 50.0f};
    m_WinTextObj->SetZIndex(30.0f);

    m_NextLevelHintObj = std::make_shared<Util::GameObject>();
    auto hintText = std::make_shared<Util::Text>(base + "/Smile Delight (Demo_Font).otf", 40, "Press ENTER for Next Level", Util::Color::FromRGB(255, 255, 255));
    m_NextLevelHintObj->SetDrawable(hintText);
    m_NextLevelHintObj->m_Transform.translation = {0.0f, -50.0f};
    m_NextLevelHintObj->SetZIndex(30.0f);

    m_StartBanner = std::make_shared<Util::GameObject>();
    m_StartBanner->SetZIndex(30.0f); // 確保在最上層
    m_StartBanner->m_Transform.translation = {0.0f, 0.0f}; // 畫面中央
    m_StartBanner->SetVisible(false);

    m_SeedChooserUI.Initialize();
}

void SceneManager::Update() {
    if (m_Phase == LevelPhase::SEED_CHOOSER)
    {
        if (m_SeedChooserUI.Update())
        {
            m_SelectedPlants = m_SeedChooserUI.GetSelectedPlants();
            m_SeedChooserUI.Hide();

            if (m_PacketManager)
            {
                m_PacketManager->InitializeWith(m_SelectedPlants);
                m_PacketManager->SetVisibleStatus(true);
                for (auto& p : m_PacketManager->GetPackets())
                {
                    m_Renderer.AddChild(p);
                }
            }
            m_Phase = LevelPhase::DAY_LEVEL;
        }
        return;
    }

    if (m_Phase != LevelPhase::DAY_LEVEL) return;

    if (!m_IsBannerFinished) {
        m_StartBannerTimer += Util::Time::GetDeltaTime();
        // 把 m_CurrentBannerPath 傳進去
        m_IsBannerFinished = m_UIController.UpdateStartBanner(m_StartBannerTimer, m_StartBanner, m_CurrentBannerPath);
        return;
    }

    float currentProgress = m_LevelController.GetProgress();

    // 呼叫我們之前寫的 UIController 函式
    m_UIController.UpdateProgressBar(currentProgress, m_ProgressBarFill);

    if (m_SunManager) {
        m_SunManager->Update();
        m_UIController.UpdateSunCount(m_SunManager->GetSunCount(), m_SunTextDrawable);
    }
    m_UIController.UpdateSunCount(m_SunManager->GetSunCount(), m_SunTextDrawable);
    // 1. 系統更新
    m_World.UpdateAll(m_Renderer);
    CombatSystem::Update(m_World, m_Renderer);
    m_LevelController.Update(m_World, m_Renderer);

    // 2. 先清理死亡實體 (這步很重要，確保 GetZombies() 反映真實存活數)
    m_World.RemoveDeadEntities(m_Renderer);

    // 3. 檢查勝利 (此時 world.GetZombies() 才是空的)
    if (m_LevelController.IsLevelComplete(m_World)) {
        m_Phase = LevelPhase::WIN;

        // 關鍵修正：確保這兩個物件被加入渲染清單
        m_Renderer.AddChild(m_WinTextObj);
        m_Renderer.AddChild(m_NextLevelHintObj);

        LOG_INFO("SceneManager: Level Win! Showing UI.");
    }

    // 4. 檢查失敗
    if (m_LevelController.IsGameOver()) {
        m_Phase = LevelPhase::FAIL;
        m_Renderer.AddChild(m_GameOver);
    }
}

void SceneManager::EnterLevel(int level) {
    LevelLoader::Initialize();
    ClearAll(); // 清除上一關殘留

    if (m_SunManager) {
        m_SunManager->Reset();           // 先清空舊太陽、重設基本狀態
        m_SunManager->InitializeLevel(level); // 再根據關卡設定 150 或 50
    }

    m_Phase = LevelPhase::DAY_LEVEL;
    m_LevelController.Initialize(level, m_Grid);

    LevelInfo info = LevelLoader::GetLevel(level);
    m_GameSceneBG->SetDrawable(std::make_shared<Util::Image>(info.bgPath));
    m_Lawn->SetDrawable(std::make_shared<Util::Image>(info.lawnPath));
    m_Lawn->m_Transform.translation = info.lawnPos;
    m_Lawn->m_Transform.scale = info.lawnScale;
    m_Lawn->SetZIndex(10.0f);

    m_Grid->Initialize(info.gridCoords);

    // 註冊渲染
    m_Renderer.AddChild(m_GameSceneBG);
    m_Renderer.AddChild(m_Lawn);
    m_Renderer.AddChild(m_SunTextObj);
    m_Renderer.AddChild(m_ProgressBarBG);
    m_Renderer.AddChild(m_ProgressBarFill);
    m_Renderer.AddChild(m_StartBanner);

    // 建立割草機
    int rowCount = static_cast<int>(info.gridCoords.size());
    for (int i = 0; i < rowCount; ++i) {
        if (!info.gridCoords[i].empty()) {
            auto mower = std::make_shared<LawnMower>(glm::vec2(-280.0f, info.gridCoords[i][0].y));
            m_World.AddMower(mower);
            m_Renderer.AddChild(mower);
        }
    }

    if (level >= 7)
    {
        m_Phase = LevelPhase::SEED_CHOOSER;
        m_UnlockedPlants = {PlantType::PEASHOOTER, PlantType::SUNFLOWER, PlantType::CHERRYBOMB, PlantType::WALLNUT, PlantType::POTATOMINE, PlantType::SNOWPEA, PlantType::CHOMPER};
        m_SelectedPlants.clear();

        if (m_PacketManager)m_PacketManager->SetVisibleStatus(false);
        m_SeedChooserUI.Show(m_UnlockedPlants);
    }else
    {
        m_Phase = LevelPhase::DAY_LEVEL;
        m_SelectedPlants = {PlantType::PEASHOOTER, PlantType::SUNFLOWER, PlantType::CHERRYBOMB, PlantType::WALLNUT, PlantType::POTATOMINE, PlantType::SNOWPEA};
        if (m_PacketManager)
        {
            m_PacketManager->Initialize();
            m_PacketManager->SetVisibleStatus(true);
            for (auto& p : m_PacketManager->GetPackets()){m_Renderer.AddChild(p);}

        }
    }

    m_StartBannerTimer = 0.0f;
    m_IsBannerFinished = false;
    m_CurrentBannerPath = "";
}

void SceneManager::AddPlant(PlantType type, glm::vec2 worldPos) {
    for (auto& p : m_World.GetPlants()) {
        if (glm::distance(p->m_Transform.translation, worldPos) < 10.0f) return;
    }
    // 修復 PlantType 轉換錯誤
    auto newPlant = PlantFactory::CreatePlant(type, worldPos, m_SunManager);
    if (newPlant) {
        m_World.AddPlant(newPlant);
        m_Renderer.AddChild(newPlant);
    }
}

void SceneManager::ClearAll() {
    m_Renderer.RemoveChild(m_MenuTopLeft);
    m_Renderer.RemoveChild(m_MenuTopRight);
    m_Renderer.RemoveChild(m_MenuBottomLeft);
    m_Renderer.RemoveChild(m_MenuBottomRight);
    m_Renderer.RemoveChild(m_GameSceneBG);
    m_Renderer.RemoveChild(m_Lawn);
    m_Renderer.RemoveChild(m_SunTextObj);
    m_Renderer.RemoveChild(m_GameOver);
    m_Renderer.RemoveChild(m_WinTextObj);
    m_Renderer.RemoveChild(m_NextLevelHintObj);
    m_Renderer.RemoveChild(m_ProgressBarBG);
    m_Renderer.RemoveChild(m_ProgressBarFill);
    m_Renderer.RemoveChild(m_StartBanner);

    if (m_PacketManager) {
        for (auto& p : m_PacketManager->GetPackets()) m_Renderer.RemoveChild(p);
        m_PacketManager->SetVisibleStatus(false);
    }

    m_World.Clear(m_Renderer);
    if (m_SunManager) m_SunManager->ClearSunsFromRenderer();
}

void SceneManager::SwitchToMenu() {
    ClearAll();
    m_Phase = LevelPhase::MENU;
    m_Renderer.AddChild(m_MenuTopLeft);
    m_Renderer.AddChild(m_MenuTopRight);
    m_Renderer.AddChild(m_MenuBottomLeft);
    m_Renderer.AddChild(m_MenuBottomRight);
}