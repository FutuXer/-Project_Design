#pragma once
#include"cocos2d.h"
#define JumpHeight 500 //人物向上跳跃的冲量大小
class Hero :public cocos2d::Sprite
{
    int health; //当前血量
    int total_health; //总血量
    // 控制地图移动的变量
    bool moveLeft = false;
    bool moveRight = false;
    // 跳跃相关
    bool isJumping = false;
public:
    static Hero* create(const std::string& filename, int initialHealth) 
    {
        Hero* sprite = new (std::nothrow) Hero();
        if (sprite && sprite->initWithFile(filename))
        {
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
    void checkAndFixHeroCollision(float delta);

    //物品
    /*物品相关*/
    void addTouchListener();                         // 读入鼠标点击
    void addBlockAtPosition(cocos2d::Vec2 position);
    void removeBlockAtPosition(cocos2d::Vec2 position); // 删除泥块的函数
    float getCurrentTime();                             // 获取当前时间的函数声明
    //cocos2d::Vector<MyItem*> items;                                             // 存放所有物品的信息数组
    //cocos2d::Vector<MyItem*> pocketItems;                                       // 存放口袋中物品的信息数组（即items的前8个）
    //void iniitems();
    //void checkPocket();                                                         // 检查口袋(只显示前10个常用物品，用于更换手里拿的物品)
    //void checkBag();                                                            // 检查背包(显示所有随身物品、制造和装备，用于更换口袋中的物品和整理背包、整理口袋)
    //void onItemMenuClicked(Ref* sender, int itemIndex);                         // 选择物品
    //void ClickToChangePosition(Ref* sender, int itemIndex);                     // 更换物品在背包中的位置
    //void ItemsInHand(int itemTag);                                              // 手中的物品，决定人物的动作
    //MyItem* movingItem;                                                         // 正在移动的物品
    //cocos2d::Sprite* itemInMove;                                                // 显示正在移动的物品（跟随鼠标）
    // 口袋图层
    //cocos2d::Layer* pocketLayer = nullptr;
    // 背包图层
    //cocos2d::Layer* bagLayer = nullptr;

    // 长按相关变量
    int touchStartFrame;
    int touchEndFrame;
    cocos2d::Vec2 touchStartPosition;
    float longPressThreshold = 0.5f; // 长按时间阈值（秒）*/
};