#include "App.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include "Util/Keycode.hpp"
#include "PlantType.hpp" // 確保引入全域定義

void App::Start() {
    m_SceneManager = std::make_unique<SceneManager>(m_Root);
    m_SceneManager->InitializeResources();
    m_SceneManager->SwitchToMenu();
    m_CurrentState = State::UPDATE;
}

int GetPlantCost(PlantType type) {
    switch (type) {
    case PlantType::PEASHOOTER: return Config::PEASHOOTER_COST;
    case PlantType::SUNFLOWER:  return Config::SUNFLOWER_COST;
    case PlantType::CHERRYBOMB: return Config::CHERRYBOMB_COST;
    case PlantType::WALLNUT:    return Config::WALLNUT_COST;
    case PlantType::POTATOMINE: return Config::POTATOMINE_COST;
    default: return 0;
    }
}

void App::Update() {
    // 1. 驅動基礎系統 (動畫、世界物理邏輯)
    m_SceneManager->Update();

    auto currentPhase = m_SceneManager->GetCurrentPhase();
    auto packetManager = m_SceneManager->GetPacketManager();

    // --- [關鍵修正 A]：無論是否點擊，每一幀都必須驅動卡片計時器 ---
    if (currentPhase == SceneManager::LevelPhase::DAY_LEVEL && packetManager) {
        for (auto& p : packetManager->GetPackets()) {
            p->Update();
        }
    }

    // 2. 分流處理各個狀態的輸入
    if (currentPhase == SceneManager::LevelPhase::MENU) {
        if (Util::Input::IsKeyDown(Util::Keycode::A)) {
            m_LevelCounter = 1;
            m_SceneManager->EnterLevel(m_LevelCounter);
            LOG_INFO("Game Start!");
        }
    }
    else if (currentPhase == SceneManager::LevelPhase::DAY_LEVEL) {

        // --- 基礎快捷鍵 ---
        if (Util::Input::IsKeyDown(Util::Keycode::N)) {
            m_LevelCounter = (m_LevelCounter % 10) + 1;
            m_SceneManager->EnterLevel(m_LevelCounter);
        }
        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            m_SceneManager->SwitchToMenu();
        }

        // --- [關鍵修正 B]：滑鼠點擊邏輯分流 ---
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            glm::vec2 mousePos = Util::Input::GetCursorPosition();
            auto packetManager = m_SceneManager->GetPacketManager();

            // 關鍵：先記錄點擊那一瞬間的選取狀態
            PlantType selected = packetManager->GetSelectedType();

            // 分支 1：選取植物 (點擊卡槽區)
            if (mousePos.x <= -350.0f) {
                packetManager->Update();
                LOG_INFO("Packet area clicked.");
            }
            // 分支 2：放置植物 (點擊草地區)
            else if (selected != PlantType::NONE) {
                auto tile = m_SceneManager->GetGrid()->GetTileFromMouse(mousePos);

                if (tile.has_value()) {
                    auto sunManager = m_SceneManager->GetSunManager();
                    int cost = GetPlantCost(selected);

                    // 檢查陽光
                    if (!sunManager->CanAfford(cost)) {
                        LOG_WARN("Not enough sun! Need: {}, Have: {}", cost, sunManager->GetSunCount());
                    }
                    else {
                        // 檢查重複種植
                        bool isOccupied = false;
                        auto& plants = m_SceneManager->GetWorld().GetPlants();
                        for (auto& p : plants) {
                            if (glm::distance(p->m_Transform.translation, tile->worldPos) < 20.0f) { // 稍微加大判定範圍
                                isOccupied = true;
                                break;
                            }
                        }

                        if (!isOccupied) {
                            m_SceneManager->AddPlant(selected, tile->worldPos);
                            packetManager->StartPacketCooldown(selected);
                            sunManager->ConsumeSun(cost);
                            packetManager->ClearSelection(); // 種植成功才清除
                            LOG_INFO("Planted! Remaining Sun: {}", sunManager->GetSunCount());
                        } else {
                            LOG_WARN("Tile already occupied!");
                        }
                    }
                }
            }
        }

        // 滑鼠右鍵：取消選取
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
            packetManager->ClearSelection();
            LOG_INFO("Selection canceled.");
        }
    }
    else if (currentPhase == SceneManager::LevelPhase::WIN) {
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_LevelCounter = (m_LevelCounter % 10) + 1;
            m_SceneManager->EnterLevel(m_LevelCounter);
        }
    }
    else if (currentPhase == SceneManager::LevelPhase::FAIL) {
        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            m_SceneManager->SwitchToMenu();
        }
    }

    // 3. 執行全域渲染更新
    m_Root.Update();

    // 4. 檢查結束程式
    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() {
    LOG_TRACE("App::End");
}