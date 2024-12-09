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

    // 获取窗口大小
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 添加一个背景颜色层
    auto bgLayer = LayerColor::create(Color4B(0, 0, 255, 128));  // 半透明的蓝色背景
    this->addChild(bgLayer);

    // 创建“回到主菜单”按钮，使用 Label 作为文本

    auto label = Label::createWithTTF(u8"回到主菜单", "../Resources/fonts/jiantisongti.ttf", 48);


    // 创建一个菜单项
    auto exitButton = MenuItemLabel::create(
        label,
        CC_CALLBACK_1(GameScene::exitGame, this));  // 创建菜单项并添加回调

    exitButton->setPosition(Vec2(visibleSize.width - 150 + origin.x, visibleSize.height - 50 + origin.y));  // 设置按钮位置

    // 创建菜单并添加按钮
    auto menu = Menu::create(exitButton, nullptr);
    menu->setPosition(Vec2::ZERO);  // 设置菜单的初始位置为零
    this->addChild(menu);  // 将菜单添加到场景

    return true;
}


void GameScene::exitGame(Ref* sender) {
    auto startScene = StartScene::createScene();  // 创建开始场景
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, startScene));  // 使用淡入淡出效果切换到开始场景
}
