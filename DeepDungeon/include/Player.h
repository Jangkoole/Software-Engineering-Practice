#pragma once

#include "Entity.h"

namespace dungeon {

// 玩家类
class Player : public Entity {
public:
    Player(int x, int y);
    
    // 经验值和等级
    int exp() const { return exp_; }
    int level() const { return level_; }
    
    void addExp(int amount);
    
    // 物品栏
    int weaponAttackBonus() const { return weaponBonus_; }
    int potionCount() const { return potionCount_; }
    
    void equipWeapon(int bonus) { weaponBonus_ = bonus; }
    void addPotion() { potionCount_++; }
    void usePotion();
    
    // 总攻击力（基础 + 武器）
    int totalAtk() const { return atk_ + weaponBonus_; }
    
private:
    int exp_;
    int level_;
    int weaponBonus_;
    int potionCount_;
};

} // namespace dungeon
