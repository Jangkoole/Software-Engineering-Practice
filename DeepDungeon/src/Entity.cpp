#include "Entity.h"

namespace dungeon {

Entity::Entity(char symbol, int x, int y, EntityType type)
    : x_(x), y_(y), symbol_(symbol), type_(type),
      hp_(50), maxHp_(50), atk_(5), def_(2) {}

void Entity::move(int dx, int dy) {
    x_ += dx;
    y_ += dy;
}

void Entity::takeDamage(int damage) {
    int actualDamage = std::max(1, damage - def_);
    hp_ = std::max(0, hp_ - actualDamage);
}

void Entity::heal(int amount) {
    hp_ = std::min(maxHp_, hp_ + amount);
}

// 怪物实现
Monster::Monster(int x, int y) 
    : Entity(dungeon::CHAR_MONSTER, x, y, EntityType::Monster) {
    hp_ = 30;
    maxHp_ = 30;
    atk_ = 8;
    def_ = 1;
}

// 物品实现
Item::Item(ItemType itemType, int x, int y)
    : Entity('\0', x, y, EntityType::Item), itemType_(itemType) {
    switch (itemType_) {
        case ItemType::Weapon:
            symbol_ = dungeon::CHAR_WEAPON;
            break;
        case ItemType::Potion:
            symbol_ = dungeon::CHAR_POTION;
            break;
        default:
            symbol_ = '?';
    }
}

} // namespace dungeon
