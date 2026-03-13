//
// Created by hankl on 2026/3/13.
//

#ifndef SUN_HPP
#define SUN_HPP

#include "pch.hpp"

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Sun : public Util::GameObject {
public:
    Sun(const glm::vec2& position, int value = 25);

    int GetValue() const { return m_Value; }
    bool IsAlive() const { return m_Alive; }

    void Collect();
    bool ContainsPoint(const glm::vec2& point) const;

private:
    int m_Value;
    bool m_Alive;
    float m_HalfWidth;
    float m_HalfHeight;
};

#endif //SUN_HPP
