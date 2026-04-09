#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace Config {
    // --- 植物數值 ---
    inline constexpr int PEASHOOTER_HP = 100;
    inline constexpr int SUNFLOWER_HP  = 100;
    inline constexpr int WALLNUT_HP    = 2000;
    inline constexpr int POTATOMINE_HP = 100;
    inline constexpr int CHERRYBOMB_HP = 9999;

    // --- 冷卻時間 (秒) ---
    inline constexpr float PEASHOOTER_COOLDOWN = 5.0f;
    inline constexpr float SUNFLOWER_COOLDOWN  = 12.0f;
    inline constexpr float CHERRYBOMB_COOLDOWN = 40.0f;
    inline constexpr float WALLNUT_COOLDOWN    = 25.0f;
    inline constexpr float POTATOMINE_COOLDOWN = 40.0f;

    // --- 價格 ---
    inline constexpr int PEASHOOTER_COST = 100;
    inline constexpr int SUNFLOWER_COST  = 50;
    inline constexpr int CHERRYBOMB_COST = 150;
    inline constexpr int WALLNUT_COST    = 50;
    inline constexpr int POTATOMINE_COST = 25;

    // --- 殭屍數值 ---
    inline constexpr int NORMAL_ZOMBIE_HP = 100;
    inline constexpr float ZOMBIE_WALK_SPEED = 20.0f;
    inline constexpr int ZOMBIE_EAT_DAMAGE = 20; // 殭屍每秒造成的傷害

    // 路障殭屍 (Conehead)
    inline constexpr int CONEHEAD_ARMOR_HP = 300;   // 路障額外血量
    inline constexpr float CONEHEAD_SPEED = 20.0f;
    inline constexpr int CONEHEAD_ANIM_FRAMES = 7;  // 強制 7 幀

    // 路障受損階段閾值 (基於 370)
    inline constexpr int CONEHEAD_STAGE1_HP = 200;  // 低於此值進入輕微損毀
    inline constexpr int CONEHEAD_STAGE2_HP = 100;  // 低於此值進入嚴重損毀

    // --- 武器與傷害 ---
    inline constexpr int PEA_DAMAGE = 15;
    inline constexpr float PEA_SPEED = 500.0f;
    inline constexpr int CHERRYBOMB_DAMAGE = 1800;
    inline constexpr float CHERRYBOMB_RANGE = 150.0f;

    // --- 遊戲邏輯 ---
    inline constexpr float SUN_PRODUCE_TIME = 20.0f; // 向日葵產陽光間隔
    //inline constexpr float ZOMBIE_SPAWN_INTERVAL = 10.0f; // 殭屍生成間隔
}

#endif