#include "GameScene.h"
#include "StartScene.h"
#include "audio/include/AudioEngine.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace cocos2d::ui;

Scene* GameScene::createScene() {
    return GameScene::create();
}

bool GameScene::init() 
{
    if (!Scene::init()) 
        return false;

    cocos2d::experimental::AudioEngine::stopAll();//先停止所有音乐

    audioID = cocos2d::experimental::AudioEngine::play2d("music.mp3", true);
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();


    this->createSettingsMenu();//右上角显示设置按钮
    return true;
}

void GameScene::showSettings(cocos2d::Ref* sender) {
    // 移除“设置”按钮所在的菜单
    this->removeChildByName("settingsMenu");

    // 获取窗口大小
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 初始按钮位置
    float initialY = visibleSize.height - 50 + origin.y;

    // 创建“回到主菜单”按钮
    auto backLabel = Label::createWithTTF(u8"回到主菜单", "../Resources/fonts/jiantisongti.ttf", 32);
    auto backButton = MenuItemLabel::create(backLabel, CC_CALLBACK_1(GameScene::exitGame, this));
    backButton->setPosition(Vec2(visibleSize.width - 100 + origin.x, initialY));

    // 创建按钮1
    auto button1Label = Label::createWithTTF(u8"调节音量", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button1 = MenuItemLabel::create(button1Label, [=](Ref* sender) {
        // 点击按钮1时调用 createVolumeSlider 函数
        createVolumeSlider();  // 调用显示进度条的函数
        });
    button1->setPosition(Vec2(visibleSize.width - 100 + origin.x, initialY - 50));
    button1->setName("volumn button");  // 给按钮1设置名字

    // 创建按钮2
    auto button2Label = Label::createWithTTF(u8"按钮2", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button2 = MenuItemLabel::create(button2Label, [](Ref* sender) { CCLOG("按钮2被点击!"); });
    button2->setPosition(Vec2(visibleSize.width - 100 + origin.x, initialY - 100));
    button2->setName("button2");  // 给按钮2设置名字

    // 创建按钮3
    auto button3Label = Label::createWithTTF(u8"按钮3", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button3 = MenuItemLabel::create(button3Label, [](Ref* sender) { CCLOG("按钮3被点击!"); });
    button3->setPosition(Vec2(visibleSize.width - 100 + origin.x, initialY - 150));
    button3->setName("button3");  // 给按钮3设置名字

    // 创建返回按钮
    auto button4Label = Label::createWithTTF(u8"返回", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button4 = MenuItemLabel::create(button4Label, [this](Ref* sender) {
        CCLOG("返回按钮被点击!");
        this->removeSettingsMenu();  // 调用移除函数
        this->createSettingsMenu();
        });
    button4->setPosition(Vec2(visibleSize.width - 100 + origin.x, initialY - 200));


    // 创建菜单并添加按钮
    auto backMenu = Menu::create(backButton, button1, button2, button3, button4, nullptr);
    backMenu->setPosition(Vec2::ZERO); // 将菜单的原点设置为(0, 0)
    this->addChild(backMenu, 1,"fourbuttonmenu");  // 添加菜单到场景中
}


void GameScene::exitGame(cocos2d::Ref* sender)
{
    // 确保退出前移除滑动条和“确定”按钮
    if (volumeSlider != nullptr)
    {
        this->removeChild(volumeSlider);
        volumeSlider = nullptr;
    }

    if (confirmMenu != nullptr)
    {
        this->removeChild(confirmMenu);
        confirmMenu = nullptr;
    }

    // 切换到 StartScene
    auto startScene = StartScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, startScene));
}



void GameScene::createVolumeSlider()
{
    // 如果之前存在滑动条和菜单，先移除它们
    if (volumeSlider && volumeSlider->getParent() == this)
    {
        this->removeChild(volumeSlider);
        volumeSlider = nullptr;
    }

    if (confirmMenu && confirmMenu->getParent() == this)
    {
        this->removeChild(confirmMenu);
        confirmMenu = nullptr;
    }

    // 获取窗口大小
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建音量控制Slider
    volumeSlider = ui::Slider::create();
    volumeSlider->loadBarTexture("slider_track.png");
    volumeSlider->loadSlidBallTextures("slider_thumb.png", "slider_thumb.png", "slider_thumb.png");
    volumeSlider->setPosition(Vec2(visibleSize.width / 2 + origin.x-50, visibleSize.height - 150 + origin.y));

    // 获取当前音量并设置滑动条百分比
    float initialVolume = cocos2d::experimental::AudioEngine::getVolume(audioID);
    volumeSlider->setPercent(initialVolume * 100);

    // 添加滑动事件监听
    volumeSlider->addEventListener([&](Ref* sender, ui::Slider::EventType type) {
        if (type == ui::Slider::EventType::ON_PERCENTAGE_CHANGED) {
            int percent = volumeSlider->getPercent();
            float volume = percent / 100.0f;
            if (cocos2d::experimental::AudioEngine::getState(audioID) == cocos2d::experimental::AudioEngine::AudioState::PLAYING) {
                cocos2d::experimental::AudioEngine::setVolume(audioID, volume);
            }
            else {
                CCLOG("Audio is not playing, cannot set volume");
            }

        }
        });

    this->addChild(volumeSlider, 1);

    // 创建“确定”按钮
    auto confirmLabel = Label::createWithTTF(u8"确定", "../Resources/fonts/jiantisongti.ttf", 28);
    auto confirmButton = MenuItemLabel::create(confirmLabel, [=](Ref* sender) {
        if (volumeSlider && volumeSlider->getParent() == this) {
            this->removeChild(volumeSlider);
            volumeSlider = nullptr;
        }

        if (confirmMenu && confirmMenu->getParent() == this) {
            this->removeChild(confirmMenu);
            confirmMenu = nullptr;
        }
        });

    confirmButton->setPosition(Vec2(visibleSize.width / 2 + origin.x-50, visibleSize.height - 200 + origin.y));

    // 创建菜单并添加“确定”按钮
    confirmMenu = Menu::create(confirmButton, nullptr);
    confirmMenu->setPosition(Vec2::ZERO);
    this->addChild(confirmMenu, 1);
}

//把设置界面（4个按钮）移除
void GameScene::removeSettingsMenu()
{
    // 根据名称移除菜单
    this->removeChildByName("fourbuttonmenu");
}
//生成设置按钮）
void GameScene::createSettingsMenu()
{
    // 获取窗口大小
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建“设置”按钮标签
    auto settingsLabel = Label::createWithTTF(u8"设置", "../Resources/fonts/jiantisongti.ttf", 48);

    // 创建“设置”按钮，点击时调用 showSettings
    auto settingsButton = MenuItemLabel::create(
        settingsLabel,
        CC_CALLBACK_1(GameScene::showSettings, this));

    settingsButton->setPosition(Vec2(visibleSize.width - 100 + origin.x, visibleSize.height - 50 + origin.y));
    auto menu = Menu::create(settingsButton, nullptr);
    menu->setPosition(Vec2::ZERO);

    // 将菜单添加到场景中，设置名称为 "settingsMenu"
    this->addChild(menu, 1, "settingsMenu");
}

