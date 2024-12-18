#include "cocos2d.h"
#include "hero.h"
#include "GameScene.h"

#define BlocksFirstGID_ 4
using namespace cocos2d;
extern TMXLayer* blocksLayer;
int checkBlockCollision(float x, float y);
Vec2 getTileCoordForPosition(float x, float y);
extern float mapWidth;
extern float mapHeight;
extern cocos2d::TMXTiledMap* map;

void Hero::addPhy()
{
    PhysicsMaterial nonBounce(1, 0, 1); // 不会反弹的物理模型
    auto bodyOfHero = PhysicsBody::createBox(this->getContentSize(), nonBounce); // 主角的刚体
    bodyOfHero->setRotationEnable(false); // 锁定主角的旋转
    bodyOfHero->setMass(1); // 设置主角的质量
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
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        touchStartFrame = Director::getInstance()->getTotalFrames();  // 获取触摸开始的帧数
        touchStartPosition = touch->getLocation();
        return true;
        };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        touchEndFrame = Director::getInstance()->getTotalFrames();  // 获取触摸结束的帧数
        float duration = (touchEndFrame - touchStartFrame) * Director::getInstance()->getAnimationInterval();  // 计算持续时间
        Vec2 touchEndPosition = touch->getLocation();

        float validRange = 100.0f;  // 有效范围，单位：像素
        float distanceToHero = touchEndPosition.distance(this->getPosition());

        // 如果长按时间超过阈值并且在有效范围内，删除泥块
        if (duration >= longPressThreshold && distanceToHero <= validRange)
        {
            removeBlockAtPosition(touchEndPosition);
        }
        else if (duration < longPressThreshold && distanceToHero <= validRange)
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

    // 检查点击位置是否在有效范围内
    if (position.distance(heroPosition) > validRange)
    {
        CCLOG("Invalid click. Click is outside the valid range.");
        return;
    }

    // 将点击位置转换为瓦片坐标
    cocos2d::Vec2 tileCoord = getTileCoordForPosition(position.x, position.y);

    // 检查该位置是否已有瓦片
    if (blocksLayer && blocksLayer->getTileGIDAt(tileCoord) != 0)
    {
        CCLOG("Tile already exists at (%f, %f).", tileCoord.x, tileCoord.y);
        return;
    }

    // 设置一个土块的 GID（假设土块的 GID 为 182，根据你的 TMX 文件来修改）
    int dirtBlockGID = 4;
    blocksLayer->setTileGID(dirtBlockGID, tileCoord);

    // 获取新生成的瓦片
    auto newTile = blocksLayer->getTileAt(tileCoord);
    if (newTile)
    {
        // 给新瓦片添加物理刚体
        cocos2d::PhysicsMaterial infinity_mass(1e10, 0, 1); // 无穷大质量的物理模型
        auto bodyOfBlock = cocos2d::PhysicsBody::createBox(map->getTileSize(), infinity_mass);
        bodyOfBlock->setGravityEnable(false); // 禁用重力影响
        newTile->setPhysicsBody(bodyOfBlock);
    }

    CCLOG("Block added at (%f, %f).", tileCoord.x, tileCoord.y);
}

// 删除泥块的函数
void Hero::removeBlockAtPosition(Vec2 position)
{
    Vec2 tileCoord = getTileCoordForPosition(position.x, position.y);
    if (blocksLayer->getTileGIDAt(tileCoord) != 0)
    {
        blocksLayer->removeTileAt(tileCoord);
        CCLOG("Block removed at tile (%f, %f).", tileCoord.x, tileCoord.y);
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

    auto item3 = MyItem::create("10.png", 30);
    items.pushBack(item3);  // 物品3：土块（30个）
    auto poccketItem3 = MyItem::create("10.png", 30);
    pocketItems.pushBack(poccketItem3);

    auto item4 = MyItem::create("12.png", 48);
    items.pushBack(item4);  // 物品4：木材（48个）
    auto poccketItem4 = MyItem::create("12.png", 48);
    pocketItems.pushBack(poccketItem4);

    while (items.size() < 32) {
        auto emptyItem = MyItem::create("0.png");
        items.pushBack(emptyItem);  // 当背包袋中的物品少于32个时
    }

    while (pocketItems.size() < 8) {
        auto pocketEmptyItem = MyItem::create("0.png");
        pocketItems.pushBack(pocketEmptyItem);  // 当背包袋中的物品少于8个时
    }
    movingItem = MyItem::create("0.png");
    movingItem->retain();
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