//
// Created by hankl on 2026/3/13.
//

#ifndef SUNFLOWER_HPP
#define SUNFLOWER_HPP

#include "Plant.hpp"

class Sunflower : public Plant {
public:
    Sunflower(int row, int col, const glm::vec2& position);

    void Update() override;

    bool CanGenerateSun(float currentTime) const;
    void RecordGenerateTime(float currentTime);

private:
    float m_GenerateInterval;
    float m_LastGenerateTime;
};

#endif //SUNFLOWER_HPP
