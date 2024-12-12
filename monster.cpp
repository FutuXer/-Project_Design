#include"monster.h"
#include"cocos2d.h"
#include"HelloWorldScene.h"
using namespace cocos2d;
void Silme::addjellyeffect() {
    auto scaleDown = ScaleTo::create(0.05f, 1.05f, 0.95f);  // ѹ��
    auto scaleUp = ScaleTo::create(0.05f, 0.95f, 1.05f); // ����
    auto restore = ScaleTo::create(0.05f, 1.0f, 1.0f);  // �ָ�  
    auto jellyEffect = Sequence::create(scaleDown, scaleUp, restore, nullptr);
    this->runAction(RepeatForever::create(jellyEffect));
}

void Silme::addPhy() {
    auto silmebody = PhysicsBody::createBox(Size(this->getContentSize()), PhysicsMaterial(1, 0, 1));
    silmebody->setMass(1);
    silmebody->setRotationEnable(false); //������ת
    this->setPhysicsBody(silmebody);
}

void Silme::die() {
    auto currentScene = Director::getInstance()->getRunningScene();
    auto myScene = dynamic_cast<HelloWorld*>(currentScene); //����ת��
    myScene->MonsterDie(); //��������һ
    // ֹͣ�����뵱ǰ������صĻ
    this->unscheduleAllCallbacks();
    this->stopAllActions();

    this->runAction(Sequence::create(
        FadeOut::create(0.5f), // ���һ������Ч��
        CallFunc::create([this]() {
            this->removeFromParent(); // �ڶ����������Ƴ�
            }),
        nullptr));
    CCLOG("Character is dead!");
}

void Zombie::die() {
    auto currentScene = Director::getInstance()->getRunningScene();
    auto myScene = dynamic_cast<HelloWorld*>(currentScene); //����ת��
    myScene->MonsterDie(); //��������һ
    // ֹͣ�����뵱ǰ������صĻ
    this->unscheduleAllCallbacks();
    this->stopAllActions();

    this->runAction(Sequence::create(
        FadeOut::create(0.5f), // ���һ������Ч��
        CallFunc::create([this]() {
            this->removeFromParent(); // �ڶ����������Ƴ�
            }),
        nullptr));
    CCLOG("Character is dead!");
}

void Silme::silme_move(float dt) {
    Vec2 Pos;
    takeDamage(20);
    Pos = this->getPosition();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    // ��������Ч������
    auto scaleDown = ScaleTo::create(0.2f, 1.2f, 0.8f);  // ѹ��
    auto scaleUp = ScaleTo::create(0.2f, 0.8f, 1.2f); // ����
    auto restore = ScaleTo::create(0.2f, 1.0f, 1.0f);  // �ָ�  
    auto jumpR = CallFunc::create([this]() {
        this->silme_jump('R'); // ������
        });
    auto jumpL = CallFunc::create([this]() {
        this->silme_jump('L'); // ������
        });
    // ��������
    auto jellyEffectR = Sequence::create(scaleDown, scaleUp, jumpR, restore, nullptr);
    auto jellyEffectL = Sequence::create(scaleDown, scaleUp, jumpL, restore, nullptr);
    if (Pos.x < visibleSize.width / 2) {
        this->runAction(jellyEffectR);
    }
    else {
        this->runAction(jellyEffectL);
    }
}

void Zombie::addAnimate() {
    // ������������
    auto animation = Animation::create();
    animation->addSpriteFrameWithFile("zombie1.png");
    animation->addSpriteFrameWithFile("zombie2.png");
    animation->addSpriteFrameWithFile("zombie3.png");
    animation->addSpriteFrameWithFile("zombie2.png");

    // ����ÿ֡����ʱ��Ϊ 0.5 �룬�ܹ� 3 ֡������ʱ��Ϊ 1.5 ��
    animation->setDelayPerUnit(0.1f);  // ÿ֡���� 0.5 ��

    // �������Ŷ���
    auto animate_once = Animate::create(animation);
    auto animate = RepeatForever::create(animate_once);
    this->runAction(animate);
}

void Zombie::zombie_move(float dt) {
    Vec2 Pos;
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Pos = this->getPosition();
    auto movebyr = MoveBy::create(dt, Vec2(75 * dt, 0)); //������
    auto movebyl = MoveBy::create(dt, Vec2(-75 * dt, 0)); //������
    if (Pos.x < visibleSize.width / 2) {
        this->setFlippedX(false); // �ָ�����
        this->runAction(movebyr);
    }
    else {
        this->setFlippedX(true);  // ���ҷ�ת
        this->runAction(movebyl);
    }
}

void Silme::silme_jump(char direction) {
    if (direction == 'R')
        this->getPhysicsBody()->applyImpulse(Vec2(200, 500)); // ������
    if (direction == 'L')
        this->getPhysicsBody()->applyImpulse(Vec2(-200, 500)); // ������
}

void Zombie::addPhy() {
    auto zombiebody = PhysicsBody::createBox(Size(this->getContentSize()), PhysicsMaterial(1, 0, 1));
    zombiebody->setMass(1);
    zombiebody->setRotationEnable(false); //������ת
    this->setPhysicsBody(zombiebody);
}

void Silme::addhealthbar() {
    // ����Ѫ��
    healthBarBG = cocos2d::Sprite::create("health_bar_bg.png"); // Ѫ������
    healthBarBG->setPosition(this->getContentSize().width / 2, this->getContentSize().height + 10);
    this->addChild(healthBarBG);

    auto healthBarFill = cocos2d::Sprite::create("health_bar_fill.png"); // Ѫ�����
    healthBar = cocos2d::ProgressTimer::create(healthBarFill);
    healthBar->setType(cocos2d::ProgressTimer::Type::BAR);
    healthBar->setMidpoint(cocos2d::Vec2(0, 0.5f)); // ��ʼ��Ϊ���
    healthBar->setBarChangeRate(cocos2d::Vec2(1, 0)); // �������
    healthBar->setPosition(healthBarBG->getPosition());
    healthBar->setPercentage(100); // ��ʼ�����
    this->addChild(healthBar);

    // ��ʼ����Ѫ��
    healthBar->setVisible(false);
    healthBarBG->setVisible(false);
}

void Zombie::addhealthbar() {
    // ����Ѫ��
    healthBarBG = cocos2d::Sprite::create("health_bar_bg.png"); // Ѫ������
    healthBarBG->setPosition(this->getContentSize().width / 2, this->getContentSize().height + 10);
    this->addChild(healthBarBG);

    auto healthBarFill = cocos2d::Sprite::create("health_bar_fill.png"); // Ѫ�����
    healthBar = cocos2d::ProgressTimer::create(healthBarFill);
    healthBar->setType(cocos2d::ProgressTimer::Type::BAR);
    healthBar->setMidpoint(cocos2d::Vec2(0, 0.5f)); // ��ʼ��Ϊ���
    healthBar->setBarChangeRate(cocos2d::Vec2(1, 0)); // �������
    healthBar->setPosition(healthBarBG->getPosition());
    healthBar->setPercentage(100); // ��ʼ�����
    this->addChild(healthBar);

    // ��ʼ����Ѫ��
    healthBar->setVisible(false);
    healthBarBG->setVisible(false);
}