#include "cocos2d.h"
#include "hero.h"
#include "GameScene.h"

#define BlocksFirstGID_ 4
using namespace cocos2d;
extern TMXLayer* blocksLayer;
int checkBlockCollision(float x, float y);
Vec2 getTileCoordForPosition(float x, float y);
extern float mapWidth;
extern float mapHeight;
extern cocos2d::TMXTiledMap* map;

void Hero::addPhy()
{
    PhysicsMaterial nonBounce(1, 0, 1); // ���ᷴ��������ģ��
    auto bodyOfHero = PhysicsBody::createBox(this->getContentSize(), nonBounce); // ���ǵĸ���
    bodyOfHero->setRotationEnable(false); // �������ǵ���ת
    bodyOfHero->setMass(1); // �������ǵ�����
    this->setPhysicsBody(bodyOfHero);
}

void Hero::addKeyboardListener()
{
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event)
        {
            // �����°���
            auto& children = this->getChildren();
            switch (keyCode)
            {
            case EventKeyboard::KeyCode::KEY_A: // �����ƶ�
                moveLeft = true;
                this->setFlippedX(false);  // ����ʱ������
                setHeroAnimation("man2.png", "man3.png", "man4.png", "man5.png");  // �л������ƶ���
                break;
            case EventKeyboard::KeyCode::KEY_D: // �����ƶ�
                moveRight = true;
                this->setFlippedX(true);  // ����ʱ����
                setHeroAnimation("man2.png", "man3.png", "man4.png", "man5.png");  // �л������ƶ���
                break;
            case EventKeyboard::KeyCode::KEY_SPACE: // ��Ծ
                performJump();
                break;
            case EventKeyboard::KeyCode::KEY_ESCAPE: // �л�����/�ڴ�
                // ��ֹ��������Ϊ�������ʧ��Ʒ�����������е���Ʒ���ڵ�һ����λ����
                if (itemInMove)
                    itemInMove->setTexture("0.png");
                if (movingItem->getItemName() != "0.png") {
                    for (int i = 0; i < items.size(); ++i) {
                        if (items.at(i)->getItemName() == "0.png") {
                            // ����Ʒ���ڵ�һ����λ����
                            items.at(i)->coppyItem(movingItem);
                            items.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                            // ����Ķ��˵�һ�е�ͼ�꣬ͬ���޸Ŀڴ��е���Ʒ
                            if (i < 8) {
                                pocketItems.at(i)->coppyItem(movingItem);
                                pocketItems.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                            }
                            break;
                        }
                    }
                    movingItem = MyItem::create("0.png");  // ����ҹرձ���ʱ
                    movingItem->retain();
                }

                for (auto& child : children) {
                    if (child->isVisible()) {
                        child->setVisible(false);
                    }
                    else {
                        child->setVisible(true);
                    }
                }
                break;
            default:
                break;
            }
        };

    keyboardListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        // �����ɿ�����
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_A: // ֹͣ�����ƶ�
            moveLeft = false;
            this->stopAllActions();   // ֹͣ���ж���
            this->setSpriteFrame("man1.png"); // ���þ�ֹ֡
            break;
        case EventKeyboard::KeyCode::KEY_D: // ֹͣ�����ƶ�
            moveRight = false;
            this->stopAllActions();   // ֹͣ���ж���
            this->setSpriteFrame("man1.png"); // ���þ�ֹ֡
            break;
        default:
            break;
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void Hero::addTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        touchStartFrame = Director::getInstance()->getTotalFrames();  // ��ȡ������ʼ��֡��
        touchStartPosition = touch->getLocation();
        return true;
        };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        touchEndFrame = Director::getInstance()->getTotalFrames();  // ��ȡ����������֡��
        float duration = (touchEndFrame - touchStartFrame) * Director::getInstance()->getAnimationInterval();  // �������ʱ��
        Vec2 touchEndPosition = touch->getLocation();

        float validRange = 100.0f;  // ��Ч��Χ����λ������
        float distanceToHero = touchEndPosition.distance(this->getPosition());

        // �������ʱ�䳬����ֵ��������Ч��Χ�ڣ�ɾ�����
        if (duration >= longPressThreshold && distanceToHero <= validRange)
        {
            removeBlockAtPosition(touchEndPosition);
        }
        else if (duration < longPressThreshold && distanceToHero <= validRange)
        {
            addBlockAtPosition(touchEndPosition);
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Hero::applyJump()
{
    //auto mapSize = map->getMapSize();
    //auto tileSize = map->getTileSize();
    //float mapHeight = mapSize.height * tileSize.height;
    /*�����ﳬ���ϱ߽�ʱ������*/
    //if (mapPosition.y >= -mapHeight / 2 + this->getContentSize().height)
    //{
    this->getPhysicsBody()->applyImpulse(Vec2(0, JumpHeight));
    //}
    isJumping = false;  // ֹͣ��Ծ����ʼ��������
}
void Hero::setHeroAnimation(const std::string& frame2, const std::string& frame3, const std::string& frame4, const std::string& frame5)
{
    // ��������֡����
    Vector<SpriteFrame*> frames;
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame2));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame3));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame4));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame5));

    // ��������
    Animation* animation = Animation::createWithSpriteFrames(frames, 0.1f);  // 0.1f Ϊÿ֮֡��ļ��
    Animate* animate = Animate::create(animation);  // ������������

    // ִ�ж���
    this->runAction(RepeatForever::create(animate));  // �ظ�ִ�ж���
}
void Hero::performJump()
{
    // ����ɫ�Ƿ�վ�ڵ����ϣ�ֻ��վ�ڵ����ϲ�����Ծ
    if (!isJumping && (checkBlockCollision(this->getPositionX(), this->getPositionY() - 25) >= BlocksFirstGID_ || checkBlockCollision(this->getPositionX() - 9, this->getPositionY() - 25) >= BlocksFirstGID_ || checkBlockCollision(this->getPositionX() + 9, this->getPositionY() - 25) >= BlocksFirstGID_))
    {
        isJumping = true;               // ����Ϊ��Ծ״̬
    }
}

