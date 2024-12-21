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
    auto hero = myScene->getChildByName("hero");
    auto TheHero = dynamic_cast<Hero*>(hero);
    TheHero->pickUpItems("50.png", 5);                   // ��ʷ��ķ���ö5ͭ��
     
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
    auto hero = myScene->getChildByName("hero");
    auto TheHero = dynamic_cast<Hero*>(hero);
    TheHero->pickUpItems("50.png", 10);                   // ��ʬ���ö10ͭ��
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
    auto attackRange = Rect(this->getPositionX() - this->getContentSize().width / 2,  // ��߽�
        this->getPositionY() - this->getContentSize().height / 2,  // �±߽�
        this->getContentSize().width,                          // ���
        this->getContentSize().height);                         // �߶�

    // ʹ�����������ѯ
    auto physicsWorld = this->getScene()->getPhysicsWorld();
    physicsWorld->queryRect([this](PhysicsWorld& world, PhysicsShape& shape, void* data) -> bool {
        auto body = shape.getBody();
        if (body->getCategoryBitmask() == 0x01) { // ����Ƿ�������
            auto node = body->getNode();
            auto hero = dynamic_cast<Hero*>(node); // �� Node תΪ Hero ����
            hero->takedamage(5); // ���﹥������
        }
        return true; // ������ѯ
        }, attackRange, NULL);
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
            auto node = body->getNode();
            auto hero = dynamic_cast<Hero*>(node); // �� Node תΪ Hero ����
            hero->takedamage(10); // ���﹥������
        }
        return true; // ������ѯ
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
                }, 5.0f, "HideHealthBar"); //5�벻�ܹ���������Ѫ��
            auto highlight = TintTo::create(0.2f, 255, 0, 0); // ��ɫ����
            auto reset = TintTo::create(0.2f, 255, 255, 255); // �ָ�ԭɫ
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
                }, 5.0f, "HideHealthBar"); //5�벻�ܹ���������Ѫ��
            auto highlight = TintTo::create(0.2f, 255, 0, 0); // ��ɫ����
            auto reset = TintTo::create(0.2f, 255, 255, 255); // �ָ�ԭɫ
            auto sequence = Sequence::create(highlight, reset, nullptr);
            this->runAction(sequence);
        }
    }
}

//����д����npc���

void NPC::npc_move(float dt) {
    Vec2 Pos;
    auto currentScene = Director::getInstance()->getRunningScene();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    auto Hero = currentScene->getChildByName("hero"); // ��ȡ���ǵ�λ��

    if (Hero == NULL) {
        return;
    }

    auto Pos_H = Hero->getPosition();
    Pos = this->getPosition();

    // �������
    float distance = Pos.distance(Pos_H);

    auto movebyr = MoveBy::create(dt, Vec2(75 * dt, 0)); // ������
    auto movebyl = MoveBy::create(dt, Vec2(-75 * dt, 0)); // ������

    // �ж� NPC �Ƿ�Ӧ�������ƶ�
    if (Pos.x < Pos_H.x && Pos.x < (visibleSize.width + mapWidth) / 2 - this->getContentSize().width / 2 && Pos_H.x - Pos.x > 100) {
        this->setFlippedX(true);  // �ָ��������������Ҳࣩ
        this->runAction(movebyr);  // ִ�������ƶ��Ķ���
    }
    // �ж� NPC �Ƿ�Ӧ�������ƶ�
    else if (Pos.x >= Pos_H.x && Pos.x > (visibleSize.width - mapWidth) / 2 + this->getContentSize().width / 2 && Pos.x - Pos_H.x > 100) {
        this->setFlippedX(false);  // ��ת����������ࣩ
        this->runAction(movebyl);  // ִ�������ƶ��Ķ���
    }
    // ��� NPC û���ƶ�������ͣ
    else {
        this->stopActionByTag(1);  // ֹͣ���ҵĶ���
        this->stopActionByTag(2);  // ֹͣ����Ķ���
    }

    // �ж������ǵľ��룬ֹͣ��ʼ����
    if (distance < 100 && isAnimationPlaying) {  // ����С��100ʱֹͣ����
        stopAnimate();  // ֹͣ����
        isAnimationPlaying = 0;
    }
    else if (distance >= 100 && !isAnimationPlaying) {  // �������100ʱ��ʼ����
        addAnimate();  // ��������
        isAnimationPlaying = 1;
    }
}

// ֹͣ�����ĺ���
void NPC::stopAnimate() {
    this->stopAllActions();
    isAnimationPlaying = 0;
}

