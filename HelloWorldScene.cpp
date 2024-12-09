#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

bool HelloWorld::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto mapLayer = MapLayer::create();
    this->addChild(mapLayer);

    // ������ң�������λ�����õ� Y = 450
    player = Player::createPlayer();
    player->setPosition(Vec2(100, 450));
    this->addChild(player);

    // ��ȡ��Ļ�ߴ�
    screenSize = Director::getInstance()->getVisibleSize();

    // ��Ӽ����¼�������
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    scheduleUpdate();

    return true;
}

void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    keysPressed.insert(keyCode);
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    keysPressed.erase(keyCode);
}

void HelloWorld::update(float delta)
{
    // ���ݰ��µİ��������ƶ���ң���������Ļ�ߴ���б߽���
    if (keysPressed.count(EventKeyboard::KeyCode::KEY_LEFT_ARROW) || keysPressed.count(EventKeyboard::KeyCode::KEY_A))
    {
        player->moveLeft(screenSize.width);
    }
    if (keysPressed.count(EventKeyboard::KeyCode::KEY_RIGHT_ARROW) || keysPressed.count(EventKeyboard::KeyCode::KEY_D))
    {
        player->moveRight(screenSize.width);
    }
    if (keysPressed.count(EventKeyboard::KeyCode::KEY_UP_ARROW) || keysPressed.count(EventKeyboard::KeyCode::KEY_W))
    {
        player->moveUp(screenSize.height);
    }
    if (keysPressed.count(EventKeyboard::KeyCode::KEY_DOWN_ARROW) || keysPressed.count(EventKeyboard::KeyCode::KEY_S))
    {
        player->moveDown(screenSize.height);
    }
}
