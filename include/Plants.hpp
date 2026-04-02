#ifndef PLANTS_HPP
#define PLANTS_HPP

#include "Util/GameObject.hpp"
#include "PlantType.hpp"
#include "AbilityInterfaces.hpp"

class Plant : public Util::GameObject {
public:
    Plant(PlantType type, glm::vec2 pos, int hp)
        : m_Type(type), m_Hp(hp), m_MaxHp(hp) {
        m_Transform.translation = pos;
        m_Transform.scale = {2.5f, 2.5f};
        SetZIndex(11.0f);
    }

    virtual void Update() = 0;

    // 基礎屬性存取
    bool IsDead() const { return m_IsDead; }
    PlantType GetType() const { return m_Type; }
    void TakeDamage(int d) { m_Hp -= d; if (m_Hp <= 0) m_IsDead = true; }

protected:
    PlantType m_Type;
    int m_Hp, m_MaxHp;
    bool m_IsDead = false;
};

#endif