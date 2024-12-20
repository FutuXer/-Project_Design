#include "cocos2d.h"
#include "hero.h"
#include"monster.h"

#define BlocksFirstGID_ 4
using namespace cocos2d;
extern TMXLayer* blocksLayer;
int checkBlockCollision(float x, float y);
Vec2 getTileCoordForPosition(float x, float y);
extern float mapWidth;
extern float mapHeight;
extern cocos2d::TMXTiledMap* map;
extern TMXLayer* chair;
extern TMXLayer* toptree;
extern TMXLayer* wood;
extern TMXLayer* chest;
extern TMXLayer* door;
extern TMXLayer* desk;
#define Attackwidth 50
#define Attackheight 50
#define falldamagecontrol 50 //控制摔落伤害量的值

void Hero::addPhy() 
{
    PhysicsMaterial nonBounce(1, 0, 1); // 不会反弹的物理模型
    auto bodyOfHero = PhysicsBody::createBox(this->getContentSize(), nonBounce); // 主角的刚体
    bodyOfHero->setRotationEnable(false); // 锁定主角的旋转
    bodyOfHero->setMass(1); // 设置主角的质量
    bodyOfHero->setCategoryBitmask(0x01);
    bodyOfHero->setContactTestBitmask(0x02 | 0x04 | 0x06 | 0x08); //**********设置碰撞监听
    this->setPhysicsBody(bodyOfHero);
}

void Hero::addKeyboardListener() 
{
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event)
        {
        // 处理按下按键
        auto& children = this->getChildren();
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_A: // 向左移动
            moveLeft = true;
            this->setFlippedX(false);  // 左移时不镜像
            setHeroAnimation("man2.png", "man3.png", "man4.png", "man5.png");  // 切换到左移动画
            break;
        case EventKeyboard::KeyCode::KEY_D: // 向右移动
            moveRight = true;
            this->setFlippedX(true);  // 右移时镜像
            setHeroAnimation("man2.png", "man3.png", "man4.png", "man5.png");  // 切换到右移动画
            break;
        case EventKeyboard::KeyCode::KEY_SPACE: // 跳跃
            performJump();
            break;
        case EventKeyboard::KeyCode::KEY_ESCAPE: // 切换背包/口袋
            // 防止背包中因为整理而丢失物品，把尚在手中的物品放在第一个空位置上
            if (itemInMove)
                itemInMove->setTexture("0.png");
            if (movingItem->getItemName() != "0.png") {
                for (int i = 0; i < items.size(); ++i) {
                    if (items.at(i)->getItemName() == "0.png") {
                        // 把物品放在第一个空位置上
                        items.at(i)->coppyItem(movingItem);
                        items.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                        // 如果改动了第一行的图标，同步修改口袋中的物品
                        if (i < 8) {
                            pocketItems.at(i)->coppyItem(movingItem);
                            pocketItems.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                        }
                        break;
                    }
                }
                movingItem = MyItem::create("0.png");  // 当玩家关闭背包时
                movingItem->retain();
            }
            
            for (auto& child : children) {
                if (child->isVisible()) {
                    child->setVisible(false);
                }
                else {
                    child->setVisible(true);
                }
            }
            break;
        default:
            break;
        }
        };

    keyboardListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        // 处理松开按键
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_A: // 停止向左移动
            moveLeft = false;
            this->stopAllActions();   // 停止所有动画
            this->setSpriteFrame("man1.png"); // 设置静止帧
            break;
        case EventKeyboard::KeyCode::KEY_D: // 停止向右移动
            moveRight = false;
            this->stopAllActions();   // 停止所有动画
            this->setSpriteFrame("man1.png"); // 设置静止帧
            break;
        default:
            break;
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void Hero::addTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        touchStartFrame = Director::getInstance()->getTotalFrames();  // 获取触摸开始的帧数
        touchStartPosition = touch->getLocation();
        return true;
        };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        touchEndFrame = Director::getInstance()->getTotalFrames();  // 获取触摸结束的帧数
        float duration = (touchEndFrame - touchStartFrame) * Director::getInstance()->getAnimationInterval();  // 计算持续时间
        Vec2 touchEndPosition = touch->getLocation();

        // 如果长按时间超过阈值，撸掉物块
        // 只有拿工具，才能撸掉物块
        int flag = 0;                       
        if (usingItem >= 0 && usingItem < pocketItems.size() && items.at(usingItem)->getItemKind() == 2) {
            flag = 1;
            if (items.at(usingItem)->getItemName()[1] == '1')
                longPressThreshold = 0.2f;                    // 手持铁镐可以加快挖掘速度
        }
        if (duration >= longPressThreshold && flag == 1)
        {
            removeBlockAtPosition(touchEndPosition);
            longPressThreshold = 0.5f;                        // 挖掘完毕，恢复0.5s，防止对平时的点击判断造成干扰
        }
        else if (duration < longPressThreshold)//&& distanceToHero <= validRange
        {
            addBlockAtPosition(touchEndPosition);
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Hero::applyJump()
{
    //auto mapSize = map->getMapSize();
    //auto tileSize = map->getTileSize();
    //float mapHeight = mapSize.height * tileSize.height;
    /*在人物超出上边界时，不跳*/
    //if (mapPosition.y >= -mapHeight / 2 + this->getContentSize().height)
    //{
    this->getPhysicsBody()->applyImpulse(Vec2(0, JumpHeight));
    //}
    isJumping = false;  // 停止跳跃，开始自由落体
}
void Hero::setHeroAnimation(const std::string& frame2, const std::string& frame3, const std::string& frame4, const std::string& frame5)
{
    // 创建精灵帧对象
    Vector<SpriteFrame*> frames;
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame2));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame3));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame4));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame5));

    // 创建动画
    Animation* animation = Animation::createWithSpriteFrames(frames, 0.1f);  // 0.1f 为每帧之间的间隔
    Animate* animate = Animate::create(animation);  // 创建动画动作

    // 执行动画
    this->runAction(RepeatForever::create(animate));  // 重复执行动画
}
void Hero::performJump()
{
    // 检查角色是否站在地面上，只有站在地面上才能跳跃
    if (!isJumping && (checkBlockCollision(this->getPositionX(), this->getPositionY() - 25) >= BlocksFirstGID_ || checkBlockCollision(this->getPositionX() - 9, this->getPositionY() - 25) >= BlocksFirstGID_ || checkBlockCollision(this->getPositionX() + 9, this->getPositionY() - 25) >= BlocksFirstGID_))
    {
        isJumping = true;               // 设置为跳跃状态
    }
}

