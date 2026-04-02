#ifndef GRID_HPP
#define GRID_HPP

#include "pch.hpp"
#include <vector>
#include <optional>
#include <cmath>

class Grid {
public:
    struct Tile {
        int row;
        int col;
        glm::vec2 worldPos;
    };

    // 使用二維陣列直接初始化所有格子的座標
    void Initialize(const std::vector<std::vector<glm::vec2>>& coordinateMap) {
        m_CoordinateMap = coordinateMap;
        m_Rows = static_cast<int>(coordinateMap.size());
        m_Cols = m_Rows > 0 ? static_cast<int>(coordinateMap[0].size()) : 0;
    }

    [[nodiscard]] std::optional<Tile> GetTileFromMouse(glm::vec2 mousePos) const
    {
        float minDistance = 50.0f; // 點擊判定的靈敏度（半徑），可依需求調整
        std::optional<Tile> bestMatch = std::nullopt;

        for (int r = 0; r < m_Rows; ++r) {
            for (int c = 0; c < m_Cols; ++c) {
                float dist = glm::distance(mousePos, m_CoordinateMap[r][c]);
                if (dist < minDistance) {
                    minDistance = dist;
                    bestMatch = Tile{r, c, m_CoordinateMap[r][c]};
                }
            }
        }
        return bestMatch;
    }

    [[nodiscard]] glm::vec2 GetWorldPos(int row, int col) const
    {
        if (row >= 0 && row < m_Rows && col >= 0 && col < m_Cols) {
            return m_CoordinateMap[row][col];
        }
        return {0, 0};
    }

    [[nodiscard]] int GetRows() const
    {
        return m_Rows;
    }

private:
    int m_Rows = 0;
    int m_Cols = 0;
    std::vector<std::vector<glm::vec2>> m_CoordinateMap;
};

#endif