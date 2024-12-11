#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
#define JumpHeight 500 //人物向上跳跃的冲量大小
#define BlocksFirstGID 10409
#define End 10673

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

    // 加载精灵帧
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("man.plist");  // 加载 plist 文件

    auto textureCache = Director::getInstance()->getTextureCache();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建地图
    map = TMXTiledMap::create("map0.tmx");

    // 获取地图尺寸
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
    map->setName("map"); // 设置名称
    this->addChild(map, 0);


    // 加载图层
    blocksLayer = map->getLayer("blocks");
    itemsLayer = map->getLayer("items");
    PhysicsMaterial nonBounce(1, 0, 1); // 不会反弹的物理模型
    PhysicsMaterial infinity_mass(1e10, 0, 1); // 无穷大质量的物理模型
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
                auto bodyOfBlocks = PhysicsBody::createBox(tileSize, infinity_mass);
                bodyOfBlocks->setGravityEnable(false);   // 禁用重力影响
                // 将刚体附加到瓦片节点
                tile->setPhysicsBody(bodyOfBlocks);
            }
        }
    }

    // 创建主角
    hero = Sprite::create("man1.png");
    if (!hero)
    {
        CCLOG("Failed to load hero.");
        return false;
    }
    auto bodyOfHero = PhysicsBody::createBox(hero->getContentSize(), nonBounce); // 主角的刚体
    bodyOfHero->setRotationEnable(false); // 锁定主角的旋转
    bodyOfHero->setMass(1); // 设置主角的质量
    hero->setPhysicsBody(bodyOfHero);
    // 主角初始位置设置为屏幕中心
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    hero->setName("hero"); // 设置名称
    this->addChild(hero, 1); // 将主角添加到场景，而不是地图

    // 初始化地图位置
    mapPosition = Vec2((visibleSize.width - mapWidth) / 2, (visibleSize.height - mapHeight) / 2);
    map->setPosition(mapPosition);

    // 添加键盘操作
    addKeyboardListener(hero);

    // 调度物理更新
    this->schedule([=](float deltaTime) {
        this->updatePhysicsWorld(deltaTime);
        }, "update_physics_world_key");

    this->scheduleUpdate(); // 调度更新方法

    return true;
}

void GameScene::update(float delta)
{
    // 获取地图和主角
    if (!map || !hero)
    {
        CCLOG("Map or Hero not found.");
        return;
    }

    // 获取可见尺寸和地图尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    // 更新跳跃和下落逻辑
    if (isJumping)
    {
        applyJump(delta);
    }

    checkAndFixHeroCollision();

    // 更新左右移动逻辑
    if (moveLeft)
    {
        mapPosition.x += 3; // 向左移动      
        // 限制地图不能超出左边界
        if (mapPosition.x >= visibleSize.width / 2 - 12)
        {
            mapPosition.x = visibleSize.width / 2 - 12;
        }
    }
    if (moveRight)
    {
        mapPosition.x -= 3; // 向右移动
        // 限制地图不能超出右边界
        if (mapPosition.x <= visibleSize.width / 2 - mapWidth + 12)
        {
            mapPosition.x = visibleSize.width / 2 - mapWidth + 12;           
        }
    }

    /*阻止人物超出上边界*/
    if (mapPosition.y < -mapHeight / 2 + hero->getContentSize().height)
        hero->getPhysicsBody()->setVelocity(Vec2(0, -70));

    // 更新地图位置
    Vec2 tmpPosition = hero->getPosition();

    mapPosition.x = mapPosition.x - (tmpPosition.x - visibleSize.width / 2);
    mapPosition.y = mapPosition.y - (tmpPosition.y - visibleSize.height / 2);
    map->setPosition(mapPosition);
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);

    /*// 更新可见范围的瓦片
    RenderManager* renderManager = new RenderManager(map);
    renderManager->updateVisibleTiles(mapPosition, visibleSize);
    delete renderManager;*/
    
}

