#include"monster.h"
#include"cocos2d.h"
#include"HelloWorldScene.h"
using namespace cocos2d;
void Silme::addjellyeffect() {
    auto scaleDown = ScaleTo::create(0.05f, 1.05f, 0.95f);  // 压缩
    auto scaleUp = ScaleTo::create(0.05f, 0.95f, 1.05f); // 拉伸
    auto restore = ScaleTo::create(0.05f, 1.0f, 1.0f);  // 恢复  
    auto jellyEffect = Sequence::create(scaleDown, scaleUp, restore, nullptr);
    this->runAction(RepeatForever::create(jellyEffect));
}

void Silme::addPhy() {
    auto silmebody = PhysicsBody::createBox(Size(this->getContentSize()), PhysicsMaterial(1, 0, 1));
    silmebody->setMass(1);
    silmebody->setRotationEnable(false); //锁定旋转
    this->setPhysicsBody(silmebody);
}

void Silme::die() {
    auto currentScene = Director::getInstance()->getRunningScene();
    auto myScene = dynamic_cast<HelloWorld*>(currentScene); //类型转换
    myScene->MonsterDie(); //怪物数减一
    // 停止所有与当前精灵相关的活动
    this->unscheduleAllCallbacks();
    this->stopAllActions();

    this->runAction(Sequence::create(
        FadeOut::create(0.5f), // 添加一个淡出效果
        CallFunc::create([this]() {
            this->removeFromParent(); // 在动作结束后移除
            }),
        nullptr));
    CCLOG("Character is dead!");
}

void Zombie::die() {
    auto currentScene = Director::getInstance()->getRunningScene();
    auto myScene = dynamic_cast<HelloWorld*>(currentScene); //类型转换
    myScene->MonsterDie(); //怪物数减一
    // 停止所有与当前精灵相关的活动
    this->unscheduleAllCallbacks();
    this->stopAllActions();

    this->runAction(Sequence::create(
        FadeOut::create(0.5f), // 添加一个淡出效果
        CallFunc::create([this]() {
            this->removeFromParent(); // 在动作结束后移除
            }),
        nullptr));
    CCLOG("Character is dead!");
}

void Silme::silme_move(float dt) {
    Vec2 Pos;
    takeDamage(20);
    Pos = this->getPosition();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    // 创建果冻效果动画
    auto scaleDown = ScaleTo::create(0.2f, 1.2f, 0.8f);  // 压缩
    auto scaleUp = ScaleTo::create(0.2f, 0.8f, 1.2f); // 拉伸
    auto restore = ScaleTo::create(0.2f, 1.0f, 1.0f);  // 恢复  
    auto jumpR = CallFunc::create([this]() {
        this->silme_jump('R'); // 向右跳
        });
    auto jumpL = CallFunc::create([this]() {
        this->silme_jump('L'); // 向左跳
        });
    // 动画序列
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
    // 创建动画对象
    auto animation = Animation::create();
    animation->addSpriteFrameWithFile("zombie1.png");
    animation->addSpriteFrameWithFile("zombie2.png");
    animation->addSpriteFrameWithFile("zombie3.png");
    animation->addSpriteFrameWithFile("zombie2.png");

    // 设置每帧持续时间为 0.5 秒，总共 3 帧，动画时长为 1.5 秒
    animation->setDelayPerUnit(0.1f);  // 每帧持续 0.5 秒

    // 创建播放动作
    auto animate_once = Animate::create(animation);
    auto animate = RepeatForever::create(animate_once);
    this->runAction(animate);
}

void Zombie::zombie_move(float dt) {
    Vec2 Pos;
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Pos = this->getPosition();
    auto movebyr = MoveBy::create(dt, Vec2(75 * dt, 0)); //向右走
    auto movebyl = MoveBy::create(dt, Vec2(-75 * dt, 0)); //向左走
    if (Pos.x < visibleSize.width / 2) {
        this->setFlippedX(false); // 恢复正常
        this->runAction(movebyr);
    }
    else {
        this->setFlippedX(true);  // 左右翻转
        this->runAction(movebyl);
    }
}

void Silme::silme_jump(char direction) {
    if (direction == 'R')
        this->getPhysicsBody()->applyImpulse(Vec2(200, 500)); // 向右跳
    if (direction == 'L')
        this->getPhysicsBody()->applyImpulse(Vec2(-200, 500)); // 向左跳
}

void Zombie::addPhy() {
    auto zombiebody = PhysicsBody::createBox(Size(this->getContentSize()), PhysicsMaterial(1, 0, 1));
    zombiebody->setMass(1);
    zombiebody->setRotationEnable(false); //锁定旋转
    this->setPhysicsBody(zombiebody);
}

void Silme::addhealthbar() {
    // 创建血条
    healthBarBG = cocos2d::Sprite::create("health_bar_bg.png"); // 血条背景
    healthBarBG->setPosition(this->getContentSize().width / 2, this->getContentSize().height + 10);
    this->addChild(healthBarBG);

    auto healthBarFill = cocos2d::Sprite::create("health_bar_fill.png"); // 血条填充
    healthBar = cocos2d::ProgressTimer::create(healthBarFill);
    healthBar->setType(cocos2d::ProgressTimer::Type::BAR);
    healthBar->setMidpoint(cocos2d::Vec2(0, 0.5f)); // 起始点为左侧
    healthBar->setBarChangeRate(cocos2d::Vec2(1, 0)); // 横向填充
    healthBar->setPosition(healthBarBG->getPosition());
    healthBar->setPercentage(100); // 初始填充满
    this->addChild(healthBar);

    // 初始隐藏血条
    healthBar->setVisible(false);
    healthBarBG->setVisible(false);
}

void Zombie::addhealthbar() {
    // 创建血条
    healthBarBG = cocos2d::Sprite::create("health_bar_bg.png"); // 血条背景
    healthBarBG->setPosition(this->getContentSize().width / 2, this->getContentSize().height + 10);
    this->addChild(healthBarBG);

    auto healthBarFill = cocos2d::Sprite::create("health_bar_fill.png"); // 血条填充
    healthBar = cocos2d::ProgressTimer::create(healthBarFill);
    healthBar->setType(cocos2d::ProgressTimer::Type::BAR);
    healthBar->setMidpoint(cocos2d::Vec2(0, 0.5f)); // 起始点为左侧
    healthBar->setBarChangeRate(cocos2d::Vec2(1, 0)); // 横向填充
    healthBar->setPosition(healthBarBG->getPosition());
    healthBar->setPercentage(100); // 初始填充满
    this->addChild(healthBar);

    // 初始隐藏血条
    healthBar->setVisible(false);
    healthBarBG->setVisible(false);
}