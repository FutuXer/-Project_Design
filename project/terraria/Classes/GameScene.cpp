#include "GameScene.h"

USING_NS_CC;

const float gravity = -3.0f; // 重力加速度
const float maxFallSpeed = -200.0f; // 最大下落速度
const float MAX_VELOCITY = 150.0f;  // 最大水平速度

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

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建地图
    map = TMXTiledMap::create("testmap.tmx");
    if (!map)
    {
        CCLOG("Failed to load map.");
        return false;
    }
    map->setAnchorPoint(Vec2(0, 0));
    map->setScale(1.0f);
    map->setName("map"); // 设置名称
    this->addChild(map, 0);

    // 获取地图尺寸
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    // 加载图层
    blocksLayer = map->getLayer("blocks");
    itemsLayer = map->getLayer("items");
    PhysicsMaterial nonBounce(1, 0, 1); // 不会反弹的物理模型
    PhysicsMaterial infinity_mass(1e10, 0, 1); // 无穷大质量的物理模型
    for (int x = 0; x < mapSize.width; x++) {
        for (int y = 0; y < mapSize.height; y++) {
            // 获取瓦片
            auto tile = blocksLayer->getTileAt(Vec2(x, y));

            // 如果瓦片存在，则创建刚体
            if (tile) {
                // 创建物理刚体
                auto bodyOfBlocks = PhysicsBody::createBox(tileSize, infinity_mass);
                bodyOfBlocks->setGravityEnable(false);   // 禁用重力影响
                // 将刚体附加到瓦片节点
                tile->setPhysicsBody(bodyOfBlocks);
            }
        }
    }

    // 创建主角
    hero = Sprite::create("hero.png");
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

    // 更新左右移动逻辑
    if (moveLeft)
    {
        mapPosition.x += 3; // 向左移动      
        checkAndFixHeroCollision();
        // 限制地图不能超出左边界
        if (mapPosition.x >= mapWidth / 2 - 11)
        {
            mapPosition.x = mapWidth / 2 - 11;
        }
    }
    if (moveRight)
    {
        mapPosition.x -= 3; // 向右移动
        // 限制地图不能超出右边界
        if (mapPosition.x <= -mapWidth / 2 + 11)
        {
            mapPosition.x = -mapWidth / 2 + 11;
        }
        checkAndFixHeroCollision();
    }

    // 更新地图位置
    Vec2 tmpPosition = hero->getPosition();
    mapPosition.x = mapPosition.x - (tmpPosition.x - mapWidth / 2);
    mapPosition.y = mapPosition.y - (tmpPosition.y - mapHeight / 2);
    map->setPosition(mapPosition);
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
}

void GameScene::checkAndFixHeroCollision()
{
    // 获取角色当前的物理碰撞体
    Vec2 heroPos = hero->getPosition();

    // 获取角色当前位置左右的瓦片坐标
    int tileGID_left = checkBlockCollision(heroPos.x - 10, heroPos.y);
    int tileGID_right = checkBlockCollision(heroPos.x + 10, heroPos.y);

    // 如果碰到方块，则修正角色位置
    if (tileGID_left >= 181)
    {
        mapPosition.x -= 3;
    }
    // 如果碰到方块，则修正角色位置
    if (tileGID_right >= 181)
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
            break;
        case EventKeyboard::KeyCode::KEY_D: // 向右移动
            moveRight = true;
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
            break;
        case EventKeyboard::KeyCode::KEY_D: // 停止向右移动
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
    // 检查角色是否站在地面上，只有站在地面上才能跳跃
    if (!isJumping && checkBlockCollision(hero->getPositionX(), hero->getPositionY() - 15) >= 181)
    {
        isJumping = true;               // 设置为跳跃状态
        jumpVelocity = 100.0f;          // 设置跳跃速度
        maxJumpHeight = 100.0f;         // 设置最大跳跃高度
        currentJumpHeight = 0.0f;       // 初始化当前跳跃高度
    }
}

void GameScene::applyJump(int delta)
{
    hero->getPhysicsBody()->applyImpulse(Vec2(0, 500));
    isJumping = false;  // 停止跳跃，开始自由落体
}

int GameScene::checkBlockCollision(float x, float y)
{
    // 获取瓦片坐标
    Vec2 tileCoord = getTileCoordForPosition(x, y);

    // 首先检查 blocksLayer 上的瓦片 GID
    int blockGID = blocksLayer->getTileGIDAt(tileCoord);

    if (blockGID >= 181) // 如果是方块（GID >= 181）
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
