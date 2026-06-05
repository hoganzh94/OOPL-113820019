```mermaid
classDiagram
%% ------------------------------------
%% 核心與基礎物件 (Core & Base Objects)
%% ------------------------------------
class GameObject {
<<base>>
+SetDrawable()
+m_Transform
}

    class Plant {
        <<abstract>>
        -PlantType m_Type
        -int m_HP
    }

    class Zombie {
        <<abstract>>
        -int m_HP
        -float m_Speed
    }

    %% 實體繼承結構
    GameObject <|-- Plant
    GameObject <|-- Zombie
    GameObject <|-- Sun
    GameObject <|-- Pea
    GameObject <|-- LawnMower

    Pea <|-- SnowPeaBullet : 繼承
    
    %% ------------------------------------
    %% 植物子類別 (Plants Hierarchy)
    %% ------------------------------------
    Plant <|-- Peashooter
    Plant <|-- Sunflower
    Plant <|-- Wallnut
    Plant <|-- CherryBomb
    Plant <|-- PotatoMine
    Plant <|-- Repeater
    Plant <|-- SnowPea
    Plant <|-- Chomper

    %% ------------------------------------
    %% 殭屍子類別 (Zombies Hierarchy)
    %% ------------------------------------
    Zombie <|-- NormalZombie
    Zombie <|-- FlagZombie
    Zombie <|-- ConeHeadZombie
    Zombie <|-- BucketHeadZombie

    %% ------------------------------------
    %% 遊戲系統與管理器 (Systems & Managers)
    %% ------------------------------------
    class App
    class SceneManager
    class GameWorld
    class Grid
    class LevelController
    class LevelLoader
    class CombatSystem
    class RenderSystem
    class SunManager
    class PlantFactory
    class Config {
        <<utility>>
    }
    
    App --> SceneManager : 控制場景切換
    SceneManager --> GameWorld : 載入遊戲世界
    GameWorld *-- Grid : 擁有網格地圖
    GameWorld --> LevelController : 管理關卡進度
    LevelController --> LevelLoader : 讀取關卡資料
    GameWorld --> CombatSystem : 處理碰撞與戰鬥
    GameWorld --> RenderSystem : 處理畫面渲染
    GameWorld --> SunManager : 管理陽光資源
    GameWorld --> PlantFactory : 實例化植物

    %% ------------------------------------
    %% 使用者介面與種子卡 (UI & Seed Packets)
    %% ------------------------------------
    class UIController
    class SeedChooserUI
    class SeedPacketManager
    class SeedPacket
    
    UIController --> SeedChooserUI : 顯示選卡畫面
    SeedChooserUI --> SeedPacketManager : 管理卡槽
    SeedPacketManager *-- SeedPacket : 包含多張卡片

    %% ------------------------------------
    %% 介面與策略模式 (Interfaces & Strategies)
    %% ------------------------------------
    class GameObserver {
        <<interface>>
        +OnEvent()
    }
    class TargetingStrategy {
        <<interface>>
        +FindTarget()
    }
    class AbilityInterfaces {
        <<interface>>
    }

    GameWorld ..|> GameObserver : 實作觀察者
    Zombie --> TargetingStrategy : 使用索敵策略
    Plant ..|> AbilityInterfaces : 實作特殊能力介面
    
    %% ------------------------------------
    %% 列舉與統整 (Enums & Includes)
    %% ------------------------------------
    class PlantType {
        <<enumeration>>
    }
    Plant --> PlantType

    %% AllPlants 與 AllZombies 主要作為 Header 集成，在邏輯圖中以關聯呈現
    class AllPlants { <<header>> }
    class AllZombies { <<header>> }