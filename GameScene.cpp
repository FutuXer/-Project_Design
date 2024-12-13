#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
#define End 10673
#define HeroHealth 100 //Ӣ�۵�Ѫ��
#define BlocksFirstGID 10409

// ���浱ǰ��ͼ��λ��
cocos2d::Vec2 mapPosition;
//��ͼ
cocos2d::TMXTiledMap* map;
// ͼ�����
cocos2d::TMXLayer* blocksLayer = nullptr; // ���ܴ�Խ���赲��
cocos2d::TMXLayer* itemsLayer = nullptr;  // �ɴ�Խ����Ʒ��

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
    hero = Hero::create("man1.png", HeroHealth);
    if (!hero)
    {
        CCLOG("Failed to load hero.");
        return false;
    }
    hero->addPhy();
    // ���ǳ�ʼλ������Ϊ��Ļ����
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    hero->setName("hero"); // ��������
    this->addChild(hero, 1); // ��������ӵ������������ǵ�ͼ

    // ��ʼ����ͼλ��
    mapPosition = Vec2((visibleSize.width - mapWidth) / 2, (visibleSize.height - mapHeight) / 2);
    map->setPosition(mapPosition);

    // ��Ӽ��̲���
    hero->addKeyboardListener();

    // �����������
    this->schedule([=](float deltaTime) {
        this->updatePhysicsWorld(deltaTime);
        }, "update_physics_world_key");

    hero->schedule(CC_SCHEDULE_SELECTOR(Hero::HeroFunc));

    return true;
}


/*int GameScene::checkBlockCollision(float x, float y)
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
}*/

void GameScene::updatePhysicsWorld(float delta)
{
    // ʹ�ù̶�ʱ�䲽�������������
    for (int i = 0; i < 3; ++i)
    {
        this->getPhysicsWorld()->step(1 / 180.0f);  // ÿ�������� 1/180 ��
    }
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

    // ���ȼ�� blocksLayer �ϵ���Ƭ GID
    int blockGID = blocksLayer->getTileGIDAt(tileCoord);

    if (blockGID >= BlocksFirstGID) // ����Ƿ��飨GID >= BlocksFirstGID��
    {
        return blockGID; // ���ط���� GID����ʾ�����ϰ���
    }

    return 0; // û����ײ
}




