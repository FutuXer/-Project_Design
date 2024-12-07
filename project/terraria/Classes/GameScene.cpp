#include "GameScene.h"

USING_NS_CC;

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

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建地图
    auto map = TMXTiledMap::create("testmap.tmx");
    if (!map)
    {
        CCLOG("Failed to load map.");
        return false;
    }
    map->setAnchorPoint(Vec2(0, 0));
    map->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    map->setScale(1.0f);
    map->setName("map"); // 设置名称
    this->addChild(map, 0);

    // 创建主角
    auto hero = Sprite::create("hero.png");
    if (!hero)
    {
        CCLOG("Failed to load hero.");
        return false;
    }
    // 主角初始位置设置为屏幕中心
    hero->setPosition(visibleSize / 2);
    hero->setName("hero"); // 设置名称
    this->addChild(hero, 1); // 将主角添加到场景，而不是地图

    // 调整地图初始位置，使屏幕中心对齐主角
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    Vec2 screenCenter = visibleSize / 2;
    Vec2 heroTilePosition = Vec2(5 * tileSize.width, 5 * tileSize.height);
    Vec2 mapInitialPosition = screenCenter - heroTilePosition;

    // 边界限制
    mapInitialPosition.x = std::min(mapInitialPosition.x, 0.0f); // 左边界
    mapInitialPosition.x = std::max(mapInitialPosition.x, visibleSize.width - mapWidth); // 右边界
    mapInitialPosition.y = std::min(mapInitialPosition.y, 0.0f); // 下边界
    mapInitialPosition.y = std::max(mapInitialPosition.y, visibleSize.height - mapHeight); // 上边界

    map->setPosition(mapInitialPosition);

    // 添加键盘操作
    addKeyboardListener(hero);

    this->scheduleUpdate();

    return true;
}

void GameScene::update(float delta)
{
    // 获取地图和主角
    auto map = dynamic_cast<TMXTiledMap*>(this->getChildByName("map"));
    auto hero = dynamic_cast<Sprite*>(this->getChildByName("hero"));

    if (!map || !hero)
    {
        CCLOG("Map or Hero not found.");
        return;
    }

    // 获取主角位置
    auto heroPosition = hero->getPosition();

    // 获取屏幕可视区域大小
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 获取地图尺寸
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    // 计算屏幕中心点的偏移量
    Vec2 screenCenter = Vec2(visibleSize.width / 2, visibleSize.height / 2);

    // 初始化地图的新位置（以主角居中计算）
    Vec2 mapPosition = screenCenter - heroPosition;

    // 限制地图位置，防止超出边界，避免黑边
    mapPosition.x = std::min(mapPosition.x, 0.0f); // 地图左边界
    mapPosition.x = std::max(mapPosition.x, visibleSize.width - mapWidth); // 地图右边界
    mapPosition.y = std::min(mapPosition.y, 0.0f); // 地图下边界
    mapPosition.y = std::max(mapPosition.y, visibleSize.height - mapHeight); // 地图上边界

    // 更新地图位置
    map->setPosition(mapPosition);

    // 主角始终保持逻辑上的绝对位置（相对于地图）
    // 不需要单独调整 hero->setPosition，因为其位置用于驱动地图
}

void GameScene::addKeyboardListener(Sprite* hero)
{
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        Vec2 heroPosition = hero->getPosition();

        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_A: // 左移
            hero->setPosition(heroPosition + Vec2(-10, 0));
            break;
        case EventKeyboard::KeyCode::KEY_D: // 右移
            hero->setPosition(heroPosition + Vec2(10, 0));
            break;
        case EventKeyboard::KeyCode::KEY_SPACE: // 跳跃
            performJump(hero);
            break;
        default:
            break;
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void GameScene::performJump(Sprite* hero)
{
    // 跳跃动作：上升 30 像素点，然后回到原位
    auto jumpUp = MoveBy::create(0.2f, Vec2(0, 30));  // 上升
    auto jumpDown = MoveBy::create(0.2f, Vec2(0, -30)); // 返回
    auto jumpSequence = Sequence::create(jumpUp, jumpDown, nullptr);
    hero->runAction(jumpSequence);
}


