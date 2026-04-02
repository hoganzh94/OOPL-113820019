#ifndef UICONTROLLER_HPP
#define UICONTROLLER_HPP

#include <memory>
#include <algorithm>
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"

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
};

#endif