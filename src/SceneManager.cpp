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
    m_ProgressBarFill->SetDrawable(std::make_shared<Util::Image>(base + "/Image/Scene/Progress Bar - tag.png"));
    m_ProgressBarFill->m_Transform.translation = {-580.0f, -320.0f}; // 初始在最左邊
    m_ProgressBarFill->SetZIndex(19.0f);

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
    m_StartBanner->SetZIndex(30.0f);
    m_StartBanner->m_Transform.translation = {0.0f, 0.0f};
    m_StartBanner->SetVisible(false);

    m_SeedChooserUI.Initialize();

    // 1. 植物拖曳預覽 (保留解決殘影問題的優化寫法)
    m_DragPlantObj = std::make_shared<Util::GameObject>();
    m_DragPlantObj->SetZIndex(95.0f);
    m_DragPlantObj->SetVisible(false);
    m_Renderer.AddChild(m_DragPlantObj);

    m_ShovelButtonObj = std::make_shared<Util::GameObject>();
    m_ShovelButtonObj->SetDrawable(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/Image/Scene/Shovel.png"));
    m_ShovelButtonObj->m_Transform.translation = { 480.0f, 280.0f }; // 右上角位置
    m_ShovelButtonObj->m_Transform.scale = { 2.5f, 2.5f };
    m_ShovelButtonObj->SetZIndex(80.0f);
    m_Renderer.AddChild(m_ShovelButtonObj);

    m_DragShovelObj = std::make_shared<Util::GameObject>();
    m_DragShovelObj->SetDrawable(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/Image/Scene/Shovel.png"));
    m_DragShovelObj->SetZIndex(95.0f);
    m_DragShovelObj->SetVisible(false); // 預設隱藏
    m_Renderer.AddChild(m_DragShovelObj);
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
        return;
    }

    // --- UI 更新 ---
    float currentProgress = m_LevelController.GetProgress();
    m_UIController.UpdateProgressBar(currentProgress, m_ProgressBarFill);

    if (m_SunManager) {
        m_SunManager->Update();
        m_UIController.UpdateSunCount(m_SunManager->GetSunCount(), m_SunTextDrawable);
    }

    // === ★ 單次點擊偵測 ===
    static bool s_PrevLeftBtn = false;
    bool currLeftBtn = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);
    bool isLeftBtnClick = (currLeftBtn && !s_PrevLeftBtn);
    s_PrevLeftBtn = currLeftBtn;

    // ==========================================
    // ★ 邏輯層：只處理「狀態變更」，不管圖片顯示
    // ==========================================

    // [狀態 C] 手中拿著鏟子
    if (m_IsShoveling) {
        // 右鍵取消挖除
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
            m_IsShoveling = false;
        }
        // 左鍵單次點擊：確認挖除
        else if (isLeftBtnClick) {
            glm::vec2 snapPos = SnapToGrid(Util::Input::GetCursorPosition());
            if (RemovePlantAt(snapPos)) {
                LOG_INFO("植物已被挖除！");
            }
            // 無論有沒有成功挖除，點擊後都恢復空手狀態
            m_IsShoveling = false;
        }
    }
    // [狀態 A] 手中已經拿著植物 (判斷種植與取消)
    else if (m_DraggingPlantType != PlantType::NONE) {
        // 偵測滑鼠右鍵：取消種植
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
            m_DraggingPlantType = PlantType::NONE;
        }
        // 偵測滑鼠左鍵單次點擊：確認種植
        else if (isLeftBtnClick) {
            int cost = GetPlantCost(m_DraggingPlantType);

            if (m_SunManager->GetSunCount() >= cost) {
                glm::vec2 snapPos = SnapToGrid(Util::Input::GetCursorPosition());

                if (AddPlant(m_DraggingPlantType, snapPos)) {
                    m_SunManager->ConsumeSun(cost);
                    if (m_PacketManager) m_PacketManager->StartPacketCooldown(m_DraggingPlantType);

                    // 種植成功，清空手中植物
                    m_DraggingPlantType = PlantType::NONE;
                } else {
                    LOG_INFO("種植失敗：這個位子已經有植物了！");
                }
            } else {
                LOG_INFO("種植失敗：陽光不足！");
                // 陽光不足時通常會強制取消選取 (如果你希望保留在手上可以註解掉這行)
                m_DraggingPlantType = PlantType::NONE;
            }
        }
    }
    // ==========================================
    // ★ 狀態 B：手上是空的 (判斷是要拿植物 或拿鏟子)
    // ==========================================
    else {
        // 1. 偵測點擊鏟子按鈕
        if (isLeftBtnClick && m_ShovelButtonObj) {
            glm::vec2 mousePos = Util::Input::GetCursorPosition();
            if (glm::distance(mousePos, m_ShovelButtonObj->m_Transform.translation) < 40.0f) {
                m_IsShoveling = true;
            }
        }

        // 2. 偵測點擊卡牌
        if (m_PacketManager) {
            m_PacketManager->Update(); // 讓卡牌跑冷卻等動畫
            if (m_PacketManager->GetSelectedType() != PlantType::NONE) {

                // ★★★ 終極防禦機制：攔截幽靈點擊 ★★★
                // 必須是 isLeftBtnClick (單次點擊瞬間) 為 true 時，才允許把卡牌拿起來！
                // 這樣就算 SeedPacket 誤判了滑鼠長按，我們也不會理它。
                if (isLeftBtnClick) {
                    m_DraggingPlantType = m_PacketManager->GetSelectedType();

                    // 只有確定拿起時才換圖片
                    if (m_DragPlantObj != nullptr) {
                        m_DragPlantObj->SetDrawable(std::make_shared<Util::Image>(GetPlantIdleImagePath(m_DraggingPlantType)));
                    }
                    LOG_INFO("正常操作：從卡牌拿起了植物！");
                } else {
                    LOG_INFO("成功攔截：忽略了因滑鼠按住導致的 SeedPacket 誤判！");
                }

                // 無論如何都清空卡牌區的選取狀態，防止殘留
                m_PacketManager->ClearSelection();
            }
        }
    }

    // ==========================================
    // ★ 渲染層：強制視覺覆寫 (絕對防禦殘影)
    // ==========================================

    // 植物預覽圖的絕對控制
    if (m_DragPlantObj != nullptr) {
        if (m_DraggingPlantType != PlantType::NONE) {
            // 只要手上有植物，無條件顯示並跟隨滑鼠
            m_DragPlantObj->SetVisible(true);
            m_DragPlantObj->m_Transform.translation = Util::Input::GetCursorPosition();
            m_DragPlantObj->m_Transform.scale = {2.5f, 2.5f};
        } else {
            // 只要手上沒植物，無條件隱藏並踢到螢幕外
            m_DragPlantObj->SetVisible(false);
            m_DragPlantObj->m_Transform.translation = {-9999.0f, -9999.0f};
        }
    }

    // 鏟子預覽圖的絕對控制
    if (m_DragShovelObj != nullptr) {
        if (m_IsShoveling) {
            m_DragShovelObj->SetVisible(true);
            m_DragShovelObj->m_Transform.translation = Util::Input::GetCursorPosition();
            m_DragShovelObj->m_Transform.scale = {2.5f, 2.5f};
        } else {
            m_DragShovelObj->SetVisible(false);
            m_DragShovelObj->m_Transform.translation = {-9999.0f, -9999.0f};
        }
    }

    // ==========================================
    // 1. 系統更新
    m_World.UpdateAll(m_Renderer);
    CombatSystem::Update(m_World, m_Renderer);
    m_LevelController.Update(m_World, m_Renderer);

    // 2. 清理死亡實體
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
    ClearAll();

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
        m_SunManager->Reset();
        m_SunManager->InitializeLevel(level);
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

    for (int r = 0; r < m_Grid->GetRows(); ++r) {
        for (int c = 0; c < 9; ++c) {
            glm::vec2 cellPos = m_Grid->GetWorldPos(r, c);
            float dist = glm::distance(mousePos, cellPos);
            if (dist < minDistance) {
                minDistance = dist;
                closestPos = cellPos;
            }
        }
    }
    return closestPos;
}

bool SceneManager::RemovePlantAt(glm::vec2 targetPos) {
    // 遍歷所有植物，找出距離點擊位置最近的植物
    for (auto& p : m_World.GetPlants()) {
        if (glm::distance(p->m_Transform.translation, targetPos) < 20.0f) {
            p->TakeDamage(9999); // 造成巨大傷害使其死亡，由 World 系統自動回收
            return true;
        }
    }
    return false;
}
