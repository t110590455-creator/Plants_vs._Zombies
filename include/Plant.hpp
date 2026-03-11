//
// Created by hankl on 2026/3/10.
//

#ifndef PLANT_HPP
#define PLANT_HPP

#include "pch.hpp"

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Plant : public Util::GameObject {
public:
    Plant(const std::string& imagePath, int row, int col, const glm::vec2& position, int hp);
    virtual ~Plant() = default;

    virtual void Update() {}
    virtual void TakeDamage(int damage);

    int GetRow() const { return m_Row; }
    int GetCol() const { return m_Col; }
    int GetHP() const { return m_HP; }
    bool IsAlive() const { return m_Alive; }

protected:
    int m_Row;
    int m_Col;
    int m_HP;
    bool m_Alive;
};

#endif //PLANT_HPP
