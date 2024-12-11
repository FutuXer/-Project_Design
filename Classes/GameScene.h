#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include <iostream>
using namespace std;

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    virtual void update(float delta) override;
    CREATE_FUNC(GameScene);

private:
    void onMouseMove(cocos2d::Event* event);
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

    // 存放所有物品的字符串数组
    vector<string> itemImages = {
        "bronzen_pickaxe.png",  // 初始物品1：铜镐
        "bronzen_axe.png",      // 初始物品2：铜斧
        "bronzen_hammer.png",   // 初始物品3：铜锤
        "bronzen_sword.png",    // 初始物品4：铜剑
    };
    void checkPocket();                                                         // 检查口袋(只显示前10个常用物品，用于更换手里拿的物品)
    void checkBag();                                                            // 检查背包(显示所有随身物品、制造和装备，用于更换口袋中的物品和整理背包、整理口袋)
    void onItemMenuClicked(Ref* sender, int itemIndex);                         // 选择物品
    void ClickToChangePosition(Ref* sender, int itemIndex);                     // 更换物品在背包中的位置
    void ItemsInHand(int itemTag);                                              // 手中的物品
    cocos2d::Sprite* itemInMove = nullptr;                                      // 正在背包中移动的物品精灵

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