#include "GameScene.h"
#include "StartScene.h"
#include "audio/include/AudioEngine.h"
#include "ui/CocosGUI.h"
#include"monster.h"

//#include "SimpleAudioEngine.h"

USING_NS_CC;
#define HeroHealth 100 //Ӣ�۵�Ѫ��
#define BlocksFirstGID 4
#define MonsterNumMax 6 //����������
#define Fallwithouthurt 600 //ˤ�䲻���˺�������ٶ�
#define Undergroundpos 200 //���������ֽ��ߵ�y����
#define  UndergroundvisibileSzie 100 //���¿��Ӱ뾶

//��ͼ
cocos2d::TMXTiledMap* map;
float mapWidth;
float mapHeight;

// ͼ�����
cocos2d::TMXLayer* blocksLayer = nullptr; // ���ܴ�Խ���赲��
cocos2d::TMXLayer* chair = nullptr;  // �ɴ�Խ����Ʒ��
cocos2d::TMXLayer* toptree = nullptr;
cocos2d::TMXLayer* wood = nullptr;
cocos2d::TMXLayer* chest = nullptr;
cocos2d::TMXLayer* door = nullptr;
cocos2d::TMXLayer* desk = nullptr;

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
    /*MUSIC*/
    cocos2d::experimental::AudioEngine::stopAll();//��ֹͣ��������
    audioID = cocos2d::experimental::AudioEngine::play2d("music02.mp3", true);
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();
    

    initWithPhysics(); // �����������
    this->getPhysicsWorld()->setGravity(Vec2(0, -980)); // ��������

    auto camera = Camera::create();
    camera->setCameraFlag(CameraFlag::USER1);
    this->addChild(camera);
    camera->setName("Camera");

    // ���ؾ���֡
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("man.plist");  // ���� plist �ļ�

    auto textureCache = Director::getInstance()->getTextureCache();

    // ������ͼ
    map = TMXTiledMap::create("map2.tmx");

    // ��ȡ��ͼ�ߴ�
    const auto mapSize = map->getMapSize();
    const auto tileSize = map->getTileSize();
    mapWidth = mapSize.width * tileSize.width;
    mapHeight = mapSize.height * tileSize.height;

    if (!map)
    {
        CCLOG("Failed to load map.");
        return false;
    }
    map->setAnchorPoint(Vec2(0, 0));
    map->setScale(1.0f);
    map->setName("map"); // ��������
    map->setCameraMask((unsigned short)CameraFlag::USER1);

    // ����ͼ��
    blocksLayer = map->getLayer("blocks");
    chair = map->getLayer("chair");
    toptree = map->getLayer("toptree");
    wood = map->getLayer("wood");
    chest = map->getLayer("chest");
    door = map->getLayer("door");
    desk = map->getLayer("desk");
    dayLayer = map->getLayer("day");      // ���챳��ͼ��
    nightLayer = map->getLayer("night");  // ҹ����ͼ��

    // ��ʼʱ��ʾ���챳��
    dayLayer->setVisible(true);
    nightLayer->setVisible(false);

    // ���� ClippingNode
    clippingNode = ClippingNode::create();
    clippingNode->setContentSize(map->getContentSize()); // �������òü�����Ĵ�С

    // ����һ��Բ�ε� Stencil (����)
    auto stencil = DrawNode::create();
    float radius = UndergroundvisibileSzie; // �뾶
    Vec2 circleCenter = Vec2(0, 0); // Բ��
    stencil->drawSolidCircle(circleCenter, radius, 0, 50, Color4F(1, 1, 1, 1)); // ��ɫ��ȫ��͸����Բ��
    stencil->setCameraMask((unsigned short)CameraFlag::USER1);
    clippingNode->setStencil(stencil);
    clippingNode->addChild(map); //�ѵ�ͼ��������

    PhysicsMaterial PhyM(0, 0, 1);
    for (int x = 0; x < mapSize.width; x++)
    {
        for (int y = 0; y < mapSize.height; y++)
        {
            // ��ȡ��Ƭ
            auto tile = blocksLayer->getTileAt(Vec2(x, y));

            // �����Ƭ���ڣ��򴴽�����
            if (tile)
            {
                // �����������
                auto bodyOfBlocks = PhysicsBody::createBox(tileSize, PhyM);
                bodyOfBlocks->setDynamic(false);
                bodyOfBlocks->setCategoryBitmask(0x06);    
                bodyOfBlocks->setContactTestBitmask(0x01);
                // �����帽�ӵ���Ƭ�ڵ�
                tile->setPhysicsBody(bodyOfBlocks);
            }
        }
    }

    //��ӿ���ǽ
    auto airWallD = Node::create();
    auto wallBodyD = PhysicsBody::createBox(Size(mapWidth, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));
    wallBodyD->setDynamic(false); // ��̬���壬�����ƶ�
    airWallD->setPhysicsBody(wallBodyD);
    airWallD->setPosition(Vec2(visibleSize.width / 2, (visibleSize.height - mapHeight) / 2));
    this->addChild(airWallD);

    auto airWallU = Node::create();
    auto wallBodyU = PhysicsBody::createBox(Size(mapWidth, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));
    wallBodyU->setDynamic(false); // ��̬���壬�����ƶ�
    airWallU->setPhysicsBody(wallBodyU);
    airWallU->setPosition(Vec2(visibleSize.width / 2, (visibleSize.height + mapHeight) / 2));
    this->addChild(airWallU);

    // ��������
    hero = Hero::create("man1.png", HeroHealth);
    if (!hero)
    {
        CCLOG("Failed to load hero.");
        return false;
    }
    hero->addPhy();
    hero->addhealthbar(); //���Ѫ��
    hero->setCameraMask((unsigned short)CameraFlag::USER1);
    // ���ǳ�ʼλ������Ϊ��Ļ����
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    hero->setName("hero"); // ��������
    this->addChild(hero, 1); // ��������ӵ������������ǵ�ͼ
    // ������Ʒ����ʼ��
    hero->iniItems();

    // ��ʼ����ͼλ��
    auto mapPosition = Vec2((visibleSize.width - mapWidth) / 2, (visibleSize.height - mapHeight) / 2);
    map->setPosition(mapPosition);

    // ��Ӽ��̲���
    hero->addKeyboardListener();
    // ���������
    hero->addTouchListener();
    hero->addTouchListenerAttack();

    hero->schedule(CC_SCHEDULE_SELECTOR(Hero::HeroFunc)); //���ǵ��˶�

    // ��ʼ��̫��
    auto sun = Sprite::create("Sun.png");
    sun->setScale(0.5f);  // ������С
    sun->setCameraMask((unsigned short)CameraFlag::USER1);
    sun->setPosition(0, visibleSize.height * 0.85f);
    clippingNode->addChild(sun, 10, "sun");

    // ��ʼ������
    auto moon = Sprite::create("Moon.png");
    moon->setScale(0.5f);  // ������С
    moon->setCameraMask((unsigned short)CameraFlag::USER1);
    moon->setPosition(0, visibleSize.height * 0.85f);
    moon->setVisible(false);
    clippingNode->addChild(moon, 10, "moon"); 

    this->createSettingsMenu();//���Ͻ���ʾ���ð�ť

    this->addChild(clippingNode); 

    /********�й����*******/
    this->schedule(CC_SCHEDULE_SELECTOR(GameScene::MonsterFunc), 5.0f); //������������˶�
    this->schedule(CC_SCHEDULE_SELECTOR(GameScene::NPCFunc), 1.0f); //npc���������˶�

     // ���������ײ������
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::Falljudgement, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    /*�����ӽ�λ��*/
    this->schedule([=](float delta) {
        updateCameraPosition(hero);
        }, "update_camera");

    // ��̬��������λ�ú�״̬
    schedule([=](float dt) {
        Vec2 Pos = hero->getPosition();
        const Size visibleSize = Director::getInstance()->getVisibleSize();
        stencil->setPosition(Pos);  // ���ָ�������
        if (Pos.y < Undergroundpos) {
            clippingNode->setInverted(false);  // ����ʹ������
            stencil->setVisible(true);
        }
        else {
            clippingNode->setInverted(true);  // ��ת���֣�Ч��Ϊ����
            stencil->setVisible(false);
        }

        }, "update_mask");

    // ����������µ���
    this->schedule([=](float delta) {
        updateWeather(delta);  // ��������
        }, 1.0f / 60.0f, "weather_update_key");

    return true;
}


