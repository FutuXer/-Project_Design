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
    int GameScene::checkBlockCollision(float x, float y);      // �����blocksLayer����ײ
    float getBlockTopY(float heroX, float heroY);
    void updatePhysicsWorld(float delta);

    //void addPhysicsBodiesForVisibleTiles();

    void updateCameraPosition(cocos2d::Sprite* player);

    // ������Ա�����Ա㱣���ͼ�����ǵ�λ�ù�ϵ
    Hero* hero;

    //����ϵͳ
    bool isDay;                          // ��ǰ�Ƿ����
    float timeOfDay;                     // ��Ϸ�ڵ�ʱ��
    cocos2d::TMXLayer* dayLayer;         // ���챳��ͼ��
    cocos2d::TMXLayer* nightLayer;       // ҹ����ͼ��
    void updateWeather(float deltaTime);  
    void changeBackground();

    

};

#endif // __GAME_SCENE_H__






