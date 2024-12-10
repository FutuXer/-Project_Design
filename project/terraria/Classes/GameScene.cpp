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

    cocos2d::experimental::AudioEngine::stopAll();//��ֹͣ��������

    audioID = cocos2d::experimental::AudioEngine::play2d("music.mp3", true);
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();


    this->createSettingsMenu();//���Ͻ���ʾ���ð�ť
    return true;
}

void GameScene::showSettings(cocos2d::Ref* sender) {
    // �Ƴ������á���ť���ڵĲ˵�
    this->removeChildByName("settingsMenu");

    // ��ȡ���ڴ�С
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ��ʼ��ťλ��
    float initialY = visibleSize.height - 50 + origin.y;

    // �������ص����˵�����ť
    auto backLabel = Label::createWithTTF(u8"�ص����˵�", "../Resources/fonts/jiantisongti.ttf", 32);
    auto backButton = MenuItemLabel::create(backLabel, CC_CALLBACK_1(GameScene::exitGame, this));
    backButton->setPosition(Vec2(visibleSize.width - 100 + origin.x, initialY));

    // ������ť1
    auto button1Label = Label::createWithTTF(u8"��������", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button1 = MenuItemLabel::create(button1Label, [=](Ref* sender) {
        // �����ť1ʱ���� createVolumeSlider ����
        createVolumeSlider();  // ������ʾ�������ĺ���
        });
    button1->setPosition(Vec2(visibleSize.width - 100 + origin.x, initialY - 50));
    button1->setName("volumn button");  // ����ť1��������

    // ������ť2
    auto button2Label = Label::createWithTTF(u8"��ť2", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button2 = MenuItemLabel::create(button2Label, [](Ref* sender) { CCLOG("��ť2�����!"); });
    button2->setPosition(Vec2(visibleSize.width - 100 + origin.x, initialY - 100));
    button2->setName("button2");  // ����ť2��������

    // ������ť3
    auto button3Label = Label::createWithTTF(u8"��ť3", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button3 = MenuItemLabel::create(button3Label, [](Ref* sender) { CCLOG("��ť3�����!"); });
    button3->setPosition(Vec2(visibleSize.width - 100 + origin.x, initialY - 150));
    button3->setName("button3");  // ����ť3��������

    // �������ذ�ť
    auto button4Label = Label::createWithTTF(u8"����", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button4 = MenuItemLabel::create(button4Label, [this](Ref* sender) {
        CCLOG("���ذ�ť�����!");
        this->removeSettingsMenu();  // �����Ƴ�����
        this->createSettingsMenu();
        });
    button4->setPosition(Vec2(visibleSize.width - 100 + origin.x, initialY - 200));


    // �����˵�����Ӱ�ť
    auto backMenu = Menu::create(backButton, button1, button2, button3, button4, nullptr);
    backMenu->setPosition(Vec2::ZERO); // ���˵���ԭ������Ϊ(0, 0)
    this->addChild(backMenu, 1,"fourbuttonmenu");  // ��Ӳ˵���������
}


void GameScene::exitGame(cocos2d::Ref* sender)
{
    // ȷ���˳�ǰ�Ƴ��������͡�ȷ������ť
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

    // �л��� StartScene
    auto startScene = StartScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, startScene));
}



void GameScene::createVolumeSlider()
{
    // ���֮ǰ���ڻ������Ͳ˵������Ƴ�����
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

    // ��ȡ���ڴ�С
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ������������Slider
    volumeSlider = ui::Slider::create();
    volumeSlider->loadBarTexture("slider_track.png");
    volumeSlider->loadSlidBallTextures("slider_thumb.png", "slider_thumb.png", "slider_thumb.png");
    volumeSlider->setPosition(Vec2(visibleSize.width / 2 + origin.x-50, visibleSize.height - 150 + origin.y));

    // ��ȡ��ǰ���������û������ٷֱ�
    float initialVolume = cocos2d::experimental::AudioEngine::getVolume(audioID);
    volumeSlider->setPercent(initialVolume * 100);

    // ��ӻ����¼�����
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

    // ������ȷ������ť
    auto confirmLabel = Label::createWithTTF(u8"ȷ��", "../Resources/fonts/jiantisongti.ttf", 28);
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

    // �����˵�����ӡ�ȷ������ť
    confirmMenu = Menu::create(confirmButton, nullptr);
    confirmMenu->setPosition(Vec2::ZERO);
    this->addChild(confirmMenu, 1);
}

//�����ý��棨4����ť���Ƴ�
void GameScene::removeSettingsMenu()
{
    // ���������Ƴ��˵�
    this->removeChildByName("fourbuttonmenu");
}
//�������ð�ť��
void GameScene::createSettingsMenu()
{
    // ��ȡ���ڴ�С
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ���������á���ť��ǩ
    auto settingsLabel = Label::createWithTTF(u8"����", "../Resources/fonts/jiantisongti.ttf", 48);

    // ���������á���ť�����ʱ���� showSettings
    auto settingsButton = MenuItemLabel::create(
        settingsLabel,
        CC_CALLBACK_1(GameScene::showSettings, this));

    settingsButton->setPosition(Vec2(visibleSize.width - 100 + origin.x, visibleSize.height - 50 + origin.y));
    auto menu = Menu::create(settingsButton, nullptr);
    menu->setPosition(Vec2::ZERO);

    // ���˵���ӵ������У���������Ϊ "settingsMenu"
    this->addChild(menu, 1, "settingsMenu");
}

