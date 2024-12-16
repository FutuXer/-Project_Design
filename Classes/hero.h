#pragma once
#include"cocos2d.h"
#define JumpHeight 500 //����������Ծ�ĳ�����С
class Hero :public cocos2d::Sprite
{
    int health; //��ǰѪ��
    int total_health; //��Ѫ��
    // ���Ƶ�ͼ�ƶ��ı���
    bool moveLeft = false;
    bool moveRight = false;
    // ��Ծ���
    bool isJumping = false;
public:
    static Hero* create(const std::string& filename, int initialHealth) 
    {
        Hero* sprite = new (std::nothrow) Hero();
        if (sprite && sprite->initWithFile(filename))
        {
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
    void checkAndFixHeroCollision(float delta);

    //��Ʒ
    /*��Ʒ���*/
    void addTouchListener();                         // ���������
    void addBlockAtPosition(cocos2d::Vec2 position);
    void removeBlockAtPosition(cocos2d::Vec2 position); // ɾ�����ĺ���
    float getCurrentTime();                             // ��ȡ��ǰʱ��ĺ�������
    //cocos2d::Vector<MyItem*> items;                                             // ���������Ʒ����Ϣ����
    //cocos2d::Vector<MyItem*> pocketItems;                                       // ��ſڴ�����Ʒ����Ϣ���飨��items��ǰ8����
    //void iniitems();
    //void checkPocket();                                                         // ���ڴ�(ֻ��ʾǰ10��������Ʒ�����ڸ��������õ���Ʒ)
    //void checkBag();                                                            // ��鱳��(��ʾ����������Ʒ�������װ�������ڸ����ڴ��е���Ʒ��������������ڴ�)
    //void onItemMenuClicked(Ref* sender, int itemIndex);                         // ѡ����Ʒ
    //void ClickToChangePosition(Ref* sender, int itemIndex);                     // ������Ʒ�ڱ����е�λ��
    //void ItemsInHand(int itemTag);                                              // ���е���Ʒ����������Ķ���
    //MyItem* movingItem;                                                         // �����ƶ�����Ʒ
    //cocos2d::Sprite* itemInMove;                                                // ��ʾ�����ƶ�����Ʒ��������꣩
    // �ڴ�ͼ��
    //cocos2d::Layer* pocketLayer = nullptr;
    // ����ͼ��
    //cocos2d::Layer* bagLayer = nullptr;

    // ������ر���
    int touchStartFrame;
    int touchEndFrame;
    cocos2d::Vec2 touchStartPosition;
    float longPressThreshold = 0.5f; // ����ʱ����ֵ���룩*/
};