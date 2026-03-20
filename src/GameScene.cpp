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
    //背景
    auto backgroundImage = std::make_shared<Util::Image>(
        RESOURCE_DIR "/background.jpg"
    );
    m_Background = std::make_shared<Util::GameObject>(backgroundImage, 0.0f);
    m_Background->m_Transform.translation = {0.0f, 0.0f};
    m_Renderer.AddChild(m_Background);

    auto chooserbackgroundImage = std::make_shared<Util::Image>(
        RESOURCE_DIR "/ChooserBackground.png"
    );
    m_ChooserBackground = std::make_shared<Util::GameObject>(chooserbackgroundImage, 1.0f);
    m_ChooserBackground->m_Transform.translation = {-300.0f, 260.0f};
    m_Renderer.AddChild(m_ChooserBackground);


    // Gameover畫面
    auto gameOverImage = std::make_shared<Util::Image>(
        RESOURCE_DIR "/game_over.png"
    );
    m_GameOverText = std::make_shared<Util::GameObject>(gameOverImage, 100.0f);
    m_GameOverText->m_Transform.translation = {0.0f, 0.0f};
    m_GameOverText->SetVisible(false);
    m_Renderer.AddChild(m_GameOverText);

    // Sun Text
    // 這段依你的 Text API 微調
    m_SunText = std::make_shared<Util::Text>(
        RESOURCE_DIR "/font.ttf",
        20,
        "200",
        Util::Color(0, 0, 0)
    );
    m_SunTextObject = std::make_shared<Util::GameObject>(m_SunText, 50.0f);
    m_SunTextObject->m_Transform.translation = {-510.0f, 230.0f};
    m_Renderer.AddChild(m_SunTextObject);

    // 卡片UI
    auto peashooterCard = std::make_shared<SeedCard>(
        RESOURCE_DIR "/card_peashooter.png",
        PlantType::PEASHOOTER,
        100,
        5.0f,
        glm::vec2(-455.0f, 262.0f)
    );

    auto sunflowerCard = std::make_shared<SeedCard>(
        RESOURCE_DIR "/card_sunflower.png",
        PlantType::SUNFLOWER,
        50,
        5.0f,
        glm::vec2(-400.0f, 262.0f)
    );

    m_ShovelButton = std::make_shared<ShovelButton>(
    RESOURCE_DIR "/shovel2.png",
    glm::vec2(-580.0f, -100.0f)
    );

    m_SeedCards.push_back(peashooterCard);
    m_SeedCards.push_back(sunflowerCard);

    m_Renderer.AddChild(peashooterCard);
    m_Renderer.AddChild(sunflowerCard);
    UpdateSeedCardSelectionVisual();

    m_Renderer.AddChild(m_ShovelButton);
    UpdateShovelVisual();

    CreateLawnMowers();

    m_LastSpawnTime = Util::Time::GetElapsedTimeMs() / 1000.0f;

    // 初始化關卡
    m_LevelStartTime = Util::Time::GetElapsedTimeMs() / 1000.0f;
    m_LastSkySunSpawnTime = m_LevelStartTime;
    m_NextSkySunInterval = 9.0f;
    InitLevelWaves();

    //
    auto victoryImage = std::make_shared<Util::Image>(
    RESOURCE_DIR "/game_victory.png"
    );

    m_VictoryText = std::make_shared<Util::GameObject>(victoryImage, 100.0f);
    m_VictoryText->m_Transform.translation = {0.0f, 0.0f};
    m_VictoryText->SetVisible(false);

    m_Renderer.AddChild(m_VictoryText);
}

void GameScene::Update() {
    if (m_State == State::GAME_OVER) {
        m_Renderer.Update();
        return;
    }

    if (m_LevelState == LevelState::VICTORY) {
        UpdateSuns();
        RemoveDeadSuns();
        m_Renderer.Update();
        return;
    }

    HandleInput();

    // 植物類別Update
    for (auto& plant : m_Plants) {
        if (plant->IsAlive()) {
            plant->Update();
        }
    }

    UpdateSunflowers();
    UpdateSkySunSpawning();
    UpdateSuns();

    // 生成殭屍
    //TrySpawnZombie();

    UpdateLevelFlow();

    CheckLawnMowerActivation();
    UpdateLawnMowers();

    UpdateZombiePlantInteractions();
    UpdateZombies();

    UpdatePeashooters();
    UpdateProjectiles();
    CheckProjectileZombieCollisions();
    CheckLawnMowerZombieCollisions();

    RemoveDeadProjectiles();
    RemoveDeadPlants();
    RemoveDeadZombies();
    RemoveDeadSuns();
    RemoveDeadLawnMowers();

    CheckGameOver();
    UpdateSeedCardUsabilityVisual();

    m_Renderer.Update();
}