void Hero::HeroFunc(float delta)
{
    // ��ȡ�ɼ��ߴ��Ӣ�۵�λ�ú�Ӣ�۵Ĵ�С
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 Pos = this->getPosition();
    Size HeroSize = this->getContentSize();

    // ������Ծ�������߼�
    if (isJumping)
    {
        applyJump();
    }

    checkAndFixHeroCollision(delta);

    auto moveR = MoveBy::create(delta, Vec2(3, 0));
    auto moveL = MoveBy::create(delta, Vec2(-3, 0));
    // ���������ƶ��߼�
    if (moveLeft && Pos.x > (visibleSize.width - mapWidth) / 2 + HeroSize.width / 2)
    {
        this->runAction(moveL);
    }
    if (moveRight && Pos.x < (visibleSize.width + mapWidth) / 2 - HeroSize.width / 2)
    {
        this->runAction(moveR);
    }

    // ���µ�ͼλ��
   /*Vec2 tmpPosition = this->getPosition();

    mapPosition.x = mapPosition.x - (tmpPosition.x - visibleSize.width / 2);
    mapPosition.y = mapPosition.y - (tmpPosition.y - visibleSize.height / 2);
    map->setPosition(mapPosition);
    this->setPosition(visibleSize.width / 2, visibleSize.height / 2);*/

    /*// ���¿ɼ���Χ����Ƭ
    RenderManager* renderManager = new RenderManager(map);
    renderManager->updateVisibleTiles(mapPosition, visibleSize);
    delete renderManager;*/

}

void Hero::checkAndFixHeroCollision(float delta)
{
    // ��ȡ��ɫ��ǰ��������ײ��
    Vec2 heroPos = this->getPosition();

    auto moveR = MoveBy::create(delta, Vec2(3, 0));
    auto moveL = MoveBy::create(delta, Vec2(-3, 0));

    // ��ȡ��ɫ��ǰλ�����ҵ���Ƭ����
    int tileGID_left1 = checkBlockCollision(heroPos.x - 8, heroPos.y - 16);
    int tileGID_left2 = checkBlockCollision(heroPos.x - 8, heroPos.y + 12);
    int tileGID_left3 = checkBlockCollision(heroPos.x - 16, heroPos.y + 5);
    int tileGID_right1 = checkBlockCollision(heroPos.x + 8, heroPos.y - 16);
    int tileGID_right2 = checkBlockCollision(heroPos.x + 8, heroPos.y + 12);
    int tileGID_right3 = checkBlockCollision(heroPos.x + 16, heroPos.y + 5);

    // ����������飬��������ɫλ��
    if (tileGID_left1 >= BlocksFirstGID_ || tileGID_left2 >= BlocksFirstGID_ || tileGID_left3 >= BlocksFirstGID_)
    {
        this->runAction(moveR);
    }
    // ����������飬��������ɫλ��
    if (tileGID_right1 >= BlocksFirstGID_ || tileGID_right2 >= BlocksFirstGID_ || tileGID_right3 >= BlocksFirstGID_)
    {
        this->runAction(moveL);
    }
}

