//
// Created by hankl on 2026/3/10.
//

#ifndef PEASHOOTER_HPP
#define PEASHOOTER_HPP

#include "Plant.hpp"

class Peashooter : public Plant {
public:
    Peashooter(int row, int col, const glm::vec2& position);

    void Update() override;

    bool CanShoot(float currentTime) const;
    void RecordShootTime(float currentTime);

private:
    float m_ShootInterval;
    float m_LastShootTime;
};

#endif //PEASHOOTER_HPP