/*�õ���Ƭ����*/
Vec2 getTileCoordForPosition(float x, float y)
{
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    // ��ȡ��ͼ��Tile��С
    Size tileSize = map->getTileSize();
    Vec2 mapOrigin = map->getPosition();

    // ����������ת��Ϊ��ͼ����
    float mapX = x - mapOrigin.x;
    float mapY = y - mapOrigin.y;

    // ������Ƭ����
    int tileX = mapX / tileSize.width;
    int tileY = (map->getMapSize().height * tileSize.height - mapY) / tileSize.height;

    return Vec2(tileX, tileY);
    
}

/*�ж�����Ƭ�Ƿ�����ײ*/
int checkBlockCollision(float x, float y)
{
    // ��ȡ��Ƭ����
    Vec2 tileCoord = getTileCoordForPosition(x, y);

    //ȡ��ͼ��Ĵ�С
    Size layerSize = blocksLayer->getLayerSize();

    if (tileCoord.x >= 0 && tileCoord.x < layerSize.width && tileCoord.y >= 0 && tileCoord.y < layerSize.height) { //�ж���Ƭ��λ���Ƿ�Ϸ�
        // ���ȼ�� blocksLayer �ϵ���Ƭ GID
        int blockGID = blocksLayer->getTileGIDAt(tileCoord);

        if (blockGID >= BlocksFirstGID) // ����Ƿ��飨GID >= BlocksFirstGID��
        {
            return blockGID; // ���ط���� GID����ʾ�����ϰ���
        }
        else {
            return 0; // û����ײ
        }
    }
    return BlocksFirstGID;
}

