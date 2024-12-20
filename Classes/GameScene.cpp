#include "GameScene.h"
#include "StartScene.h"
#include "audio/include/AudioEngine.h"
#include "ui/CocosGUI.h"
#include"monster.h"

//#include "SimpleAudioEngine.h"

USING_NS_CC;
#define HeroHealth 100 //英雄的血量
#define BlocksFirstGID 4
#define MonsterNumMax 6 //最大怪物数量
#define Fallwithouthurt 600 //摔落不受伤害的最大速度
#define Undergroundpos 200 //地下与地面分界线的y坐标
#define  UndergroundvisibileSzie 100 //地下可视半径

//地图
cocos2d::TMXTiledMap* map;
float mapWidth;
float mapHeight;

// 图层相关
cocos2d::TMXLayer* blocksLayer = nullptr; // 不能穿越的阻挡层
cocos2d::TMXLayer* chair = nullptr;  // 可穿越的物品层
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
    cocos2d::experimental::AudioEngine::stopAll();//先停止所有音乐
    audioID = cocos2d::experimental::AudioEngine::play2d("music02.mp3", true);
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();
    

    initWithPhysics(); // 引入物理组件
    this->getPhysicsWorld()->setGravity(Vec2(0, -980)); // 设置重力

    auto camera = Camera::create();
    camera->setCameraFlag(CameraFlag::USER1);
    this->addChild(camera);
    camera->setName("Camera");

    // 加载精灵帧
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("man.plist");  // 加载 plist 文件

    auto textureCache = Director::getInstance()->getTextureCache();

    // 创建地图
    map = TMXTiledMap::create("map2.tmx");

    // 获取地图尺寸
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
    map->setName("map"); // 设置名称
    map->setCameraMask((unsigned short)CameraFlag::USER1);

    // 加载图层
    blocksLayer = map->getLayer("blocks");
    chair = map->getLayer("chair");
    toptree = map->getLayer("toptree");
    wood = map->getLayer("wood");
    chest = map->getLayer("chest");
    door = map->getLayer("door");
    desk = map->getLayer("desk");
    dayLayer = map->getLayer("day");      // 白天背景图层
    nightLayer = map->getLayer("night");  // 夜晚背景图层

    // 初始时显示白天背景
    dayLayer->setVisible(true);
    nightLayer->setVisible(false);

    // 创建 ClippingNode
    clippingNode = ClippingNode::create();
    clippingNode->setContentSize(map->getContentSize()); // 可以设置裁剪区域的大小

    // 创建一个圆形的 Stencil (遮罩)
    auto stencil = DrawNode::create();
    float radius = UndergroundvisibileSzie; // 半径
    Vec2 circleCenter = Vec2(0, 0); // 圆心
    stencil->drawSolidCircle(circleCenter, radius, 0, 50, Color4F(1, 1, 1, 1)); // 白色完全不透明的圆形
    stencil->setCameraMask((unsigned short)CameraFlag::USER1);
    clippingNode->setStencil(stencil);
    clippingNode->addChild(map); //把地图加入遮罩

    PhysicsMaterial PhyM(0, 0, 1);
    for (int x = 0; x < mapSize.width; x++)
    {
        for (int y = 0; y < mapSize.height; y++)
        {
            // 获取瓦片
            auto tile = blocksLayer->getTileAt(Vec2(x, y));

            // 如果瓦片存在，则创建刚体
            if (tile)
            {
                // 创建物理刚体
                auto bodyOfBlocks = PhysicsBody::createBox(tileSize, PhyM);
                bodyOfBlocks->setDynamic(false);
                bodyOfBlocks->setCategoryBitmask(0x06);    
                bodyOfBlocks->setContactTestBitmask(0x01);
                // 将刚体附加到瓦片节点
                tile->setPhysicsBody(bodyOfBlocks);
            }
        }
    }

    //添加空气墙
    auto airWallD = Node::create();
    auto wallBodyD = PhysicsBody::createBox(Size(mapWidth, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));
    wallBodyD->setDynamic(false); // 静态物体，不会移动
    airWallD->setPhysicsBody(wallBodyD);
    airWallD->setPosition(Vec2(visibleSize.width / 2, (visibleSize.height - mapHeight) / 2));
    this->addChild(airWallD);

    auto airWallU = Node::create();
    auto wallBodyU = PhysicsBody::createBox(Size(mapWidth, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));
    wallBodyU->setDynamic(false); // 静态物体，不会移动
    airWallU->setPhysicsBody(wallBodyU);
    airWallU->setPosition(Vec2(visibleSize.width / 2, (visibleSize.height + mapHeight) / 2));
    this->addChild(airWallU);

    // 创建主角
    hero = Hero::create("man1.png", HeroHealth);
    if (!hero)
    {
        CCLOG("Failed to load hero.");
        return false;
    }
    hero->addPhy();
    hero->addhealthbar(); //添加血条
    hero->setCameraMask((unsigned short)CameraFlag::USER1);
    // 主角初始位置设置为屏幕中心
    hero->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    hero->setName("hero"); // 设置名称
    this->addChild(hero, 1); // 将主角添加到场景，而不是地图
    // 人物物品栏初始化
    hero->iniItems();

    // 初始化地图位置
    auto mapPosition = Vec2((visibleSize.width - mapWidth) / 2, (visibleSize.height - mapHeight) / 2);
    map->setPosition(mapPosition);

    // 添加键盘操作
    hero->addKeyboardListener();
    // 添加鼠标操作
    hero->addTouchListener();
    hero->addTouchListenerAttack();

    hero->schedule(CC_SCHEDULE_SELECTOR(Hero::HeroFunc)); //主角的运动

    // 初始化太阳
    auto sun = Sprite::create("Sun.png");
    sun->setScale(0.5f);  // 调整大小
    sun->setCameraMask((unsigned short)CameraFlag::USER1);
    sun->setPosition(0, visibleSize.height * 0.85f);
    clippingNode->addChild(sun, 10, "sun");

    // 初始化月亮
    auto moon = Sprite::create("Moon.png");
    moon->setScale(0.5f);  // 调整大小
    moon->setCameraMask((unsigned short)CameraFlag::USER1);
    moon->setPosition(0, visibleSize.height * 0.85f);
    moon->setVisible(false);
    clippingNode->addChild(moon, 10, "moon"); 

    this->createSettingsMenu();//右上角显示设置按钮

    this->addChild(clippingNode); 

    /********敌怪相关*******/
    this->schedule(CC_SCHEDULE_SELECTOR(GameScene::MonsterFunc), 5.0f); //怪物的生成与运动
    this->schedule(CC_SCHEDULE_SELECTOR(GameScene::NPCFunc), 1.0f); //npc的生成与运动

     // 添加物理碰撞监听器
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::Falljudgement, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    /*更新视角位置*/
    this->schedule([=](float delta) {
        updateCameraPosition(hero);
        }, "update_camera");

    // 动态更新遮罩位置和状态
    schedule([=](float dt) {
        Vec2 Pos = hero->getPosition();
        const Size visibleSize = Director::getInstance()->getVisibleSize();
        stencil->setPosition(Pos);  // 遮罩跟随主角
        if (Pos.y < Undergroundpos) {
            clippingNode->setInverted(false);  // 政策使用遮罩
            stencil->setVisible(true);
        }
        else {
            clippingNode->setInverted(true);  // 反转遮罩，效果为禁用
            stencil->setVisible(false);
        }

        }, "update_mask");

    // 添加天气更新调度
    this->schedule([=](float delta) {
        updateWeather(delta);  // 更新天气
        }, 1.0f / 60.0f, "weather_update_key");

    return true;
}


