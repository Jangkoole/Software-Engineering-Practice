#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Map.h"
#include "Player.h"
#include "Entity.h"

namespace dungeon {

// 游戏输入命令
enum class Command {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Wait,
    UsePotion,
    Quit,
    Unknown
};

// 游戏主类
class Game {
public:
    Game();
    
    // 初始化游戏
    void init();
    
    // 游戏主循环
    void run();
    
    // 处理输入
    Command handleInput(const std::string& input);
    
    // 更新游戏状态
    void update();
    
    // 渲染游戏
    std::string render() const;
    
    // 游戏是否结束
    bool isGameOver() const { return gameOver_; }
    
    // 获取游戏日志
    const std::vector<std::string>& logs() const { return log_; }
    
    // 添加日志
    void addLog(const std::string& message);

private:
    Map map_;
    std::unique_ptr<Player> player_;
    std::vector<std::unique_ptr<Entity>> entities_;
    
    bool gameOver_;
    bool gameWon_;
    std::vector<std::string> log_;
    
    // 游戏逻辑
    void attackMonster(Monster* monster);
    void updateMonsters();
    void checkCollisions();
    void pickupItem();
    
    // FOV 计算
    void updateFOV();
    
    // 辅助函数
    Player* getPlayer() { return player_.get(); }
    Monster* getMonsterAt(int x, int y);
    Item* getItemAt(int x, int y);

public:
    // 玩家移动（公开给 main 调用）
    void tryMovePlayer(int dx, int dy);
};

} // namespace dungeon
