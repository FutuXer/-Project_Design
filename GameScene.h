#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include"hero.h"

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
    int GameScene::checkBlockCollision(float x, float y);      // 检查与blocksLayer的碰撞
    void interactWithItems();                        // 与itemsLayer进行交互
    float getBlockTopY(float heroX, float heroY);
    void updatePhysicsWorld(float delta);

    //void addPhysicsBodiesForVisibleTiles();

    void updateCameraPosition(cocos2d::Sprite* player); //更新视角的函数

    // 新增成员变量以便保存地图和主角的位置关系
    Hero* hero;

    /*敌怪相关*/
    void MonsterFunc(float dt); //生成怪物与运行怪物的函数 
    int numberOfMonsters = 0; //地图中怪物的数量
    int numberOfnpcs = 0; //地图中npc的数量
    void GameScene::NPCFunc(float dt);


    bool onContactBegin(cocos2d::PhysicsContact& contact); //添加碰撞
};

#endif // __GAME_SCENE_H__




