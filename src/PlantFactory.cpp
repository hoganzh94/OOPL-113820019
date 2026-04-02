#include "PlantFactory.hpp"
#include "AllPlants.hpp" // 引入具體植物類別 (Peashooter, Sunflower...)
#include "SunManager.hpp"

std::shared_ptr<Plant> PlantFactory::CreatePlant(
    PlantType type,
    glm::vec2 pos,
    std::shared_ptr<SunManager> sunMgr
) {
    switch (type) {
    case PlantType::PEASHOOTER:
        return std::make_shared<Peashooter>(pos);

    case PlantType::SUNFLOWER:
        return std::make_shared<Sunflower>(pos, sunMgr);

    case PlantType::CHERRYBOMB:
        return std::make_shared<CherryBomb>(pos);

    case PlantType::WALLNUT:
        return std::make_shared<Wallnut>(pos);

    case PlantType::POTATOMINE:
        return std::make_shared<PotatoMine>(pos);

    default:
        return nullptr;
    }
}