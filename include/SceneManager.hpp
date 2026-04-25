#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "pch.hpp"
#include "GameWorld.hpp"
#include "LevelController.hpp"
#include "Grid.hpp"
#include "SeedPacketManager.hpp"
#include "SunManager.hpp"
#include "PlantType.hpp"
#include "UIController.hpp"
#include "Util/Renderer.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "SeedChooserUI.hpp"

class SceneManager {
public:
    enum class LevelPhase { MENU, SEED_CHOOSER, DAY_LEVEL, FAIL, WIN };

    explicit SceneManager(Util::Renderer& renderer);
    void InitializeResources();
    void SwitchToMenu();
    void EnterLevel(int level);
    void Update();
    void AddPlant(PlantType type, glm::vec2 worldPos);
    GameWorld& GetWorld() { return m_World; }
    [[nodiscard]] const GameWorld& GetWorld() const { return m_World; }

    [[nodiscard]] LevelPhase GetCurrentPhase() const { return m_Phase; }
    [[nodiscard]] std::shared_ptr<Grid> GetGrid() const { return m_Grid; }
    [[nodiscard]] std::shared_ptr<SeedPacketManager> GetPacketManager() const { return m_PacketManager; }
    [[nodiscard]] std::shared_ptr<SunManager> GetSunManager() const { return m_SunManager; }

private:
    void ClearAll();

    Util::Renderer& m_Renderer;
    LevelPhase m_Phase = LevelPhase::MENU;

    // 核心系統
    GameWorld m_World;
    LevelController m_LevelController;
    UIController m_UIController;
    std::shared_ptr<Grid> m_Grid;

    // UI 與 背景物件 (修復 Cannot resolve symbol 的關鍵)
    std::shared_ptr<Util::GameObject> m_MenuTopLeft, m_MenuTopRight, m_MenuBottomLeft, m_MenuBottomRight;
    std::shared_ptr<Util::GameObject> m_GameSceneBG, m_Lawn;
    std::shared_ptr<Util::GameObject> m_SunTextObj, m_GameOver, m_WinTextObj, m_NextLevelHintObj;
    std::shared_ptr<Util::GameObject> m_ProgressBarBG, m_ProgressBarFill;

    // 管理員
    std::shared_ptr<SeedPacketManager> m_PacketManager;
    std::shared_ptr<SunManager> m_SunManager;
    std::shared_ptr<Util::Text> m_SunTextDrawable;

    std::shared_ptr<Util::GameObject> m_StartBanner;
    std::string m_CurrentBannerPath = "";
    float m_StartBannerTimer = 0.0f;
    bool m_IsBannerFinished = false;

    std::vector<PlantType> m_UnlockedPlants;
    std::vector<PlantType> m_SelectedPlants;
    SeedChooserUI m_SeedChooserUI;
};

#endif