//
// Created by hankl on 2026/3/10.
//
#include "GameBoard.hpp"

#include "Plant.hpp"

// 建構子與初始化 (Constructor)
GameBoard::GameBoard()
    : m_GridStartX(-460.0f), // 參數設定：定義了棋盤在螢幕上的起點 (460,-270)(左下角) 以及每一格的寬度與高度。
      m_GridStartY(-270.0f),
      m_CellWidth(85.0f),
      m_CellHeight(100.0f) {
    for (int r = 0; r < ROWS; ++r) {    // 清空棋盤：使用雙重迴圈將二維陣列 m_Grid 初始化為 nullptr，代表遊戲開始時所有格子都沒有植物。
        for (int c = 0; c < COLS; ++c) {
            m_Grid[r][c] = nullptr;
        }
    }
}

// 用於判斷玩家點擊了哪一格
bool GameBoard::ScreenToGrid(float x, float y, int& row, int& col) const {
    if (x < m_GridStartX || y < m_GridStartY) {
        return false;
    }

    col = static_cast<int>((x - m_GridStartX) / m_CellWidth);
    row = static_cast<int>((y - m_GridStartY) / m_CellHeight);

    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        return false;
    }

    return true;
}

// GridToScreen：回傳格子的 左上角 座標。
glm::vec2 GameBoard::GridToScreen(int row, int col) const {
    return {
        m_GridStartX + col * m_CellWidth,
        m_GridStartY + row * m_CellHeight
    };
}

// GetCellCenter：回傳格子的 中心點 座標。公式如下：
glm::vec2 GameBoard::GetCellCenter(int row, int col) const {
    return {
        m_GridStartX + col * m_CellWidth + m_CellWidth * 0.5f,
        m_GridStartY + row * m_CellHeight + m_CellHeight * 0.5f
    };
}

// IsCellEmpty：檢查 m_Grid[row][col] 是否為空指標。
// 如果玩家想在已經有植物的地方再種一顆，這個函式就能用來擋掉。
bool GameBoard::IsCellEmpty(int row, int col) const {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        return false;
    }
    return m_Grid[row][col] == nullptr;
}

// PlacePlant：將植物物件的指標存入陣列中。
void GameBoard::PlacePlant(Plant* plant, int row, int col) {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        return;
    }
    m_Grid[row][col] = plant;
}

// GetPlant：取得特定位置的植物指標（例如用於判斷殭屍是否正在啃食該格的植物）。
Plant* GameBoard::GetPlant(int row, int col) const {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        return nullptr;
    }
    return m_Grid[row][col];
}

// 讓 GameBoard 能移除植物
void GameBoard::RemovePlant(int row, int col) {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        return;
    }
    m_Grid[row][col] = nullptr;
}