void GameScene::checkAndFixHeroCollision()
{
    // 获取角色当前的物理碰撞体
    Vec2 heroPos = hero->getPosition();

    // 获取角色当前位置左右的瓦片坐标
    int tileGID_left1 = checkBlockCollision(heroPos.x - 8, heroPos.y - 7);
    int tileGID_left2 = checkBlockCollision(heroPos.x - 8, heroPos.y + 7);
    int tileGID_right1 = checkBlockCollision(heroPos.x + 8, heroPos.y - 7);
    int tileGID_right2 = checkBlockCollision(heroPos.x + 8, heroPos.y + 7);

    // 如果碰到方块，则修正角色位置
    if (tileGID_left1 >= BlocksFirstGID || tileGID_left2 >= BlocksFirstGID)
    {
        mapPosition.x -= 3;
    }
    // 如果碰到方块，则修正角色位置
    if (tileGID_right1 >= BlocksFirstGID || tileGID_right2 >= BlocksFirstGID)
    {
        mapPosition.x += 3;
    }
}

void GameScene::addKeyboardListener(Sprite* hero)
{
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        // 处理按下按键
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_A: // 向左移动
            moveLeft = true;
            hero->setFlippedX(false);  // 左移时不镜像
            setHeroAnimation("man2.png", "man3.png", "man4.png", "man5.png");  // 切换到左移动画
            break;
        case EventKeyboard::KeyCode::KEY_D: // 向右移动
            moveRight = true;
            hero->setFlippedX(true);  // 右移时镜像
            setHeroAnimation("man2.png", "man3.png", "man4.png", "man5.png");  // 切换到右移动画
            break;
        case EventKeyboard::KeyCode::KEY_SPACE: // 跳跃
            performJump(hero);
            break;
        default:
            break;
        }
        };

    keyboardListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        // 处理松开按键
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_A: // 停止向左移动
            moveLeft = false;
            hero->stopAllActions();   // 停止所有动画
            hero->setSpriteFrame("man1.png"); // 设置静止帧
            break;
        case EventKeyboard::KeyCode::KEY_D: // 停止向右移动
            moveRight = false;
            hero->stopAllActions();   // 停止所有动画
            hero->setSpriteFrame("man1.png"); // 设置静止帧
            break;
        default:
            break;
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void GameScene::performJump(Sprite* hero)
{
    // 检查角色是否站在地面上，只有站在地面上才能跳跃
    if (!isJumping && checkBlockCollision(hero->getPositionX(), hero->getPositionY() - 15) >= BlocksFirstGID)
    {
        isJumping = true;               // 设置为跳跃状态
        /*jumpVelocity = 100.0f;          // 设置跳跃速度
        maxJumpHeight = 100.0f;         // 设置最大跳跃高度
        currentJumpHeight = 0.0f;       // 初始化当前跳跃高度*/
    }
}

void GameScene::applyJump(int delta)
{
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapHeight = mapSize.height * tileSize.height;
    /*在人物超出上边界时，不跳*/
    if (mapPosition.y >= -mapHeight / 2 + hero->getContentSize().height)
    {
        hero->getPhysicsBody()->applyImpulse(Vec2(0, JumpHeight));
    }
    isJumping = false;  // 停止跳跃，开始自由落体
}

int GameScene::checkBlockCollision(float x, float y)
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

Vec2 GameScene::getTileCoordForPosition(float x, float y)
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

void GameScene::updatePhysicsWorld(float delta)
{
    // 使用固定时间步长进行物理更新
    for (int i = 0; i < 3; ++i)
    {
        this->getPhysicsWorld()->step(1 / 180.0f);  // 每次物理步进 1/180 秒
    }
}

void GameScene::setHeroAnimation(const std::string& frame2, const std::string& frame3, const std::string& frame4, const std::string& frame5)
{
    // 创建精灵帧对象
    Vector<SpriteFrame*> frames;
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame2));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame3));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame4));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame5));

    // 创建动画
    Animation* animation = Animation::createWithSpriteFrames(frames, 0.1f);  // 0.1f 为每帧之间的间隔
    Animate* animate = Animate::create(animation);  // 创建动画动作

    // 执行动画
    hero->runAction(RepeatForever::create(animate));  // 重复执行动画
}