void NPC::addPhy() {
    auto npcbody = PhysicsBody::createBox(Size(this->getContentSize()), PhysicsMaterial(1, 0, 1));
    npcbody->setMass(1);
    npcbody->setRotationEnable(false); //������ת
    /*��*/
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
    // ���忿����Χ
    auto attackRange = Rect(this->getPositionX() - this->getContentSize().width / 2,  // ��߽�
        this->getPositionY() - this->getContentSize().height / 2,  // �±߽�
        this->getContentSize().width,                          // ���
        this->getContentSize().height);                         // �߶�

    // ʹ�����������ѯ
    auto physicsWorld = this->getScene()->getPhysicsWorld();
    physicsWorld->queryRect([this](PhysicsWorld& world, PhysicsShape& shape, void* data) -> bool {
        auto body = shape.getBody();
        if (body->getCategoryBitmask() == 0x01) { // ����Ƿ�������
            auto hero = body->getNode();

            /*
            this->action(); // ���﹥������
            return false;         // ֹͣ��ѯ���Ѿ��ҵ�����
            */
        }
        return true; // ������ѯ
        }, attackRange, NULL);
}

void NPC::action() {
    auto currentScene = Director::getInstance()->getRunningScene();
    auto temp = currentScene->getChildByName("hero"); //ȡ�����ǵ�λ��
    auto hero = dynamic_cast<Hero*>(temp); //����ת��
    if (hero == NULL)
        return;
    hero->takedamage(10);
}