void Hero::HeroFunc(float delta)
{
    // 获取可见尺寸和英雄的位置和英雄的大小
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 Pos = this->getPosition();
    Size HeroSize = this->getContentSize();

    // 更新跳跃和下落逻辑
    if (isJumping)
    {
        applyJump();
    }

    checkAndFixHeroCollision(delta);

    auto moveR = MoveBy::create(delta, Vec2(3, 0));
    auto moveL = MoveBy::create(delta, Vec2(-3, 0));
    // 更新左右移动逻辑
    if (moveLeft && Pos.x > (visibleSize.width - mapWidth) / 2 + HeroSize.width / 2)
    {
        this->runAction(moveL);
    }
    if (moveRight && Pos.x < (visibleSize.width + mapWidth) / 2 - HeroSize.width / 2)
    {
        this->runAction(moveR);
    }

    // 更新地图位置
   /*Vec2 tmpPosition = this->getPosition();

    mapPosition.x = mapPosition.x - (tmpPosition.x - visibleSize.width / 2);
    mapPosition.y = mapPosition.y - (tmpPosition.y - visibleSize.height / 2);
    map->setPosition(mapPosition);
    this->setPosition(visibleSize.width / 2, visibleSize.height / 2);*/

    /*// 更新可见范围的瓦片
    RenderManager* renderManager = new RenderManager(map);
    renderManager->updateVisibleTiles(mapPosition, visibleSize);
    delete renderManager;*/

}

