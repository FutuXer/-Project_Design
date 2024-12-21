#include "StartScene.h"
#include "GameScene.h"
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
USING_NS_CC;

Scene* StartScene::createScene() 
{
    return StartScene::create();
}

bool StartScene::init()
{
    if (!Scene::init())
    {
        return false;
    }
    cocos2d::experimental::AudioEngine::stopAll();

    // ��ȡ���ڴ�С
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ��ӱ���
    auto newBackground = Sprite::create("startscene_background.png");  // ȷ����Դ�ļ�����
    if (newBackground != nullptr)
    {
        newBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(newBackground, -2);  // ȷ���ڱ���󷽣��㼶����Ϊ -2
    }

    // �����Ϸ����
    auto background = Sprite::create("game title.png");
    if (background != nullptr) 
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(background, -1);
    }

    // ��������ʼ��Ϸ����ť
    auto startLabel = Label::createWithTTF(u8"��ʼ��Ϸ", "../Resources/fonts/jiantisongti.ttf", 48);
    auto startButton = MenuItemLabel::create(
        startLabel,
        CC_CALLBACK_1(StartScene::startGame, this)
    );
    startButton->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 130));

    // �������˳�����ť
    auto exitLabel = Label::createWithTTF(u8"�˳�", "../Resources/fonts/jiantisongti.ttf", 48);
    auto exitButton = MenuItemLabel::create(
        exitLabel,
        CC_CALLBACK_1(StartScene::exitGame, this)
    );
    exitButton->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 200));

    // �����˵�����Ӱ�ť
    auto menu = Menu::create(startButton, exitButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);

    return true;
}

void StartScene::startGame(Ref* sender) 
{
    auto gameScene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, gameScene));
}

void StartScene::exitGame(Ref* sender)
{
    Director::getInstance()->end();
}
