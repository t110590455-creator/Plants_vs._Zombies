//
// Created by hankl on 2026/3/20.
//

#ifndef LEVELDATA_HPP
#define LEVELDATA_HPP

#include "pch.hpp"

enum class ZombieType {
    BASIC,
    CONEHEAD
};

struct SpawnEvent {
    ZombieType type;
    int row;
    float spawnTime;
    bool spawned = false;
};

struct Wave {
    std::vector<SpawnEvent> events;
    bool isFinalWave = false;
};


#endif //LEVELDATA_HPP
