//
// Created by hankl on 2026/3/13.
//

#ifndef SEEDCARD_HPP
#define SEEDCARD_HPP

#include "pch.hpp"

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

// 植物種類
enum class PlantType {
    PEASHOOTER,
    SUNFLOWER,
    CHERRYBOMB
};

class SeedCard : public Util::GameObject {
public:
    SeedCard(const std::string& imagePath,
             PlantType plantType,
             int cost,
             float cooldown,
             const glm::vec2& position);

    PlantType GetPlantType() const { return m_PlantType; }
    int GetCost() const { return m_Cost; }

    bool ContainsPoint(const glm::vec2& point) const;

    void SetSelected(bool selected);
    bool IsSelected() const { return m_Selected; }

    bool IsCoolingDown(float currentTime) const;
    bool IsUsable(int currentSunPoints, float currentTime) const;
    void TriggerCooldown(float currentTime);

    float GetRemainingCooldown(float currentTime) const;

    // 將卡片顯示縮放乘上一個因子（例如 0.9 / 1.1）
    void SetVisualScaleFactor(float factor);

private:
    PlantType m_PlantType;
    int m_Cost;
    bool m_Selected;

    float m_Cooldown;
    float m_LastUsedTime;

    float m_Width;
    float m_Height;

    glm::vec2 m_BaseScale {1.0f, 1.0f};
};

#endif //SEEDCARD_HPP
