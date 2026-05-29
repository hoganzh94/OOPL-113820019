#ifndef TARGETINGSTRATEGY_HPP
#define TARGETINGSTRATEGY_HPP

#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include "Zombies.hpp"

class Zombie; // 前置宣告，避免引入過多標頭檔

class ITargetingStrategy {
public:
    virtual ~ITargetingStrategy() = default;
    // 回傳找到的第一個合法目標，若無則回傳 nullptr
    virtual std::shared_ptr<Zombie> FindTarget(glm::vec2 plantPos, const std::vector<std::shared_ptr<Zombie>>& rowZombies) = 0;
};

// 實作一：全線射擊 (豌豆射手、雙發豌豆使用)
class ForwardTargeting : public ITargetingStrategy {
public:
    std::shared_ptr<Zombie> FindTarget(glm::vec2 plantPos, const std::vector<std::shared_ptr<Zombie>>& rowZombies) override {
        for (const auto& z : rowZombies) {
            if (!z->IsDead() && z->GetX() > plantPos.x) {
                return z; // 找到前方有存活的殭屍
            }
        }
        return nullptr;
    }
};

// 實作二：近戰範圍 (大嘴花使用)
class MeleeTargeting : public ITargetingStrategy {
private:
    float m_Range;
public:
    MeleeTargeting(float range) : m_Range(range) {}
    std::shared_ptr<Zombie> FindTarget(glm::vec2 plantPos, const std::vector<std::shared_ptr<Zombie>>& rowZombies) override {
        for (const auto& z : rowZombies) {
            float dist = z->GetX() - plantPos.x;
            if (!z->IsDead() && dist > 0 && dist <= m_Range) {
                return z; // 找到在攻擊範圍內的殭屍
            }
        }
        return nullptr;
    }
};

#endif