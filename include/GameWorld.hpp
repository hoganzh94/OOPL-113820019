#ifndef GAMEWORLD_HPP
#define GAMEWORLD_HPP

#include <vector>
#include <memory>
#include <algorithm>
#include "Plants.hpp"
#include "Zombies.hpp"
#include "Pea.hpp"
#include "LawnMower.hpp"
#include "Util/Renderer.hpp" // 必須引入，解決 Incomplete Type

class GameWorld {
public:
    // --- 實體管理 (解決 AddPlant, AddMower, AddPea) ---
    void AddPlant(std::shared_ptr<Plant> p) { m_Plants.push_back(p); }
    void AddZombie(std::shared_ptr<Zombie> z) { m_Zombies.push_back(z); }
    void AddPea(std::shared_ptr<Pea> p) { m_Peas.push_back(p); }
    void AddMower(std::shared_ptr<LawnMower> m) { m_Mowers.push_back(m); }

    // --- 查詢功能 (解決 GetZombiesInRow) ---
    std::vector<std::shared_ptr<Zombie>> GetZombiesInRow(float y, float tolerance = 20.0f) {
        std::vector<std::shared_ptr<Zombie>> result;
        for (auto& z : m_Zombies) {
            if (!z->IsDead() && std::abs(z->GetY() - y) < tolerance) {
                result.push_back(z);
            }
        }
        return result;
    }

    // --- 系統更新 (解決 UpdateAll) ---
    void UpdateAll(Util::Renderer& renderer) {
        for (auto& p : m_Plants) p->Update();
        for (auto& z : m_Zombies) z->Update();
        for (auto& pea : m_Peas) pea->Update();
        for (auto& m : m_Mowers) m->Update();
    }

    // --- 清理功能 (解決 Clear, RemoveDeadEntities) ---
    void RemoveDeadEntities(Util::Renderer& renderer) {
        auto cleanup = [&](auto& vec) {
            vec.erase(std::remove_if(vec.begin(), vec.end(), [&](auto& e) {
                if (e->IsDead()) {
                    renderer.RemoveChild(e);
                    return true;
                }
                return false;
            }), vec.end());
        };
        cleanup(m_Plants); cleanup(m_Zombies); cleanup(m_Peas); cleanup(m_Mowers);
    }

    void Clear(Util::Renderer& renderer) {
        for (auto& p : m_Plants) renderer.RemoveChild(p);
        for (auto& z : m_Zombies) renderer.RemoveChild(z);
        for (auto& pea : m_Peas) renderer.RemoveChild(pea);
        for (auto& m : m_Mowers) renderer.RemoveChild(m);
        m_Plants.clear(); m_Zombies.clear(); m_Peas.clear(); m_Mowers.clear();
    }

    // --- Getter (解決 GetPlants, GetZombies, GetPeas, GetMowers) ---
    auto& GetPlants() { return m_Plants; }
    auto& GetZombies() { return m_Zombies; }
    auto& GetPeas() { return m_Peas; }
    auto& GetMowers() { return m_Mowers; }

private:
    std::vector<std::shared_ptr<Plant>> m_Plants;
    std::vector<std::shared_ptr<Zombie>> m_Zombies;
    std::vector<std::shared_ptr<Pea>> m_Peas;
    std::vector<std::shared_ptr<LawnMower>> m_Mowers;
};

#endif