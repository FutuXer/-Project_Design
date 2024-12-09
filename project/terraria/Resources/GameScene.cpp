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

    // �������ص����˵�����ť��ʹ�� Label ��Ϊ�ı�

    auto label = Label::createWithTTF(u8"�ص����˵�", "../Resources/fonts/jiantisongti.ttf", 48);


    // ����һ���˵���
    auto exitButton = MenuItemLabel::create(
        label,
        CC_CALLBACK_1(GameScene::exitGame, this));  // �����˵����ӻص�

    exitButton->setPosition(Vec2(visibleSize.width - 150 + origin.x, visibleSize.height - 50 + origin.y));  // ���ð�ťλ��

    // �����˵�����Ӱ�ť
    auto menu = Menu::create(exitButton, nullptr);
    menu->setPosition(Vec2::ZERO);  // ���ò˵��ĳ�ʼλ��Ϊ��
    this->addChild(menu);  // ���˵���ӵ�����

    return true;
}


void GameScene::exitGame(Ref* sender) {
    auto startScene = StartScene::createScene();  // ������ʼ����
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, startScene));  // ʹ�õ��뵭��Ч���л�����ʼ����
}
