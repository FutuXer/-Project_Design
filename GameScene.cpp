#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
#define End 10673
#define HeroHealth 100 //英雄的血量
#define BlocksFirstGID 10409


//地图
cocos2d::TMXTiledMap* map;
float mapWidth;
float mapHeight;

// 图层相关
cocos2d::TMXLayer* blocksLayer = nullptr; // 不能穿越的阻挡层
cocos2d::TMXLayer* itemsLayer = nullptr;  // 可穿越的物品层

/*class RenderManager
{
private:
    TMXTiledMap* _map;
    Size _tileSize;
    Size _mapSize;
    Rect _lastVisibleRect; // 记录上一次的可见范围

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

        // 计算新可见范围
        Rect visibleRect(minX, minY, maxX - minX, maxY - minY);

        if (visibleRect.equals(_lastVisibleRect))
        {
            // 如果范围未变化，跳过更新
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

    initWithPhysics(); // 引入物理组件
    this->getPhysicsWorld()->setGravity(Vec2(0, -980)); // 设置重力
    this->getPhysicsWorld()->setAutoStep(false); // 禁用自动步进

    auto camera = Camera::create();
    camera->setCameraFlag(CameraFlag::USER1);
    this->addChild(camera);
    camera->setName("Camera");

    // 加载精灵帧
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("man.plist");  // 加载 plist 文件

    auto textureCache = Director::getInstance()->getTextureCache();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建地图
    map = TMXTiledMap::create("map1.tmx");

    // 获取地图尺寸
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
    map->setName("map"); // 设置名称
    this->addChild(map, 0);
    map->setCameraMask((unsigned short)CameraFlag::USER1);

    // 加载图层
    blocksLayer = map->getLayer("blocks");
    itemsLayer = map->getLayer("items");
    PhysicsMaterial PhyM(1, 0, 1); // 无穷大质量的物理模型
    for (int x = 0; x < mapSize.width; x++)
    {
        for (int y = 0; y < mapSize.height; y++)
        {
            // 获取瓦片
            auto tile = blocksLayer->getTileAt(Vec2(x, y));

            // 如果瓦片存在，则创建刚体
            if (tile)
            {
                // 创建物理刚体
                auto bodyOfBlocks = PhysicsBody::createBox(tileSize, PhyM);
                bodyOfBlocks->setDynamic(false);
                // 将刚体附加到瓦片节点
                tile->setPhysicsBody(bodyOfBlocks);
            }
        }
    }

    //添加空气墙
    auto airWallD = Node::create();
    auto wallBodyD = PhysicsBody::createBox(Size(mapWidth, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));
    wallBodyD->setDynamic(false); // 静态物体，不会移动
    airWallD->setPhysicsBody(wallBodyD);
    airWallD->setPosition(Vec2(visibleSize.width / 2, (visibleSize.height - mapHeight) / 2));
    this->addChild(airWallD);

    auto airWallU = Node::create();
    auto wallBodyU = PhysicsBody::createBox(Size(mapWidth, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));
    wallBodyU->setDynamic(false); // 静态物体，不会移动
    airWallU->setPhysicsBody(wallBodyU);
    airWallU->setPosition(Vec2(visibleSize.width / 2, (visibleSize.height + mapHeight) / 2));
    this->addChild(airWallU);

    // 创建主角
    hero = Hero::create("man1.png", HeroHealth);
    if (!hero)
    {
        CCLOG("Failed to load hero.");
        return false;
    }
    hero->addPhy();
    hero->setCameraMask((unsigned short)CameraFlag::USER1);
    // 主角初始位置设置为屏幕中心
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    hero->setName("hero"); // 设置名称
    this->addChild(hero, 1); // 将主角添加到场景，而不是地图

    // 初始化地图位置
    auto mapPosition = Vec2((visibleSize.width - mapWidth) / 2, (visibleSize.height - mapHeight) / 2);
    map->setPosition(mapPosition);

    // 添加键盘操作
    hero->addKeyboardListener();

    // 调度物理更新
    this->schedule([=](float deltaTime) {
        this->updatePhysicsWorld(deltaTime);
        }, "update_physics_world_key");

    hero->schedule(CC_SCHEDULE_SELECTOR(Hero::HeroFunc)); //主角的运动

    /*更新视角位置*/
    this->schedule([=](float delta) {
        updateCameraPosition(hero);
        }, "update_camera");

    return true;
}


void GameScene::updatePhysicsWorld(float delta)
{
    // 使用固定时间步长进行物理更新
    for (int i = 0; i < 3; ++i)
    {
        this->getPhysicsWorld()->step(1 / 180.0f);  // 每次物理步进 1/180 秒
    }
}
/*得到瓦片坐标*/
Vec2 getTileCoordForPosition(float x, float y)
{
    // 获取地图的Tile大小
    Size tileSize = map->getTileSize();
    Vec2 mapOrigin = map->getPosition();

    // 将世界坐标转化为地图坐标
    float mapX = x - mapOrigin.x;
    float mapY = y - mapOrigin.y;

    // 计算瓦片坐标
    int tileX = mapX / tileSize.width;
    int tileY = (map->getMapSize().height * tileSize.height - mapY) / tileSize.height;

    return Vec2(tileX, tileY);
}
/*判断与瓦片是否发生碰撞*/
int checkBlockCollision(float x, float y)
{
    // 获取瓦片坐标
    Vec2 tileCoord = getTileCoordForPosition(x, y);

    // 首先检查 blocksLayer 上的瓦片 GID
    int blockGID = blocksLayer->getTileGIDAt(tileCoord);

    if (blockGID >= BlocksFirstGID) // 如果是方块（GID >= BlocksFirstGID）
    {
        return blockGID; // 返回方块的 GID，表示碰到障碍物
    }

    return 0; // 没有碰撞
}

/*更新视角的函数*/
void GameScene::updateCameraPosition(Sprite* player) {
    auto camera = this->getChildByName<Camera*>("Camera"); 
    auto playerPosition = player->getPosition();

    // 获取屏幕的大小
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 保持摄像机中心在主角位置
    camera->setPosition(Vec2(playerPosition.x, playerPosition.y));
}

/*void GameScene::addPhysicsBodiesForVisibleTiles() {
    auto tileMap = map;
    auto collidableLayer = blocksLayer;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 获取屏幕视野范围
    Rect visibleRect = Rect(origin.x, origin.y, visibleSize.width, visibleSize.height);

    // 遍历地图中的所有瓦片
    for (int x = 0; x < tileMap->getMapSize().width; ++x) {
        for (int y = 0; y < tileMap->getMapSize().height; ++y) {
            auto tilePos = Vec2(x, y);
            auto tileSprite = collidableLayer->getTileAt(tilePos);
            if (!tileSprite) continue; // 跳过无效瓦片

            // 获取瓦片的包围框
            auto tileBoundingBox = tileSprite->getBoundingBox();

            // 检查是否在视野范围内
            if (visibleRect.intersectsRect(tileBoundingBox)) {
                // 如果瓦片没有物理刚体，添加物理刚体
                if (!tileSprite->getPhysicsBody()) {
                    auto physicsBody = PhysicsBody::createBox(tileBoundingBox.size, PhysicsMaterial(1e10, 0, 1));
                    physicsBody->setDynamic(false); // 静态刚体
                    tileSprite->addComponent(physicsBody);
                }
            }
            else {
                // 如果瓦片不在视野范围内，移除物理刚体
                if (tileSprite->getPhysicsBody()) {
                    tileSprite->removeComponent(tileSprite->getPhysicsBody());
                }
            }
        }
    }
}*/