// ��ȡ��ǰʱ�䣨�룩
float Hero::getCurrentTime()
{
    // ��ȡ��ǰʱ�������������
    return std::chrono::duration<float>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void Hero::addBlockAtPosition(cocos2d::Vec2 position)
{
    // ����������Χ����Ч�������
    float validRange = 100.0f;  // ��Ч����뾶���ɸ�����Ҫ����
    cocos2d::Vec2 heroPosition = this->getPosition();

    // �����λ���Ƿ�����Ч��Χ��
    if (position.distance(heroPosition) > validRange)
    {
        CCLOG("Invalid click. Click is outside the valid range.");
        return;
    }

    // �����λ��ת��Ϊ��Ƭ����
    cocos2d::Vec2 tileCoord = getTileCoordForPosition(position.x, position.y);

    // ����λ���Ƿ�������Ƭ
    if (blocksLayer && blocksLayer->getTileGIDAt(tileCoord) != 0)
    {
        CCLOG("Tile already exists at (%f, %f).", tileCoord.x, tileCoord.y);
        return;
    }

    // ����һ������� GID����������� GID Ϊ 182��������� TMX �ļ����޸ģ�
    int dirtBlockGID = 4;
    blocksLayer->setTileGID(dirtBlockGID, tileCoord);

    // ��ȡ�����ɵ���Ƭ
    auto newTile = blocksLayer->getTileAt(tileCoord);
    if (newTile)
    {
        // ������Ƭ����������
        cocos2d::PhysicsMaterial infinity_mass(1e10, 0, 1); // ���������������ģ��
        auto bodyOfBlock = cocos2d::PhysicsBody::createBox(map->getTileSize(), infinity_mass);
        bodyOfBlock->setGravityEnable(false); // ��������Ӱ��
        newTile->setPhysicsBody(bodyOfBlock);
    }

    CCLOG("Block added at (%f, %f).", tileCoord.x, tileCoord.y);
}

// ɾ�����ĺ���
void Hero::removeBlockAtPosition(Vec2 position)
{
    Vec2 tileCoord = getTileCoordForPosition(position.x, position.y);
    if (blocksLayer->getTileGIDAt(tileCoord) != 0)
    {
        blocksLayer->removeTileAt(tileCoord);
        CCLOG("Block removed at tile (%f, %f).", tileCoord.x, tileCoord.y);
    }
    else
    {
        CCLOG("No block to remove at (%f, %f).", tileCoord.x, tileCoord.y);
    }
}

void Hero::iniItems()
{
    auto item1 = MyItem::create("20.png");
    items.pushBack(item1);  // ��Ʒ1��ͭ��
    auto poccketItem1 = MyItem::create("20.png");
    pocketItems.pushBack(poccketItem1);

    auto item2 = MyItem::create("30.png");
    items.pushBack(item2);  // ��Ʒ2��ͭ��
    auto poccketItem2 = MyItem::create("30.png");
    pocketItems.pushBack(poccketItem2);

    auto item3 = MyItem::create("10.png", 30);
    items.pushBack(item3);  // ��Ʒ3�����飨30����
    auto poccketItem3 = MyItem::create("10.png", 30);
    pocketItems.pushBack(poccketItem3);

    auto item4 = MyItem::create("12.png", 48);
    items.pushBack(item4);  // ��Ʒ4��ľ�ģ�48����
    auto poccketItem4 = MyItem::create("12.png", 48);
    pocketItems.pushBack(poccketItem4);

    while (items.size() < 32) {
        auto emptyItem = MyItem::create("0.png");
        items.pushBack(emptyItem);  // ���������е���Ʒ����32��ʱ
    }

    while (pocketItems.size() < 8) {
        auto pocketEmptyItem = MyItem::create("0.png");
        pocketItems.pushBack(pocketEmptyItem);  // ���������е���Ʒ����8��ʱ
    }
    movingItem = MyItem::create("0.png");
    movingItem->retain();
    this->calculate();                          // ��ʼ�����Ժϳɵ���Ʒ

    this->checkPocket();    // ��ʾ�ڴ���Ʒ�����������е���Ʒ��
    this->checkBag();       // ��ʾ��Ʒ����������������������Ʒ��
    this->checkProduction();// ��ʾ��������

    // �������¼�����
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = [=](EventMouse* event) {
        if (movingItem->isVisible() && itemInMove) {
            itemInMove->setPosition(event->getLocation());
            auto visibleSize = Director::getInstance()->getVisibleSize();
            itemInMove->setPositionY(0.5 * visibleSize.height - itemInMove->getPosition().y);
            itemInMove->setPositionX(itemInMove->getPosition().x - 0.5 * visibleSize.width);   // ����ת��
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}