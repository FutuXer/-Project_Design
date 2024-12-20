#pragma once
#include"cocos2d.h"
#include "items.h"
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

    //********�������
    bool isRecovering = false;  //�Ƿ��ڻ�Ѫ
    cocos2d::ProgressTimer* healthBar; // Ѫ��
    cocos2d::Sprite* healthBarBG; //Ѫ������
    //********�������
    bool canAttack = false;
    bool isOnCooldown = false;
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
    /*��Ʒ���*/
    void iniItems();                                                            // ��ʼ��������Ʒ����
    cocos2d::Vector<MyItem*> items;                                             // ���������Ʒ����Ϣ����
    cocos2d::Vector<MyItem*> itemsToBeMade;                                     // ��������ܱ���������Ʒ
    cocos2d::Vector<MyItem*> pocketItems;                                       // ��ſڴ�����Ʒ����Ϣ���飨��items��ǰ8����
    void checkPocket();                                                         // ���ڴ�(ֻ��ʾǰ10��������Ʒ�����ڸ��������õ���Ʒ)
    void checkBag();                                                            // ��鱳��(��ʾ����������Ʒ�����ڸ����ڴ��е���Ʒ��������)
    void checkProduction();                                                     // �����Ա���������Ʒ����ʾ�������������뱳�������������Ʒ������
    void onItemMenuClicked(Ref* sender, int itemIndex);                         // ѡ����Ʒ
    void ClickToChangePosition(Ref* sender, int itemIndex);                     // ������Ʒ�ڱ����е�λ��
    void ClickToProduce(Ref* sender, int itemIndex);                            // ���°�ť���ϳ���Ʒ����һ�κϳ�һ����
    void pickUpItems(std::string Name);                                         // ʰȡ��Ʒ
    MyItem* movingItem;                                                         // �����ƶ�����Ʒ
    int usingItem;                                                              // ���ڱ�ʹ�õ���Ʒ��ֻ����ţ�������ŵĸı�ʵʱ���ã�
    cocos2d::Sprite* itemInMove;                                                // ��ʾ�����ƶ�����Ʒ��������꣩
    void calculate();                                                           // ����Ŀǰ���Ժϳɵ�װ��                       
    int findItemCount(const std::string& itemName, int& Index) {
        for (int i = 0; i < items.size(); i++) {
            if (items.at(i)->getItemName() == itemName) {
                Index = i;
                return items.at(i)->getItemNum();
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
    // ÿһ�д���һ����Ʒ����һ������Ʒ��ţ��ڶ�����gid
    int dictionaryToPlace[9][2]{
        10,4,
        11,1912,
        12,347,
        13,9650,
        14,6788,
        40,19211,
        41,19209,
        42,19210,
        43,9757,
    };

    void addTouchListener();                            // ���������
    void addBlockAtPosition(cocos2d::Vec2 position);
    void removeBlockAtPosition(cocos2d::Vec2 position); // ɾ�����ĺ���
    float getCurrentTime();                             // ��ȡ��ǰʱ��ĺ�������


    // ������ر���
    int touchStartFrame;
    int touchEndFrame;
    cocos2d::Vec2 touchStartPosition;
    float longPressThreshold = 0.5f; // ����ʱ����ֵ���룩*/

    //*******�������
    void takedamage(int damage); //�ܵ��˺�
    void die(); //�����߼�
    void startHealthRecovery(); //��Ѫ
    void stopHealthRecovery(); //���ܵ�����ʱ��ϻ�Ѫ
    void addhealthbar(); //���Ѫ��
    //*******�������
    void AttackMonsters(); //�����й�
    void addTouchListenerAttack(); // ���������(����)
    void applyFallDamage(float fallSpeed);  //����ˤ���˺�
};