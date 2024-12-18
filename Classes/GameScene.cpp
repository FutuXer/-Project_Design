#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include <chrono>

USING_NS_CC;
#define End 10673
#define HeroHealth 100 //Ӣ�۵�Ѫ��
#define BlocksFirstGID 4


//��ͼ
cocos2d::TMXTiledMap* map;
float mapWidth;
float mapHeight;

// ͼ�����
cocos2d::TMXLayer* blocksLayer = nullptr; // ���ܴ�Խ���赲��
cocos2d::TMXLayer* itemsLayer = nullptr;  // �ɴ�Խ����Ʒ��

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

    auto camera = Camera::create();
    camera->setCameraFlag(CameraFlag::USER1);
    this->addChild(camera);
    camera->setName("Camera");

    // ���ؾ���֡
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("man.plist");  // ���� plist �ļ�

    auto textureCache = Director::getInstance()->getTextureCache();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ������ͼ
    map = TMXTiledMap::create("map1.tmx");

    // ��ȡ��ͼ�ߴ�
    const auto mapSize = map->getMapSize();
    const auto tileSize = map->getTileSize();
    mapWidth = mapSize.width * tileSize.width;
    mapHeight = mapSize.height * tileSize.height;

    if (!map)
    {
        CCLOG("Failed to load map.");
        return false;
    }
    map->setAnchorPoint(Vec2(0, 0));
    map->setScale(1.0f);
    map->setName("map"); // ��������
    this->addChild(map, 0);
    map->setCameraMask((unsigned short)CameraFlag::USER1);

    // ����ͼ��
    blocksLayer = map->getLayer("blocks");
    itemsLayer = map->getLayer("items");
    dayLayer = map->getLayer("day");      // ���챳��ͼ��
    nightLayer = map->getLayer("night");  // ҹ����ͼ��
    // ��ʼʱ��ʾ���챳��
    dayLayer->setVisible(true);
    nightLayer->setVisible(false);

    PhysicsMaterial PhyM(0, 0, 0);
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
                auto bodyOfBlocks = PhysicsBody::createBox(tileSize, PhyM);
                bodyOfBlocks->setDynamic(false);
                // �����帽�ӵ���Ƭ�ڵ�
                tile->setPhysicsBody(bodyOfBlocks);
            }
        }
    }

    //��ӿ���ǽ
    auto airWallD = Node::create();
    auto wallBodyD = PhysicsBody::createBox(Size(mapWidth, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));
    wallBodyD->setDynamic(false); // ��̬���壬�����ƶ�
    airWallD->setPhysicsBody(wallBodyD);
    airWallD->setPosition(Vec2(visibleSize.width / 2, (visibleSize.height - mapHeight) / 2));
    this->addChild(airWallD);

    auto airWallU = Node::create();
    auto wallBodyU = PhysicsBody::createBox(Size(mapWidth, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));
    wallBodyU->setDynamic(false); // ��̬���壬�����ƶ�
    airWallU->setPhysicsBody(wallBodyU);
    airWallU->setPosition(Vec2(visibleSize.width / 2, (visibleSize.height + mapHeight) / 2));
    this->addChild(airWallU);

    // ��������
    hero = Hero::create("man1.png", HeroHealth);
    if (!hero)
    {
        CCLOG("Failed to load hero.");
        return false;
    }
    hero->addPhy();
    hero->setCameraMask((unsigned short)CameraFlag::USER1);
    // ���ǳ�ʼλ������Ϊ��Ļ����
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    hero->setName("hero"); // ��������
    this->addChild(hero, 1); // ��������ӵ������������ǵ�ͼ
    // ������Ʒ����ʼ��
    hero->iniItems();

    // ��ʼ����ͼλ��
    auto mapPosition = Vec2((visibleSize.width - mapWidth) / 2, (visibleSize.height - mapHeight) / 2);
    map->setPosition(mapPosition);

    // ��Ӽ��̲���
    hero->addKeyboardListener();
    // ���������
    hero->addTouchListener();

    hero->schedule(CC_SCHEDULE_SELECTOR(Hero::HeroFunc)); //���ǵ��˶�

    /*�����ӽ�λ��*/
    this->schedule([=](float delta) {
        updateCameraPosition(hero);
        }, "update_camera");

    // ����������µ���
    this->schedule([=](float delta) {
        updateWeather(delta);  // ��������
        }, 1.0f / 60.0f, "weather_update_key");

    return true;
}


