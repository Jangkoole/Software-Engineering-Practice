# 深度地牢 (Deep Dungeon)

一个硬核 Roguelike 地牢游戏，纯控制台版本。

## 构建说明

### 依赖
- C++17 兼容编译器 (GCC, Clang, MSVC)
- CMake 3.16+

### Windows (MSVC)
```bash
cd DeepDungeon
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

### Windows (MinGW)
```bash
cd DeepDungeon
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

### Linux/Mac
```bash
cd DeepDungeon
mkdir build && cd build
cmake ..
make
```

## 游戏操作

| 按键 | 功能 |
|------|------|
| W | 向上移动 |
| A | 向左移动 |
| S | 向下移动 |
| D | 向右移动 |
| 空格 | 等待一回合 |
| P | 使用药水 |
| Q | 退出游戏 |

## 游戏符号

| 符号 | 含义 |
|------|------|
| @ | 玩家 |
| # | 墙壁 |
| . | 地板 |
| ~ | 水（导电） |
| g | 怪物 (Goblin) |
| / | 武器 |
| ! | 药水 |

## 开发计划

- [x] Sprint 1: 基础框架、移动、战斗
- [ ] Sprint 2: 战争迷雾、怪物 AI
- [ ] Sprint 3: 环境互动、回放系统
