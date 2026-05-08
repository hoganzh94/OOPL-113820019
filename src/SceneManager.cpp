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

    m_DragPlantObj = std::make_shared<Util::GameObject>();
    m_DragPlantObj->SetZIndex(90.0f);
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
            m_StartBannerTimer = 0.0f;
            m_IsBannerFinished = false;
            m_CurrentBannerPath = "";
            m_Phase = LevelPhase::DAY_LEVEL;
        }
        return;
    }

    if (m_Phase != LevelPhase::DAY_LEVEL) return;

    // --- 開場動畫檢查 ---
    if (!m_IsBannerFinished) {
        m_StartBannerTimer += Util::Time::GetDeltaTime();
        m_IsBannerFinished = m_UIController.UpdateStartBanner(m_StartBannerTimer, m_StartBanner, m_CurrentBannerPath);
        return; // 動畫還沒播完前，遊戲時間暫停
    }

    // --- UI 更新 ---
    float currentProgress = m_LevelController.GetProgress();
    m_UIController.UpdateProgressBar(currentProgress, m_ProgressBarFill);

    if (m_SunManager) {
        m_SunManager->Update();
        m_UIController.UpdateSunCount(m_SunManager->GetSunCount(), m_SunTextDrawable);
    }

    if (m_PacketManager) {
        m_PacketManager->Update(); // 讓卡槽偵測滑鼠點擊

        // 如果玩家剛剛點擊了一張可用的卡牌
        if (m_PacketManager->GetSelectedType() != PlantType::NONE) {
            // 如果原本手上已經有拿植物了，先把它從畫面上移除
            if (m_DraggingPlantType != PlantType::NONE) {
                m_Renderer.RemoveChild(m_DragPlantObj);
            }

            // 記錄現在拿著什麼植物
            m_DraggingPlantType = m_PacketManager->GetSelectedType();

            // 換上對應的植物圖片，並加到畫面上
            m_DragPlantObj->SetDrawable(std::make_shared<Util::Image>(GetPlantIdleImagePath(m_DraggingPlantType)));
            m_Renderer.AddChild(m_DragPlantObj);

            // 清除卡槽的選取紀錄，避免重複觸發
            m_PacketManager->ClearSelection();
        }
    }

    // ==========================================
    // ★ 狀態 A：手中已經拿著植物 (判斷種植與取消)
    // ==========================================
    if (m_DraggingPlantType != PlantType::NONE) {
        // 讓植物不斷更新至目前滑鼠所在的座標
        m_DragPlantObj->m_Transform.translation = Util::Input::GetCursorPosition();

        // ★ 關鍵防連點機制：只要玩家鬆開了左鍵，才允許下一次的種植點擊
        if (!Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            m_CanPlant = true;
        }

        // 偵測滑鼠右鍵 (MOUSE_RB)：取消種植
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
            m_DraggingPlantType = PlantType::NONE;
            m_Renderer.RemoveChild(m_DragPlantObj);
            m_CanPlant = false; // 取消時重置狀態
        }
        // 偵測滑鼠左鍵 (MOUSE_LB)：確認種植 (必須 m_CanPlant 為 true 才能種)
        else if (m_CanPlant && Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            int cost = GetPlantCost(m_DraggingPlantType);

            if (m_SunManager->GetSunCount() >= cost) {
                glm::vec2 snapPos = SnapToGrid(Util::Input::GetCursorPosition());

                if (AddPlant(m_DraggingPlantType, snapPos)) {
                    m_SunManager->ConsumeSun(cost);

                    if (m_PacketManager) {
                        m_PacketManager->StartPacketCooldown(m_DraggingPlantType);
                    }

                    m_DraggingPlantType = PlantType::NONE;
                    m_Renderer.RemoveChild(m_DragPlantObj);
                    m_CanPlant = false; // 種植成功後重置狀態
                } else {
                    LOG_INFO("種植失敗：這個位子已經有植物了！");
                }
            } else {
                LOG_INFO("種植失敗：陽光不足！");
            }
        }
    }
    // ==========================================
    // ★ 狀態 B：手上是空的 (判斷是否要拿起植物)
    // ==========================================
    else {
        if (m_PacketManager) {
            m_PacketManager->Update();
            if (m_PacketManager->GetSelectedType() != PlantType::NONE) {
                m_DraggingPlantType = m_PacketManager->GetSelectedType();
                m_DragPlantObj->SetDrawable(std::make_shared<Util::Image>(GetPlantIdleImagePath(m_DraggingPlantType)));
                m_DragPlantObj->m_Transform.scale = {2.5f, 2.5f};
                m_Renderer.AddChild(m_DragPlantObj);
                m_PacketManager->ClearSelection();

                m_CanPlant = false; // ★ 拿起的瞬間，強制鎖定種植行為，直到玩家鬆開滑鼠
            }
        }
    }

    // 1. 系統更新 (遊戲世界真正運轉)
    m_World.UpdateAll(m_Renderer);
    CombatSystem::Update(m_World, m_Renderer);
    m_LevelController.Update(m_World, m_Renderer);

    // 2. 先清理死亡實體
    m_World.RemoveDeadEntities(m_Renderer);

    // 3. 檢查勝利
    if (m_LevelController.IsLevelComplete(m_World)) {
        m_Phase = LevelPhase::WIN;
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

    m_UnlockedPlants = LevelLoader::GetUnlockedPlants(level);
    m_SelectedPlants.clear();

    if (level >= 8)
    {
        m_Phase = LevelPhase::SEED_CHOOSER;
        if (m_PacketManager) m_PacketManager->SetVisibleStatus(false);

        m_SeedChooserUI.Show(m_UnlockedPlants);
    }else
    {
        m_Phase = LevelPhase::DAY_LEVEL;

        m_SelectedPlants = m_UnlockedPlants;

        if (m_PacketManager)
        {
            m_PacketManager->InitializeWith(m_SelectedPlants);
            m_PacketManager->SetVisibleStatus(true);
            for (auto& p : m_PacketManager->GetPackets()) m_Renderer.AddChild(p);
        }
    }

    if (m_SunManager) {
        m_SunManager->Reset();           // 先清空舊太陽、重設基本狀態
        m_SunManager->InitializeLevel(level); // 再根據關卡設定 150 或 50
    }

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
    m_StartBanner->SetVisible(false);

    // 建立割草機
    int rowCount = static_cast<int>(info.gridCoords.size());
    for (int i = 0; i < rowCount; ++i) {
        if (!info.gridCoords[i].empty()) {
            auto mower = std::make_shared<LawnMower>(glm::vec2(-280.0f, info.gridCoords[i][0].y));
            m_World.AddMower(mower);
            m_Renderer.AddChild(mower);
        }
    }

    m_StartBannerTimer = 0.0f;
    m_IsBannerFinished = false;
    m_CurrentBannerPath = "";
}

