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
    void addTouchListener();                         // ���������
    void addKeyboardListener(cocos2d::Sprite* hero); // ��Ӽ��̼�����
    void performJump(cocos2d::Sprite* hero);         // ִ����Ծ����
    int GameScene::checkBlockCollision(float x, float y);      // �����blocksLayer����ײ
    void applyGravity(float delta);                   // Ӧ������
    void applyJump(int delta);
    void interactWithItems();                        // ��itemsLayer���н���
    float getBlockTopY(float heroX, float heroY);
    void updatePhysicsWorld(float delta);
    void checkAndFixHeroCollision();
    void addBlockAtPosition(cocos2d::Vec2 position);
    void removeBlockAtPosition(cocos2d::Vec2 position); // ɾ�����ĺ���
    float getCurrentTime();                             // ��ȡ��ǰʱ��ĺ�������

    /*��Ʒ���*/
    cocos2d::Vector<MyItem*> items;                                             // ���������Ʒ����Ϣ����
    cocos2d::Vector<MyItem*> pocketItems;                                       // ��ſڴ�����Ʒ����Ϣ���飨��items��ǰ8����
    void checkPocket();                                                         // ���ڴ�(ֻ��ʾǰ10��������Ʒ�����ڸ��������õ���Ʒ)
    void checkBag();                                                            // ��鱳��(��ʾ����������Ʒ�������װ�������ڸ����ڴ��е���Ʒ��������������ڴ�)
    void onItemMenuClicked(Ref* sender, int itemIndex);                         // ѡ����Ʒ
    void ClickToChangePosition(Ref* sender, int itemIndex);                     // ������Ʒ�ڱ����е�λ��
    void ItemsInHand(int itemTag);                                              // ���е���Ʒ����������Ķ���
    MyItem* movingItem;                                                         // �����ƶ�����Ʒ
    cocos2d::Sprite* itemInMove;                                                // ��ʾ�����ƶ�����Ʒ��������꣩

    // ��ȡ��Ƭ����
    cocos2d::Vec2 getTileCoordForPosition(float x, float y);

    // ������Ա�����Ա㱣���ͼ�����ǵ�λ�ù�ϵ
    cocos2d::TMXTiledMap* map;
    cocos2d::Sprite* hero;

    // ���浱ǰ��ͼ��λ��
    cocos2d::Vec2 mapPosition;

    // �ڴ�ͼ��
    cocos2d::Layer* pocketLayer = nullptr;
    // ����ͼ��
    cocos2d::Layer* bagLayer = nullptr;

    // ���Ƶ�ͼ�ƶ��ı���
    bool moveLeft = false;
    bool moveRight = false;

    // ��Ծ���
    bool isJumping = false;
    float jumpVelocity = 0.0f;
    float jumpAcceleration = -200.0f;
    float maxJumpHeight = 30.0f;
    float currentJumpHeight = 0.0f;

    // ͼ�����
    cocos2d::TMXLayer* blocksLayer = nullptr; // ���ܴ�Խ���赲��
    cocos2d::TMXLayer* itemsLayer = nullptr;  // �ɴ�Խ����Ʒ��

    // 
    float fallSpeed = 0.0f;            // ��ǰ�����ٶ�
    float fallAcceleration = -3.0f;    // ������ٶȣ�ͨ���ǵ����������ٶȣ�

    // ������ر���
    int touchStartFrame;
    int touchEndFrame;
    cocos2d::Vec2 touchStartPosition;
    float longPressThreshold = 0.5f; // ����ʱ����ֵ���룩
};

#endif // __GAME_SCENE_H__