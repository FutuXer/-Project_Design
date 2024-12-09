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
    auto map = TMXTiledMap::create("testmap.tmx");
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
    auto blocksLayer = map->getLayer("blocks");
    auto itemsLayer = map->getLayer("items");
    PhysicsMaterial nonBounce(1, 0, 1); //���ᷴ��������ģ��
    for (int x = 0; x < mapSize.width; x++) {
        for (int y = 0; y < mapSize.height; y++) {
            // ��ȡ��Ƭ
            auto tile = blocksLayer->getTileAt(Vec2(x, y));

            // �����Ƭ���ڣ��򴴽�����
            if (tile) {
                // �����������
                auto bodyOfBlocks = PhysicsBody::createBox(tileSize,nonBounce);
                bodyOfBlocks->setGravityEnable(false);   // ��������Ӱ��

                // �����帽�ӵ���Ƭ�ڵ�
                tile->setPhysicsBody(bodyOfBlocks);
            }
        }
    }

    

    // ��������
    auto hero = Sprite::create("hero.png");
    if (!hero)
    {
        CCLOG("Failed to load hero.");
        return false;
    }
    auto bodyOfHero = PhysicsBody::createBox(hero->getContentSize(), nonBounce); //���ǵĸ���
    hero->setPhysicsBody(bodyOfHero);

    // ���ǳ�ʼλ������Ϊ��Ļ����
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    hero->setName("hero"); // ��������
    this->addChild(hero, 1); // ��������ӵ������������ǵ�ͼ

    // ��ʼ����ͼλ��
    auto mapPosition = Vec2((visibleSize.width - mapWidth) / 2, (visibleSize.height - mapHeight) / 2);
    map->setPosition(mapPosition);

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
        /* // �����Ծ�Ƿ������赲��
        //int tileGID = checkBlockCollision(hero->getPositionX(), hero->getPositionY() - currentJumpHeight);
        if (1) // ����������飨GID >= 181��//tileGID >= 181
        {
            currentJumpHeight = 0.0f; // ֹͣ��Ծ
            jumpVelocity = 0.0f;      // ��Ծ�ٶȹ���
            isJumping = false;        // ��Ծ״̬����
        }*/

        /*// ������Ծ�߶�
        currentJumpHeight += jumpVelocity * delta;
        jumpVelocity += jumpAcceleration * delta;

        // ���µ�ͼλ�ã�������Ծ���������

        map->setPosition(mapPosition.x, mapPosition.y - currentJumpHeight);

        // �����Ծ�Ƿ�������ٶ�Ϊ 0 ʱ������Ծ��
        if (jumpVelocity <= 0.0f)
        {
            jumpVelocity = 0.0f;  // ȷ���ٶ�Ϊ 0
            isJumping = false;    // ��Ծ����
            currentJumpHeight = 0.0f; // ������Ծ�߶�
        }*/
        applyJump(delta);
    }
    else
    {
        // ��������
        //applyGravity(delta);
    }

    

    // ���µ�ͼλ��
    //mapPosition.y = std::max(mapPosition.y - currentJumpHeight, -map->getTileSize().height); // ��ֹ��ͼλ�ó����±߽�
    //map->setPosition(mapPosition);

    // ���������ƶ��߼�
    if (moveLeft)
    {
        mapPosition.x += 3; // �����ƶ�
        // ���Ƶ�ͼ���ܳ�����߽�
        if (mapPosition.x >= mapWidth / 2 - 10)
        {
            mapPosition.x = mapWidth / 2 - 10;
        }
        /*if (checkBlockCollision(hero->getPositionX() + 10, hero->getPositionY()) >= 181)
        {
            mapPosition.x = mapPosition.x - 10;
        }*/
    }
    if (moveRight)
    {
        mapPosition.x -= 3; // �����ƶ�
        // ���Ƶ�ͼ���ܳ����ұ߽�
        if (mapPosition.x <= -mapWidth / 2 + 10)
        {
            mapPosition.x = -mapWidth / 2 + 10;
        }
        /*if (checkBlockCollision(hero->getPositionX() - 10, hero->getPositionY()) >= 181)
        {
            mapPosition.x = mapPosition.x + 10;
        }*/
    }

    map->setPosition(mapPosition);
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
    if (!isJumping)
    {
        isJumping = true;               // ����Ϊ��Ծ״̬
        jumpVelocity = 100.0f;          // ������Ծ�ٶ�
        maxJumpHeight = 100.0f;         // ���������Ծ�߶�
        currentJumpHeight = 0.0f;       // ��ʼ����ǰ��Ծ�߶�
    }
}

void GameScene::applyJump(float delta)
{
    currentJumpHeight += jumpVelocity * delta; // �Ժ㶨�ٶ�����

    // ���� mapPosition.y�����ֽ�ɫ����Ļ�м�
    mapPosition.y -= jumpVelocity * delta;

    // ����Ƿ�ﵽ�����Ծ�߶�
    if (currentJumpHeight >= maxJumpHeight)
    {
        isJumping = false;  // ֹͣ��Ծ����ʼ��������
        currentJumpHeight = 0.0f;
    }
}

/*void GameScene::applyGravity(float delta)
{
    // ����ɫ�Ƿ��ڿ�������
    if (checkBlockCollision(hero->getPositionX(), hero->getPositionY() - 15) < 181) // �������û���赲��
    {
        fallSpeed += fallAcceleration * delta; // ������ٶ�
        mapPosition.y -= fallSpeed; // ���µ�ͼλ�ã���ɫʼ�ձ�������Ļ�м�
        //map->setPosition(mapPosition.x, mapPosition.y);
        //hero->setPositionY(hero->getPositionY() + fallSpeed); // �������ǵ�Yλ��
    }
    else
    {   
        // �����ɫ����ˣ�ֹͣ����
        fallSpeed = 0.0f;
    }
}*/

/*int GameScene::checkBlockCollision(float x, float y)
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
}*/

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




    