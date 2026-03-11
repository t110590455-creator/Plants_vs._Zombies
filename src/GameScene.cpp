//
// Created by hankl on 2026/3/10.
//
#include "GameScene.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"

GameScene::GameScene() {
    auto backgroundImage = std::make_shared<Util::Image>(
        RESOURCE_DIR "/background.jpg"
    );

    m_Background = std::make_shared<Util::GameObject>(backgroundImage, 0.0f);
    m_Background->m_Transform.translation = {0.0f, 0.0f};

    m_Renderer.AddChild(m_Background);

    m_LastSpawnTime = Util::Time::GetElapsedTimeMs() / 1000.0f;
}

void GameScene::Update() {
    HandleInput();

    for (auto& plant : m_Plants) {
        plant->Update();
    }

    TrySpawnZombie();

    UpdateZombiePlantInteractions();
    UpdateZombies();

    UpdatePeashooters();
    UpdateProjectiles();
    CheckProjectileZombieCollisions();

    RemoveDeadProjectiles();
    RemoveDeadPlants();
    RemoveDeadZombies();

    m_Renderer.Update();
}

void GameScene::HandleInput() {
    // 防止連續觸發：利用 m_WasMousePressed 變數來記錄上一幀的狀態。
    // 這能確保玩家按住滑鼠時，只會觸發一次種植動作，而不是每秒噴出 60 顆植物。
    const bool isMousePressed = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);

    if (isMousePressed && !m_WasMousePressed) {
        // 呼叫種植邏輯：當偵測到滑鼠左鍵按下，且上一幀沒按時，執行
        TryPlantAtMousePosition();
    }

    m_WasMousePressed = isMousePressed;
}

void GameScene::TryPlantAtMousePosition() {
    const glm::vec2 mousePos = Util::Input::GetCursorPosition();

    int row = -1;
    int col = -1;

    // 呼叫了GameBoard中的ScreenToGrid。如果玩家點在選單或背景空白處，這行會直接擋掉。
    if (!m_Board.ScreenToGrid(mousePos.x, mousePos.y, row, col)) {
        LOG_DEBUG("Click outside board => x: {} y: {}", mousePos.x, mousePos.y);
        return;
    }

    // 檢查格子是否已有植物
    if (!m_Board.IsCellEmpty(row, col)) {
        LOG_DEBUG("Cell already occupied => row: {} col: {}", row, col);
        return;
    }

    // 取得格子的中心點
    glm::vec2 cellCenter = m_Board.GetCellCenter(row, col);

    // 建立植物物件
    auto plant = std::make_shared<Peashooter>(row, col, cellCenter);

    m_Plants.push_back(plant); // 存入一個 std::vector 或清單，方便後續統一管理（例如讓所有植物一起攻擊）。
    m_Board.PlacePlant(plant.get(), row, col); // m_Board：在棋盤陣列中標記該位置已有植物。
    m_Renderer.AddChild(plant);

    LOG_DEBUG("Plant placed => row: {} col: {}", row, col);
}

void GameScene::TrySpawnZombie() {
    const float currentTime = Util::Time::GetElapsedTimeMs() / 1000.0f;

    if (currentTime - m_LastSpawnTime < m_SpawnInterval) {
        return;
    }

    m_LastSpawnTime = currentTime;

    int row = rand() % GameBoard::ROWS;
    glm::vec2 spawnPos = m_Board.GetCellCenter(row, GameBoard::COLS - 1);
    spawnPos.x += 300.0f;

    auto zombie = std::make_shared<BasicZombie>(row, spawnPos);

    m_Zombies.push_back(zombie);
    m_Renderer.AddChild(zombie);

    LOG_DEBUG("Zombie spawned => row: {}", row);
}

void GameScene::UpdateZombies() {
    for (auto& zombie : m_Zombies) {
        if (!zombie->IsAlive()) {
            continue;
        }

        Plant* targetPlant = FindPlantInZombieFront(zombie.get());
        if (targetPlant != nullptr) {
            continue;
        }

        zombie->Update();
    }
}

void GameScene::UpdatePeashooters() {
    const float currentTime = Util::Time::GetElapsedTimeMs() / 1000.0f;

    for (auto& plantBase : m_Plants) {
        auto peashooter = std::dynamic_pointer_cast<Peashooter>(plantBase);
        if (!peashooter) {
            continue;
        }

        if (!peashooter->IsAlive()) {
            continue;
        }

        const int row = peashooter->GetRow();
        const float x = peashooter->m_Transform.translation.x;

        if (!HasZombieInFront(row, x)) {
            continue;
        }

        if (!peashooter->CanShoot(currentTime)) {
            continue;
        }

        glm::vec2 peaPos = peashooter->m_Transform.translation;
        peaPos.x += 45.0f;

        auto pea = std::make_shared<Pea>(row, peaPos);
        m_Projectiles.push_back(pea);
        m_Renderer.AddChild(pea);

        peashooter->RecordShootTime(currentTime);

        LOG_DEBUG("Pea fired => row: {}", row);
    }
}

