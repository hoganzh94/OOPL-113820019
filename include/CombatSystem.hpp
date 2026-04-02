#ifndef COMBATSYSTEM_HPP
#define COMBATSYSTEM_HPP

#include "GameWorld.hpp"
#include "Config.hpp"
#include "AbilityInterfaces.hpp"
#include "AllPlants.hpp"
#include <cmath>
#include <algorithm>

class CombatSystem {
public:
    static void Update(GameWorld& world, Util::Renderer& renderer) {
        // 使用 world.GetXXX() 取得容器 (解決之前 Getter 的報錯)
        auto& plants = world.GetPlants();
        auto& zombies = world.GetZombies();
        auto& peas = world.GetPeas();
        auto& mowers = world.GetMowers();

        // 1. 處理攻擊行為 (IAttacker)
        for (auto& p : plants) {
            if (auto attacker = std::dynamic_pointer_cast<IAttacker>(p)) {
                auto rowZombies = world.GetZombiesInRow(p->m_Transform.translation.y);
                // 傳入 3 個參數：殭屍列表, 渲染器, 遊戲世界
                attacker->Attack(rowZombies, renderer, world);
            }
        }

        // 2. 處理殭屍與植物碰撞反應 (ICollisionReactive & 啃食判定)
        for (auto& z : zombies) {
            if (z->IsDead()) continue;
            std::shared_ptr<Plant> target = nullptr;

            for (auto& p : plants) {
                if (p->IsDead()) continue;

                float dy = std::abs(z->GetY() - p->m_Transform.translation.y);
                float dx = z->GetX() - p->m_Transform.translation.x;

                if (dy < 40.0f) {
                    // --- A. 特殊碰撞 (如地雷引爆) ---
                    if (auto reactive = std::dynamic_pointer_cast<ICollisionReactive>(p)) {
                        if (std::abs(dx) < 30.0f) {
                            reactive->OnCollision(z);
                            if (z->IsDead()) { target = nullptr; break; }
                        }
                    }

                    // --- B. 啃食判定 (排除已 Ready 的地雷) ---
                    if (dx > 10.0f && dx < 50.0f) {
                        bool isReadyPotato = false;
                        if (p->GetType() == PlantType::POTATOMINE) {
                            auto potato = std::static_pointer_cast<PotatoMine>(p);
                            if (potato->IsReadyToExplode()) isReadyPotato = true;
                        }

                        if (!isReadyPotato) {
                            target = p;
                            break;
                        }
                    }
                }
            }
            z->SetTargetPlant(target);
        }

        // 3. 子彈與殭屍碰撞
        for (auto& pea : peas) {
            if (pea->IsDead() || pea->IsExploding()) continue;

            for (auto& z : zombies) {
                if (z->IsDead()) continue;

                float dy = std::abs(pea->m_Transform.translation.y - z->GetY());
                float dx = std::abs(pea->m_Transform.translation.x - z->GetX());

                if (dy < 40.0f && dx < 30.0f) {
                    z->TakeDamage(Config::PEA_DAMAGE);
                    pea->Hit();
                    break;
                }
            }
        }

        // 4. 處理爆炸傷害 (IExplosive - 櫻桃炸彈)
        for (auto& p : plants) {
            if (auto explosive = std::dynamic_pointer_cast<IExplosive>(p)) {
                if (explosive->IsExplodingNow()) {
                    float range = explosive->GetExplosionRange();
                    for (auto& z : zombies) {
                        if (!z->IsDead() && glm::distance(z->m_Transform.translation, p->m_Transform.translation) < range) {
                            z->TakeDamage(explosive->GetExplosionDamage());
                        }
                    }
                    // 這裡可以在 CherryBomb 內部處理重置，或在此處處理
                }
            }
        }

        // 5. 處理割草機
        for (auto& m : mowers) {
            if (m->IsDead()) continue;

            if (m->GetState() == LawnMower::State::IDLE) {
                for (auto& z : zombies) {
                    if (std::abs(z->GetY() - m->m_Transform.translation.y) < 40.0f &&
                        z->GetX() - m->m_Transform.translation.x < 20.0f) {
                        m->Activate();
                        break;
                    }
                }
            } else if (m->GetState() == LawnMower::State::ACTIVATED) {
                for (auto& z : zombies) {
                    if (std::abs(z->GetY() - m->m_Transform.translation.y) < 40.0f &&
                        std::abs(z->GetX() - m->m_Transform.translation.x) < 50.0f) {
                        z->TakeDamage(9999);
                    }
                }
            }
        }
    }
};

#endif