void NPC::addTouchListener()
{
    // ����һ������¼�������
    auto listener = EventListenerMouse::create();

    // ��������¼�������
    listener->onMouseDown = [this](Event* event)
        {
            auto mouseEvent = dynamic_cast<EventMouse*>(event);
            if (mouseEvent && mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
            {
                auto location = mouseEvent->getLocationInView(); // ��ȡ���λ�ã��������ͼ��

                // ��ӡ�����λ�� (��Ļ����)
                CCLOG("Mouse right-clicked at: (%.2f, %.2f)", location.x, location.y);

                // ��ȡ�������Ļ���꣨����ڴ������½ǵ����꣩
                auto spritePos = this->getPosition();

                // ת������ĳ�������Ϊ��Ļ����
                auto screenSpritePos = this->getParent()->convertToWorldSpace(spritePos);

                // ��ȡ��ǰ����
                auto scene = Director::getInstance()->getRunningScene();

                // �ӳ����л�ȡ����ڵ�
                auto camera = scene->getChildByName("Camera");  // �����������ڵ������� "camera"

                // ��ȡ��������꣨�������꣩
                auto cameraPos = camera->getPosition();

                // ��ȡ��ǰ�����Ŀɼ������С
                Size visibleSize = Director::getInstance()->getVisibleSize();

                // ��ȡ��ǰ������ԭ��λ�ã����½ǵ����꣩
                Vec2 visibleOrigin = Director::getInstance()->getVisibleOrigin();

                // ���㾫���������Ļ���꣨���������ƫ�ƣ�
                Vec2 finalSpriteScreenPos = screenSpritePos - cameraPos + visibleSize / 2;

                CCLOG("Sprite final screen position1: (%.2f, %.2f)", finalSpriteScreenPos.x, finalSpriteScreenPos.y);

                // ��ȡ���ǣ�Hero��
                auto currentScene = Director::getInstance()->getRunningScene();
                auto hero = currentScene->getChildByName("hero");

                if (hero)
                {
                    // ��ȡNPC�ĳ�������
                    auto spritePos = this->getPosition();

                    // ��ȡ���ǵĳ�������
                    auto heroPos = hero->getPosition();

                    // ���� NPC ������֮��ĳ����������
                    float deltaX = std::abs(spritePos.x - heroPos.x);
                    float deltaY = std::abs(spritePos.y - heroPos.y);

                    // ���ˮƽ����ֱ������붼С�� 100 ���أ�����ʾ�����˵�
                    if (deltaX < 100 && deltaY < 100)
                    {
                        showInteractionMenu(); // ��ʾ�����˵�
                        CCLOG("Interaction Menu Displayed");
                    }
                    else
                    {
                        CCLOG("NPC and Hero are too far apart to interact.");
                    }
                }
            }
        };

    // ����¼����������¼��ַ���
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void NPC::showInteractionMenu()
{
    // ����˵��Ѵ��ڣ����Ƴ���
    if (interactionMenu != nullptr)
    {
        return;
    }

    // ������һ��ͼƬ�Ͱ�ť
    auto button1 = MenuItemImage::create(
        "32.png", // ����״̬��ͼƬ
        "32.png", // ���״̬��ͼƬ
        CC_CALLBACK_1(NPC::button1Callback, this)   //  ���򱦽���1��ң�
    );

    // �����ڶ���ͼƬ�Ͱ�ť
    auto button2 = MenuItemImage::create(
        "22.png",
        "22.png",
        CC_CALLBACK_1(NPC::button2Callback, this)   // ����ǿ�����ӣ�1��ң�
    );

    // �����˳���ť
    auto exitLabel = Label::createWithTTF(u8"�˳�", "fonts/jiantisongti.ttf", 24);
    auto exitButton = MenuItemLabel::create(
        exitLabel,
        CC_CALLBACK_1(NPC::exitButtonCallback, this)
    );

    // �����˵�������ť��ӵ��˵���
    interactionMenu = Menu::create(button1, button2, exitButton, nullptr);

    // ��ȡNPC�ĵ�ǰλ��
    auto spritePos = this->getPosition();

    // ��ȡ�����е����λ�ã����ڼ��������Ļ��λ��
    auto scene = Director::getInstance()->getRunningScene();
    auto camera = scene->getChildByName("Camera");
    auto cameraPos = camera->getPosition();

    // ��ȡ��ǰ�����Ŀɼ������С
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // ���� NPC ����Ļ�ϵ�λ�ã��������ƫ�ƣ�
    Vec2 finalSpriteScreenPos = spritePos - cameraPos - visibleSize / 2;

    // ����������ť��λ�ã�ʹ������ʾ�� NPC �Ϸ�
    button1->setPosition(finalSpriteScreenPos.x - 60, finalSpriteScreenPos.y + 80);
    button2->setPosition(finalSpriteScreenPos.x, finalSpriteScreenPos.y + 80);
    exitButton->setPosition(finalSpriteScreenPos.x + 60, finalSpriteScreenPos.y + 80);

    // ���ò˵�������ͷ���룬ȷ���� USER1 �����Ⱦ
    interactionMenu->setCameraMask((unsigned short)CameraFlag::USER1);
    button1->setCameraMask((unsigned short)CameraFlag::USER1);
    button2->setCameraMask((unsigned short)CameraFlag::USER1);
    exitButton->setCameraMask((unsigned short)CameraFlag::USER1);
    exitLabel->setCameraMask((unsigned short)CameraFlag::USER1);

    // ���˵���ӵ� NPC �ڵ���
    this->addChild(interactionMenu);
}

void NPC::exitButtonCallback(Ref* sender)
{
    // �Ƴ��˵�
    if (interactionMenu != nullptr)
    {
        interactionMenu->removeFromParent();
        interactionMenu = nullptr; // ȷ���´β������Ƴ��Ѿ��Ƴ��Ĳ˵�
    }
}

// ��һ��ͼƬ��ť�Ļص�����
void NPC::button1Callback(Ref* sender)
{
    CCLOG("��һ��ͼƬ��ť�����");
    auto currentScene = Director::getInstance()->getRunningScene();
    auto myScene = dynamic_cast<GameScene*>(currentScene); //����ת��
    auto hero = myScene->getChildByName("hero");
    auto TheHero = dynamic_cast<Hero*>(hero);
    int num = -1;
    if (TheHero->findItemCount("52.png", num) <= 0)
        return;                                        // ����
    else {
        TheHero->pickUpItems("32.png");
        TheHero->pickUpItems("52.png", -1);            // ��Ҽ�һ
    }
}

// �ڶ���ͼƬ��ť�Ļص�����
void NPC::button2Callback(Ref* sender)
{
    CCLOG("�ڶ���ͼƬ��ť�����");
    auto currentScene = Director::getInstance()->getRunningScene();
    auto myScene = dynamic_cast<GameScene*>(currentScene); //����ת��
    auto hero = myScene->getChildByName("hero");
    auto TheHero = dynamic_cast<Hero*>(hero);
    int num = -1;
    if (TheHero->findItemCount("52.png", num) <= 0)
        return;                                        // ����
    else {
        TheHero->pickUpItems("22.png");
        TheHero->pickUpItems("52.png", -1);            // ��Ҽ�һ
    }
}

void NPC::addAnimate() {
    // ������������
    auto animation = Animation::create();
    animation->addSpriteFrameWithFile("npc1.png");
    animation->addSpriteFrameWithFile("npc2.png");
    animation->addSpriteFrameWithFile("npc3.png");
    animation->addSpriteFrameWithFile("npc2.png");

    // ����ÿ֡����ʱ��Ϊ 0.5 �룬�ܹ� 3 ֡������ʱ��Ϊ 1.5 ��
    animation->setDelayPerUnit(0.1f);  // ÿ֡���� 0.5 ��

    // �������Ŷ���
    auto animate_once = Animate::create(animation);
    auto animate = RepeatForever::create(animate_once);
    this->runAction(animate);
}