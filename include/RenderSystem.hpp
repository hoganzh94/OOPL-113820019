#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "GameWorld.hpp"
#include "Util/Renderer.hpp"

class RenderSystem {
public:
    static void Update(GameWorld& world, Util::Renderer& renderer) {
        // 渲染系統負責同步邏輯世界與渲染層
    }
    
    // 當世界產生新物件時，由系統負責加入渲染器
    static void OnEntitySpawned(std::shared_ptr<Util::GameObject> entity, Util::Renderer& renderer) {
        renderer.AddChild(entity);
    }

    // 當物件死亡時，由系統負責移除
    static void OnEntityDestroyed(std::shared_ptr<Util::GameObject> entity, Util::Renderer& renderer) {
        renderer.RemoveChild(entity);
    }
};

#endif