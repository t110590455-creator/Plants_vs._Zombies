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

    // 讓所有卡片顯示成固定大小（避免素材太大、點擊區域不一致）
    const glm::vec2 drawableSize = m_Drawable ? m_Drawable->GetSize() : glm::vec2(1.0f, 1.0f);
    const float sx = (drawableSize.x != 0.0f) ? (m_Width / drawableSize.x) : 1.0f;
    const float sy = (drawableSize.y != 0.0f) ? (m_Height / drawableSize.y) : 1.0f;
    m_BaseScale = {sx, sy};
    m_Transform.scale = m_BaseScale;
}


// 確認屬標點擊時是否點在卡片範圍中
bool SeedCard::ContainsPoint(const glm::vec2& point) const {
    // 卡片的上下左右
    // 以「顯示尺寸」為準（因為我們會縮放到固定 52x73，再乘上視覺縮放因子）
    const glm::vec2 scaledSize = GetScaledSize();
    const float left = m_Transform.translation.x - scaledSize.x / 2.0f;
    const float right = m_Transform.translation.x + scaledSize.x / 2.0f;
    const float top = m_Transform.translation.y + scaledSize.y / 2.0f;
    const float bottom = m_Transform.translation.y - scaledSize.y / 2.0f;

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

void SeedCard::SetVisualScaleFactor(float factor) {
    m_Transform.scale = m_BaseScale * factor;
}