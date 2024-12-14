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

private:
    int GameScene::checkBlockCollision(float x, float y);      // 检查与blocksLayer的碰撞
    void interactWithItems();                        // 与itemsLayer进行交互
    float getBlockTopY(float heroX, float heroY);
    void updatePhysicsWorld(float delta);

    //void addPhysicsBodiesForVisibleTiles();

    void updateCameraPosition(cocos2d::Sprite* player);

    // 新增成员变量以便保存地图和主角的位置关系
    Hero* hero;
};

#endif // __GAME_SCENE_H__




