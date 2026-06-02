```mermaid
classDiagram
    %% ------------------------------------
    %% 植物繼承體系 (Plants Hierarchy)
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

    GameObject <|-- Plant
    Plant <|-- Peashooter
    Plant <|-- Sunflower
    Plant <|-- CherryBomb
    Plant <|-- Chomper
    Plant <|-- WallNut
    Plant <|-- PotatoMine
    Plant <|-- SnowPea
    Plant <|-- Repeater

    %% ------------------------------------
    %% 殭屍繼承體系 (Zombies Hierarchy)
    %% ------------------------------------
    class Zombie {
        <<abstract>>
        -int m_HP
        -float m_Speed
    }

    GameObject <|-- Zombie
    Zombie <|-- NormalZombie
    Zombie <|-- ConeHeadZombie
    Zombie <|-- BucketHeadZombie
    Zombie <|-- FlagZombie

    %% ------------------------------------
    %% 子彈與其他實體 (Projectiles & Others)
    %% ------------------------------------
    class Pea {
        -int m_Damage
    }
    GameObject <|-- Pea
    Pea <|-- SnowPeaBullet

    GameObject <|-- LawnMower

    %% ------------------------------------
    %% 介面 (Interfaces)
    %% ------------------------------------
    class IAttacker {
        <<interface>>
        +Attack()
    }
    class IExplosive {
        <<interface>>
        +IsExplodingNow()
    }
    class ICollisionReactive {
        <<interface>>
        +OnCollision()
    }

    Peashooter ..|> IAttacker
    Repeater ..|> IAttacker
    Chomper ..|> IAttacker
    CherryBomb ..|> IExplosive
    PotatoMine ..|> ICollisionReactive
    PotatoMine ..|> IExplosive

    %% ------------------------------------
    %% 系統管理器 (Systems)
    %% ------------------------------------
    class SceneManager {
        -GameWorld m_World
        -SeedPacketManager m_PacketManager
        -SunManager m_SunManager
        -Grid m_Grid
        +Update()
        +EnterLevel()
    }

    class GameWorld {
        +AddPlant()
        +AddZombie()
        +UpdateAll()
    }

    class PlantFactory {
        <<static>>
        +CreatePlant(type, pos)
    }

    SceneManager *-- GameWorld
    GameWorld o-- Plant
    GameWorld o-- Zombie