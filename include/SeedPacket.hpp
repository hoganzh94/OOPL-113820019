#ifndef SEEDPACKET_HPP
#define SEEDPACKET_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "PlantType.hpp" // 引入全域型別

class SeedPacket : public Util::GameObject {
public:
    // 移除原本在 class 內部的 enum class PlantType { ... };

    SeedPacket(PlantType type, const std::string& imagePath) : m_Type(type) {
        SetDrawable(std::make_shared<Util::Image>(imagePath));
        m_Transform.scale = {3.0f, 3.0f};
        SetZIndex(15.0f);
    }

    void SetPosition(glm::vec2 pos) { m_Transform.translation = pos; }

    // 回傳全域型別
    PlantType GetType() const { return m_Type; }

    bool IsMouseHovering() {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        glm::vec2 pos = m_Transform.translation;
        return mousePos.x >= pos.x - 25.0f && mousePos.x <= pos.x + 25.0f &&
               mousePos.y >= pos.y - 35.0f && mousePos.y <= pos.y + 35.0f;
    }

private:
    PlantType m_Type; // 使用全域型別
};

#endif