void GameScene::HandleInput() {
    // 選擇植物 目前用NUM_*來決定種類，之後改成卡片UI
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_1)) {
        m_SelectedPlantType = PlantType::PEASHOOTER;
        UpdateSeedCardSelectionVisual();
    }

    if (Util::Input::IsKeyDown(Util::Keycode::NUM_2)) {
        m_SelectedPlantType = PlantType::SUNFLOWER;
        UpdateSeedCardSelectionVisual();
    }
    // 防止連續觸發：利用 m_WasMousePressed 變數來記錄上一幀的狀態。
    // 這能確保玩家按住滑鼠時，只會觸發一次種植動作，而不是每秒噴出 60 顆植物。
    const bool isMousePressed = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);

    if (isMousePressed && !m_WasMousePressed) {
        if (!TryToggleShovelAtMousePosition()) {
            if (m_ShovelMode) {
                TryRemovePlantAtMousePosition();
            } else if (!TrySelectSeedCardAtMousePosition()) {
                if (!TryCollectSunAtMousePosition()) {
                    TryPlantAtMousePosition();
                }
            }
        }
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
    std::shared_ptr<Plant> plant = nullptr;

    if (m_SelectedPlantType == PlantType::PEASHOOTER) {
        plant = std::make_shared<Peashooter>(row, col, cellCenter);
    } else if (m_SelectedPlantType == PlantType::SUNFLOWER) {
        plant = std::make_shared<Sunflower>(row, col, cellCenter);
    }

    const float currentTime = Util::Time::GetElapsedTimeMs() / 1000.0f;
    SeedCard* selectedCard = FindSeedCardByPlantType(m_SelectedPlantType);

    if (plant == nullptr) {
        return;
    }

    if (!selectedCard->IsUsable(m_SunPoints, currentTime)) {
        LOG_DEBUG("Card unavailable => cooling or insufficient sun");
        return;
    }

    // 檢查Cost是否足夠種植目前所選植物
    if (m_SunPoints < plant->GetCost()) {
        LOG_DEBUG("Not enough sun => current: {}, need: {}", m_SunPoints, plant->GetCost());
        return;
    }

    m_SunPoints -= plant->GetCost();
    UpdateSunText();

    selectedCard->TriggerCooldown(currentTime);

    m_Plants.push_back(plant); // 存入一個 std::vector 或清單，方便後續統一管理（例如讓所有植物一起攻擊）。
    m_Board.PlacePlant(plant.get(), row, col); // m_Board：在棋盤陣列中標記該位置已有植物。
    m_Renderer.AddChild(plant);

    UpdateSeedCardSelectionVisual();
    UpdateSeedCardUsabilityVisual();

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

    // LOG_DEBUG("Zombie spawned => row: {}", row);
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

void GameScene::CheckGameOver() {
    if (m_LevelState == LevelState::VICTORY) {
        return;
    }
    for (const auto& zombie : m_Zombies) {
        if (!zombie->IsAlive()) {
            continue;
        }

        if (zombie->m_Transform.translation.x <= -460.0f) { // 代表殭屍走到畫面左邊某條線時判定失敗。
            EnterGameOver();                                //這個值你可能之後要依背景圖微調。
            return;
        }
        // LOG_DEBUG("Zombie x = {}", zombie->m_Transform.translation.x);
    }

}

void GameScene::EnterGameOver() {
    m_State = State::GAME_OVER;
    m_GameOverText->SetVisible(true);

    LOG_DEBUG("GAME OVER");
}

// 如果你的 Util::Text 支援改字串，就加入這個函式。
void GameScene::UpdateSunText() {
    if (m_SunText != nullptr) {
        m_SunText->SetText(std::to_string(m_SunPoints));
    }
}

bool GameScene::TryCollectSunAtMousePosition() {
    const glm::vec2 mousePos = Util::Input::GetCursorPosition();

    for (auto& sun : m_Suns) {
        if (!sun->IsAlive()) {
            continue;
        }

        if (sun->ContainsPoint(mousePos)) {
            m_SunPoints += sun->GetValue();
            sun->Collect();
            sun->SetVisible(false);
            UpdateSunText();

            LOG_DEBUG("Sun collected => +{}, current sun: {}", sun->GetValue(), m_SunPoints);
            return true;
        }
    }

    return false;
}

void GameScene::UpdateSunflowers() {
    const float currentTime = Util::Time::GetElapsedTimeMs() / 1000.0f;

    for (auto& plantBase : m_Plants) {
        auto sunflower = std::dynamic_pointer_cast<Sunflower>(plantBase);
        if (!sunflower) {
            continue;
        }

        if (!sunflower->IsAlive()) {
            continue;
        }

        if (!sunflower->CanGenerateSun(currentTime)) {
            continue;
        }

        glm::vec2 sunPos = sunflower->m_Transform.translation;
        sunPos.y -= 20.0f;

        auto sun = std::make_shared<Sun>(sunPos, 25);
        m_Suns.push_back(sun);
        m_Renderer.AddChild(sun);

        sunflower->RecordGenerateTime(currentTime);

        LOG_DEBUG("Sun generated => row: {}, col: {}", sunflower->GetRow(), sunflower->GetCol());
    }
}

void GameScene::RemoveDeadSuns() {
    m_Suns.erase(
        std::remove_if(
            m_Suns.begin(),
            m_Suns.end(),
            [](const std::shared_ptr<Sun>& sun) {
                return !sun->IsAlive();
            }
        ),
        m_Suns.end()
    );
}

bool GameScene::TrySelectSeedCardAtMousePosition() {
    const glm::vec2 mousePos = Util::Input::GetCursorPosition();
    const float currentTime = Util::Time::GetElapsedTimeMs() / 1000.0f;

    for (auto& card : m_SeedCards) {
        if (!card->ContainsPoint(mousePos)) {
            continue;
        }

        if (!card->IsUsable(m_SunPoints, currentTime)) {
            LOG_DEBUG("Card cannot be selected now");
            return true;
        }

        m_SelectedPlantType = card->GetPlantType();
        UpdateSeedCardSelectionVisual();
        UpdateSeedCardUsabilityVisual();

        if (m_SelectedPlantType == PlantType::PEASHOOTER) {
            LOG_DEBUG("Selected card => Peashooter");
        } else if (m_SelectedPlantType == PlantType::SUNFLOWER) {
            LOG_DEBUG("Selected card => Sunflower");
        }

        return true;
    }

    return false;
}

void GameScene::UpdateSeedCardSelectionVisual() {
    for (auto& card : m_SeedCards) {
        card->SetSelected(card->GetPlantType() == m_SelectedPlantType);
    }

    UpdateSeedCardUsabilityVisual();
}

SeedCard* GameScene::FindSeedCardByPlantType(PlantType plantType) const {
    for (const auto& card : m_SeedCards) {
        if (card->GetPlantType() == plantType) {
            return card.get();
        }
    }
    return nullptr;
}

void GameScene::UpdateSeedCardUsabilityVisual() {
    const float currentTime = Util::Time::GetElapsedTimeMs() / 1000.0f;

    for (auto& card : m_SeedCards) {
        const bool usable = card->IsUsable(m_SunPoints, currentTime);
        const bool selected = (card->GetPlantType() == m_SelectedPlantType);

        if (!usable) {
            card->m_Transform.scale = {0.9f, 0.9f};
        } else if (selected) {
            card->m_Transform.scale = {1.1f, 1.1f};
        } else {
            card->m_Transform.scale = {1.0f, 1.0f};
        }
    }
}


bool GameScene::TryToggleShovelAtMousePosition() {
    if (m_ShovelButton == nullptr) {
        return false;
    }

    const glm::vec2 mousePos = Util::Input::GetCursorPosition();

    if (!m_ShovelButton->ContainsPoint(mousePos)) {
        return false;
    }

    m_ShovelMode = !m_ShovelMode;
    UpdateShovelVisual();

    if (m_ShovelMode) {
        LOG_DEBUG("Shovel mode ON");
    } else {
        LOG_DEBUG("Shovel mode OFF");
    }

    return true;
}


void GameScene::UpdateShovelVisual() {
    if (m_ShovelButton != nullptr) {
        m_ShovelButton->SetSelected(m_ShovelMode);
    }
}


void GameScene::TryRemovePlantAtMousePosition() {
    const glm::vec2 mousePos = Util::Input::GetCursorPosition();

    int row = -1;
    int col = -1;

    if (!m_Board.ScreenToGrid(mousePos.x, mousePos.y, row, col)) {
        return;
    }

    Plant* targetPlant = m_Board.GetPlant(row, col);
    if (targetPlant == nullptr) {
        LOG_DEBUG("No plant to remove => row: {} col: {}", row, col);
        return;
    }

    targetPlant->SetVisible(false);
    m_Board.RemovePlant(row, col);

    m_Plants.erase(
        std::remove_if(
            m_Plants.begin(),
            m_Plants.end(),
            [targetPlant](const std::shared_ptr<Plant>& plant) {
                return plant.get() == targetPlant;
            }
        ),
        m_Plants.end()
    );

    m_ShovelMode = false;
    UpdateShovelVisual();

    LOG_DEBUG("Plant removed => row: {} col: {}", row, col);
}


// 割草機
void GameScene::CreateLawnMowers() {
    for (int row = 0; row < GameBoard::ROWS; ++row) {
        glm::vec2 pos = m_Board.GetCellCenter(row, 0);
        pos.x = -460.0f;

        auto mower = std::make_shared<LawnMower>(row, pos);
        m_LawnMowers.push_back(mower);
        m_Renderer.AddChild(mower);
    }
}

void GameScene::UpdateLawnMowers() {
    for (auto& mower : m_LawnMowers) {
        if (!mower->IsAlive()) {
            continue;
        }

        mower->Update();
    }
}


void GameScene::CheckLawnMowerActivation() {
    for (auto& mower : m_LawnMowers) {
        if (!mower->IsAlive() || mower->IsActivated()) {
            continue;
        }

        for (auto& zombie : m_Zombies) {
            if (!zombie->IsAlive()) {
                continue;
            }

            if (zombie->GetRow() != mower->GetRow()) {
                continue;
            }

            const float dx = std::abs(zombie->m_Transform.translation.x - mower->m_Transform.translation.x);

            if (dx < 60.0f) {
                mower->Activate();
                LOG_DEBUG("Lawn mower activated => row: {}", mower->GetRow());
                break;
            }
        }
    }
}

void GameScene::CheckLawnMowerZombieCollisions() {
    for (auto& mower : m_LawnMowers) {
        if (!mower->IsAlive() || !mower->IsActivated()) {
            continue;
        }

        for (auto& zombie : m_Zombies) {
            if (!zombie->IsAlive()) {
                continue;
            }

            if (zombie->GetRow() != mower->GetRow()) {
                continue;
            }

            const float dx = std::abs(zombie->m_Transform.translation.x - mower->m_Transform.translation.x);
            const float dy = std::abs(zombie->m_Transform.translation.y - mower->m_Transform.translation.y);

            if (dx < 55.0f && dy < 45.0f) {
                zombie->TakeDamage(99999);
                LOG_DEBUG("Zombie crushed by lawn mower => row: {}", zombie->GetRow());
            }
        }
    }
}

void GameScene::RemoveDeadLawnMowers() {
    for (auto& mower : m_LawnMowers) {
        if (!mower->IsAlive()) {
            continue;
        }

        if (mower->IsActivated() && mower->m_Transform.translation.x > 2000.0f) {
            mower->SetVisible(false);
            mower->Destroy();
        }
    }

    m_LawnMowers.erase(
        std::remove_if(
            m_LawnMowers.begin(),
            m_LawnMowers.end(),
            [](const std::shared_ptr<LawnMower>& mower) {
                return !mower->IsAlive();
            }
        ),
        m_LawnMowers.end()
    );
}


// wave
void GameScene::InitLevelWaves() {
    m_Waves.clear();

    Wave normalWave1;
    normalWave1.isFinalWave = false;
    normalWave1.events = {
        {ZombieType::BASIC,    1, 16.0f, false},
        {ZombieType::BASIC,    3, 20.0f, false},
        {ZombieType::BASIC,    0, 24.0f, false},
        {ZombieType::BASIC,    4, 28.0f, false},
        {ZombieType::CONEHEAD, 2, 32.0f, false},
    };

    Wave normalWave2;
    normalWave2.isFinalWave = false;
    normalWave2.events = {
        {ZombieType::BASIC,    2, 36.0f, false},
        {ZombieType::BASIC,    1, 39.0f, false},
        {ZombieType::CONEHEAD, 4, 42.0f, false},
        {ZombieType::BASIC,    0, 45.0f, false},
        {ZombieType::BASIC,    3, 48.0f, false},
    };

    Wave finalWave;
    finalWave.isFinalWave = true;
    finalWave.events = {
        {ZombieType::CONEHEAD, 2, 55.0f, false},
        {ZombieType::BASIC,    1, 56.0f, false},
        {ZombieType::BASIC,    3, 57.0f, false},
        {ZombieType::CONEHEAD, 0, 58.0f, false},
        {ZombieType::BASIC,    4, 59.0f, false},
    };

    m_Waves.push_back(normalWave1);
    m_Waves.push_back(normalWave2);
    m_Waves.push_back(finalWave);
}

std::shared_ptr<Zombie> GameScene::CreateZombieByType(ZombieType type, int row, const glm::vec2& position) {
    switch (type) {
        case ZombieType::BASIC:
            return std::make_shared<BasicZombie>(row, position);
        case ZombieType::CONEHEAD:
            return std::make_shared<ConeheadZombie>(row, position);
        default:
            return nullptr;
    }
}

void GameScene::SpawnZombieFromEvent(SpawnEvent& event) {
    glm::vec2 spawnPos = m_Board.GetCellCenter(event.row, GameBoard::COLS - 1);
    spawnPos.x += 300.0f;

    auto zombie = CreateZombieByType(event.type, event.row, spawnPos);
    if (zombie == nullptr) {
        return;
    }

    m_Zombies.push_back(zombie);
    m_Renderer.AddChild(zombie);

    event.spawned = true;

    LOG_DEBUG("Wave zombie spawned => row: {}", event.row);
}

void GameScene::UpdateLevelFlow() {
    const float currentTime = Util::Time::GetElapsedTimeMs() / 1000.0f;
    const float elapsed = currentTime - m_LevelStartTime;

    if (m_LevelState == LevelState::PREPARE) {
        if (elapsed >= m_PrepareDuration) {
            m_LevelState = LevelState::NORMAL;
            LOG_DEBUG("Level state => NORMAL");
        } else {
            return;
        }
    }

    for (auto& wave : m_Waves) {
        for (auto& event : wave.events) {
            if (!event.spawned && elapsed >= event.spawnTime) {
                SpawnZombieFromEvent(event);

                if (wave.isFinalWave && m_LevelState != LevelState::FINAL) {
                    m_LevelState = LevelState::FINAL;
                    LOG_DEBUG("Level state => FINAL");
                }
            }
        }
    }

    if (AreAllWavesFinished() && AreAllZombiesCleared()) {
        EnterVictory();
    }
}

bool GameScene::AreAllWavesFinished() const {
    for (const auto& wave : m_Waves) {
        for (const auto& event : wave.events) {
            if (!event.spawned) {
                return false;
            }
        }
    }
    return true;
}

bool GameScene::AreAllZombiesCleared() const {
    for (const auto& zombie : m_Zombies) {
        if (zombie->IsAlive()) {
            return false;
        }
    }
    return true;
}


void GameScene::EnterVictory() {
    if (m_LevelState == LevelState::VICTORY) {
        return;
    }

    m_LevelState = LevelState::VICTORY;
    if (m_VictoryText != nullptr) {
        m_VictoryText->SetVisible(true);
    }

    LOG_DEBUG("VICTORY");
}

void GameScene::UpdateSkySunSpawning() {
    const float currentTime = Util::Time::GetElapsedTimeMs() / 1000.0f;

    if (currentTime - m_LastSkySunSpawnTime < m_NextSkySunInterval) {
        return;
    }

    m_LastSkySunSpawnTime = currentTime;
    m_NextSkySunInterval = 8.0f + static_cast<float>(rand() % 5);

    float x = 260.0f + static_cast<float>(rand() % 700);
    float targetY = 120.0f + static_cast<float>(rand() % 350);

    glm::vec2 startPos{x, -50.0f};
    glm::vec2 targetPos{x, targetY};

    auto sun = std::make_shared<Sun>(startPos, targetPos, 25);
    m_Suns.push_back(sun);
    m_Renderer.AddChild(sun);

    LOG_DEBUG("Sky sun spawned");
}

void GameScene::UpdateSuns() {
    for (auto& sun : m_Suns) {
        if (sun->IsAlive()) {
            sun->Update();
        }
    }
}