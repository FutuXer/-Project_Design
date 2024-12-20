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
    /*�й����*/
    void MonsterDie() { numberOfMonsters--; } //��������ʱ���ٹ�������

private:
    int checkBlockCollision(float x, float y);      // �����blocksLayer����ײ
    float getBlockTopY(float heroX, float heroY);
    void updatePhysicsWorld(float delta);

    //void addPhysicsBodiesForVisibleTiles();

    void updateCameraPosition(cocos2d::Sprite* player);

    // ������Ա�����Ա㱣���ͼ�����ǵ�λ�ù�ϵ
    Hero* hero;

    //����ϵͳ
    bool isDay = 1;                      // ��ǰ�Ƿ����
    float timeOfDay;                     // ��Ϸ�ڵ�ʱ��
    cocos2d::TMXLayer* dayLayer;         // ���챳��ͼ��
    cocos2d::TMXLayer* nightLayer;       // ҹ����ͼ��
    void updateWeather(float deltaTime);  
    void changeBackground();
    void updateSunAndMoonPosition(float deltaTime);

    /*menu*/
    int audioID = -1;  // ��ʼ�� audioID����ֹδ������Ϊ
    cocos2d::ui::Slider* volumeSlider = nullptr;  // ����������ָ��
    cocos2d::Menu* confirmMenu = nullptr;         // ȷ����ť�˵�ָ��

    // ��������
    void showSettings(cocos2d::Ref* sender);  // ��ʾ���ò˵�
    void exitGame(cocos2d::Ref* sender);      // �˳������˵�
    void createVolumeSlider();                // ����������������ȷ����ť
    void removeSettingsMenu();                // �Ƴ����ò˵�
    void createSettingsMenu();                // �������ò˵�

    /*�й����*/
    void MonsterFunc(float dt); //���ɹ��������й���ĺ��� 
    int numberOfMonsters = 0; //��ͼ�й��������
    int numberOfnpcs = 0; //��ͼ��npc������
    void GameScene::NPCFunc(float dt);
    bool Falljudgement(cocos2d::PhysicsContact& contact);

    cocos2d::ClippingNode* clippingNode;
};

#endif // __GAME_SCENE_H__