/*得到瓦片坐标*/
Vec2 getTileCoordForPosition(float x, float y)
{
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    // 获取地图的Tile大小
    Size tileSize = map->getTileSize();
    Vec2 mapOrigin = map->getPosition();

    // 将世界坐标转化为地图坐标
    float mapX = x - mapOrigin.x;
    float mapY = y - mapOrigin.y;

    // 计算瓦片坐标
    int tileX = mapX / tileSize.width;
    int tileY = (map->getMapSize().height * tileSize.height - mapY) / tileSize.height;

    return Vec2(tileX, tileY);
    
}

/*判断与瓦片是否发生碰撞*/
int checkBlockCollision(float x, float y)
{
    // 获取瓦片坐标
    Vec2 tileCoord = getTileCoordForPosition(x, y);

    //取得图层的大小
    Size layerSize = blocksLayer->getLayerSize();

    if (tileCoord.x >= 0 && tileCoord.x < layerSize.width && tileCoord.y >= 0 && tileCoord.y < layerSize.height) { //判断瓦片的位置是否合法
        // 首先检查 blocksLayer 上的瓦片 GID
        int blockGID = blocksLayer->getTileGIDAt(tileCoord);

        if (blockGID >= BlocksFirstGID) // 如果是方块（GID >= BlocksFirstGID）
        {
            return blockGID; // 返回方块的 GID，表示碰到障碍物
        }
        else {
            return 0; // 没有碰撞
        }
    }
    return BlocksFirstGID;
}

