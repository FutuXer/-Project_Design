#include"monster.h"
#include"cocos2d.h"
#include"GameScene.h"
#include"hero.h"
using namespace cocos2d;
extern float mapWidth;
#define BlocksFirstGID__ 4
int checkBlockCollision(float x, float y);
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
    silmebody->setCategoryBitmask(0x02);
    silmebody->setContactTestBitmask(0x01);
    this->setPhysicsBody(silmebody);
}

void Silme::die() {
    auto currentScene = Director::getInstance()->getRunningScene();
    auto myScene = dynamic_cast<GameScene*>(currentScene); //����ת��
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
    auto myScene = dynamic_cast<GameScene*>(currentScene); //����ת��
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
    auto currentScene = Director::getInstance()->getRunningScene();
    auto Hero = currentScene->getChildByName("hero"); //ȡ�����ǵ�λ��
    if (Hero == NULL)
        return;
    auto Pos_H = Hero->getPosition();
    Pos = this->getPosition();
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
    if (Pos.x < Pos_H.x) {
        this->runAction(jellyEffectR);
    }
    else {
        this->runAction(jellyEffectL);
    }
   
}

void Silme::brodercheck(float dt) {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 Pos = this->getPosition();
    if (Pos.x > (visibleSize.width + mapWidth) / 2 - this->getContentSize().width / 2) {//�����ұ߽�
        Vec2 velocity = this->getPhysicsBody()->getVelocity();
        this->getPhysicsBody()->setVelocity(Vec2(-10, velocity.y));
    }
    if (Pos.x < (visibleSize.width - mapWidth) / 2 + this->getContentSize().width / 2) {//������߽�
        Vec2 velocity = this->getPhysicsBody()->getVelocity();
        this->getPhysicsBody()->setVelocity(Vec2(10, velocity.y));
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
    auto currentScene = Director::getInstance()->getRunningScene();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    auto Hero = currentScene->getChildByName("hero"); //ȡ�����ǵ�λ��
    if (Hero == NULL)
        return;
    auto Pos_H = Hero->getPosition();
    Pos = this->getPosition();
    auto movebyr = MoveBy::create(dt, Vec2(75 * dt, 0)); //������
    auto movebyl = MoveBy::create(dt, Vec2(-75 * dt, 0)); //������
    if (Pos.x < Pos_H.x && Pos.x < (visibleSize.width + mapWidth) / 2 - this->getContentSize().width / 2) {
        this->setFlippedX(false); // �ָ�����
        this->runAction(movebyr);
    }
    if (Pos.x >= Pos_H.x && Pos.x > (visibleSize.width - mapWidth) / 2 + this->getContentSize().width / 2) {
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
    zombiebody->setCategoryBitmask(0x04);
    zombiebody->setContactTestBitmask(0x01);
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

void Zombie::jump(float dt) {
    Vec2 Pos = this->getPosition();
    int tileGID_left1 = checkBlockCollision(Pos.x - 17, Pos.y - 10);
    int tileGID_left2 = checkBlockCollision(Pos.x - 17, Pos.y + 10);
    int tileGID_right1 = checkBlockCollision(Pos.x + 17, Pos.y - 10);
    int tileGID_right2 = checkBlockCollision(Pos.x + 17, Pos.y + 10);
    if (tileGID_left1 >= BlocksFirstGID__ || tileGID_left2 >= BlocksFirstGID__ || tileGID_right1 >= BlocksFirstGID__ || tileGID_right2 >= BlocksFirstGID__)
        this->getPhysicsBody()->applyImpulse(Vec2(0, 500));
}

void Silme::AttackPlayer(float dt) {
    // �������Ĺ�����Χ
    auto attackRange = Rect(this->getPositionX() - this->getContentSize().width/2,  // ��߽�
        this->getPositionY() - this->getContentSize().height/2,  // �±߽�
        this->getContentSize().width,                          // ���
        this->getContentSize().height);                         // �߶�

    // ʹ�����������ѯ
    auto physicsWorld = this->getScene()->getPhysicsWorld();
    physicsWorld->queryRect([this](PhysicsWorld& world, PhysicsShape& shape, void* data) -> bool {
        auto body = shape.getBody();
        if (body->getCategoryBitmask() == 0x01) { // ����Ƿ�������
            this->DoAttack(); // ���﹥������
            return false;         // ֹͣ��ѯ���Ѿ��ҵ�����
        }
        return true; // ������ѯ
        }, attackRange, NULL);
}

void Silme::DoAttack() {
    auto currentScene = Director::getInstance()->getRunningScene();
    auto temp = currentScene->getChildByName("hero"); //ȡ�����ǵ�λ��
    auto hero = dynamic_cast<Hero*>(temp); //����ת��
    if (hero == NULL)
        return;
    hero->takedamage(5);
}

void Zombie::AttackPlayer(float dt) {
    // �������Ĺ�����Χ
    auto attackRange = Rect(this->getPositionX() - this->getContentSize().width / 2,  // ��߽�
        this->getPositionY() - this->getContentSize().height / 2,  // �±߽�
        this->getContentSize().width,                          // ���
        this->getContentSize().height);                         // �߶�

    // ʹ�����������ѯ
    auto physicsWorld = this->getScene()->getPhysicsWorld();
    physicsWorld->queryRect([this](PhysicsWorld& world, PhysicsShape& shape, void* data) -> bool {
        auto body = shape.getBody();
        if (body->getCategoryBitmask() == 0x01) { // ����Ƿ�������
            this->DoAttack(); // ���﹥������
            return false;         // ֹͣ��ѯ���Ѿ��ҵ�����
        }
        return true; // ������ѯ
        }, attackRange, NULL);
}

void Zombie::DoAttack() {
    auto currentScene = Director::getInstance()->getRunningScene();
    auto temp = currentScene->getChildByName("hero"); //ȡ�����ǵ�λ��
    auto hero = dynamic_cast<Hero*>(temp); //����ת��
    if (hero == NULL)
        return;
    hero->takedamage(10);
}