bool SceneManager::AddPlant(PlantType type, glm::vec2 worldPos) {
    for (auto& p : m_World.GetPlants()) {
        if (glm::distance(p->m_Transform.translation, worldPos) < 10.0f) return false;
    }
    // 修復 PlantType 轉換錯誤
    auto newPlant = PlantFactory::CreatePlant(type, worldPos, m_SunManager);
    if (newPlant) {
        m_World.AddPlant(newPlant);
        m_Renderer.AddChild(newPlant);
        return true;
    }
    return false;
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

std::string SceneManager::GetPlantIdleImagePath(PlantType type)
{
    std::string base = std::string(RESOURCE_DIR) + "/Image/Plant/";
    switch(type) {
        // 請將這裡的 "Idle_1.png" 或路徑，改成你實際的植物第一幀圖片名稱
        case PlantType::PEASHOOTER: return base + "Peashooter/Idle/Peashooter - Idle 1.png";
        case PlantType::SUNFLOWER:  return base + "Sunflower/Idle/Sunflower - Idle 1.png";
        case PlantType::CHERRYBOMB: return base + "Cherrybomb/Exploding/Cherry Bomb - Exploding 1.png";
        case PlantType::WALLNUT:    return base + "Wallnut/Idle1/Wall-Nut - Idle1 1.png";
        case PlantType::POTATOMINE: return base + "Potato mine/Idle/Potato Mine - Idle 2.png";
        case PlantType::SNOWPEA:    return base + "Snow Pea/Idle/Snow Pea - Idle 1.png";
        case PlantType::CHOMPER:    return base + "Chomper/Idle/Chomper - Idle 1.png";
        default: return base + "Peashooter/Idle/Peashooter - Idle 1.png";
    }
}

int SceneManager::GetPlantCost(PlantType type) {
    switch(type) {
    case PlantType::PEASHOOTER: return Config::PEASHOOTER_COST;
    case PlantType::SUNFLOWER:  return Config::SUNFLOWER_COST;
    case PlantType::CHERRYBOMB: return Config::CHERRYBOMB_COST;
    case PlantType::WALLNUT:    return Config::WALLNUT_COST;
    case PlantType::POTATOMINE: return Config::POTATOMINE_COST;
    case PlantType::SNOWPEA:    return Config::SNOWPEA_COST;
    case PlantType::CHOMPER:    return Config::CHOMPER_COST;
    default: return 0;
    }
}

glm::vec2 SceneManager::SnapToGrid(glm::vec2 mousePos) {
    if (!m_Grid) return mousePos;

    glm::vec2 closestPos = mousePos;
    float minDistance = 9999.0f;

    // 遍歷整個網格 (假設 5 行 9 列)
    for (int r = 0; r < m_Grid->GetRows(); ++r) {
        for (int c = 0; c < 9; ++c) {
            glm::vec2 cellPos = m_Grid->GetWorldPos(r, c);
            float dist = glm::distance(mousePos, cellPos);
            // 找出距離滑鼠最近的格子中心點
            if (dist < minDistance) {
                minDistance = dist;
                closestPos = cellPos;
            }
        }
    }
    return closestPos;
}