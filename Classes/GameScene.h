#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "hero.h"
#include "ui/CocosGUI.h"

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    //virtual void update(float delta) override;
    CREATE_FUNC(GameScene);

private:
    int GameScene::checkBlockCollision(float x, float y);      // 检查与blocksLayer的碰撞
    float getBlockTopY(float heroX, float heroY);
    void updatePhysicsWorld(float delta);

    //void addPhysicsBodiesForVisibleTiles();

    void updateCameraPosition(cocos2d::Sprite* player);

    // 新增成员变量以便保存地图和主角的位置关系
    Hero* hero;

    //天气系统
    bool isDay;                          // 当前是否白天
    float timeOfDay;                     // 游戏内的时间
    cocos2d::TMXLayer* dayLayer;         // 白天背景图层
    cocos2d::TMXLayer* nightLayer;       // 夜晚背景图层
    void updateWeather(float deltaTime);  
    void changeBackground();

    

};

#endif // __GAME_SCENE_H__






