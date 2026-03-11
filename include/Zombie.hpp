//
// Created by hankl on 2026/3/10.
//

#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "pch.hpp"

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Zombie : public Util::GameObject {
public:
    Zombie(const std::string& imagePath,
           int row,
           const glm::vec2& position,
           int hp,
           float speed);

    virtual ~Zombie() = default;

    virtual void Update();
    virtual void TakeDamage(int damage);

    int GetRow() const { return m_Row; }
    int GetHP() const { return m_HP; }
    float GetSpeed() const { return m_Speed; }
    bool IsAlive() const { return m_Alive; }

protected:
    int m_Row;
    int m_HP;
    float m_Speed;
    bool m_Alive;
};

#endif //ZOMBIE_HPP
