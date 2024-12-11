#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
#define JumpHeight 500 //����������Ծ�ĳ�����С
#define BlocksFirstGID 10409
#define End 10673

/*class RenderManager
{
private:
    TMXTiledMap* _map;
    Size _tileSize;
    Size _mapSize;
    Rect _lastVisibleRect; // ��¼��һ�εĿɼ���Χ

public:
    RenderManager(TMXTiledMap* map)
        : _map(map),
        _tileSize(map->getTileSize()),
        _mapSize(map->getMapSize()),
        _lastVisibleRect(Rect::ZERO) {}

    void updateVisibleTiles(const Vec2& mapPosition, const Size& visibleSize)
    {
        float minX = -mapPosition.x;
        float maxX = minX + visibleSize.width;
        float minY = -mapPosition.y;
        float maxY = minY + visibleSize.height;

        // �����¿ɼ���Χ
        Rect visibleRect(minX, minY, maxX - minX, maxY - minY);

        if (visibleRect.equals(_lastVisibleRect))
        {
            // �����Χδ�仯����������
            return;
        }
        _lastVisibleRect = visibleRect;

        int minTileX = std::max(0, static_cast<int>(minX / _tileSize.width));
        int maxTileX = std::min(static_cast<int>(_mapSize.width) - 1, static_cast<int>(maxX / _tileSize.width));
        int minTileY = std::max(0, static_cast<int>((_mapSize.height * _tileSize.height - maxY) / _tileSize.height));
        int maxTileY = std::min(static_cast<int>(_mapSize.height) - 1, static_cast<int>((_mapSize.height * _tileSize.height - minY) / _tileSize.height));

        for (auto& child : _map->getChildren())
        {
            auto layer = dynamic_cast<TMXLayer*>(child);
            if (!layer) continue;

            for (int x = 0; x < _mapSize.width; ++x)
            {
                for (int y = 0; y < _mapSize.height; ++y)
                {
                    auto tile = layer->getTileAt(Vec2(x, y));
                    if (!tile) continue;

                    bool isVisible = (x >= minTileX && x <= maxTileX && y >= minTileY && y <= maxTileY);
                    tile->setVisible(isVisible);
                }
            }
        }
    }
};*/
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

    // ���ؾ���֡
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("man.plist");  // ���� plist �ļ�

    auto textureCache = Director::getInstance()->getTextureCache();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ������ͼ
    map = TMXTiledMap::create("map0.tmx");

    // ��ȡ��ͼ�ߴ�
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    if (!map)
    {
        CCLOG("Failed to load map.");
        return false;
    }
    map->setAnchorPoint(Vec2(0, 0));
    map->setScale(1.0f);
    map->setName("map"); // ��������
    this->addChild(map, 0);


    // ����ͼ��
    blocksLayer = map->getLayer("blocks");
    itemsLayer = map->getLayer("items");
    PhysicsMaterial nonBounce(1, 0, 1); // ���ᷴ��������ģ��
    PhysicsMaterial infinity_mass(1e10, 0, 1); // ���������������ģ��
    for (int x = 0; x < mapSize.width; x++)
    {
        for (int y = 0; y < mapSize.height; y++)
        {
            // ��ȡ��Ƭ
            auto tile = blocksLayer->getTileAt(Vec2(x, y));

            // �����Ƭ���ڣ��򴴽�����
            if (tile)
            {
                // �����������
                auto bodyOfBlocks = PhysicsBody::createBox(tileSize, infinity_mass);
                bodyOfBlocks->setGravityEnable(false);   // ��������Ӱ��
                // �����帽�ӵ���Ƭ�ڵ�
                tile->setPhysicsBody(bodyOfBlocks);
            }
        }
    }

    // ��������
    hero = Sprite::create("man1.png");
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

    // �����������
    this->schedule([=](float deltaTime) {
        this->updatePhysicsWorld(deltaTime);
        }, "update_physics_world_key");

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

    checkAndFixHeroCollision();

    // ���������ƶ��߼�
    if (moveLeft)
    {
        mapPosition.x += 3; // �����ƶ�      
        // ���Ƶ�ͼ���ܳ�����߽�
        if (mapPosition.x >= visibleSize.width / 2 - 12)
        {
            mapPosition.x = visibleSize.width / 2 - 12;
        }
    }
    if (moveRight)
    {
        mapPosition.x -= 3; // �����ƶ�
        // ���Ƶ�ͼ���ܳ����ұ߽�
        if (mapPosition.x <= visibleSize.width / 2 - mapWidth + 12)
        {
            mapPosition.x = visibleSize.width / 2 - mapWidth + 12;           
        }
    }

    /*��ֹ���ﳬ���ϱ߽�*/
    if (mapPosition.y < -mapHeight / 2 + hero->getContentSize().height)
        hero->getPhysicsBody()->setVelocity(Vec2(0, -70));

    // ���µ�ͼλ��
    Vec2 tmpPosition = hero->getPosition();

    mapPosition.x = mapPosition.x - (tmpPosition.x - visibleSize.width / 2);
    mapPosition.y = mapPosition.y - (tmpPosition.y - visibleSize.height / 2);
    map->setPosition(mapPosition);
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);

    /*// ���¿ɼ���Χ����Ƭ
    RenderManager* renderManager = new RenderManager(map);
    renderManager->updateVisibleTiles(mapPosition, visibleSize);
    delete renderManager;*/
    
}