/*�õ���Ƭ����*/
Vec2 getTileCoordForPosition(float x, float y)
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

/*�ж�����Ƭ�Ƿ�����ײ*/
int checkBlockCollision(float x, float y)
{
    // ��ȡ��Ƭ����
    Vec2 tileCoord = getTileCoordForPosition(x, y);

    //ȡ��ͼ��Ĵ�С
    Size layerSize = blocksLayer->getLayerSize();

    if (tileCoord.x >= 0 && tileCoord.x < layerSize.width && tileCoord.y >= 0 && tileCoord.y < layerSize.height) { //�ж���Ƭ��λ���Ƿ�Ϸ�
        // ���ȼ�� blocksLayer �ϵ���Ƭ GID
        int blockGID = blocksLayer->getTileGIDAt(tileCoord);

        if (blockGID >= BlocksFirstGID) // ����Ƿ��飨GID >= BlocksFirstGID��
        {
            return blockGID; // ���ط���� GID����ʾ�����ϰ���
        }
        else {
            return 0; // û����ײ
        }
    }
    return BlocksFirstGID;
}

/*�����ӽǵĺ���*/
void GameScene::updateCameraPosition(Sprite* player) {
    auto camera = this->getChildByName<Camera*>("Camera");
    auto playerPosition = player->getPosition();

    // �������������������λ��
    camera->setPosition(Vec2(playerPosition.x, playerPosition.y));
}

/*void GameScene::addPhysicsBodiesForVisibleTiles() {
    auto tileMap = map;
    auto collidableLayer = blocksLayer;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // ��ȡ��Ļ��Ұ��Χ
    Rect visibleRect = Rect(origin.x, origin.y, visibleSize.width, visibleSize.height);

    // ������ͼ�е�������Ƭ
    for (int x = 0; x < tileMap->getMapSize().width; ++x) {
        for (int y = 0; y < tileMap->getMapSize().height; ++y) {
            auto tilePos = Vec2(x, y);
            auto tileSprite = collidableLayer->getTileAt(tilePos);
            if (!tileSprite) continue; // ������Ч��Ƭ

            // ��ȡ��Ƭ�İ�Χ��
            auto tileBoundingBox = tileSprite->getBoundingBox();

            // ����Ƿ�����Ұ��Χ��
            if (visibleRect.intersectsRect(tileBoundingBox)) {
                // �����Ƭû��������壬����������
                if (!tileSprite->getPhysicsBody()) {
                    auto physicsBody = PhysicsBody::createBox(tileBoundingBox.size, PhysicsMaterial(1e10, 0, 1));
                    physicsBody->setDynamic(false); // ��̬����
                    tileSprite->addComponent(physicsBody);
                }
            }
            else {
                // �����Ƭ������Ұ��Χ�ڣ��Ƴ��������
                if (tileSprite->getPhysicsBody()) {
                    tileSprite->removeComponent(tileSprite->getPhysicsBody());
                }
            }
        }
    }
}*/

void GameScene::updateWeather(float deltaTime)
{
    // ������ʹ��timeOfDay���жϰ����ҹ��
    timeOfDay += deltaTime;

    if (timeOfDay >= 24)
    {
        timeOfDay = 0;
        isDay = !isDay;
        changeBackground();
    }
}

void GameScene::changeBackground()
{
    if (isDay)
    {
        dayLayer->setVisible(true);  // ��ʾ���챳��
        nightLayer->setVisible(false);  // ����ҹ����
    }
    else
    {
        dayLayer->setVisible(false);  // ���ذ��챳��
        nightLayer->setVisible(true);  // ��ʾҹ����
    }
}