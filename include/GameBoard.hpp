//
// Created by hankl on 2026/3/10.
//

#ifndef GAMEBOARD_HPP
#define GAMEBOARD_HPP

#include "pch.hpp"

class Plant;

class GameBoard {
public:
    static constexpr int ROWS = 5;
    static constexpr int COLS = 9;

    GameBoard();

    bool ScreenToGrid(float x, float y, int& row, int& col) const;
    glm::vec2 GridToScreen(int row, int col) const;
    glm::vec2 GetCellCenter(int row, int col) const;

    bool IsCellEmpty(int row, int col) const;
    void PlacePlant(Plant* plant, int row, int col);
    Plant* GetPlant(int row, int col) const;
    void RemovePlant(int row, int col);

private:
    float m_GridStartX;
    float m_GridStartY;
    float m_CellWidth;
    float m_CellHeight;

    Plant* m_Grid[ROWS][COLS];
};


#endif //GAMEBOARD_HPP
