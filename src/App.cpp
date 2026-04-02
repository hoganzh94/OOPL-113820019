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

void App::Update() {
    // 1. 驅動 SceneManager 的內部邏輯更新
    m_SceneManager->Update();

    // 取得當前場景狀態
    auto currentPhase = m_SceneManager->GetCurrentPhase();

    // --- 分流處理各個狀態的按鍵輸入 ---

    // 狀態 A：主選單
    if (currentPhase == SceneManager::LevelPhase::MENU) {
        if (Util::Input::IsKeyDown(Util::Keycode::A)) {
            m_LevelCounter = 1; // 從第一關開始
            m_SceneManager->EnterLevel(m_LevelCounter);
            LOG_INFO("Game Start! Entering Level 1...");
        }
    }

    // 狀態 B：遊戲進行中
    else if (currentPhase == SceneManager::LevelPhase::DAY_LEVEL) {

        // 1. 基礎捷徑鍵
        if (Util::Input::IsKeyDown(Util::Keycode::N)) {
            m_LevelCounter = (m_LevelCounter % 10) + 1; // 強制跳下一關
            m_SceneManager->EnterLevel(m_LevelCounter);
            LOG_INFO("Cheated to Next Level: {}", m_LevelCounter);
        }
        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            m_SceneManager->SwitchToMenu();
            LOG_INFO("Returning to Menu...");
        }

        // 2. 滑鼠左鍵：處理卡槽選取與草地種植
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            glm::vec2 mousePos = Util::Input::GetCursorPosition();
            auto packetManager = m_SceneManager->GetPacketManager();

            // 使用全域 PlantType
            PlantType typeBeforeClick = packetManager->GetSelectedType();

            packetManager->Update();

            PlantType typeAfterClick = packetManager->GetSelectedType();

            // 邏輯判定
            if (mousePos.x > -350.0f && typeBeforeClick != PlantType::NONE) {
                if (typeBeforeClick == typeAfterClick) {
                    auto tile = m_SceneManager->GetGrid()->GetTileFromMouse(mousePos);
                    if (tile.has_value()) {
                        // AddPlant 現在接收 PlantType
                        m_SceneManager->AddPlant(typeBeforeClick, tile->worldPos);
                        packetManager->ClearSelection();
                        LOG_INFO("Plant placed successfully!");
                    }
                }
            }
        }

        // 3. 滑鼠右鍵：取消選取
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
            m_SceneManager->GetPacketManager()->ClearSelection();
            LOG_INFO("Plant selection canceled.");
        }
    }

    // 狀態 C：遊戲勝利
    else if (currentPhase == SceneManager::LevelPhase::WIN) {
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_LevelCounter = (m_LevelCounter % 10) + 1;
            m_SceneManager->EnterLevel(m_LevelCounter);
            LOG_INFO("Winner proceeds to Level {}", m_LevelCounter);
        }
    }

    // 狀態 D：遊戲失敗
    else if (currentPhase == SceneManager::LevelPhase::FAIL) {
        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            m_SceneManager->SwitchToMenu();
            LOG_INFO("Game Restarted after Failure.");
        }
    }

    // --- 全域渲染 ---
    m_Root.Update();

    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() {
    LOG_TRACE("App::End");
}