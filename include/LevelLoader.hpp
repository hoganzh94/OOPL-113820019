#ifndef LEVELLOADER_HPP
#define LEVELLOADER_HPP

#include <string>
#include <vector>
#include <map>
#include <glm/vec2.hpp>

// 定義關卡資產與座標結構
struct LevelInfo
{
    std::string bgPath; // 背景路徑
    std::string lawnPath; // 草地路徑
    glm::vec2 lawnPos; // 草地座標
    glm::vec2 lawnScale; // 草地縮放
    std::vector<std::vector<glm::vec2>> gridCoords; // 5x9 或其他尺寸的網格
};

class LevelLoader
{
public:
    static void Initialize(); // 預載所有關卡資料
    static LevelInfo GetLevel(int level);

private:
    static std::map<int, LevelInfo> m_Data;
    static bool m_IsInitialized;
};

#endif