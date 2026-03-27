#ifndef CHERRYBOMB_HPP
#define CHERRYBOMB_HPP

#include "Plant.hpp"

class CherryBomb : public Plant {
public:
    CherryBomb(int row, int col, const glm::vec2& position);

    // 覆寫 Update 以處理爆炸計時
    void Update() override;

    // 讓外部 GameManager 知道現在是否該結算傷害
    bool ShouldTriggerDamage() const { return m_TriggerDamage; }
    
    // 傷害結算完後由外部呼叫，標記爆炸完成
    void MarkAsExploded() { m_TriggerDamage = false; m_Alive = false; }

private:
    float m_ExplodeTimer = 1.2f;    // 爆炸倒數（秒）
    bool m_TriggerDamage = false;   // 是否處於「爆炸瞬間」狀態
    bool m_IsRed = false;           // 視覺狀態紀錄
    int m_LastAnimFrame = 0;       // 避免每幀都重載貼圖
};

#endif // CHERRYBOMB_HPP