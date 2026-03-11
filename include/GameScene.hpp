//
// Created by hankl on 2026/3/10.
//

#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include "pch.hpp"

#include "GameBoard.hpp"
#include "BasicZombie.hpp"
#include "Pea.hpp"
#include "Peashooter.hpp"
#include "Plant.hpp"
#include "Projectile.hpp"
#include "Zombie.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"

class GameScene {
public:
    GameScene();

    void Update();

private:
    void HandleInput();
    void TryPlantAtMousePosition();
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

private:
    GameBoard m_Board;
    Util::Renderer m_Renderer;

    std::shared_ptr<Util::GameObject> m_Background;
    std::vector<std::shared_ptr<Plant>> m_Plants;
    std::vector<std::shared_ptr<Zombie>> m_Zombies;
    std::vector<std::shared_ptr<Projectile>> m_Projectiles;

    bool m_WasMousePressed = false;

    float m_SpawnInterval = 3.0f;
    float m_LastSpawnTime = 0.0f;
};


#endif //GAMESCENE_HPP
