#ifndef SEEDPACKET_HPP
#define SEEDPACKET_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Time.hpp"
#include "PlantType.hpp"
#include "Config.hpp"
#include <vector>
#include <string>

class SeedPacket : public Util::GameObject {
public:
    SeedPacket(PlantType type, const std::string& normalPath, float cooldownTime)
        : m_Type(type), m_NormalPath(normalPath), m_CooldownDuration(cooldownTime) {

        m_NormalDrawable = std::make_shared<Util::Image>(normalPath);
        SetDrawable(m_NormalDrawable);
        m_Transform.scale = {3.0f, 3.0f};
        SetZIndex(15.0f);

        // --- 修正點 2：處理 Wallnut 路徑特殊命名問題 ---
        std::string baseDir = "C:/Users/user/OOPL-113820019/Resources/Image/Plant/";
        std::string folderName;
        std::string fileNamePart;

        switch(type) {
            case PlantType::PEASHOOTER: folderName = "Peashooter"; fileNamePart = "Peashooter"; break;
            case PlantType::SUNFLOWER:  folderName = "Sunflower";  fileNamePart = "Sunflower";  break;
            case PlantType::CHERRYBOMB: folderName = "Cherrybomb"; fileNamePart = "Cherry Bomb"; break;
            case PlantType::WALLNUT:    folderName = "Wallnut";    fileNamePart = "Wall-Nut";    break; // 這裡修正為 Wall-Nut
            case PlantType::POTATOMINE: folderName = "Potato mine"; fileNamePart = "Potato Mine"; break;
            default: folderName = "Peashooter"; fileNamePart = "Peashooter"; break;
        }

        // 預載 6 幀 Loading 圖片
        for (int i = 1; i <= 6; ++i) {
            // 格式範例：.../Wallnut/Loding/Wall-Nut - Loding 1.png
            std::string fullPath = baseDir + folderName + "/Loding/" + fileNamePart + " - Loding " + std::to_string(i) + ".png";
            m_LoadingPaths.push_back(fullPath);
        }
    }

    // SeedPacket.hpp 內的 Update 函式
    void Update() {
        if (!m_IsLoading) return;

        m_Timer += Util::Time::GetDeltaTime();
        LOG_DEBUG("Packet Timer: {}", m_Timer);

        // 判斷是否結束冷卻
        if (m_Timer >= m_CooldownDuration) {
            m_IsLoading = false;
            m_Timer = 0.0f;
            SetDrawable(m_NormalDrawable); // 變回彩色
            return; // 結束本次更新
        }

        // 計算 Loading 幀數 (0~5)
        float progress = m_Timer / m_CooldownDuration;
        int frameIndex = static_cast<int>(progress * 6.0f);

        // 防呆處理
        if (frameIndex < 0) frameIndex = 0;
        if (frameIndex > 5) frameIndex = 5;

        // 更新圖片
        SetDrawable(std::make_shared<Util::Image>(m_LoadingPaths[frameIndex]));
    }

    // SeedPacket.hpp 內的 StartCooldown 函式
    void StartCooldown() {
        // --- 關鍵防護：如果已經在 Loading，就直接回傳，不重置 Timer ---
        if (m_IsLoading) return;

        if (m_CooldownDuration <= 0.0f) return;

        m_IsLoading = true;
        m_Timer = 0.0f;

        // 立即換上第一幀圖片
        if (!m_LoadingPaths.empty()) {
            SetDrawable(std::make_shared<Util::Image>(m_LoadingPaths[0]));
        }
    }

    void SetPosition(glm::vec2 pos) { m_Transform.translation = pos; }
    PlantType GetType() const { return m_Type; }
    bool IsReady() const { return !m_IsLoading; }

    bool IsMouseHovering() {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        glm::vec2 pos = m_Transform.translation;
        return mousePos.x >= pos.x - 25.0f && mousePos.x <= pos.x + 25.0f &&
               mousePos.y >= pos.y - 35.0f && mousePos.y <= pos.y + 35.0f;
    }

private:
    PlantType m_Type;
    std::string m_NormalPath;
    std::vector<std::string> m_LoadingPaths;
    std::shared_ptr<Util::Image> m_NormalDrawable;

    bool m_IsLoading = false;
    float m_Timer = 0.0f;
    float m_CooldownDuration;
};

#endif