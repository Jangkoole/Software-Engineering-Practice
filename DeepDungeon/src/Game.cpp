#include "Game.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

namespace dungeon {

Game::Game() : gameOver_(false), gameWon_(false) {}

void Game::init() {
    addLog("=== Welcome to Deep Dungeon ===");
    addLog("Use W/A/S/D to move, Q to quit");
    
    // 生成地图
    map_.generate();
    
    // 找到玩家的出生点（第一个地板格子）
    bool playerPlaced = false;
    for (int y = 0; y < map_.height() && !playerPlaced; y++) {
        for (int x = 0; x < map_.width() && !playerPlaced; x++) {
            if (map_.isWalkable(x, y)) {
                player_ = std::make_unique<Player>(x, y);
                playerPlaced = true;
            }
        }
    }
    
    // 生成一些怪物
    std::mt19937 rng(456);
    std::uniform_int_distribution<int> posDist(0, map_.width() - 1);
    
    for (int i = 0; i < 5; i++) {
        int x, y;
        int attempts = 0;
        do {
            x = posDist(rng) % map_.width();
            y = posDist(rng) % map_.height();
            attempts++;
        } while ((!map_.isWalkable(x, y) || getMonsterAt(x, y) != nullptr) && attempts < 100);
        
        if (attempts < 100) {
            entities_.push_back(std::make_unique<Monster>(x, y));
        }
    }
    
    // 生成一些物品
    std::uniform_int_distribution<int> itemTypeDist(0, 1);
    for (int i = 0; i < 3; i++) {
        int x, y;
        int attempts = 0;
        do {
            x = posDist(rng) % map_.width();
            y = posDist(rng) % map_.height();
            attempts++;
        } while ((!map_.isWalkable(x, y) || getMonsterAt(x, y) != nullptr || getItemAt(x, y) != nullptr) && attempts < 100);
        
        if (attempts < 100) {
            ItemType type = (itemTypeDist(rng) == 0) ? ItemType::Weapon : ItemType::Potion;
            entities_.push_back(std::make_unique<Item>(type, x, y));
        }
    }
    
    updateFOV();
    addLog("You entered a mysterious dungeon...");
}

Command Game::handleInput(const std::string& input) {
    if (input.empty()) return Command::Unknown;
    
    char c = std::tolower(input[0]);
    switch (c) {
        case 'w': return Command::MoveUp;
        case 's': return Command::MoveDown;
        case 'a': return Command::MoveLeft;
        case 'd': return Command::MoveRight;
        case ' ': return Command::Wait;
        case 'p': return Command::UsePotion;
        case 'q': return Command::Quit;
        default: return Command::Unknown;
    }
}

void Game::update() {
    if (gameOver_) return;
    
    updateMonsters();
    checkCollisions();
    updateFOV();
}

void Game::tryMovePlayer(int dx, int dy) {
    int newX = player_->x() + dx;
    int newY = player_->y() + dy;
    
    // 检查边界
    if (!map_.isValid(newX, newY)) {
        addLog("That's a wall.");
        return;
    }
    
    // 检查是否有怪物
    Monster* monster = getMonsterAt(newX, newY);
    if (monster) {
        attackMonster(monster);
        return;
    }
    
    // 检查是否可通行
    if (!map_.isWalkable(newX, newY)) {
        addLog("That's an obstacle.");
        return;
    }
    
    // 移动
    player_->move(dx, dy);
    addLog("You moved.");
    
    // 检查拾取
    pickupItem();
}

void Game::attackMonster(Monster* monster) {
    int damage = player_->totalAtk();
    monster->takeDamage(damage);
    addLog("You attacked the monster for " + std::to_string(damage) + " damage.");
    
    if (!monster->isAlive()) {
        addLog("Monster defeated! Gained 20 exp.");
        player_->addExp(20);
        
        // 移除怪物
        entities_.erase(
            std::remove_if(entities_.begin(), entities_.end(),
                [monster](const std::unique_ptr<Entity>& e) {
                    return e.get() == monster;
                }),
            entities_.end()
        );
        
        // 检查胜利条件
        if (entities_.empty()) {
            gameWon_ = true;
            gameOver_ = true;
            addLog("=== You cleared all monsters, Victory! ===");
        }
    }
}

void Game::updateMonsters() {
    for (auto& entity : entities_) {
        if (entity->type() != EntityType::Monster) continue;
        
        Monster* monster = static_cast<Monster*>(entity.get());
        if (!monster->isAlive()) continue;
        
        int px = player_->x(), py = player_->y();
        int mx = monster->x(), my = monster->y();
        
        // 简单追踪 AI
        int dx = 0, dy = 0;
        if (px > mx) dx = 1;
        else if (px < mx) dx = -1;
        else if (py > my) dy = 1;
        else if (py < my) dy = -1;
        
        // 检查是否可以移动
        if (dx != 0 || dy != 0) {
            int newX = mx + dx, newY = my + dy;
            
            // 检查是否是玩家位置（攻击）
            if (newX == px && newY == py) {
                int damage = std::max(1, monster->atk() - player_->def());
                player_->takeDamage(damage);
                addLog("Monster attacks you for " + std::to_string(damage) + " damage!");
                
                if (!player_->isAlive()) {
                    gameOver_ = true;
                    addLog("=== You died, Game Over ===");
                }
            }
            // 检查是否是可通行的空地
            else if (map_.isWalkable(newX, newY) && getMonsterAt(newX, newY) == nullptr) {
                monster->setPosition(newX, newY);
            }
        }
    }
}

void Game::checkCollisions() {
    // 预留用于其他碰撞检测
}

void Game::pickupItem() {
    Item* item = getItemAt(player_->x(), player_->y());
    if (item) {
        switch (item->itemType()) {
            case ItemType::Weapon:
                player_->equipWeapon(5);
                addLog("You found a sword! ATK +5");
                break;
            case ItemType::Potion:
                player_->addPotion();
                addLog("You found a potion! Press P to use");
                break;
        }
        
        // 移除物品
        entities_.erase(
            std::remove_if(entities_.begin(), entities_.end(),
                [item](const std::unique_ptr<Entity>& e) {
                    return e.get() == item;
                }),
            entities_.end()
        );
    }
}

void Game::updateFOV() {
    map_.clearVisibility();
    
    int px = player_->x(), py = player_->y();
    
    // 简单圆形 FOV
    for (int dy = -FOV_RADIUS; dy <= FOV_RADIUS; dy++) {
        for (int dx = -FOV_RADIUS; dx <= FOV_RADIUS; dx++) {
            if (dx * dx + dy * dy <= FOV_RADIUS * FOV_RADIUS) {
                int x = px + dx, y = py + dy;
                if (map_.isValid(x, y)) {
                    map_.setTileVisible(x, y);
                }
            }
        }
    }
}

std::string Game::render() const {
    std::string result;
    
    // 渲染地图
    for (int y = 0; y < map_.height(); y++) {
        for (int x = 0; x < map_.width(); x++) {
            const Tile& tile = map_.tile(x, y);
            
            // 检查是否有实体在这里
            char ch = ' ';
            bool hasEntity = false;
            
            // 检查玩家
            if (player_->x() == x && player_->y() == y) {
                ch = dungeon::CHAR_PLAYER;
                hasEntity = true;
            }
            
            // 检查其他实体
            if (!hasEntity && tile.visible) {
                for (const auto& entity : entities_) {
                    if (entity->x() == x && entity->y() == y) {
                        ch = entity->symbol();
                        hasEntity = true;
                        break;
                    }
                }
            }
            
            // 根据可见性渲染
            if (tile.visible) {
                if (hasEntity) {
                    result += ch;
                } else {
                    switch (tile.type) {
                        case TileType::Wall: result += '#'; break;
                        case TileType::Floor: result += '.'; break;
                        case TileType::Water: result += '~'; break;
                    }
                }
            } else if (tile.explored) {
                result += ' ';  // 已探索但不可见，显示空白
            } else {
                result += ' ';  // 未探索，显示空白
            }
        }
        result += '\n';
    }
    
    // 渲染状态栏
    result += "\n";
    result += "=== Status ===\n";
    result += "HP: " + std::to_string(player_->hp()) + "/" + std::to_string(player_->maxHp());
    result += "  ATK: " + std::to_string(player_->totalAtk());
    result += "  DEF: " + std::to_string(player_->def());
    result += "  LV: " + std::to_string(player_->level());
    result += "  Potions: " + std::to_string(player_->potionCount()) + "\n";
    
    // 渲染日志（最近 5 条）
    result += "\n=== Log ===\n";
    size_t startIdx = (log_.size() > 5) ? log_.size() - 5 : 0;
    for (size_t i = startIdx; i < log_.size(); i++) {
        result += log_[i] + "\n";
    }
    
    result += "\nCommands: W/A/S/D move, Space wait, P potion, Q quit\n";
    
    return result;
}

void Game::addLog(const std::string& message) {
    log_.push_back(message);
    // 限制日志数量
    if (log_.size() > 20) {
        log_.erase(log_.begin());
    }
}

Monster* Game::getMonsterAt(int x, int y) {
    for (auto& entity : entities_) {
        if (entity->type() == EntityType::Monster && 
            entity->x() == x && entity->y() == y) {
            return static_cast<Monster*>(entity.get());
        }
    }
    return nullptr;
}

Item* Game::getItemAt(int x, int y) {
    for (auto& entity : entities_) {
        if (entity->type() == EntityType::Item && 
            entity->x() == x && entity->y() == y) {
            return static_cast<Item*>(entity.get());
        }
    }
    return nullptr;
}

} // namespace dungeon
