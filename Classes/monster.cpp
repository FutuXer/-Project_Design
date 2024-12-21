#include"monster.h"
#include"cocos2d.h"
#include"GameScene.h"
#include"hero.h"
using namespace cocos2d;
extern float mapWidth;
#define BlocksFirstGID__ 4
int checkBlockCollision(float x, float y);
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
    silmebody->setCategoryBitmask(0x02);
    silmebody->setContactTestBitmask(0x01);
    this->setPhysicsBody(silmebody);
}

void Silme::die() {
    auto currentScene = Director::getInstance()->getRunningScene();
    auto myScene = dynamic_cast<GameScene*>(currentScene); //类型转换
    myScene->MonsterDie(); //怪物数减一
    auto hero = myScene->getChildByName("hero");
    auto TheHero = dynamic_cast<Hero*>(hero);
    TheHero->pickUpItems("50.png", 5);                   // 打史莱姆获得枚5铜币
     
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
    auto myScene = dynamic_cast<GameScene*>(currentScene); //类型转换
    myScene->MonsterDie(); //怪物数减一
    auto hero = myScene->getChildByName("hero");
    auto TheHero = dynamic_cast<Hero*>(hero);
    TheHero->pickUpItems("50.png", 10);                   // 打僵尸获得枚10铜币
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
    auto currentScene = Director::getInstance()->getRunningScene();
    auto Hero = currentScene->getChildByName("hero"); //取得主角的位置
    if (Hero == NULL)
        return;
    auto Pos_H = Hero->getPosition();
    Pos = this->getPosition();
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
    if (Pos.x > (visibleSize.width + mapWidth) / 2 - this->getContentSize().width / 2) {//超出右边界
        Vec2 velocity = this->getPhysicsBody()->getVelocity();
        this->getPhysicsBody()->setVelocity(Vec2(-10, velocity.y));
    }
    if (Pos.x < (visibleSize.width - mapWidth) / 2 + this->getContentSize().width / 2) {//超出左边界
        Vec2 velocity = this->getPhysicsBody()->getVelocity();
        this->getPhysicsBody()->setVelocity(Vec2(10, velocity.y));
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
    auto currentScene = Director::getInstance()->getRunningScene();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    auto Hero = currentScene->getChildByName("hero"); //取得主角的位置
    if (Hero == NULL)
        return;
    auto Pos_H = Hero->getPosition();
    Pos = this->getPosition();
    auto movebyr = MoveBy::create(dt, Vec2(75 * dt, 0)); //向右走
    auto movebyl = MoveBy::create(dt, Vec2(-75 * dt, 0)); //向左走
    if (Pos.x < Pos_H.x && Pos.x < (visibleSize.width + mapWidth) / 2 - this->getContentSize().width / 2) {
        this->setFlippedX(false); // 恢复正常
        this->runAction(movebyr);
    }
    if (Pos.x >= Pos_H.x && Pos.x > (visibleSize.width - mapWidth) / 2 + this->getContentSize().width / 2) {
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
    zombiebody->setCategoryBitmask(0x04);
    zombiebody->setContactTestBitmask(0x01);
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
    // 定义怪物的攻击范围
    auto attackRange = Rect(this->getPositionX() - this->getContentSize().width / 2,  // 左边界
        this->getPositionY() - this->getContentSize().height / 2,  // 下边界
        this->getContentSize().width,                          // 宽度
        this->getContentSize().height);                         // 高度

    // 使用物理世界查询
    auto physicsWorld = this->getScene()->getPhysicsWorld();
    physicsWorld->queryRect([this](PhysicsWorld& world, PhysicsShape& shape, void* data) -> bool {
        auto body = shape.getBody();
        if (body->getCategoryBitmask() == 0x01) { // 检查是否是主角
            auto node = body->getNode();
            auto hero = dynamic_cast<Hero*>(node); // 将 Node 转为 Hero 类型
            hero->takedamage(5); // 怪物攻击主角
        }
        return true; // 继续查询
        }, attackRange, NULL);
}


void Zombie::AttackPlayer(float dt) {
    // 定义怪物的攻击范围
    auto attackRange = Rect(this->getPositionX() - this->getContentSize().width / 2,  // 左边界
        this->getPositionY() - this->getContentSize().height / 2,  // 下边界
        this->getContentSize().width,                          // 宽度
        this->getContentSize().height);                         // 高度

    // 使用物理世界查询
    auto physicsWorld = this->getScene()->getPhysicsWorld();
    physicsWorld->queryRect([this](PhysicsWorld& world, PhysicsShape& shape, void* data) -> bool {
        auto body = shape.getBody();
        if (body->getCategoryBitmask() == 0x01) { // 检查是否是主角
            auto node = body->getNode();
            auto hero = dynamic_cast<Hero*>(node); // 将 Node 转为 Hero 类型
            hero->takedamage(10); // 怪物攻击主角
        }
        return true; // 继续查询
        }, attackRange, NULL);
}

void Zombie::takeDamage(int damage) {
    {
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
                }, 5.0f, "HideHealthBar"); //5秒不受攻击则隐藏血条
            auto highlight = TintTo::create(0.2f, 255, 0, 0); // 红色高亮
            auto reset = TintTo::create(0.2f, 255, 255, 255); // 恢复原色
            auto sequence = Sequence::create(highlight, reset, nullptr);
            this->runAction(sequence);
        }
    }
}

