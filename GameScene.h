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
    /*�й����*/
    void MonsterDie() { numberOfMonsters--; } //��������ʱ���ٹ�������

private:
    int GameScene::checkBlockCollision(float x, float y);      // �����blocksLayer����ײ
    void interactWithItems();                        // ��itemsLayer���н���
    float getBlockTopY(float heroX, float heroY);
    void updatePhysicsWorld(float delta);

    //void addPhysicsBodiesForVisibleTiles();

    void updateCameraPosition(cocos2d::Sprite* player); //�����ӽǵĺ���

    // ������Ա�����Ա㱣���ͼ�����ǵ�λ�ù�ϵ
    Hero* hero;

    /*�й����*/
    void MonsterFunc(float dt); //���ɹ��������й���ĺ��� 
    int numberOfMonsters = 0; //��ͼ�й��������
    int numberOfnpcs = 0; //��ͼ��npc������
    void GameScene::NPCFunc(float dt);


    bool onContactBegin(cocos2d::PhysicsContact& contact); //�����ײ
};

#endif // __GAME_SCENE_H__