/*�����ӽǵĺ���*/
void GameScene::updateCameraPosition(Sprite* player) {
    auto camera = this->getChildByName<Camera*>("Camera");
    auto playerPosition = player->getPosition();

    // �������������������λ��
    camera->setPosition(Vec2(playerPosition.x, playerPosition.y));
}

/*void GameScene::addPhysicsBodiesForVisibleTiles() {
    auto tileMap = map;
    auto collidableLayer = blocksLayer;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // ��ȡ��Ļ��Ұ��Χ
    Rect visibleRect = Rect(origin.x, origin.y, visibleSize.width, visibleSize.height);

    // ������ͼ�е�������Ƭ
    for (int x = 0; x < tileMap->getMapSize().width; ++x) {
        for (int y = 0; y < tileMap->getMapSize().height; ++y) {
            auto tilePos = Vec2(x, y);
            auto tileSprite = collidableLayer->getTileAt(tilePos);
            if (!tileSprite) continue; // ������Ч��Ƭ

            // ��ȡ��Ƭ�İ�Χ��
            auto tileBoundingBox = tileSprite->getBoundingBox();

            // ����Ƿ�����Ұ��Χ��
            if (visibleRect.intersectsRect(tileBoundingBox)) {
                // �����Ƭû��������壬����������
                if (!tileSprite->getPhysicsBody()) {
                    auto physicsBody = PhysicsBody::createBox(tileBoundingBox.size, PhysicsMaterial(1e10, 0, 1));
                    physicsBody->setDynamic(false); // ��̬����
                    tileSprite->addComponent(physicsBody);
                }
            }
            else {
                // �����Ƭ������Ұ��Χ�ڣ��Ƴ��������
                if (tileSprite->getPhysicsBody()) {
                    tileSprite->removeComponent(tileSprite->getPhysicsBody());
                }
            }
        }
    }
}*/

