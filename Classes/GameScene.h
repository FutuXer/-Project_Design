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
    cocos2d::Vector<MyItem*> itemsToBeMade;                                     // ��������ܱ���������Ʒ
    cocos2d::Vector<MyItem*> pocketItems;                                       // ��ſڴ�����Ʒ����Ϣ���飨��items��ǰ8����
    void checkPocket();                                                         // ���ڴ�(ֻ��ʾǰ10��������Ʒ�����ڸ��������õ���Ʒ)
    void checkBag();                                                            // ��鱳��(��ʾ����������Ʒ�����ڸ����ڴ��е���Ʒ��������)
    void checkProduction();                                                     // �����Ա���������Ʒ����ʾ�������������뱳�������������Ʒ������
    void onItemMenuClicked(Ref* sender, int itemIndex);                         // ѡ����Ʒ
    void ClickToChangePosition(Ref* sender, int itemIndex);                     // ������Ʒ�ڱ����е�λ��
    void ClickToProduce(Ref* sender, int itemIndex);                            // ���°�ť���ϳ���Ʒ����һ�κϳ�һ����
    void ItemsInHand(int itemTag);                                              // ���е���Ʒ����������Ķ���
    MyItem* movingItem;                                                         // �����ƶ�����Ʒ
    cocos2d::Sprite* itemInMove;                                                // ��ʾ�����ƶ�����Ʒ��������꣩
    void calculate();                                                           // ����Ŀǰ���Ժϳɵ�װ��                       
    int findItemCount(const std::string& itemName,int &Index) {
        for (int i = 0; i < items.size(); i++) {
            if (items.at(i)->getItemName() == itemName) {
                Index = i;
                return items.at(i)->getNum();
            }
        }
        return 0; // û���ҵ�ʱ���� 0
    }
    // �ϳ���Ʒ���ֵ䣺ÿ�����ִ����Ʒ��ţ����������ԭ��1�ĸ�����ԭ��1�ı�š�ԭ��2�ĸ�����ԭ��2�ı��
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
        0,0,0,0,            // ������λ���Ա�������Ӻϳɵ���Ʒ
    };

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