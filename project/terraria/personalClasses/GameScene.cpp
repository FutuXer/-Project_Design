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

    // 添加地图
    auto map = TMXTiledMap::create("maps/main_map.tmx");
    if (map)
    {
        this->addChild(map, 0);
    }

    // 添加精灵
    auto forestSprite = Sprite::create("images/forest.png");
    if (forestSprite)
    {
        forestSprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(forestSprite, 1);
    }

    return true;
}
