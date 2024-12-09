#include "GameScene.h"

USING_NS_CC;

const float gravity = -3.0f; // �������ٶ�
const float maxFallSpeed = -200.0f; // ��������ٶ�
bool overland = 0;

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

    initWithPhysics();//�����������
    this->getPhysicsWorld()->setGravity(Vec2(0, -980)); //��������

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
    PhysicsMaterial nonBounce(1, 0, 1); //���ᷴ��������ģ��
    PhysicsMaterial infinity_mass(1e10, 0, 1); //���������������ģ��
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
    auto bodyOfHero = PhysicsBody::createBox(hero->getContentSize(), nonBounce); //���ǵĸ���
    bodyOfHero->setRotationEnable(false);//�������ǵ���ת
    bodyOfHero->setMass(1); //�������ǵ�����
    hero->setPhysicsBody(bodyOfHero);
    // ���ǳ�ʼλ������Ϊ��Ļ����
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    hero->setName("hero"); // ��������
    this->addChild(hero, 1); // ��������ӵ������������ǵ�ͼ

    // ��ʼ����ͼλ��
    mapPosition = Vec2((visibleSize.width - mapWidth) / 2, (visibleSize.height - mapHeight) / 2);
    map->setPosition(mapPosition);

    //�����¿���ǽ
    //auto airWall_down = Node::create();
    //auto wallBody_down = PhysicsBody::createBox(Size(visibleSize.width, 100), PhysicsMaterial(0.0f, 1.0f, 1.0f));
    //wallBody_down->setDynamic(false); // ��̬���壬�����ƶ�
    //airWall_down->setPhysicsBody(wallBody_down);
    //airWall_down->setPosition(Vec2(origin.x + visibleSize.width / 2, -50));
    //this->addChild(airWall_down);

    //�����Ͽ���ǽ
    //auto airWall_up = Node::create();
    //auto wallBody_up = PhysicsBody::createBox(Size(visibleSize.width, 100), PhysicsMaterial(0.0f, 1.0f, 1.0f));
    //wallBody_up->setDynamic(false); // ��̬���壬�����ƶ�
    //airWall_up->setPhysicsBody(wallBody_up);
    //airWall_up->setPosition(Vec2(origin.x + visibleSize.width / 2, visibleSize.height - 10));

    // ��ӵ�������
    //this->addChild(airWall_up);

    // ��Ӽ��̲���
    addKeyboardListener(hero);

    this->scheduleUpdate(); // ���ȸ��·���

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

    // ���������ƶ��߼�
    if (moveLeft)
    {
        mapPosition.x += 3; // �����ƶ�
        // ���Ƶ�ͼ���ܳ�����߽�
        if (mapPosition.x >= mapWidth / 2 - 10)
        {
            mapPosition.x = mapWidth / 2 - 10;
        }
    }
    if (moveRight)
    {
        mapPosition.x -= 3; // �����ƶ�
        // ���Ƶ�ͼ���ܳ����ұ߽�
        if (mapPosition.x <= -mapWidth / 2 + 10)
        {
            mapPosition.x = -mapWidth / 2 + 10;
        }
    }

    Vec2 tmpPosition = hero->getPosition();
    mapPosition.x = mapPosition.x - (tmpPosition.x - mapWidth / 2);
    mapPosition.y = mapPosition.y - (tmpPosition.y - mapHeight / 2);
    map->setPosition(mapPosition);
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
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
    hero->getPhysicsBody()->applyImpulse(Vec2(0, 500));
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




    