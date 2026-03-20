//
// Created by hankl on 2026/3/13.
//
#include "SeedCard.hpp"

SeedCard::SeedCard(const std::string& imagePath,
                   PlantType plantType,
                   int cost,
                   float cooldown,
                   const glm::vec2& position)
    : Util::GameObject(std::make_shared<Util::Image>(imagePath), 30.0f),
      m_PlantType(plantType),
      m_Cost(cost),
      m_Selected(false),
      m_Cooldown(cooldown),
      m_LastUsedTime(-999.0f),
      m_Width(52.0f),
      m_Height(73.0f) {
    m_Transform.translation = position;
}


// 確認屬標點擊時是否點在卡片範圍中
bool SeedCard::ContainsPoint(const glm::vec2& point) const {
    // 卡片的上下左右
    const float left = m_Transform.translation.x - m_Width/2;
    const float right = m_Transform.translation.x + m_Width/2;
    const float top = m_Transform.translation.y + m_Height/2;
    const float bottom = m_Transform.translation.y - m_Height/2;

    // 如果point在卡片範圍中回傳True
    return point.x >= left && point.x <= right &&
           point.y <= top && point.y >= bottom;
}


// 設為選擇/未選擇
void SeedCard::SetSelected(bool selected) {
    m_Selected = selected;
}

// 回傳是否在冷卻中
bool SeedCard::IsCoolingDown(float currentTime) const {
    return currentTime - m_LastUsedTime < m_Cooldown;
}

// 回傳是否可種植此植物
bool SeedCard::IsUsable(int currentSunPoints, float currentTime) const {
    if (currentSunPoints < m_Cost) {
        return false;
    }

    if (IsCoolingDown(currentTime)) {
        return false;
    }

    return true;
}

void SeedCard::TriggerCooldown(float currentTime) {
    m_LastUsedTime = currentTime;
}

float SeedCard::GetRemainingCooldown(float currentTime) const {
    const float remaining = m_Cooldown - (currentTime - m_LastUsedTime);
    return std::max(0.0f, remaining);
}