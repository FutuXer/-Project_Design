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
    void addKeyboardListener(cocos2d::Sprite* hero); // ��Ӽ��̼�����
    void performJump(cocos2d::Sprite* hero);         // ִ����Ծ����
    int GameScene::checkBlockCollision(float x, float y);      // �����blocksLayer����ײ
    void applyGravity(float delta);                   // Ӧ������
    void applyJump(float delta);
    void interactWithItems();                        // ��itemsLayer���н���
    float getBlockTopY(float heroX, float heroY);



    // ��ȡ��Ƭ����
    cocos2d::Vec2 getTileCoordForPosition(float x, float y);

    // ������Ա�����Ա㱣���ͼ�����ǵ�λ�ù�ϵ
    cocos2d::TMXTiledMap* map;
    cocos2d::Sprite* hero;

    // ���浱ǰ��ͼ��λ��
    cocos2d::Vec2 mapPosition;

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
};

#endif // __GAME_SCENE_H__




