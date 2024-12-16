#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "items.h"
#include "ui/CocosGUI.h"

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    virtual void update(float delta) override;
    CREATE_FUNC(GameScene);

private:
    void addTouchListener();                         // 读入鼠标点击
    void addKeyboardListener(cocos2d::Sprite* hero); // 添加键盘监听器
    void performJump(cocos2d::Sprite* hero);         // 执行跳跃动作
    int GameScene::checkBlockCollision(float x, float y);      // 检查与blocksLayer的碰撞
    void applyGravity(float delta);                   // 应用重力
    void applyJump(int delta);
    void interactWithItems();                        // 与itemsLayer进行交互
    float getBlockTopY(float heroX, float heroY);
    void updatePhysicsWorld(float delta);
    void checkAndFixHeroCollision();
    void addBlockAtPosition(cocos2d::Vec2 position);
    void removeBlockAtPosition(cocos2d::Vec2 position); // 删除泥块的函数
    float getCurrentTime();                             // 获取当前时间的函数声明

    /*物品相关*/
    cocos2d::Vector<MyItem*> items;                                             // 存放所有物品的信息数组
    cocos2d::Vector<MyItem*> itemsToBeMade;                                     // 存放所有能被制作的物品
    cocos2d::Vector<MyItem*> pocketItems;                                       // 存放口袋中物品的信息数组（即items的前8个）
    void checkPocket();                                                         // 检查口袋(只显示前10个常用物品，用于更换手里拿的物品)
    void checkBag();                                                            // 检查背包(显示所有随身物品，用于更换口袋中的物品、整理背包)
    void checkProduction();                                                     // 检查可以被制作的物品，显示出来，并可以与背包互动（完成物品制作）
    void onItemMenuClicked(Ref* sender, int itemIndex);                         // 选择物品
    void ClickToChangePosition(Ref* sender, int itemIndex);                     // 更换物品在背包中的位置
    void ClickToProduce(Ref* sender, int itemIndex);                            // 按下按钮来合成物品（按一次合成一个）
    void ItemsInHand(int itemTag);                                              // 手中的物品，决定人物的动作
    MyItem* movingItem;                                                         // 正在移动的物品
    cocos2d::Sprite* itemInMove;                                                // 显示正在移动的物品（跟随鼠标）
    void calculate();                                                           // 计算目前可以合成的装备                       
    int findItemCount(const std::string& itemName,int &Index) {
        for (int i = 0; i < items.size(); i++) {
            if (items.at(i)->getItemName() == itemName) {
                Index = i;
                return items.at(i)->getNum();
            }
        }
        return 0; // 没有找到时返回 0
    }
    // 合成物品的字典：每行首字代表产品编号，随后依次是原料1的个数、原料1的编号、原料2的个数、原料2的编号
    int dictionary[12][5] = {
        20,4,12,8,13,
        21,3,12,8,14,
        30,6,13,0,0,
        31,8,14,0,0,
        40,10,12,0,0,
        41,4,12,0,0,
        42,6,12,0,0,
        43,8,12,2,14,
        51,20,50,0,0,
        52,20,51,0,0,
        0,0,0,0,            // 用来补位，以备后来添加合成的物品
    };

    // 获取瓦片坐标
    cocos2d::Vec2 getTileCoordForPosition(float x, float y);

    // 新增成员变量以便保存地图和主角的位置关系
    cocos2d::TMXTiledMap* map;
    cocos2d::Sprite* hero;

    // 保存当前地图的位置
    cocos2d::Vec2 mapPosition;

    // 口袋图层
    cocos2d::Layer* pocketLayer = nullptr;
    // 背包图层
    cocos2d::Layer* bagLayer = nullptr;

    // 控制地图移动的变量
    bool moveLeft = false;
    bool moveRight = false;

    // 跳跃相关
    bool isJumping = false;
    float jumpVelocity = 0.0f;
    float jumpAcceleration = -200.0f;
    float maxJumpHeight = 30.0f;
    float currentJumpHeight = 0.0f;

    // 图层相关
    cocos2d::TMXLayer* blocksLayer = nullptr; // 不能穿越的阻挡层
    cocos2d::TMXLayer* itemsLayer = nullptr;  // 可穿越的物品层

    // 
    float fallSpeed = 0.0f;            // 当前下落速度
    float fallAcceleration = -3.0f;    // 下落加速度（通常是地球重力加速度）

    // 长按相关变量
    int touchStartFrame;
    int touchEndFrame;
    cocos2d::Vec2 touchStartPosition;
    float longPressThreshold = 0.5f; // 长按时间阈值（秒）
};

#endif // __GAME_SCENE_H__