void Silme::takeDamage(int damage) {
    {
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
                }, 5.0f, "HideHealthBar"); //5秒不受攻击则隐藏血条
            auto highlight = TintTo::create(0.2f, 255, 0, 0); // 红色高亮
            auto reset = TintTo::create(0.2f, 255, 255, 255); // 恢复原色
            auto sequence = Sequence::create(highlight, reset, nullptr);
            this->runAction(sequence);
        }
    }
}

//以下写的是npc相关

void NPC::npc_move(float dt) {
    Vec2 Pos;
    auto currentScene = Director::getInstance()->getRunningScene();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    auto Hero = currentScene->getChildByName("hero"); // 获取主角的位置

    if (Hero == NULL) {
        return;
    }

    auto Pos_H = Hero->getPosition();
    Pos = this->getPosition();

    // 计算距离
    float distance = Pos.distance(Pos_H);

    auto movebyr = MoveBy::create(dt, Vec2(75 * dt, 0)); // 向右走
    auto movebyl = MoveBy::create(dt, Vec2(-75 * dt, 0)); // 向左走

    // 判断 NPC 是否应该向右移动
    if (Pos.x < Pos_H.x && Pos.x < (visibleSize.width + mapWidth) / 2 - this->getContentSize().width / 2 && Pos_H.x - Pos.x > 100) {
        this->setFlippedX(true);  // 恢复正常朝向（面向右侧）
        this->runAction(movebyr);  // 执行向右移动的动作
    }
    // 判断 NPC 是否应该向左移动
    else if (Pos.x >= Pos_H.x && Pos.x > (visibleSize.width - mapWidth) / 2 + this->getContentSize().width / 2 && Pos.x - Pos_H.x > 100) {
        this->setFlippedX(false);  // 翻转朝向（面向左侧）
        this->runAction(movebyl);  // 执行向左移动的动作
    }
    // 如果 NPC 没有移动，则暂停
    else {
        this->stopActionByTag(1);  // 停止向右的动作
        this->stopActionByTag(2);  // 停止向左的动作
    }

    // 判断与主角的距离，停止或开始动画
    if (distance < 100 && isAnimationPlaying) {  // 距离小于100时停止动画
        stopAnimate();  // 停止动画
        isAnimationPlaying = 0;
    }
    else if (distance >= 100 && !isAnimationPlaying) {  // 距离大于100时开始动画
        addAnimate();  // 启动动画
        isAnimationPlaying = 1;
    }
}

// 停止动画的函数
void NPC::stopAnimate() {
    this->stopAllActions();
    isAnimationPlaying = 0;
}

void NPC::addPhy() {
    auto npcbody = PhysicsBody::createBox(Size(this->getContentSize()), PhysicsMaterial(1, 0, 1));
    npcbody->setMass(1);
    npcbody->setRotationEnable(false); //锁定旋转
    /*孙*/
    npcbody->setCategoryBitmask(0x08);
    npcbody->setContactTestBitmask(0x01);

    this->setPhysicsBody(npcbody);
}


void NPC::jump(float dt) {
    Vec2 Pos = this->getPosition();
    int tileGID_left1 = checkBlockCollision(Pos.x - 17, Pos.y - 10);
    int tileGID_left2 = checkBlockCollision(Pos.x - 17, Pos.y + 10);
    int tileGID_right1 = checkBlockCollision(Pos.x + 17, Pos.y - 10);
    int tileGID_right2 = checkBlockCollision(Pos.x + 17, Pos.y + 10);
    if (tileGID_left1 >= BlocksFirstGID__ || tileGID_left2 >= BlocksFirstGID__ || tileGID_right1 >= BlocksFirstGID__ || tileGID_right2 >= BlocksFirstGID__)
        this->getPhysicsBody()->applyImpulse(Vec2(0, 500));
}


