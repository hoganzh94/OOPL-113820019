#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "pch.hpp"

class GameObject
{
    public:
        virtual ~GameObject() = default;

        virtual void Update() = 0;//每幀執行邏輯

        virtual void Draw() = 0;//每幀執行繪製

        //取得位置(碰撞偵測用)
        [[nodiscard]] virtual float GetX() const { return m_X; }
        [[nodiscard]] virtual float GetY() const { return m_Y; }

    protected:
        float m_X = 0, m_Y = 0;
        int m_HP = 100;
};

#endif //GAMEOBJECT_HPP