void GameScene::updateWeather(float deltaTime)
{
    // ������ʹ��timeOfDay���жϰ����ҹ��
    timeOfDay += deltaTime;
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // �趨̫���������ĸ߶ȣ���������Ļ�Ϸ��Ĺ̶��߶ȣ�
    float height = visibleSize.height * 0.85f;  // �̶�����Ļ�� 80% �߶�
    if (timeOfDay >= 24)
    {
        timeOfDay = 0;
        isDay = !isDay;
        // ��ȡ̫��������������
        auto sun = clippingNode->getChildByName<Sprite*>("sun"); 
        auto moon = clippingNode->getChildByName<Sprite*>("moon"); 
        sun->setPosition(0, visibleSize.height * 0.85f);
        moon->setPosition(0, visibleSize.height * 0.85f);
        changeBackground();
    }

    updateSunAndMoonPosition(deltaTime);  // ����̫����������λ��
}

void GameScene::changeBackground()
{
    if (isDay)
    {
        dayLayer->setVisible(true);  // ��ʾ���챳��
        nightLayer->setVisible(false);  // ����ҹ����
    }
    else
    {
        dayLayer->setVisible(false);  // ���ذ��챳��
        nightLayer->setVisible(true);  // ��ʾҹ����
    }
}

void GameScene::updateSunAndMoonPosition(float deltaTime)
{
    // ��ȡ̫��������������
    auto sun = clippingNode->getChildByName<Sprite*>("sun");
    auto moon = clippingNode->getChildByName<Sprite*>("moon");

    // ��ȡ��ͼ��ƫ�������������Խ�̫��������λ������ڵ�ͼ������
    Vec2 mapPosition = this->getPosition();  // ��ȡ��ǰ��ͼλ��
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // �趨̫���������ĸ߶ȣ���������Ļ�Ϸ��Ĺ̶��߶ȣ�
    float height = visibleSize.height * 0.85f;  // �̶�����Ļ�� 80% �߶�

    Vec2 sunPosition = sun->getPosition();
    Vec2 moonPosition = moon->getPosition();

    if (isDay)
    {
        sun->setVisible(true);
        moon->setVisible(false);
        sun->setPosition(sunPosition.x + 50 * deltaTime, height);
    }
    else
    {
        moon->setVisible(true);
        sun->setVisible(false);
        moon->setPosition(moonPosition.x + 50 * deltaTime, height);
    }
}


