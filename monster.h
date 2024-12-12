#pragma once
#include"cocos2d.h"
class Silme :public cocos2d::Sprite {
    int health; // 当前血量
    int total_health; //总血量
    cocos2d::ProgressTimer* healthBar;       // 血条
    cocos2d::Sprite* healthBarBG; //血条背景
public:
    static Silme* create(const std::string& filename, int initialHealth) {
        Silme* sprite = new (std::nothrow) Silme();
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            sprite->total_health = initialHealth; // 设置初始血量
            sprite->health = sprite->total_health;
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    // 减少血量
    void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            die();
        }
        else {
            healthBar->setVisible(true);
            healthBarBG->setVisible(true);
            healthBar->setPercentage((float)health / total_health * 100);
            this->scheduleOnce([this](float) {
                this->healthBar->setVisible(false);
                this->healthBarBG->setVisible(false);
                }, 5.0f, "HideHealthBar"); //5秒不受攻击则隐藏血条
        }
    }

    // 精灵死亡逻辑
    void die();
    void silme_move(float dt); //史莱姆的运动逻辑
    void silme_jump(char direction);
    void addjellyeffect(); //添加了果冻效果
    void addPhy(); //添加了物理效果
    void addhealthbar(); //添加血条
};

class Zombie :public cocos2d::Sprite {
    int health; //血量
    int total_health; //总血量
    cocos2d::ProgressTimer* healthBar;       // 血条
    cocos2d::Sprite* healthBarBG; //血条背景
public:
    // 创建精灵的静态方法
    static Zombie* create(const std::string& filename, int initialHealth) {
        Zombie* sprite = new (std::nothrow) Zombie();
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            sprite->total_health = initialHealth; // 设置初始血量
            sprite->health = sprite->total_health;
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }

    // 减少血量
    void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            die();
        }
        else {
            healthBar->setVisible(true);
            healthBarBG->setVisible(true);
            healthBar->setPercentage((float)health / total_health * 100);
            this->scheduleOnce([this](float) {
                this->healthBar->setVisible(false);
                this->healthBarBG->setVisible(false);
                }, 5.0f, "HideHealthBar"); //5秒不受攻击则隐藏血条
        }
    }

    // 精灵死亡逻辑
    void die();
    void zombie_move(float dt);
    void addPhy(); //添加物理效果
    void addAnimate(); //添加动画
    void addhealthbar(); //添加血条
};