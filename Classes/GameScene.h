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
    /*敌怪相关*/
    void MonsterDie() { numberOfMonsters--; } //怪物死亡时减少怪物数量

private:
    int checkBlockCollision(float x, float y);      // 检查与blocksLayer的碰撞
    float getBlockTopY(float heroX, float heroY);
    void updatePhysicsWorld(float delta);

    //void addPhysicsBodiesForVisibleTiles();

    void updateCameraPosition(cocos2d::Sprite* player);

    // 新增成员变量以便保存地图和主角的位置关系
    Hero* hero;

    //天气系统
    bool isDay = 1;                      // 当前是否白天
    float timeOfDay;                     // 游戏内的时间
    cocos2d::TMXLayer* dayLayer;         // 白天背景图层
    cocos2d::TMXLayer* nightLayer;       // 夜晚背景图层
    void updateWeather(float deltaTime);  
    void changeBackground();
    void updateSunAndMoonPosition(float deltaTime);

    /*menu*/
    int audioID = -1;  // 初始化 audioID，防止未定义行为
    cocos2d::ui::Slider* volumeSlider = nullptr;  // 音量滑动条指针
    cocos2d::Menu* confirmMenu = nullptr;         // 确定按钮菜单指针

    // 函数声明
    void showSettings(cocos2d::Ref* sender);  // 显示设置菜单
    void exitGame(cocos2d::Ref* sender);      // 退出到主菜单
    void createVolumeSlider();                // 创建音量滑动条和确定按钮
    void removeSettingsMenu();                // 移除设置菜单
    void createSettingsMenu();                // 创建设置菜单

    /*敌怪相关*/
    void MonsterFunc(float dt); //生成怪物与运行怪物的函数 
    int numberOfMonsters = 0; //地图中怪物的数量
    int numberOfnpcs = 0; //地图中npc的数量
    void GameScene::NPCFunc(float dt);
    bool Falljudgement(cocos2d::PhysicsContact& contact);

    cocos2d::ClippingNode* clippingNode;
};

#endif // __GAME_SCENE_H__