void NPC::approachplayer(float dt) {
    // 定义靠近范围
    auto attackRange = Rect(this->getPositionX() - this->getContentSize().width / 2,  // 左边界
        this->getPositionY() - this->getContentSize().height / 2,  // 下边界
        this->getContentSize().width,                          // 宽度
        this->getContentSize().height);                         // 高度

    // 使用物理世界查询
    auto physicsWorld = this->getScene()->getPhysicsWorld();
    physicsWorld->queryRect([this](PhysicsWorld& world, PhysicsShape& shape, void* data) -> bool {
        auto body = shape.getBody();
        if (body->getCategoryBitmask() == 0x01) { // 检查是否是主角
            auto hero = body->getNode();

            /*
            this->action(); // 怪物攻击主角
            return false;         // 停止查询，已经找到主角
            */
        }
        return true; // 继续查询
        }, attackRange, NULL);
}

void NPC::action() {
    auto currentScene = Director::getInstance()->getRunningScene();
    auto temp = currentScene->getChildByName("hero"); //取得主角的位置
    auto hero = dynamic_cast<Hero*>(temp); //类型转换
    if (hero == NULL)
        return;
    hero->takedamage(10);
}


void NPC::addTouchListener()
{
    // 创建一个鼠标事件监听器
    auto listener = EventListenerMouse::create();

    // 设置鼠标事件监听器
    listener->onMouseDown = [this](Event* event)
        {
            auto mouseEvent = dynamic_cast<EventMouse*>(event);
            if (mouseEvent && mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
            {
                auto location = mouseEvent->getLocationInView(); // 获取鼠标位置（相对于视图）

                // 打印鼠标点击位置 (屏幕坐标)
                CCLOG("Mouse right-clicked at: (%.2f, %.2f)", location.x, location.y);

                // 获取精灵的屏幕坐标（相对于窗口左下角的坐标）
                auto spritePos = this->getPosition();

                // 转换精灵的场景坐标为屏幕坐标
                auto screenSpritePos = this->getParent()->convertToWorldSpace(spritePos);

                // 获取当前场景
                auto scene = Director::getInstance()->getRunningScene();

                // 从场景中获取相机节点
                auto camera = scene->getChildByName("Camera");  // 假设你的相机节点名字是 "camera"

                // 获取相机的坐标（场景坐标）
                auto cameraPos = camera->getPosition();

                // 获取当前场景的可见区域大小
                Size visibleSize = Director::getInstance()->getVisibleSize();

                // 获取当前场景的原点位置（左下角的坐标）
                Vec2 visibleOrigin = Director::getInstance()->getVisibleOrigin();

                // 计算精灵的最终屏幕坐标（考虑了相机偏移）
                Vec2 finalSpriteScreenPos = screenSpritePos - cameraPos + visibleSize / 2;

                CCLOG("Sprite final screen position1: (%.2f, %.2f)", finalSpriteScreenPos.x, finalSpriteScreenPos.y);

                // 获取主角（Hero）
                auto currentScene = Director::getInstance()->getRunningScene();
                auto hero = currentScene->getChildByName("hero");

                if (hero)
                {
                    // 获取NPC的场景坐标
                    auto spritePos = this->getPosition();

                    // 获取主角的场景坐标
                    auto heroPos = hero->getPosition();

                    // 计算 NPC 和主角之间的场景坐标距离
                    float deltaX = std::abs(spritePos.x - heroPos.x);
                    float deltaY = std::abs(spritePos.y - heroPos.y);

                    // 如果水平和竖直方向距离都小于 100 像素，则显示交互菜单
                    if (deltaX < 100 && deltaY < 100)
                    {
                        showInteractionMenu(); // 显示交互菜单
                        CCLOG("Interaction Menu Displayed");
                    }
                    else
                    {
                        CCLOG("NPC and Hero are too far apart to interact.");
                    }
                }
            }
        };

    // 添加事件监听器到事件分发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void NPC::showInteractionMenu()
{
    // 如果菜单已存在，先移除它
    if (interactionMenu != nullptr)
    {
        return;
    }

    // 创建第一个图片型按钮
    auto button1 = MenuItemImage::create(
        "32.png", // 正常状态的图片
        "32.png", // 点击状态的图片
        CC_CALLBACK_1(NPC::button1Callback, this)   //  购买宝剑（1金币）
    );

    // 创建第二个图片型按钮
    auto button2 = MenuItemImage::create(
        "22.png",
        "22.png",
        CC_CALLBACK_1(NPC::button2Callback, this)   // 购买强力镐子（1金币）
    );

    // 创建退出按钮
    auto exitLabel = Label::createWithTTF(u8"退出", "fonts/jiantisongti.ttf", 24);
    auto exitButton = MenuItemLabel::create(
        exitLabel,
        CC_CALLBACK_1(NPC::exitButtonCallback, this)
    );

    // 创建菜单并将按钮添加到菜单中
    interactionMenu = Menu::create(button1, button2, exitButton, nullptr);

    // 获取NPC的当前位置
    auto spritePos = this->getPosition();

    // 获取场景中的相机位置，用于计算相对屏幕的位置
    auto scene = Director::getInstance()->getRunningScene();
    auto camera = scene->getChildByName("Camera");
    auto cameraPos = camera->getPosition();

    // 获取当前场景的可见区域大小
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // 计算 NPC 在屏幕上的位置（考虑相机偏移）
    Vec2 finalSpriteScreenPos = spritePos - cameraPos - visibleSize / 2;

    // 设置三个按钮的位置，使它们显示在 NPC 上方
    button1->setPosition(finalSpriteScreenPos.x - 60, finalSpriteScreenPos.y + 80);
    button2->setPosition(finalSpriteScreenPos.x, finalSpriteScreenPos.y + 80);
    exitButton->setPosition(finalSpriteScreenPos.x + 60, finalSpriteScreenPos.y + 80);

    // 设置菜单的摄像头掩码，确保由 USER1 相机渲染
    interactionMenu->setCameraMask((unsigned short)CameraFlag::USER1);
    button1->setCameraMask((unsigned short)CameraFlag::USER1);
    button2->setCameraMask((unsigned short)CameraFlag::USER1);
    exitButton->setCameraMask((unsigned short)CameraFlag::USER1);
    exitLabel->setCameraMask((unsigned short)CameraFlag::USER1);

    // 将菜单添加到 NPC 节点中
    this->addChild(interactionMenu);
}

void NPC::exitButtonCallback(Ref* sender)
{
    // 移除菜单
    if (interactionMenu != nullptr)
    {
        interactionMenu->removeFromParent();
        interactionMenu = nullptr; // 确保下次不会再移除已经移除的菜单
    }
}

// 第一个图片按钮的回调函数
void NPC::button1Callback(Ref* sender)
{
    CCLOG("第一个图片按钮被点击");
    auto currentScene = Director::getInstance()->getRunningScene();
    auto myScene = dynamic_cast<GameScene*>(currentScene); //类型转换
    auto hero = myScene->getChildByName("hero");
    auto TheHero = dynamic_cast<Hero*>(hero);
    int num = -1;
    if (TheHero->findItemCount("52.png", num) <= 0)
        return;                                        // 买不起
    else {
        TheHero->pickUpItems("32.png");
        TheHero->pickUpItems("52.png", -1);            // 金币减一
    }
}

// 第二个图片按钮的回调函数
void NPC::button2Callback(Ref* sender)
{
    CCLOG("第二个图片按钮被点击");
    auto currentScene = Director::getInstance()->getRunningScene();
    auto myScene = dynamic_cast<GameScene*>(currentScene); //类型转换
    auto hero = myScene->getChildByName("hero");
    auto TheHero = dynamic_cast<Hero*>(hero);
    int num = -1;
    if (TheHero->findItemCount("52.png", num) <= 0)
        return;                                        // 买不起
    else {
        TheHero->pickUpItems("22.png");
        TheHero->pickUpItems("52.png", -1);            // 金币减一
    }
}

void NPC::addAnimate() {
    // 创建动画对象
    auto animation = Animation::create();
    animation->addSpriteFrameWithFile("npc1.png");
    animation->addSpriteFrameWithFile("npc2.png");
    animation->addSpriteFrameWithFile("npc3.png");
    animation->addSpriteFrameWithFile("npc2.png");

    // 设置每帧持续时间为 0.5 秒，总共 3 帧，动画时长为 1.5 秒
    animation->setDelayPerUnit(0.1f);  // 每帧持续 0.5 秒

    // 创建播放动作
    auto animate_once = Animate::create(animation);
    auto animate = RepeatForever::create(animate_once);
    this->runAction(animate);
}