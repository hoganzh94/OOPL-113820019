```mermaid
classDiagram
    %% -------------------------------------
    %% 核心管理系統 (Core Systems)
    %% -------------------------------------
    class App {
        -m_CurrentState: State
        -m_SceneManager: SceneManager
        +Start()
        +Update()
    }

    class SceneManager {
        -m_World: GameWorld
        -m_LevelController: LevelController
        -m_PacketManager: SeedPacketManager
        -m_SunManager: SunManager
        -m_SeedChooserUI: SeedChooserUI
        -m_UIController: UIController
        -m_DraggingPlantType: PlantType
        +EnterLevel(int level)
        +Update()
        +AddPlant(PlantType, vec2) bool
    }

    class GameWorld {
        -m_Plants: vector~Plant~
        -m_Zombies: vector~Zombie~
        -m_Peas: vector~Pea~
        -m_Mowers: vector~LawnMower~
        +UpdateAll()
        +RemoveDeadEntities()
        +GetZombiesInRow()
    }

    class LevelController {
        -m_LevelData: LevelData
        -m_SpawnedCount: int
        -m_SpawnTimer: float
        +Initialize(int level, Grid grid)
        +Update(world, renderer)
        +CheckHugeWave()
    }

    class CombatSystem {
        <<static>>
        +Update(world, renderer)
        -CheckCollisions()
    }

    %% 關聯性
    App --> SceneManager
    SceneManager *-- GameWorld
    SceneManager *-- LevelController
    SceneManager *-- SeedPacketManager
    SceneManager *-- SunManager
    SceneManager *-- SeedChooserUI
    SceneManager *-- UIController
    SceneManager ..> CombatSystem : calls

    %% -------------------------------------
    %% 實體階層 (Entity Hierarchies)
    %% -------------------------------------
    class Plant {
        <<abstract>>
        #m_Hp: int
        #m_State: PlantState
        +Update()
        +TakeDamage(amount)
    }

    class Zombie {
        <<abstract>>
        #m_Hp: int
        #m_Speed: float
        #m_State: ZombieState
        +Update()
        +TakeDamage(amount)
    }

    class Pea {
        #m_Damage: int
        +Update()
    }

    class SnowPeaBullet {
        +ApplySlowEffect()
    }

    class LawnMower {
        +Update()
        +Trigger()
    }

    class Sun {
        +Update()
        +Collect()
    }

    %% 繼承關係
    Plant <|-- Peashooter
    Plant <|-- Sunflower
    Plant <|-- Wallnut
    Plant <|-- CherryBomb
    Plant <|-- PotatoMine
    Plant <|-- SnowPea
    Plant <|-- Chomper

    Zombie <|-- NormalZombie
    Zombie <|-- ConeHeadZombie
    Zombie <|-- BucketHeadZombie
    Zombie <|-- FlagZombie

    Pea <|-- SnowPeaBullet

    %% 世界包含實體
    GameWorld o-- Plant
    GameWorld o-- Zombie
    GameWorld o-- Pea
    GameWorld o-- LawnMower

    %% -------------------------------------
    %% UI 與資源管理 (UI & Managers)
    %% -------------------------------------
    class SeedPacketManager {
        -m_Packets: vector~SeedPacket~
        +InitializeWith(plants)
        +Update()
        +GetSelectedType()
    }

    class SeedPacket {
        -m_Type: PlantType
        -m_Cooldown: float
        +Update()
    }

    class SunManager {
        -m_Suns: vector~Sun~
        -m_SunCount: int
        +Update()
        +ConsumeSun(cost) bool
    }

    class SeedChooserUI {
        -m_Cards: vector~CardInfo~
        -m_SelectedPlants: vector~PlantType~
        +Show(unlockedPlants)
        +Update()
        +GetSelectedPlants()
    }

    %% 管理器關聯
    SeedPacketManager *-- SeedPacket
    SunManager *-- Sun

    %% -------------------------------------
    %% 工廠與載入器 (Factories & Loaders)
    %% -------------------------------------
    class PlantFactory {
        <<static>>
        +CreatePlant(type, pos, sunManager) Plant
    }

    class LevelLoader {
        <<static>>
        +GetLevel(level) LevelInfo
        +GetUnlockedPlants(level) vector~PlantType~
    }

    SceneManager ..> PlantFactory : uses
    SceneManager ..> LevelLoader : uses
    LevelController ..> LevelLoader : uses