void Hero::checkAndFixHeroCollision(float delta)
{
    // 获取角色当前的物理碰撞体
    Vec2 heroPos = this->getPosition();

    auto moveR = MoveBy::create(delta, Vec2(3, 0));
    auto moveL = MoveBy::create(delta, Vec2(-3, 0));

    // 获取角色当前位置左右的瓦片坐标
    int tileGID_left1 = checkBlockCollision(heroPos.x - 8, heroPos.y - 16);
    int tileGID_left2 = checkBlockCollision(heroPos.x - 8, heroPos.y + 12);
    int tileGID_left3 = checkBlockCollision(heroPos.x - 16, heroPos.y + 5);
    int tileGID_right1 = checkBlockCollision(heroPos.x + 8, heroPos.y - 16);
    int tileGID_right2 = checkBlockCollision(heroPos.x + 8, heroPos.y + 12);
    int tileGID_right3 = checkBlockCollision(heroPos.x + 16, heroPos.y + 5);

    // 如果碰到方块，则修正角色位置
    if (tileGID_left1 >= BlocksFirstGID_ || tileGID_left2 >= BlocksFirstGID_ || tileGID_left3 >= BlocksFirstGID_)
    {
        this->runAction(moveR);
    }
    // 如果碰到方块，则修正角色位置
    if (tileGID_right1 >= BlocksFirstGID_ || tileGID_right2 >= BlocksFirstGID_ || tileGID_right3 >= BlocksFirstGID_)
    {
        this->runAction(moveL);
    }
}

