#include "PlantFactory.hpp"
#include "AllPlants.hpp"
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

    case PlantType::SNOWPEA:
        return std::make_shared<SnowPea>(pos);

    case PlantType::CHOMPER:
        return std::make_shared<Chomper>(pos);

    default:
        return nullptr;
    }
}