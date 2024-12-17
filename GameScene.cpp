#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include"monster.h"

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

//*********�������
#define MonsterNumMax 6 //����������

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
    map = TMXTiledMap::create("map1(2).tmx");

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

    // ��ʼ����ͼλ��
    auto mapPosition = Vec2((visibleSize.width - mapWidth) / 2, (visibleSize.height - mapHeight) / 2);
    map->setPosition(mapPosition);

    // ��Ӽ��̲���
    hero->addKeyboardListener();


    hero->schedule(CC_SCHEDULE_SELECTOR(Hero::HeroFunc)); //���ǵ��˶�

    /********�й����*******/this->schedule(CC_SCHEDULE_SELECTOR(GameScene::MonsterFunc), 1.0f); //������������˶�


    /*�����ӽ�λ��*/
    this->schedule([=](float delta) {
        updateCameraPosition(hero);
        }, "update_camera");

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


//*********�й����

/*���������й���ĺ���*/
void GameScene::MonsterFunc(float dt) {
    // ��ȡ��Ļ��С
    Size visibleSize = Director::getInstance()->getVisibleSize();

    if (numberOfMonsters < MonsterNumMax) {
        int choice = RandomHelper::random_int(0, 1); //ѡ������ʷ��ķ���ǽ�ʬ
        if (choice == 0) {
            auto silme = Silme::create("slime.png", 100);
            float x_S = RandomHelper::random_real((visibleSize.width - mapWidth)/2 + silme->getContentSize().width / 2, (visibleSize.width + mapWidth) / 2 - silme->getContentSize().width / 2); //�����������
            silme->setPosition(Vec2(x_S, visibleSize.height / 1.5f));
            silme->addPhy();
            silme->addjellyeffect();
            silme->addhealthbar();
            this->addChild(silme);
            silme->setCameraMask((unsigned short)CameraFlag::USER1);
            silme->schedule(CC_SCHEDULE_SELECTOR(Silme::silme_move), 3);
            silme->schedule(CC_SCHEDULE_SELECTOR(Silme::brodercheck));
            silme->schedule(CC_SCHEDULE_SELECTOR(Silme::AttackPlayer), 1, -1, 0);
        }
        if (choice == 1) {
            auto zombie = Zombie::create("zombie1.png", 100);
            float x_Z = RandomHelper::random_real((visibleSize.width - mapWidth) / 2 + zombie->getContentSize().width / 2, (visibleSize.width + mapWidth) / 2 - zombie->getContentSize().width / 2); //�����������
            zombie->setPosition(Vec2(x_Z, visibleSize.height / 1.5f));
            zombie->addPhy();
            zombie->addAnimate();
            zombie->addhealthbar();
            this->addChild(zombie);
            zombie->setCameraMask((unsigned short)CameraFlag::USER1);
            zombie->schedule(CC_SCHEDULE_SELECTOR(Zombie::zombie_move));
            zombie->schedule(CC_SCHEDULE_SELECTOR(Zombie::jump), 1);
            zombie->schedule(CC_SCHEDULE_SELECTOR(Zombie::AttackPlayer), 1, -1, 0);
        }
        numberOfMonsters++;
    }
}
