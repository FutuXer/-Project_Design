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

    // 创建“设置”按钮
    auto settingsLabel = Label::createWithTTF(u8"设置", "../Resources/fonts/jiantisongti.ttf", 48);
    auto settingsButton = MenuItemLabel::create(
        settingsLabel,
        CC_CALLBACK_1(GameScene::showSettings, this));  // 点击调用 showSettings

    settingsButton->setPosition(Vec2(visibleSize.width - 100 + origin.x, visibleSize.height - 50 + origin.y));

    // 创建菜单并添加“设置”按钮
    auto menu = Menu::create(settingsButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1, "settingsMenu");  // 添加标签方便移除或替换

    return true;
}

// 显示“回到主菜单”按钮
void GameScene::showSettings(cocos2d::Ref* sender) {
    // 移除“设置”按钮所在的菜单
    this->removeChildByName("settingsMenu");

    // 获取窗口大小
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建“回到主菜单”按钮
    auto backLabel = Label::createWithTTF(u8"回到主菜单", "../Resources/fonts/jiantisongti.ttf", 32);
    auto backButton = MenuItemLabel::create(
        backLabel,
        CC_CALLBACK_1(GameScene::exitGame, this));  // 点击调用 exitGame

    backButton->setPosition(Vec2(visibleSize.width - 100 + origin.x, visibleSize.height - 50 + origin.y));


    // 创建菜单并添加“回到主菜单”按钮
    auto backMenu = Menu::create(backButton, nullptr);
    backMenu->setPosition(Vec2::ZERO);
    this->addChild(backMenu, 1);
}

// 回到主菜单
void GameScene::exitGame(cocos2d::Ref* sender) {
    auto startScene = StartScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, startScene));
}
