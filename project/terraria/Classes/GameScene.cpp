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

    // ������ͼ
    auto map = TMXTiledMap::create("testmap.tmx");
    if (!map)
    {
        CCLOG("Failed to load map.");
        return false;
    }
    map->setAnchorPoint(Vec2(0, 0));
    map->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    map->setScale(1.0f);
    map->setName("map"); // ��������
    this->addChild(map, 0);

    // ��������
    auto hero = Sprite::create("hero.png");
    if (!hero)
    {
        CCLOG("Failed to load hero.");
        return false;
    }
    // ���ǳ�ʼλ������Ϊ��Ļ����
    hero->setPosition(visibleSize / 2);
    hero->setName("hero"); // ��������
    this->addChild(hero, 1); // ��������ӵ������������ǵ�ͼ

    // ������ͼ��ʼλ�ã�ʹ��Ļ���Ķ�������
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    Vec2 screenCenter = visibleSize / 2;
    Vec2 heroTilePosition = Vec2(5 * tileSize.width, 5 * tileSize.height);
    Vec2 mapInitialPosition = screenCenter - heroTilePosition;

    // �߽�����
    mapInitialPosition.x = std::min(mapInitialPosition.x, 0.0f); // ��߽�
    mapInitialPosition.x = std::max(mapInitialPosition.x, visibleSize.width - mapWidth); // �ұ߽�
    mapInitialPosition.y = std::min(mapInitialPosition.y, 0.0f); // �±߽�
    mapInitialPosition.y = std::max(mapInitialPosition.y, visibleSize.height - mapHeight); // �ϱ߽�

    map->setPosition(mapInitialPosition);

    // ��Ӽ��̲���
    addKeyboardListener(hero);

    this->scheduleUpdate();

    return true;
}

void GameScene::update(float delta)
{
    // ��ȡ��ͼ������
    auto map = dynamic_cast<TMXTiledMap*>(this->getChildByName("map"));
    auto hero = dynamic_cast<Sprite*>(this->getChildByName("hero"));

    if (!map || !hero)
    {
        CCLOG("Map or Hero not found.");
        return;
    }

    // ��ȡ����λ��
    auto heroPosition = hero->getPosition();

    // ��ȡ��Ļ���������С
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // ��ȡ��ͼ�ߴ�
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    // ������Ļ���ĵ��ƫ����
    Vec2 screenCenter = Vec2(visibleSize.width / 2, visibleSize.height / 2);

    // ��ʼ����ͼ����λ�ã������Ǿ��м��㣩
    Vec2 mapPosition = screenCenter - heroPosition;

    // ���Ƶ�ͼλ�ã���ֹ�����߽磬����ڱ�
    mapPosition.x = std::min(mapPosition.x, 0.0f); // ��ͼ��߽�
    mapPosition.x = std::max(mapPosition.x, visibleSize.width - mapWidth); // ��ͼ�ұ߽�
    mapPosition.y = std::min(mapPosition.y, 0.0f); // ��ͼ�±߽�
    mapPosition.y = std::max(mapPosition.y, visibleSize.height - mapHeight); // ��ͼ�ϱ߽�

    // ���µ�ͼλ��
    map->setPosition(mapPosition);

    // ����ʼ�ձ����߼��ϵľ���λ�ã�����ڵ�ͼ��
    // ����Ҫ�������� hero->setPosition����Ϊ��λ������������ͼ
}

void GameScene::addKeyboardListener(Sprite* hero)
{
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        Vec2 heroPosition = hero->getPosition();

        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_A: // ����
            hero->setPosition(heroPosition + Vec2(-10, 0));
            break;
        case EventKeyboard::KeyCode::KEY_D: // ����
            hero->setPosition(heroPosition + Vec2(10, 0));
            break;
        case EventKeyboard::KeyCode::KEY_SPACE: // ��Ծ
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
    // ��Ծ���������� 30 ���ص㣬Ȼ��ص�ԭλ
    auto jumpUp = MoveBy::create(0.2f, Vec2(0, 30));  // ����
    auto jumpDown = MoveBy::create(0.2f, Vec2(0, -30)); // ����
    auto jumpSequence = Sequence::create(jumpUp, jumpDown, nullptr);
    hero->runAction(jumpSequence);
}


