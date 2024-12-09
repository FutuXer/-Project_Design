#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    virtual void update(float delta) override;
    CREATE_FUNC(GameScene);

private:
    void addKeyboardListener(cocos2d::Sprite* hero); // 添加键盘监听器
    void performJump(cocos2d::Sprite* hero);         // 执行跳跃动作
    int GameScene::checkBlockCollision(float x, float y);      // 检查与blocksLayer的碰撞
    void applyGravity(float delta);                   // 应用重力
    void applyJump(float delta);
    void interactWithItems();                        // 与itemsLayer进行交互
    float getBlockTopY(float heroX, float heroY);



    // 获取瓦片坐标
    cocos2d::Vec2 getTileCoordForPosition(float x, float y);

    // 新增成员变量以便保存地图和主角的位置关系
    cocos2d::TMXTiledMap* map;
    cocos2d::Sprite* hero;

    // 保存当前地图的位置
    cocos2d::Vec2 mapPosition;

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
};

#endif // __GAME_SCENE_H__