void GameScene::UpdateProjectiles() {
    for (auto& projectile : m_Projectiles) {
        if (projectile->IsAlive()) {
            projectile->Update();
        }
    }
}

bool GameScene::HasZombieInFront(int row, float x) const {
    for (const auto& zombie : m_Zombies) {
        if (!zombie->IsAlive()) {
            continue;
        }

        if (zombie->GetRow() != row) {
            continue;
        }

        if (zombie->m_Transform.translation.x > x) {
            return true;
        }
    }
    return false;
}

void GameScene::CheckProjectileZombieCollisions() {
    for (auto& projectile : m_Projectiles) {
        if (!projectile->IsAlive()) {
            continue;
        }

        for (auto& zombie : m_Zombies) {
            if (!zombie->IsAlive()) {
                continue;
            }

            if (projectile->GetRow() != zombie->GetRow()) {
                continue;
            }

            const float dx = std::abs(projectile->m_Transform.translation.x - zombie->m_Transform.translation.x);
            const float dy = std::abs(projectile->m_Transform.translation.y - zombie->m_Transform.translation.y);

            if (dx < 40.0f && dy < 40.0f) {
                zombie->TakeDamage(projectile->GetDamage());
                projectile->Destroy();

                LOG_DEBUG("Zombie hit => row: {}, hp: {}", zombie->GetRow(), zombie->GetHP());
                break;
            }
        }
    }
}

void GameScene::RemoveDeadProjectiles() {
    m_Projectiles.erase(
        std::remove_if(
            m_Projectiles.begin(),
            m_Projectiles.end(),
            [](const std::shared_ptr<Projectile>& projectile) {
                if (!projectile->IsAlive()) {
                    projectile->SetVisible(false);
                    return true;
                }

                if (projectile->m_Transform.translation.x > 2000.0f) {
                    projectile->SetVisible(false);
                    return true;
                }

                return false;
            }
        ),
        m_Projectiles.end()
    );
}

Plant* GameScene::FindPlantInZombieFront(Zombie* zombie) const {
    if (zombie == nullptr || !zombie->IsAlive()) {
        return nullptr;
    }

    for (const auto& plant : m_Plants) {
        if (!plant->IsAlive()) {
            continue;
        }

        if (plant->GetRow() != zombie->GetRow()) {
            continue;
        }

        const float dx = std::abs(plant->m_Transform.translation.x - zombie->m_Transform.translation.x);
        const float dy = std::abs(plant->m_Transform.translation.y - zombie->m_Transform.translation.y);

        if (dx < 55.0f && dy < 50.0f) {
            return plant.get();
        }
    }

    return nullptr;
}

void GameScene::UpdateZombiePlantInteractions() {
    const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    const int zombieDamagePerSecond = 25;

    for (auto& zombie : m_Zombies) {
        if (!zombie->IsAlive()) {
            continue;
        }

        Plant* targetPlant = FindPlantInZombieFront(zombie.get());
        if (targetPlant != nullptr) {
            int damage = static_cast<int>(zombieDamagePerSecond * deltaTime);
            if (damage < 1) {
                damage = 1;
            }

            targetPlant->TakeDamage(damage);

            LOG_DEBUG("Zombie eating plant => row: {}, plant hp: {}", targetPlant->GetRow(), targetPlant->GetHP());
        }
    }
}

void GameScene::RemoveDeadPlants() {
    for (auto& plant : m_Plants) {
        if (!plant->IsAlive()) {
            m_Board.RemovePlant(plant->GetRow(), plant->GetCol());
            plant->SetVisible(false);
        }
    }

    m_Plants.erase(
        std::remove_if(
            m_Plants.begin(),
            m_Plants.end(),
            [](const std::shared_ptr<Plant>& plant) {
                return !plant->IsAlive();
            }
        ),
        m_Plants.end()
    );
}

void GameScene::RemoveDeadZombies() {
    for (auto& zombie : m_Zombies) {
        if (!zombie->IsAlive()) {
            zombie->SetVisible(false);
        }
    }

    m_Zombies.erase(
        std::remove_if(
            m_Zombies.begin(),
            m_Zombies.end(),
            [](const std::shared_ptr<Zombie>& zombie) {
                return !zombie->IsAlive();
            }
        ),
        m_Zombies.end()
    );
}