/*更新视角的函数*/
void GameScene::updateCameraPosition(Sprite* player) {
    auto camera = this->getChildByName<Camera*>("Camera");
    auto playerPosition = player->getPosition();

    // 保持摄像机中心在主角位置
    camera->setPosition(Vec2(playerPosition.x, playerPosition.y));
}

/*void GameScene::addPhysicsBodiesForVisibleTiles() {
    auto tileMap = map;
    auto collidableLayer = blocksLayer;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 获取屏幕视野范围
    Rect visibleRect = Rect(origin.x, origin.y, visibleSize.width, visibleSize.height);

    // 遍历地图中的所有瓦片
    for (int x = 0; x < tileMap->getMapSize().width; ++x) {
        for (int y = 0; y < tileMap->getMapSize().height; ++y) {
            auto tilePos = Vec2(x, y);
            auto tileSprite = collidableLayer->getTileAt(tilePos);
            if (!tileSprite) continue; // 跳过无效瓦片

            // 获取瓦片的包围框
            auto tileBoundingBox = tileSprite->getBoundingBox();

            // 检查是否在视野范围内
            if (visibleRect.intersectsRect(tileBoundingBox)) {
                // 如果瓦片没有物理刚体，添加物理刚体
                if (!tileSprite->getPhysicsBody()) {
                    auto physicsBody = PhysicsBody::createBox(tileBoundingBox.size, PhysicsMaterial(1e10, 0, 1));
                    physicsBody->setDynamic(false); // 静态刚体
                    tileSprite->addComponent(physicsBody);
                }
            }
            else {
                // 如果瓦片不在视野范围内，移除物理刚体
                if (tileSprite->getPhysicsBody()) {
                    tileSprite->removeComponent(tileSprite->getPhysicsBody());
                }
            }
        }
    }
}*/

void GameScene::updateWeather(float deltaTime)
{
    // 假设你使用timeOfDay来判断白天和夜晚
    timeOfDay += deltaTime;
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // 设定太阳和月亮的高度（保持在屏幕上方的固定高度）
    float height = visibleSize.height * 0.85f;  // 固定在屏幕的 80% 高度
    if (timeOfDay >= 24)
    {
        timeOfDay = 0;
        isDay = !isDay;
        // 获取太阳和月亮的引用
        auto sun = clippingNode->getChildByName<Sprite*>("sun"); 
        auto moon = clippingNode->getChildByName<Sprite*>("moon"); 
        sun->setPosition(0, visibleSize.height * 0.85f);
        moon->setPosition(0, visibleSize.height * 0.85f);
        changeBackground();
    }

    updateSunAndMoonPosition(deltaTime);  // 更新太阳和月亮的位置
}

void GameScene::changeBackground()
{
    if (isDay)
    {
        dayLayer->setVisible(true);  // 显示白天背景
        nightLayer->setVisible(false);  // 隐藏夜晚背景
    }
    else
    {
        dayLayer->setVisible(false);  // 隐藏白天背景
        nightLayer->setVisible(true);  // 显示夜晚背景
    }
}

