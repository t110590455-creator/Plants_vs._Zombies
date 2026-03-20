//
// Created by hankl on 2026/3/10.
//

#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include "pch.hpp"

#include "GameBoard.hpp"
#include "SeedCard.hpp"
#include "ShovelButton.hpp"
#include "LawnMower.hpp"

#include "Projectile.hpp"
#include "Pea.hpp"
#include "Sun.hpp"

#include "Plant.hpp"
#include "Peashooter.hpp"
#include "Sunflower.hpp"

#include "BasicZombie.hpp"
#include "Zombie.hpp"

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"

class GameScene {
public:
    GameScene();
    void Update();

private:
    // 遊戲狀態
    enum class State {
        PLAYING,
        GAME_OVER
    };

    // 植物種類
    //enum class PlantType {
    //    PEASHOOTER,
    //    SUNFLOWER
    //};

private:
    void HandleInput();
    void TryPlantAtMousePosition();
    bool TryCollectSunAtMousePosition();

    void UpdateZombies();
    void TrySpawnZombie();

    void UpdateProjectiles();
    void UpdatePeashooters();
    void CheckProjectileZombieCollisions();
    bool HasZombieInFront(int row, float x) const;
    void RemoveDeadProjectiles();

    void UpdateZombiePlantInteractions();
    void RemoveDeadPlants();
    void RemoveDeadZombies();
    Plant* FindPlantInZombieFront(Zombie* zombie) const;

    void CheckGameOver();
    void EnterGameOver();

    void UpdateSunText();

    void UpdateSunflowers();
    void RemoveDeadSuns();

    bool TrySelectSeedCardAtMousePosition();
    void UpdateSeedCardSelectionVisual();

    SeedCard* FindSeedCardByPlantType(PlantType plantType) const;
    void UpdateSeedCardUsabilityVisual();

    bool TryToggleShovelAtMousePosition();
    void UpdateShovelVisual();
    void TryRemovePlantAtMousePosition();

    // 割草機
    void CreateLawnMowers();
    void UpdateLawnMowers();
    void CheckLawnMowerActivation();
    void CheckLawnMowerZombieCollisions();
    void RemoveDeadLawnMowers();

private:
    GameBoard m_Board;
    Util::Renderer m_Renderer;

    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<Util::GameObject> m_ChooserBackground;
    std::shared_ptr<Util::GameObject> m_GameOverText;
    std::shared_ptr<Util::Text> m_SunText;
    std::shared_ptr<Util::GameObject> m_SunTextObject; //因為 Renderer 是加 GameObject，不是直接加 Text。所以建議補這個
    std::shared_ptr<ShovelButton> m_ShovelButton;
    std::vector<std::shared_ptr<LawnMower>> m_LawnMowers;

    std::vector<std::shared_ptr<Plant>> m_Plants;
    std::vector<std::shared_ptr<Zombie>> m_Zombies;
    std::vector<std::shared_ptr<Projectile>> m_Projectiles;
    std::vector<std::shared_ptr<Sun>> m_Suns;
    std::vector<std::shared_ptr<SeedCard>> m_SeedCards;


    bool m_WasMousePressed = false;
    bool m_ShovelMode = false;

    float m_SpawnInterval = 3.0f;
    float m_LastSpawnTime = 0.0f;

    State m_State = State::PLAYING;
    PlantType m_SelectedPlantType = PlantType::PEASHOOTER; // 選擇種植植物種類

    int m_SunPoints = 200; // 玩家初始Sun數量
};


#endif //GAMESCENE_HPP
