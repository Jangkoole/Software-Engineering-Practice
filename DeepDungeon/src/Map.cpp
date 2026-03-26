#include "Map.h"
#include <random>
#include <algorithm>

namespace dungeon {

Map::Map(int width, int height)
    : width_(width), height_(height),
      tiles_(height, std::vector<Tile>(width)) {}

const Tile& Map::tile(int x, int y) const {
    return tiles_[y][x];
}

Tile& Map::tile(int x, int y) {
    return tiles_[y][x];
}

bool Map::isWalkable(int x, int y) const {
    if (!isValid(x, y)) return false;
    TileType type = tiles_[y][x].type;
    return type == TileType::Floor || type == TileType::Water;
}

bool Map::isWater(int x, int y) const {
    if (!isValid(x, y)) return false;
    return tiles_[y][x].type == TileType::Water;
}

bool Map::isValid(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

void Map::clearVisibility() {
    for (auto& row : tiles_) {
        for (auto& tile : row) {
            tile.visible = false;
        }
    }
}

void Map::setTileVisible(int x, int y) {
    if (isValid(x, y)) {
        tiles_[y][x].visible = true;
        tiles_[y][x].explored = true;
    }
}

void Map::setTileExplored(int x, int y) {
    if (isValid(x, y)) {
        tiles_[y][x].explored = true;
    }
}

void Map::generate(int seed) {
    // 初始化全为墙
    for (auto& row : tiles_) {
        for (auto& tile : row) {
            tile.type = TileType::Wall;
            tile.explored = false;
            tile.visible = false;
        }
    }
    
    generateRooms();
    generateCorridors();
    addWaterFeatures();
}

void Map::generateRooms() {
    std::mt19937 rng(42);  // 固定种子保证可重复性
    std::uniform_int_distribution<int> roomSizeDist(4, 8);
    std::uniform_int_distribution<int> posDist(1, width_ - 2);
    
    int numRooms = 6;
    for (int i = 0; i < numRooms; i++) {
        int roomW = roomSizeDist(rng);
        int roomH = roomSizeDist(rng);
        int roomX = posDist(rng) % (width_ - roomW - 1) + 1;
        int roomY = posDist(rng) % (height_ - roomH - 1) + 1;
        
        // 挖掘房间
        for (int y = roomY; y < roomY + roomH && y < height_ - 1; y++) {
            for (int x = roomX; x < roomX + roomW && x < width_ - 1; x++) {
                tiles_[y][x].type = TileType::Floor;
            }
        }
    }
}

void Map::generateCorridors() {
    // 简单连接房间：找到地板单元格并尝试连接
    std::vector<std::pair<int, int>> floorTiles;
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            if (tiles_[y][x].type == TileType::Floor) {
                floorTiles.emplace_back(x, y);
            }
        }
    }
    
    if (floorTiles.empty()) return;
    
    // 连接最近的地板单元格
    std::sort(floorTiles.begin(), floorTiles.end());
    
    for (size_t i = 0; i + 1 < floorTiles.size(); i++) {
        int x1 = floorTiles[i].first, y1 = floorTiles[i].second;
        int x2 = floorTiles[i + 1].first, y2 = floorTiles[i + 1].second;
        
        // 先水平后垂直挖走廊
        int minX = std::min(x1, x2), maxX = std::max(x1, x2);
        int minY = std::min(y1, y2), maxY = std::max(y1, y2);
        
        for (int x = minX; x <= maxX; x++) {
            if (tiles_[y1][x].type == TileType::Wall) {
                tiles_[y1][x].type = TileType::Floor;
            }
        }
        for (int y = minY; y <= maxY; y++) {
            if (tiles_[y][x2].type == TileType::Wall) {
                tiles_[y][x2].type = TileType::Floor;
            }
        }
    }
}

void Map::addWaterFeatures() {
    std::mt19937 rng(123);
    std::uniform_int_distribution<int> waterDist(0, 100);
    
    // 随机将一些地板变成水
    for (int y = 1; y < height_ - 1; y++) {
        for (int x = 1; x < width_ - 1; x++) {
            if (tiles_[y][x].type == TileType::Floor) {
                if (waterDist(rng) < 10) {  // 10% 概率变成水
                    tiles_[y][x].type = TileType::Water;
                }
            }
        }
    }
}

std::string Map::render() const {
    std::string result;
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            const Tile& tile = tiles_[y][x];
            if (!tile.explored) {
                result += ' ';  // 未探索的区域显示为空白
            } else {
                switch (tile.type) {
                    case TileType::Wall:
                        result += '#';
                        break;
                    case TileType::Floor:
                        result += '.';
                        break;
                    case TileType::Water:
                        result += '~';
                        break;
                }
            }
        }
        result += '\n';
    }
    return result;
}

} // namespace dungeon
