#pragma once
#include"cocos2d.h"
#define JumpHeight 500 //人物向上跳跃的冲量大小
class Hero :public cocos2d::Sprite {
    int health; //当前血量
    int total_health; //总血量
    // 控制地图移动的变量
    bool moveLeft = false;
    bool moveRight = false;
    // 跳跃相关
    bool isJumping = false;
public:
    static Hero* create(const std::string& filename, int initialHealth) {
        Hero* sprite = new (std::nothrow) Hero();
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            sprite->total_health = initialHealth; // 设置初始血量
            sprite->health = sprite->total_health;
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    void addPhy(); //为主角添加物理刚体性质
    void addKeyboardListener();  //添加键盘监听器
    void applyJump(); //主角跳跃
    void performJump();  
    void HeroFunc(float delta);
    void setHeroAnimation(const std::string& frame2, const std::string& frame3, const std::string& frame4, const std::string& frame5);
    void checkAndFixHeroCollision();
};