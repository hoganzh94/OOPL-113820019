#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace Config {
    // --- 植物數值 ---
    inline constexpr int PEASHOOTER_HP = 300;
    inline constexpr int SUNFLOWER_HP  = 300;
    inline constexpr int WALLNUT_HP    = 4000;
    inline constexpr int POTATOMINE_HP = 300;
    inline constexpr int CHERRYBOMB_HP = 9999;

    // --- 殭屍數值 ---
    inline constexpr int NORMAL_ZOMBIE_HP = 200;
    inline constexpr float ZOMBIE_WALK_SPEED = 30.0f;
    inline constexpr int ZOMBIE_EAT_DAMAGE = 20; // 殭屍每秒造成的傷害

    // 路障殭屍 (Conehead)
    inline constexpr int CONEHEAD_ARMOR_HP = 370;   // 路障額外血量
    inline constexpr float CONEHEAD_SPEED = 30.0f;
    inline constexpr int CONEHEAD_ANIM_FRAMES = 7;  // 強制 7 幀

    // 路障受損階段閾值 (基於 370)
    inline constexpr int CONEHEAD_STAGE1_HP = 250;  // 低於此值進入輕微損毀
    inline constexpr int CONEHEAD_STAGE2_HP = 125;  // 低於此值進入嚴重損毀

    // --- 武器與傷害 ---
    inline constexpr int PEA_DAMAGE = 20;
    inline constexpr float PEA_SPEED = 500.0f;
    inline constexpr int CHERRYBOMB_DAMAGE = 1800;
    inline constexpr float CHERRYBOMB_RANGE = 150.0f;

    // --- 遊戲邏輯 ---
    inline constexpr float SUN_PRODUCE_TIME = 24.0f; // 向日葵產陽光間隔
    inline constexpr float ZOMBIE_SPAWN_INTERVAL = 10.0f; // 殭屍生成間隔
}

#endif