void GameScene::updateSunAndMoonPosition(float deltaTime)
{
    // 获取太阳和月亮的引用
    auto sun = clippingNode->getChildByName<Sprite*>("sun");
    auto moon = clippingNode->getChildByName<Sprite*>("moon");

    // 获取地图的偏移量，这样可以将太阳和月亮位置相对于地图来调整
    Vec2 mapPosition = this->getPosition();  // 获取当前地图位置
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // 设定太阳和月亮的高度（保持在屏幕上方的固定高度）
    float height = visibleSize.height * 0.85f;  // 固定在屏幕的 80% 高度

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

    // 移除“设置”按钮所在的菜单
    hero->removeChildByName("settingsMenu");

    // 获取窗口大小
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();


    // 创建“回到主菜单”按钮
    auto backLabel = Label::createWithTTF(u8"回到主菜单", "../Resources/fonts/jiantisongti.ttf", 32);
    auto backButton = MenuItemLabel::create(backLabel, CC_CALLBACK_1(GameScene::exitGame, this));
    backButton->setPosition(Vec2(0.5 * visibleSize.width - 100 + origin.x, 0.5 * visibleSize.height - 50 + origin.y));

    // 创建按钮1
    auto button1Label = Label::createWithTTF(u8"调节音量", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button1 = MenuItemLabel::create(button1Label, [=](Ref* sender) {
        // 点击按钮1时调用 createVolumeSlider 函数
        createVolumeSlider();  // 调用显示进度条的函数
        });
    button1->setPosition(Vec2(0.5 * visibleSize.width - 100 + origin.x, 0.5 * visibleSize.height - 50 + origin.y - 50));
    button1->setName("volumn button");  // 给按钮1设置名字

    // 创建返回按钮
    auto button4Label = Label::createWithTTF(u8"返回", "../Resources/fonts/jiantisongti.ttf", 32);
    auto button4 = MenuItemLabel::create(button4Label, [this](Ref* sender) {
        CCLOG("返回按钮被点击!");
        this->removeSettingsMenu();  // 调用移除函数
        this->createSettingsMenu();  // 重新生成设置菜单
        });
    button4->setPosition(Vec2(0.5 * visibleSize.width - 100 + origin.x, 0.5 * visibleSize.height - 50 + origin.y - 100));

    // 创建菜单并添加按钮
    auto backMenu = Menu::create(backButton, button1, button4, nullptr);
    backMenu->setPosition(Vec2::ZERO); // 将菜单的原点设置为(0, 0)

    // 将菜单添加到场景中，设置名称为 "fourbuttonmenu"
    hero->addChild(backMenu, 50, "fourbuttonmenu");
    backMenu->setCameraMask((unsigned short)CameraFlag::USER1);

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

    // 获取窗口大小
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建音量控制Slider
    volumeSlider = ui::Slider::create();
    volumeSlider->loadBarTexture("slider_track.png");
    volumeSlider->loadSlidBallTextures("slider_thumb.png", "slider_thumb.png", "slider_thumb.png");
    volumeSlider->setPosition(Vec2(0, 200));
    volumeSlider->setName("volumeSlider");
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
    volumeSlider->setCameraMask((unsigned short)CameraFlag::USER1);
    hero->addChild(volumeSlider, 50);

    // 创建“确定”按钮
    auto confirmLabel = Label::createWithTTF(u8"确定", "../Resources/fonts/jiantisongti.ttf", 28);
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

    // 创建菜单并添加“确定”按钮
    confirmMenu = Menu::create(confirmButton, nullptr);
    confirmMenu->setPosition(Vec2::ZERO);
    confirmMenu->setCameraMask((unsigned short)CameraFlag::USER1);
    confirmMenu->setName("confirmMenu");
    hero->addChild(confirmMenu, 50);
}

//把设置界面（4个按钮）移除
void GameScene::removeSettingsMenu()
{
    // 根据名称移除菜单
    hero->removeChildByName("fourbuttonmenu");
}

//生成设置按钮
void GameScene::createSettingsMenu()
{
    // 获取窗口大小
    const Size visibleSize = Director::getInstance()->getVisibleSize();

    // 创建“设置”按钮标签
    auto settingsLabel = Label::createWithTTF(u8"设置", "../Resources/fonts/jiantisongti.ttf", 48);

    // 创建“设置”按钮，点击时调用 showSettings
    auto settingsButton = MenuItemLabel::create(
        settingsLabel,
        CC_CALLBACK_1(GameScene::showSettings, this));


    auto menu = Menu::create(settingsButton, nullptr);

    Vec2 adjustedPosition = Vec2(0.5 * visibleSize.width - 100, 0.5 * visibleSize.height - 50);
    settingsButton->setPosition(adjustedPosition);
    menu->setPosition(Vec2::ZERO);
    // 将菜单添加到场景中，设置名称为 "settingsMenu"
    hero->addChild(menu, 50, "settingsMenu");
    menu->setCameraMask((unsigned short)CameraFlag::USER1);
}

//*********敌怪相关

/*产生并运行怪物的函数*/
void GameScene::MonsterFunc(float dt) {
    // 获取屏幕大小
    Size visibleSize = Director::getInstance()->getVisibleSize();

    if (numberOfMonsters < MonsterNumMax) {
        int choice = RandomHelper::random_int(0, 1); //选择生成史莱姆还是僵尸
        if (choice == 0) {
            auto silme = Silme::create("slime.png", 100);
            float x_S = RandomHelper::random_real((visibleSize.width - mapWidth) / 2 + silme->getContentSize().width / 2, (visibleSize.width + mapWidth) / 2 - silme->getContentSize().width / 2); //随机生成坐标
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
            float x_Z = RandomHelper::random_real((visibleSize.width - mapWidth) / 2 + zombie->getContentSize().width / 2, (visibleSize.width + mapWidth) / 2 - zombie->getContentSize().width / 2); //随机生成坐标
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
        // 创建 NPC
        auto npc1 = NPC::create("npc1.png", 100);

        // 随机生成 NPC 的位置
        float x_Z = RandomHelper::random_real((visibleSize.width - mapWidth) / 2 + npc1->getContentSize().width / 2, (visibleSize.width + mapWidth) / 2 - npc1->getContentSize().width / 2);
        npc1->setPosition(Vec2(x_Z, visibleSize.height / 1.5f));

        // 添加物理效果
        npc1->addPhy();

        //加动画效果
        npc1->addAnimate();

        // 将 NPC 加入到场景中
        clippingNode->addChild(npc1);

        // 设置 NPC 的相机
        npc1->setCameraMask((unsigned short)CameraFlag::USER1);

        // 开始 NPC 的运动
        npc1->schedule(CC_SCHEDULE_SELECTOR(NPC::npc_move));
        npc1->schedule(CC_SCHEDULE_SELECTOR(NPC::jump), 1);
        npc1->schedule(CC_SCHEDULE_SELECTOR(NPC::approachplayer), 1, -1, 0);

        // 启用触摸监听器，检测是否点击 NPC 来显示交互菜单
        npc1->addTouchListener();
    }
    // 更新 NPC 数量
    numberOfnpcs++;
}

bool GameScene::Falljudgement(PhysicsContact& contact) {
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();

    // 检测主角与地面碰撞
    if ((bodyA->getCategoryBitmask() == 0x01) || (bodyB->getCategoryBitmask() == 0x01)) {

        // 获取主角刚体
        auto heroBody = (bodyA->getCategoryBitmask() == 0x01) ? bodyA : bodyB;

        // 获取主角的下落速度
        float fallSpeed = heroBody->getVelocity().y;

        // 如果速度超过阈值，计算伤害
        if (fallSpeed < -Fallwithouthurt) {
            auto node = heroBody->getNode();
            auto hero = dynamic_cast<Hero*>(node); // 将 Node 转为 Hero 类型
            hero->applyFallDamage(fallSpeed);
        }
    }
    return true;
}