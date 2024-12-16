#include "GameScene.h"
#include <chrono>

USING_NS_CC;

const float gravity = -3.0f; // �������ٶ�
const float maxFallSpeed = -200.0f; // ��������ٶ�
const float MAX_VELOCITY = 150.0f;  // ���ˮƽ�ٶ�

Scene* GameScene::createScene()
{
    return GameScene::create();
}

bool GameScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    initWithPhysics(); // �����������
    this->getPhysicsWorld()->setGravity(Vec2(0, -980)); // ��������
    this->getPhysicsWorld()->setAutoStep(false); // �����Զ�����

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ������ͼ
    map = TMXTiledMap::create("testmap.tmx");
    if (!map)
    {
        CCLOG("Failed to load map.");
        return false;
    }
    map->setAnchorPoint(Vec2(0, 0));
    map->setScale(1.0f);
    map->setName("map"); // ��������
    this->addChild(map, 0);

    // ��ȡ��ͼ�ߴ�
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    // ����ͼ��
    blocksLayer = map->getLayer("blocks");
    itemsLayer = map->getLayer("items");
    PhysicsMaterial nonBounce(1, 0, 1); // ���ᷴ��������ģ��
    PhysicsMaterial infinity_mass(1e10, 0, 1); // ���������������ģ��
    for (int x = 0; x < mapSize.width; x++) {
        for (int y = 0; y < mapSize.height; y++) {
            // ��ȡ��Ƭ
            auto tile = blocksLayer->getTileAt(Vec2(x, y));

            // �����Ƭ���ڣ��򴴽�����
            if (tile) {
                // �����������
                auto bodyOfBlocks = PhysicsBody::createBox(tileSize, infinity_mass);
                bodyOfBlocks->setGravityEnable(false);   // ��������Ӱ��
                // �����帽�ӵ���Ƭ�ڵ�
                tile->setPhysicsBody(bodyOfBlocks);
            }
        }
    }

    // ��������
    hero = Sprite::create("hero.png");
    if (!hero)
    {
        CCLOG("Failed to load hero.");
        return false;
    }
    auto bodyOfHero = PhysicsBody::createBox(hero->getContentSize(), nonBounce); // ���ǵĸ���
    bodyOfHero->setRotationEnable(false); // �������ǵ���ת
    bodyOfHero->setMass(1); // �������ǵ�����
    hero->setPhysicsBody(bodyOfHero);
    // ���ǳ�ʼλ������Ϊ��Ļ����
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    hero->setName("hero"); // ��������
    this->addChild(hero, 1); // ��������ӵ������������ǵ�ͼ

    // ��ʼ����ͼλ��
    mapPosition = Vec2((visibleSize.width - mapWidth) / 2, (visibleSize.height - mapHeight) / 2);
    map->setPosition(mapPosition);

    // ��Ӽ��̲���
    addKeyboardListener(hero);
    addTouchListener();

    // �����������
    this->schedule([=](float deltaTime) {
        this->updatePhysicsWorld(deltaTime);
        }, "update_physics_world_key");
    this->scheduleUpdate(); // ���ȸ��·���

    /*���¶��ǳ�ʼ����Ʒ���Ĳ���*/
    auto item1 = MyItem::create("20.png");
    items.pushBack(item1);  // ��Ʒ1��ͭ��
    auto poccketItem1 = MyItem::create("20.png");
    pocketItems.pushBack(poccketItem1);

    auto item2 = MyItem::create("30.png");
    items.pushBack(item2);  // ��Ʒ2��ͭ��
    auto poccketItem2 = MyItem::create("30.png");
    pocketItems.pushBack(poccketItem2);

    auto item3 = MyItem::create("10.png", 30);
    items.pushBack(item3);  // ��Ʒ3��ͭ��30����
    auto poccketItem3 = MyItem::create("10.png",30);
    pocketItems.pushBack(poccketItem3);

    auto item4 = MyItem::create("12.png", 48);
    items.pushBack(item4);  // ��Ʒ4��ľ�ģ�48����
    auto poccketItem4 = MyItem::create("12.png",48);
    pocketItems.pushBack(poccketItem4);

    auto item5 = MyItem::create("13.png", 48);
    items.pushBack(item5);  // ��Ʒ5��ͭ��48����
    auto poccketItem5 = MyItem::create("13.png", 48);
    pocketItems.pushBack(poccketItem5);

    auto item6 = MyItem::create("14.png", 48);
    items.pushBack(item6);  // ��Ʒ6������48����
    auto poccketItem6 = MyItem::create("14.png", 48);
    pocketItems.pushBack(poccketItem6);

    while (items.size() < 32) {
        auto emptyItem = MyItem::create("0.png");
        items.pushBack(emptyItem);  // ���������е���Ʒ����32��ʱ
    }

    while (pocketItems.size() < 8) {
        auto pocketEmptyItem = MyItem::create("0.png");
        pocketItems.pushBack(pocketEmptyItem);  // ���������е���Ʒ����8��ʱ
    }
    movingItem = MyItem::create("0.png");
    movingItem->retain();                       // ��ʼ�������ƶ�����Ʒ
    this->calculate();                          // ��ʼ�����Ժϳɵ���Ʒ

    this->checkPocket();    // ��ʾ�ڴ���Ʒ�����������е���Ʒ��
    this->checkBag();       // ��ʾ��Ʒ����������������������Ʒ��
    this->checkProduction();// ��ʾ��������

    // �������¼�����
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = [=](EventMouse* event) {
        if (movingItem->isVisible() && itemInMove) {
            itemInMove->setPosition(event->getLocation());
            itemInMove->setPositionY(Director::getInstance()->getVisibleSize().height - itemInMove->getPosition().y);
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    return true;
}

void GameScene::update(float delta)
{
    // ��ȡ��ͼ������
    if (!map || !hero)
    {
        CCLOG("Map or Hero not found.");
        return;
    }

    // ��ȡ�ɼ��ߴ�͵�ͼ�ߴ�
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    // ������Ծ�������߼�
    if (isJumping)
    {
        applyJump(delta);
    }

    checkAndFixHeroCollision();

    // ���������ƶ��߼�
    if (moveLeft)
    {
        mapPosition.x += 3; // �����ƶ�      
        checkAndFixHeroCollision();
        // ���Ƶ�ͼ���ܳ�����߽�
        if (mapPosition.x >= visibleSize.width / 2 - 11)
        {
            mapPosition.x = visibleSize.width / 2 - 11;
        }
    }
    if (moveRight)
    {
        mapPosition.x -= 3; // �����ƶ�
        // ���Ƶ�ͼ���ܳ����ұ߽�
        if (mapPosition.x <= visibleSize.width / 2 - mapWidth + 11)
        {
            mapPosition.x = visibleSize.width / 2 - mapWidth + 11;
        }
        checkAndFixHeroCollision();
    }

    // ���µ�ͼλ��
    Vec2 tmpPosition = hero->getPosition();

    mapPosition.x = mapPosition.x - (tmpPosition.x - visibleSize.width / 2);
    mapPosition.y = mapPosition.y - (tmpPosition.y - visibleSize.height / 2);
    map->setPosition(mapPosition);
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
}

void GameScene::checkAndFixHeroCollision()
{
    // ��ȡ��ɫ��ǰ��������ײ��
    Vec2 heroPos = hero->getPosition();

    // ��ȡ��ɫ��ǰλ�����ҵ���Ƭ����
    int tileGID_left = checkBlockCollision(heroPos.x - 10, heroPos.y);
    int tileGID_right = checkBlockCollision(heroPos.x + 10, heroPos.y);

    // ����������飬��������ɫλ��
    if (tileGID_left >= 181)
    {
        mapPosition.x -= 3;
    }
    // ����������飬��������ɫλ��
    if (tileGID_right >= 181)
    {
        mapPosition.x += 3;
    }
}


void GameScene::addKeyboardListener(Sprite* hero)
{
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        // �����°���
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_A: // �����ƶ�
            moveLeft = true;
            break;
        case EventKeyboard::KeyCode::KEY_D: // �����ƶ�
            moveRight = true;
            break;
        case EventKeyboard::KeyCode::KEY_SPACE: // ��Ծ
            performJump(hero);
            break;
        case EventKeyboard::KeyCode::KEY_ESCAPE: // �л�����/�ڴ�
            if (bagLayer->isVisible()) {
                if (itemInMove) {
                    itemInMove->setVisible(false);      // ������ȥ���
                    itemInMove = nullptr;
                }
                if (movingItem->getItemName() != "0.png")
                    movingItem->setVisible(false);      // ���������е���Ʒ��λ
                ClickToChangePosition(nullptr, -1);
                bagLayer->setVisible(false);
                pocketLayer->setVisible(true);
            }
            else {
                pocketLayer->setVisible(false);
                bagLayer->setVisible(true);
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
            break;
        case EventKeyboard::KeyCode::KEY_D: // ֹͣ�����ƶ�
            moveRight = false;
            break;
        default:
            break;
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void GameScene::performJump(Sprite* hero)
{
    // ����ɫ�Ƿ�վ�ڵ����ϣ�ֻ��վ�ڵ����ϲ�����Ծ
    if (!isJumping && checkBlockCollision(hero->getPositionX(), hero->getPositionY() - 15) >= 181)
    {
        isJumping = true;               // ����Ϊ��Ծ״̬
        jumpVelocity = 100.0f;          // ������Ծ�ٶ�
        maxJumpHeight = 100.0f;         // ���������Ծ�߶�
        currentJumpHeight = 0.0f;       // ��ʼ����ǰ��Ծ�߶�
    }
}

void GameScene::applyJump(int delta)
{
    hero->getPhysicsBody()->applyImpulse(Vec2(0, 300));
    isJumping = false;  // ֹͣ��Ծ����ʼ��������
}

int GameScene::checkBlockCollision(float x, float y)
{
    // ��ȡ��Ƭ����
    Vec2 tileCoord = getTileCoordForPosition(x, y);

    // ���ȼ�� blocksLayer �ϵ���Ƭ GID
    int blockGID = blocksLayer->getTileGIDAt(tileCoord);

    if (blockGID >= 181) // ����Ƿ��飨GID >= 181��
    {
        return blockGID; // ���ط���� GID����ʾ�����ϰ���
    }

    return 0; // û����ײ
}

Vec2 GameScene::getTileCoordForPosition(float x, float y)
{
    // ��ȡ��ͼ��Tile��С
    Size tileSize = map->getTileSize();
    Vec2 mapOrigin = map->getPosition();

    // ����������ת��Ϊ��ͼ����
    float mapX = x - mapOrigin.x;
    float mapY = y - mapOrigin.y;

    // ������Ƭ����
    int tileX = mapX / tileSize.width;
    int tileY = (map->getMapSize().height * tileSize.height - mapY) / tileSize.height;

    return Vec2(tileX, tileY);
}

void GameScene::updatePhysicsWorld(float delta)
{
    // ʹ�ù̶�ʱ�䲽�������������
    for (int i = 0; i < 3; ++i)
    {
        this->getPhysicsWorld()->step(1 / 180.0f);  // ÿ�������� 1/180 ��
    }
}


void GameScene::addTouchListener()
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
        float distanceToHero = touchEndPosition.distance(hero->getPosition());

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

// ��ȡ��ǰʱ�䣨�룩
float GameScene::getCurrentTime()
{
    // ��ȡ��ǰʱ�������������
    return std::chrono::duration<float>(std::chrono::system_clock::now().time_since_epoch()).count();
}


void GameScene::addBlockAtPosition(cocos2d::Vec2 position)
{
    // ����������Χ����Ч�������
    float validRange = 100.0f;  // ��Ч����뾶���ɸ�����Ҫ����
    cocos2d::Vec2 heroPosition = hero->getPosition();

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
    int dirtBlockGID = 182;
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
void GameScene::removeBlockAtPosition(Vec2 position)
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