void GameScene::checkAndFixHeroCollision()
{
    // ��ȡ��ɫ��ǰ��������ײ��
    Vec2 heroPos = hero->getPosition();

    // ��ȡ��ɫ��ǰλ�����ҵ���Ƭ����
    int tileGID_left1 = checkBlockCollision(heroPos.x - 8, heroPos.y - 7);
    int tileGID_left2 = checkBlockCollision(heroPos.x - 8, heroPos.y + 7);
    int tileGID_right1 = checkBlockCollision(heroPos.x + 8, heroPos.y - 7);
    int tileGID_right2 = checkBlockCollision(heroPos.x + 8, heroPos.y + 7);

    // ����������飬��������ɫλ��
    if (tileGID_left1 >= BlocksFirstGID || tileGID_left2 >= BlocksFirstGID)
    {
        mapPosition.x -= 3;
    }
    // ����������飬��������ɫλ��
    if (tileGID_right1 >= BlocksFirstGID || tileGID_right2 >= BlocksFirstGID)
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
            hero->setFlippedX(false);  // ����ʱ������
            setHeroAnimation("man2.png", "man3.png", "man4.png", "man5.png");  // �л������ƶ���
            break;
        case EventKeyboard::KeyCode::KEY_D: // �����ƶ�
            moveRight = true;
            hero->setFlippedX(true);  // ����ʱ����
            setHeroAnimation("man2.png", "man3.png", "man4.png", "man5.png");  // �л������ƶ���
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
            hero->stopAllActions();   // ֹͣ���ж���
            hero->setSpriteFrame("man1.png"); // ���þ�ֹ֡
            break;
        case EventKeyboard::KeyCode::KEY_D: // ֹͣ�����ƶ�
            moveRight = false;
            hero->stopAllActions();   // ֹͣ���ж���
            hero->setSpriteFrame("man1.png"); // ���þ�ֹ֡
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
    if (!isJumping && checkBlockCollision(hero->getPositionX(), hero->getPositionY() - 15) >= BlocksFirstGID)
    {
        isJumping = true;               // ����Ϊ��Ծ״̬
        /*jumpVelocity = 100.0f;          // ������Ծ�ٶ�
        maxJumpHeight = 100.0f;         // ���������Ծ�߶�
        currentJumpHeight = 0.0f;       // ��ʼ����ǰ��Ծ�߶�*/
    }
}

void GameScene::applyJump(int delta)
{
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapHeight = mapSize.height * tileSize.height;
    /*�����ﳬ���ϱ߽�ʱ������*/
    if (mapPosition.y >= -mapHeight / 2 + hero->getContentSize().height)
    {
        hero->getPhysicsBody()->applyImpulse(Vec2(0, JumpHeight));
    }
    isJumping = false;  // ֹͣ��Ծ����ʼ��������
}

int GameScene::checkBlockCollision(float x, float y)
{
    // ��ȡ��Ƭ����
    Vec2 tileCoord = getTileCoordForPosition(x, y);

    // ���ȼ�� blocksLayer �ϵ���Ƭ GID
    int blockGID = blocksLayer->getTileGIDAt(tileCoord);

    if (blockGID >= BlocksFirstGID) // ����Ƿ��飨GID >= BlocksFirstGID��
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

void GameScene::setHeroAnimation(const std::string& frame2, const std::string& frame3, const std::string& frame4, const std::string& frame5)
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
    hero->runAction(RepeatForever::create(animate));  // �ظ�ִ�ж���
}