void GameScene::showSettings(cocos2d::Ref* sender)
{
    CCLOG("Settings button clicked!");
    auto camera = this->getChildByName<Camera*>("Camera");
    Vec2 cameraPosition = camera->getPosition();

    // �Ƴ������á���ť���ڵĲ˵�
    hero->removeChildByName("settingsMenu");

    // ��ȡ���ڴ�С
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();


    // �������ص����˵�����ť
    auto backLabel = Label::createWithTTF(u8"�ص����˵�", "../Resources/fonts/jiantisongti.ttf", 32);
    auto backButton = MenuItemLabel::create(backLabel, CC_CALLBACK_1(GameScene::exitGame, this));
    backButton->setPosition(Vec2(0.5 * visibleSize.width - 100 + origin.x, 0.5 * visibleSize.height - 50 + origin.y));

    // ������ť1
    auto button1Label = Label::createWithTTF(u8"��������", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button1 = MenuItemLabel::create(button1Label, [=](Ref* sender) {
        // �����ť1ʱ���� createVolumeSlider ����
        createVolumeSlider();  // ������ʾ�������ĺ���
        });
    button1->setPosition(Vec2(0.5 * visibleSize.width - 100 + origin.x, 0.5 * visibleSize.height - 50 + origin.y - 50));
    button1->setName("volumn button");  // ����ť1��������

    // �������ذ�ť
    auto button4Label = Label::createWithTTF(u8"����", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button4 = MenuItemLabel::create(button4Label, [this](Ref* sender) {
        CCLOG("���ذ�ť�����!");
        this->removeSettingsMenu();  // �����Ƴ�����
        this->createSettingsMenu();  // �����������ò˵�
        });
    button4->setPosition(Vec2(0.5 * visibleSize.width - 100 + origin.x, 0.5 * visibleSize.height - 50 + origin.y - 100));

    // �����˵�����Ӱ�ť
    auto backMenu = Menu::create(backButton, button1, button4, nullptr);
    backMenu->setPosition(Vec2::ZERO); // ���˵���ԭ������Ϊ(0, 0)

    // ���˵���ӵ������У���������Ϊ "fourbuttonmenu"
    hero->addChild(backMenu, 50, "fourbuttonmenu");
    backMenu->setCameraMask((unsigned short)CameraFlag::USER1);

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
    if (volumeSlider && volumeSlider->getParent() == hero)
    {
        hero->removeChildByName("volumeSlider");
        volumeSlider = nullptr;
    }

    if (confirmMenu && confirmMenu->getParent() == hero)
    {
        hero->removeChildByName("");
        confirmMenu = nullptr;
    }

    // ��ȡ���ڴ�С
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ������������Slider
    volumeSlider = ui::Slider::create();
    volumeSlider->loadBarTexture("slider_track.png");
    volumeSlider->loadSlidBallTextures("slider_thumb.png", "slider_thumb.png", "slider_thumb.png");
    volumeSlider->setPosition(Vec2(0, 200));
    volumeSlider->setName("volumeSlider");
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
    volumeSlider->setCameraMask((unsigned short)CameraFlag::USER1);
    hero->addChild(volumeSlider, 50);

    // ������ȷ������ť
    auto confirmLabel = Label::createWithTTF(u8"ȷ��", "../Resources/fonts/jiantisongti.ttf", 28);
    auto confirmButton = MenuItemLabel::create(confirmLabel, [=](Ref* sender) {
        if (volumeSlider && volumeSlider->getParent() == hero) {
            hero->removeChild(volumeSlider, 1);
            volumeSlider = nullptr;
        }

        if (confirmMenu && confirmMenu->getParent() == hero) {
            hero->removeChildByName("confirmMenu");
            confirmMenu = nullptr;
        }
        });

    confirmButton->setPosition(Vec2(0, 100));

    // �����˵�����ӡ�ȷ������ť
    confirmMenu = Menu::create(confirmButton, nullptr);
    confirmMenu->setPosition(Vec2::ZERO);
    confirmMenu->setCameraMask((unsigned short)CameraFlag::USER1);
    confirmMenu->setName("confirmMenu");
    hero->addChild(confirmMenu, 50);
}

//�����ý��棨4����ť���Ƴ�
void GameScene::removeSettingsMenu()
{
    // ���������Ƴ��˵�
    hero->removeChildByName("fourbuttonmenu");
}

//�������ð�ť
void GameScene::createSettingsMenu()
{
    // ��ȡ���ڴ�С
    const Size visibleSize = Director::getInstance()->getVisibleSize();

    // ���������á���ť��ǩ
    auto settingsLabel = Label::createWithTTF(u8"����", "../Resources/fonts/jiantisongti.ttf", 48);

    // ���������á���ť�����ʱ���� showSettings
    auto settingsButton = MenuItemLabel::create(
        settingsLabel,
        CC_CALLBACK_1(GameScene::showSettings, this));


    auto menu = Menu::create(settingsButton, nullptr);

    Vec2 adjustedPosition = Vec2(0.5 * visibleSize.width - 100, 0.5 * visibleSize.height - 50);
    settingsButton->setPosition(adjustedPosition);
    menu->setPosition(Vec2::ZERO);
    // ���˵���ӵ������У���������Ϊ "settingsMenu"
    hero->addChild(menu, 50, "settingsMenu");
    menu->setCameraMask((unsigned short)CameraFlag::USER1);
}

//*********�й����

/*���������й���ĺ���*/
void GameScene::MonsterFunc(float dt) {
    // ��ȡ��Ļ��С
    Size visibleSize = Director::getInstance()->getVisibleSize();

    if (numberOfMonsters < MonsterNumMax) {
        int choice = RandomHelper::random_int(0, 1); //ѡ������ʷ��ķ���ǽ�ʬ
        if (choice == 0) {
            auto silme = Silme::create("slime.png", 100);
            float x_S = RandomHelper::random_real((visibleSize.width - mapWidth) / 2 + silme->getContentSize().width / 2, (visibleSize.width + mapWidth) / 2 - silme->getContentSize().width / 2); //�����������
            silme->setPosition(Vec2(x_S, visibleSize.height / 1.5f));
            silme->addPhy();
            silme->addjellyeffect();
            silme->addhealthbar();
            clippingNode->addChild(silme); 
            silme->setCameraMask((unsigned short)CameraFlag::USER1);
            silme->schedule(CC_SCHEDULE_SELECTOR(Silme::silme_move), 3);
            silme->schedule(CC_SCHEDULE_SELECTOR(Silme::brodercheck));
            silme->schedule(CC_SCHEDULE_SELECTOR(Silme::AttackPlayer), 1, -1, 0);
        }
        if (choice == 1) {
            auto zombie = Zombie::create("zombie1.png", 100);
            float x_Z = RandomHelper::random_real((visibleSize.width - mapWidth) / 2 + zombie->getContentSize().width / 2, (visibleSize.width + mapWidth) / 2 - zombie->getContentSize().width / 2); //�����������
            zombie->setPosition(Vec2(x_Z, visibleSize.height / 1.5f));
            zombie->addPhy();
            zombie->addAnimate();
            zombie->addhealthbar();
            clippingNode->addChild(zombie); 
            zombie->setCameraMask((unsigned short)CameraFlag::USER1);
            zombie->schedule(CC_SCHEDULE_SELECTOR(Zombie::zombie_move));
            zombie->schedule(CC_SCHEDULE_SELECTOR(Zombie::jump), 1);
            zombie->schedule(CC_SCHEDULE_SELECTOR(Zombie::AttackPlayer), 1, -1, 0);
        }
        numberOfMonsters++;
    }
}

void GameScene::NPCFunc(float dt) {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    if (numberOfnpcs < 1) {
        // ���� NPC
        auto npc1 = NPC::create("npc1.png", 100);

        // ������� NPC ��λ��
        float x_Z = RandomHelper::random_real((visibleSize.width - mapWidth) / 2 + npc1->getContentSize().width / 2, (visibleSize.width + mapWidth) / 2 - npc1->getContentSize().width / 2);
        npc1->setPosition(Vec2(x_Z, visibleSize.height / 1.5f));

        // �������Ч��
        npc1->addPhy();

        //�Ӷ���Ч��
        npc1->addAnimate();

        // �� NPC ���뵽������
        clippingNode->addChild(npc1);

        // ���� NPC �����
        npc1->setCameraMask((unsigned short)CameraFlag::USER1);

        // ��ʼ NPC ���˶�
        npc1->schedule(CC_SCHEDULE_SELECTOR(NPC::npc_move));
        npc1->schedule(CC_SCHEDULE_SELECTOR(NPC::jump), 1);
        npc1->schedule(CC_SCHEDULE_SELECTOR(NPC::approachplayer), 1, -1, 0);

        // ���ô���������������Ƿ��� NPC ����ʾ�����˵�
        npc1->addTouchListener();
    }
    // ���� NPC ����
    numberOfnpcs++;
}

bool GameScene::Falljudgement(PhysicsContact& contact) {
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();

    // ��������������ײ
    if ((bodyA->getCategoryBitmask() == 0x01) || (bodyB->getCategoryBitmask() == 0x01)) {

        // ��ȡ���Ǹ���
        auto heroBody = (bodyA->getCategoryBitmask() == 0x01) ? bodyA : bodyB;

        // ��ȡ���ǵ������ٶ�
        float fallSpeed = heroBody->getVelocity().y;

        // ����ٶȳ�����ֵ�������˺�
        if (fallSpeed < -Fallwithouthurt) {
            auto node = heroBody->getNode();
            auto hero = dynamic_cast<Hero*>(node); // �� Node תΪ Hero ����
            hero->applyFallDamage(fallSpeed);
        }
    }
    return true;
}