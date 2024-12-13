#pragma once
#include"cocos2d.h"
#define JumpHeight 500 //����������Ծ�ĳ�����С
class Hero :public cocos2d::Sprite {
    int health; //��ǰѪ��
    int total_health; //��Ѫ��
    // ���Ƶ�ͼ�ƶ��ı���
    bool moveLeft = false;
    bool moveRight = false;
    // ��Ծ���
    bool isJumping = false;
public:
    static Hero* create(const std::string& filename, int initialHealth) {
        Hero* sprite = new (std::nothrow) Hero();
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            sprite->total_health = initialHealth; // ���ó�ʼѪ��
            sprite->health = sprite->total_health;
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    void addPhy(); //Ϊ������������������
    void addKeyboardListener();  //��Ӽ��̼�����
    void applyJump(); //������Ծ
    void performJump();  
    void HeroFunc(float delta);
    void setHeroAnimation(const std::string& frame2, const std::string& frame3, const std::string& frame4, const std::string& frame5);
    void checkAndFixHeroCollision();
};