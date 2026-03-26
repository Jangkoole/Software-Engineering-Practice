#pragma once

// 游戏常量定义
namespace dungeon {

// 地图尺寸
constexpr int MAP_WIDTH = 40;
constexpr int MAP_HEIGHT = 20;

// 视野半径
constexpr int FOV_RADIUS = 5;

// 字符表示
constexpr char CHAR_PLAYER = '@';
constexpr char CHAR_WALL = '#';
constexpr char CHAR_FLOOR = '.';
constexpr char CHAR_WATER = '~';
constexpr char CHAR_MONSTER = 'g';
constexpr char CHAR_WEAPON = '/';
constexpr char CHAR_POTION = '!';

// 游戏状态
constexpr int PLAYER_BASE_HP = 100;
constexpr int PLAYER_BASE_ATK = 10;
constexpr int PLAYER_BASE_DEF = 5;

} // namespace dungeon
