#ifndef ABILITYINTERFACES_HPP
#define ABILITYINTERFACES_HPP

#include <vector>
#include <memory>

class Zombie;
class GameWorld;
namespace Util { class Renderer; }

// 攻擊者介面 (例如：豌豆射手)
class IAttacker {
public:
    virtual ~IAttacker() = default;
    // 攻擊者現在只對「世界」產生影響，不直接對「渲染器」產生影響
    virtual void Attack(std::vector<std::shared_ptr<Zombie>>& rowZombies,
                        Util::Renderer& renderer,
                        GameWorld& world) = 0;
};

// 爆炸物介面 (例如：櫻桃炸彈)
class IExplosive {
public:
    virtual ~IExplosive() = default;
    virtual bool IsExplodingNow() const = 0;
    virtual float GetExplosionRange() const = 0;
    virtual int GetExplosionDamage() const = 0;
};

// 碰撞反應介面 (例如：地雷、堅果)
class ICollisionReactive {
public:
    virtual ~ICollisionReactive() = default;
    virtual void OnCollision(std::shared_ptr<Zombie> zombie) = 0;
};

#endif