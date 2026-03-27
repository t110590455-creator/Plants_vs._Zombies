#include "CherryBomb.hpp"

#include "Util/Time.hpp"

CherryBomb::CherryBomb(int row, int col, const glm::vec2& position)
    : Plant(RESOURCE_DIR "/CherryBomb/CherryBomb_0.png", row, col, position,
            300, 150) {
}

void CherryBomb::Update() {
    if (!m_Alive) {
        return;
    }

    const float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;
    m_ExplodeTimer -= dt;

    // 蓄力動畫：依倒數進度切換 CherryBomb_0 ~ CherryBomb_6
    if (m_ExplodeTimer > 0.0f && !m_TriggerDamage) {
        constexpr float kExplodeDuration = 1.2f;
        constexpr int kFrameCount = 7;

        const float elapsed = kExplodeDuration - m_ExplodeTimer;
        int frameIndex = static_cast<int>((elapsed / kExplodeDuration) * kFrameCount);
        if (frameIndex < 0) {
            frameIndex = 0;
        } else if (frameIndex >= kFrameCount) {
            frameIndex = kFrameCount - 1;
        }

        if (frameIndex != m_LastAnimFrame) {
            m_LastAnimFrame = frameIndex;
            m_Drawable = std::make_shared<Util::Image>(
                std::string(RESOURCE_DIR "/CherryBomb/CherryBomb_") +
                std::to_string(frameIndex) + ".png");
        }
    }

    // 階段一：快爆炸時的視覺反饋（剩餘約 0.4 秒）
    if (m_ExplodeTimer < 0.4f && !m_IsRed) {
        m_IsRed = true;
        m_Transform.scale = glm::vec2(1.2f, 1.2f);
        // TODO: 你若有對應「紅色/蓄力」幀，這裡可以換貼圖
        // m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/CherryBomb/CherryBomb_1.png");
    }

    // 階段二：觸發爆炸（由 GameScene 結算九宮格傷害）
    if (m_ExplodeTimer <= 0.0f && !m_TriggerDamage) {
        m_TriggerDamage = true;

        // 爆炸特效圖
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Boom.png");
        m_Transform.scale = glm::vec2(1.5f, 1.5f);
    }
}