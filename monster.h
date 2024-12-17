#pragma once
#pragma once
#include"cocos2d.h"
class Silme :public cocos2d::Sprite {
    int health; // ��ǰѪ��
    int total_health; //��Ѫ��
    cocos2d::ProgressTimer* healthBar;       // Ѫ��
    cocos2d::Sprite* healthBarBG; //Ѫ������
public:
    static Silme* create(const std::string& filename, int initialHealth) {
        Silme* sprite = new (std::nothrow) Silme();
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            sprite->total_health = initialHealth; // ���ó�ʼѪ��
            sprite->health = sprite->total_health;
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    // ����Ѫ��
    void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            die();
        }
        else {
            healthBar->setVisible(true);
            healthBarBG->setVisible(true);
            healthBar->setPercentage((float)health / total_health * 100);
            this->scheduleOnce([this](float) {
                this->healthBar->setVisible(false);
                this->healthBarBG->setVisible(false);
                }, 5.0f, "HideHealthBar"); //5�벻�ܹ���������Ѫ��
        }
    }

    // ���������߼�
    void die();
    void silme_move(float dt); //ʷ��ķ���˶��߼�
    void silme_jump(char direction);
    void addjellyeffect(); //����˹���Ч��
    void addPhy(); //���������Ч��
    void addhealthbar(); //���Ѫ��
    void brodercheck(float dt); //��ֹʷ��ķ������ͼ�߽�
    void AttackPlayer(float dt); //��������
    void DoAttack();
};

class Zombie :public cocos2d::Sprite {
    int health; //Ѫ��
    int total_health; //��Ѫ��
    cocos2d::ProgressTimer* healthBar;       // Ѫ��
    cocos2d::Sprite* healthBarBG; //Ѫ������
public:
    // ��������ľ�̬����
    static Zombie* create(const std::string& filename, int initialHealth) {
        Zombie* sprite = new (std::nothrow) Zombie();
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            sprite->total_health = initialHealth; // ���ó�ʼѪ��
            sprite->health = sprite->total_health;
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }

    // ����Ѫ��
    void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            die();
        }
        else {
            healthBar->setVisible(true);
            healthBarBG->setVisible(true);
            healthBar->setPercentage((float)health / total_health * 100);
            this->scheduleOnce([this](float) {
                this->healthBar->setVisible(false);
                this->healthBarBG->setVisible(false);
                }, 5.0f, "HideHealthBar"); //5�벻�ܹ���������Ѫ��
        }
    }

    // ���������߼�
    void die();
    void zombie_move(float dt);
    void addPhy(); //�������Ч��
    void addAnimate(); //��Ӷ���
    void addhealthbar(); //���Ѫ��
    void jump(float dt); //��ʬ��Ծ���߼�
    void AttackPlayer(float dt); //��������
    void DoAttack();
};