// 获取当前时间（秒）
float Hero::getCurrentTime()
{
    // 获取当前时间戳，返回秒数
    return std::chrono::duration<float>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void Hero::addBlockAtPosition(cocos2d::Vec2 position)
{
    // 定义人物周围的有效点击区域
    float validRange = 100.0f;  // 有效区域半径，可根据需要调整
    cocos2d::Vec2 heroPosition = this->getPosition();
    CCLOG("hero(%f, %f)", heroPosition.x, heroPosition.y);

    // 获取当前场景
    auto scene = Director::getInstance()->getRunningScene();

    // 从场景中获取相机节点
    auto camera = scene->getChildByName("Camera");  // 假设你的相机节点名字是 "camera"

    // 获取相机的坐标（场景坐标）
    auto cameraPos = camera->getPosition();

    // 获取当前场景的可见区域大小
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 VisSize = Vec2(visibleSize.width, visibleSize.height);
    auto posi = map->getPosition();
    auto mapSize = map->getMapSize();

    // 检查点击位置是否在有效范围内
    if ((position + heroPosition - VisSize / 2).distance(heroPosition) > validRange)
    {
        CCLOG("Invalid click. Click is outside the valid range.");
        return;
    }
    CCLOG("touch(%f, %f)", position.x, position.y);

    // 计算精灵的最终屏幕坐标（考虑了相机偏移）
    Vec2 finalSpriteScreenPos = (position - posi + cameraPos - VisSize / 2) / 16;
    int tx = finalSpriteScreenPos.x;
    int ty = mapSize.height - finalSpriteScreenPos.y;

    // 将点击位置转换为瓦片坐标
    cocos2d::Vec2 tileCoord = Vec2(tx, ty);
    //getTileCoordForPosition(position.x, position.y)
    CCLOG("Tile  (%f, %f).", tileCoord.x, tileCoord.y);

    if (usingItem < 0 || usingItem >= pocketItems.size())
        return;
    int itemkind = pocketItems.at(usingItem)->getItemKind();   // 获取物品的类型

    if (itemkind != 1 && itemkind != 4)
        return;                                               // 物品不可放置（不是原料或家具）
    std::string usingItemName = pocketItems.at(usingItem)->getItemName();       // 获取物品的名字
   
    //检查该位置是否已有瓦片
    if (blocksLayer && blocksLayer->getTileGIDAt(tileCoord) != 0)
    {
        CCLOG("Tile already exists at (%f, %f).", tileCoord.x, tileCoord.y);
        return;
    }

    // 设置一个土块的 GID（假设土块的 GID 为 182，根据你的 TMX 文件来修改）
    int dirtBlockGID;
    if (itemkind == 1) {
        dirtBlockGID = dictionaryToPlace[usingItemName[1] - '0'][1];            // 在字典中取出对应的GID

        blocksLayer->setTileGID(dirtBlockGID, Vec2(tx, ty));                    // 原料类需要添加到block中，被添加刚体

        // 获取新生成的瓦片
        auto newTile = blocksLayer->getTileAt(Vec2(tx, ty));
        if (newTile)
        {
            // 给新瓦片添加物理刚体
            cocos2d::PhysicsMaterial body(1, 0, 1); // 无穷大质量的物理模型
            auto bodyOfBlock = cocos2d::PhysicsBody::createBox(map->getTileSize(), body);
            bodyOfBlock->setDynamic(false);
            newTile->setPhysicsBody(bodyOfBlock);
        }
    }
    else {
        dirtBlockGID = dictionaryToPlace[usingItemName[1] - '0' + 5][1];
        if (usingItemName == "40.png")
            desk->setTileGID(dirtBlockGID, Vec2(tx, ty));
        else if (usingItemName == "41.png")
            chair->setTileGID(dirtBlockGID, Vec2(tx, ty));
        else if (usingItemName == "42.png")
            door->setTileGID(dirtBlockGID, Vec2(tx, ty));
        else if (usingItemName == "43.png")
            chest->setTileGID(dirtBlockGID, Vec2(tx, ty));                                 // 如果是家具，分别加到对应的图层中去
    }
    
    pocketItems.at(usingItem)->setNum(pocketItems.at(usingItem)->getItemNum() - 1);        // 每放下一个物品，相应的物品减去1
    pocketItems.at(usingItem)->setCameraMask((unsigned short)CameraFlag::USER1);
    items.at(usingItem)->setNum(pocketItems.at(usingItem)->getItemNum() - 1);              // 每放下一个物品，bag中也同步减去1
    items.at(usingItem)->setCameraMask((unsigned short)CameraFlag::USER1);
    if (usingItemName != "0.png" && usingItemName != "10.png" && usingItemName != "11.png")
        this->calculate();                                                                 // 如果消耗的不是土、石块（土、石块不能做任何东西）,则重新计算合成物

    CCLOG("Block added at (%f, %f).", tileCoord.x, tileCoord.y);
}

// 删除物块的函数
void Hero::removeBlockAtPosition(Vec2 position)
{
    // 定义人物周围的有效点击区域
    float validRange = 100.0f;  // 有效区域半径，可根据需要调整
    cocos2d::Vec2 heroPosition = this->getPosition();
    CCLOG("hero(%f, %f)", heroPosition.x, heroPosition.y);

    // 获取当前场景
    auto scene = Director::getInstance()->getRunningScene();

    // 从场景中获取相机节点
    auto camera = scene->getChildByName("Camera");  // 假设你的相机节点名字是 "camera"

    // 获取相机的坐标（场景坐标）
    auto cameraPos = camera->getPosition();

    // 获取当前场景的可见区域大小
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 VisSize = Vec2(visibleSize.width, visibleSize.height);

    // 检查点击位置是否在有效范围内
    if ((position + heroPosition - VisSize / 2).distance(heroPosition) > validRange)
    {
        CCLOG("Invalid click. Click is outside the valid range.");
        return;
    }
    CCLOG("touch(%f, %f)", position.x, position.y);

    // 计算精灵的最终屏幕坐标（考虑了相机偏移）
    Vec2 finalSpriteScreenPos = position + cameraPos - VisSize / 2;

    Vec2 tileCoord = getTileCoordForPosition(finalSpriteScreenPos.x, finalSpriteScreenPos.y);

    int gid = blocksLayer->getTileGIDAt(tileCoord);
    if (gid >= 4 && gid <= 9755)
    {
        if (gid == 5729)
            gid = 4;                              // 带草的泥土，捡起来之后就是土块
        blocksLayer->removeTileAt(tileCoord);
        CCLOG("Block removed at tile (%f, %f).", tileCoord.x, tileCoord.y);
        std::string Name;             // 待添加物品的名字
        // 遍历GID字典，找到对应的物品名字
        for (int i = 0; i < 9; i++) {
            if (dictionaryToPlace[i][1] == gid) {
                Name = std::to_string(dictionaryToPlace[i][0]) + ".png";
                break;
            }
        }
        if (Name.empty())
            return;                               // 没有找到要添加的物品（这是意外情况，为了游戏的健壮性）
        this->pickUpItems(Name);
    }
    else if (chair->getTileGIDAt(tileCoord) == 19209)
    {
        chair->removeTileAt(tileCoord);
        this->pickUpItems("41.png");               // 把椅子拾取到背包
    }
    else if (toptree->getTileGIDAt(tileCoord) == 11167)
    {
        toptree->removeTileAt(tileCoord);          // 树冠无用，不添加
    }
    else if (wood->getTileGIDAt(tileCoord) == 347)
    {
        wood->removeTileAt(tileCoord);
        this->pickUpItems("12.png");               // 把木材拾取到背包
    }
    else if (chest->getTileGIDAt(tileCoord) == 9757)
    {
        chest->removeTileAt(tileCoord);
        this->pickUpItems("43.png");               // 把箱子拾取到背包
    }
    else if (door->getTileGIDAt(tileCoord) == 19210)
    {
        door->removeTileAt(tileCoord);
        this->pickUpItems("42.png");               // 把门拾取到背包
    }
    else if (desk->getTileGIDAt(tileCoord) == 19211)
    {
        desk->removeTileAt(tileCoord);
        this->pickUpItems("40.png");               // 把桌子拾取到背包
    }
    else
    {
        CCLOG("No block to remove at (%f, %f).", tileCoord.x, tileCoord.y);
    }
}

void Hero::iniItems()
{
    auto item1 = MyItem::create("20.png");
    items.pushBack(item1);  // 物品1：铜镐
    auto poccketItem1 = MyItem::create("20.png");
    pocketItems.pushBack(poccketItem1);

    auto item2 = MyItem::create("30.png");
    items.pushBack(item2);  // 物品2：铜剑
    auto poccketItem2 = MyItem::create("30.png");
    pocketItems.pushBack(poccketItem2);

    auto item3 = MyItem::create("10.png", 5);
    items.pushBack(item3);  // 物品3：土块（5个）
    auto poccketItem3 = MyItem::create("10.png", 5);
    pocketItems.pushBack(poccketItem3);

    // 开挂相关
    /*auto item4 = MyItem::create("12.png", 48);
    items.pushBack(item4);  // 物品4：木材（48个）
    auto poccketItem4 = MyItem::create("12.png", 48);
    pocketItems.pushBack(poccketItem4);

    auto item5 = MyItem::create("13.png", 48);
    items.pushBack(item5);  // 物品5：铜矿（48个）
    auto poccketItem5 = MyItem::create("13.png", 48);
    pocketItems.pushBack(poccketItem5);

    auto item6 = MyItem::create("14.png", 48);
    items.pushBack(item6);  // 物品6：铁矿（48个）
    auto poccketItem6 = MyItem::create("14.png", 48);
    pocketItems.pushBack(poccketItem6);*/

    while (items.size() < 32) {
        auto emptyItem = MyItem::create("0.png");
        items.pushBack(emptyItem);  // 当背包袋中的物品少于32个时
    }

    while (pocketItems.size() < 8) {
        auto pocketEmptyItem = MyItem::create("0.png");
        pocketItems.pushBack(pocketEmptyItem);  // 当背包袋中的物品少于8个时
    }
    movingItem = MyItem::create("0.png");
    movingItem->retain();                       // 初始化正在移动的物品
    usingItem = -1;                             // 初始化正在使用的物品
    this->calculate();                          // 初始化可以合成的物品

    this->checkPocket();    // 显示口袋物品（并更换手中的物品）
    this->checkBag();       // 显示物品栏（即背包，并整理背包物品）
    this->checkProduction();// 显示制作窗口

    // 添加鼠标事件监听
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = [=](EventMouse* event) {
        if (movingItem->isVisible() && itemInMove) {
            itemInMove->setPosition(event->getLocation());
            auto visibleSize = Director::getInstance()->getVisibleSize();
            itemInMove->setPositionY(0.5 * visibleSize.height - itemInMove->getPosition().y);
            itemInMove->setPositionX(itemInMove->getPosition().x - 0.5 * visibleSize.width);   // 坐标转换
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

//***********怪物相关
void Hero::takedamage(int damage) {
    health -= damage;
    CCLOG("hero take %d damage---health%d\n", damage, health);
    if (isRecovering)
        stopHealthRecovery(); //如果主角在回血，打断回血
    healthBar->setPercentage((float)health / total_health * 100); //修改血条
    if (health <= 0)
        die();
    else
        startHealthRecovery(); //开始回血
}
void Hero::die() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    /*孙*/
      // 延迟一帧后重置位置
    this->scheduleOnce([this](float dt) {
        auto heroBody = this->getPhysicsBody();
        if (heroBody) {
            heroBody->setVelocity(Vec2::ZERO); // 停止速度
        }
        // 重置位置到屏幕中心
        this->setPosition(Director::getInstance()->getVisibleSize() / 2);
        CCLOG("主角已重置到屏幕中心！");
        }, 0.0f, "reset_hero_position");


    health = total_health;
    healthBar->setPercentage((float)health / total_health * 100);
}
void Hero::startHealthRecovery() {
    if (isRecovering) return; // 如果已经在恢复中，不重复启动
    isRecovering = true;

    // 延迟3秒后开始恢复
    this->scheduleOnce([this](float) {
        this->schedule([this](float) {
            if (health + 5 <= total_health) {
                health += 5; // 每次恢复5点血量
                healthBar->setPercentage((float)health / total_health * 100);
                CCLOG("recovering... health: %d", health);
            }
            else {
                health = total_health;
                healthBar->setPercentage((float)health / total_health * 100);
                this->unschedule("HealthRecovery"); // 停止回血
                isRecovering = false;
            }
            }, 1.0f, "HealthRecovery"); // 每1秒恢复一次
        }, 3.0f, "StartRecovery"); // 延迟3秒启动回血
}

void Hero::stopHealthRecovery() {
    this->unschedule("StartRecovery");  // 停止延迟计时
    this->unschedule("HealthRecovery"); // 停止定时回血
    isRecovering = false;
}

void Hero::addhealthbar() {
    auto currentScene = Director::getInstance()->getRunningScene();
    // 创建血条
    healthBarBG = cocos2d::Sprite::create("health_bar_bg.png"); // 血条背景
    healthBarBG->setPosition(this->getContentSize().width / 2, this->getContentSize().height + 10);
    this->addChild(healthBarBG);

    auto healthBarFill = cocos2d::Sprite::create("health_bar_fill.png"); // 血条填充
    healthBar = cocos2d::ProgressTimer::create(healthBarFill);
    healthBar->setType(cocos2d::ProgressTimer::Type::BAR);
    healthBar->setMidpoint(cocos2d::Vec2(0, 0.5f)); // 起始点为左侧
    healthBar->setBarChangeRate(cocos2d::Vec2(1, 0)); // 横向填充
    healthBar->setPosition(healthBarBG->getPosition());
    healthBar->setPercentage(100); // 初始填充满
    this->addChild(healthBar);
}

void Hero::AttackMonsters() {
    if (usingItem >= 0 && usingItem < pocketItems.size() && pocketItems.at(usingItem)->getItemKind() == 3)
        canAttack = true;                 // 只有手持武器才能攻击
    else
        canAttack = false;
    if (canAttack == false || isOnCooldown == true)
        return;
    // 定义主角的攻击范围
    auto attackRange = Rect(this->getPositionX() - this->getContentSize().width / 2 - Attackwidth,  // 左边界
        this->getPositionY() - this->getContentSize().height / 2 - Attackheight,  // 下边界
        this->getContentSize().width + 2 * Attackwidth,                          // 宽度
        this->getContentSize().height + 2 * Attackheight);                         // 高度

    // 使用物理世界查询
    auto physicsWorld = this->getScene()->getPhysicsWorld();
    physicsWorld->queryRect([this](PhysicsWorld& world, PhysicsShape& shape, void* data) -> bool {
        auto body = shape.getBody();
        if (body->getCategoryBitmask() == 0x02) { // 检查目标是什么
            auto node = body->getNode();
            auto monster = dynamic_cast<Silme*>(node);
            if (pocketItems.at(usingItem)->getItemName() == "31.png")
                monster->takeDamage(50); // 主角攻击怪物，手持铁剑则扣除50伤害
            else
                monster->takeDamage(20); // 手持铜剑则扣除20伤害
        }
        if (body->getCategoryBitmask() == 0x04) { // 检查目标是什么
            auto node = body->getNode();
            auto monster = dynamic_cast<Zombie*>(node);
            if (pocketItems.at(usingItem)->getItemName() == "31.png")
                monster->takeDamage(50); // 主角攻击怪物，手持铁剑则扣除50伤害
            else
                monster->takeDamage(20); // 手持铜剑则扣除20伤害
        }
        return true; // 继续查询
        }, attackRange, NULL);
    // 开启冷却
    isOnCooldown = true;
    this->scheduleOnce([this](float) { isOnCooldown = false; }, 1.0f, "AttackCooldown"); // 1秒冷却
}

void Hero::addTouchListenerAttack() {
    auto mouseListener = EventListenerMouse::create();

    // 鼠标点击事件
    mouseListener->onMouseDown = [this](EventMouse* event) {
        this->AttackMonsters(); // 主角攻击周围怪物
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

void Hero::applyFallDamage(float fallSpeed) {
    // 计算伤害
    float damage = std::abs(fallSpeed) / falldamagecontrol;
    takedamage(damage);
    CCLOG("hero get falldamage:%f\n", damage);
}//计算摔落伤害