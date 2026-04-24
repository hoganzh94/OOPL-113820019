#ifndef UICONTROLLER_HPP
#define UICONTROLLER_HPP

#include <memory>
#include <algorithm>
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Image.hpp"

class UIController {
public:
    void UpdateSunCount(int count, std::shared_ptr<Util::Text> textDrawable) {
        if (textDrawable) textDrawable->SetText(std::to_string(count));
    }

    void UpdateProgressBar(float progress, std::shared_ptr<Util::GameObject> fillObj) {
        if (!fillObj) return;
        progress = std::clamp(progress, 0.0f, 1.0f);
        float barStartX = -580.0f;
        float barEndX = -320.0f;
        fillObj->m_Transform.translation.x = barStartX + (barEndX - barStartX) * progress;
    }

    bool UpdateStartBanner(float timer, std::shared_ptr<Util::GameObject> bannerObj, std::string& currentPath) {
        if (!bannerObj) return true;

        std::string base = RESOURCE_DIR;
        std::string path = "";

        if (timer < 0.6f) {
            path = base + "/Image/Scene/Start Text 1.png";
            bannerObj->SetVisible(true);
        } else if (timer < 1.2f) {
            path = base + "/Image/Scene/Start Text 2.png";
        } else if (timer < 1.8f) {
            path = base + "/Image/Scene/Start Text 3.png";
        } else {
            bannerObj->SetVisible(false);
            return true; // 播放結束
        }

        // --- 你要求修改的部分 ---
        // 移除 static，改用外部傳進來的 currentPath 做比對
        if (path != currentPath) {
            bannerObj->SetDrawable(std::make_shared<Util::Image>(path));
            currentPath = path; // 更新外部的狀態
        }
        // ------------------------

        float scaleEffect = 1.5f + 0.15f * std::sin(timer * 12.0f);
        bannerObj->m_Transform.scale = {scaleEffect, scaleEffect};

        return false;
    }
};

#endif