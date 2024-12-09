#include "GameScene.h"
#include "StartScene.h"

USING_NS_CC;

Scene* GameScene::createScene() {
    return GameScene::create();
}

bool GameScene::init() {
    if (!Scene::init()) {
        return false;
    }

    // ��ȡ���ڴ�С
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ���һ��������ɫ��
    auto bgLayer = LayerColor::create(Color4B(0, 0, 255, 128));  // ��͸������ɫ����
    this->addChild(bgLayer);

    // ���������á���ť
    auto settingsLabel = Label::createWithTTF(u8"����", "../Resources/fonts/jiantisongti.ttf", 48);
    auto settingsButton = MenuItemLabel::create(
        settingsLabel,
        CC_CALLBACK_1(GameScene::showSettings, this));  // ������� showSettings

    settingsButton->setPosition(Vec2(visibleSize.width - 100 + origin.x, visibleSize.height - 50 + origin.y));

    // �����˵�����ӡ����á���ť
    auto menu = Menu::create(settingsButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1, "settingsMenu");  // ��ӱ�ǩ�����Ƴ����滻

    return true;
}

// ��ʾ���ص����˵�����ť
void GameScene::showSettings(cocos2d::Ref* sender) {
    // �Ƴ������á���ť���ڵĲ˵�
    this->removeChildByName("settingsMenu");

    // ��ȡ���ڴ�С
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // �������ص����˵�����ť
    auto backLabel = Label::createWithTTF(u8"�ص����˵�", "../Resources/fonts/jiantisongti.ttf", 32);
    auto backButton = MenuItemLabel::create(
        backLabel,
        CC_CALLBACK_1(GameScene::exitGame, this));  // ������� exitGame

    backButton->setPosition(Vec2(visibleSize.width - 100 + origin.x, visibleSize.height - 50 + origin.y));


    // �����˵�����ӡ��ص����˵�����ť
    auto backMenu = Menu::create(backButton, nullptr);
    backMenu->setPosition(Vec2::ZERO);
    this->addChild(backMenu, 1);
}

// �ص����˵�
void GameScene::exitGame(cocos2d::Ref* sender) {
    auto startScene = StartScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, startScene));
}
