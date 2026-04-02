#ifndef PLANTFACTORY_HPP
#define PLANTFACTORY_HPP

#include <memory>
#include <glm/vec2.hpp>
#include "PlantType.hpp"
#include "Plants.hpp"

// 前向宣告
class SunManager;

class PlantFactory {
public:
    // 靜態建立方法
    static std::shared_ptr<Plant> CreatePlant(
        PlantType type,
        glm::vec2 pos,
        std::shared_ptr<SunManager> sunMgr
    );
};

#endif