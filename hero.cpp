#include"cocos2d.h"
#include"hero.h"
#define BlocksFirstGID_ 10409
using namespace cocos2d;
extern Vec2 mapPosition;
extern TMXTiledMap* map;
extern TMXLayer* blocksLayer;
int checkBlockCollision(float x, float y);
Vec2 getTileCoordForPosition(float x, float y);

void Hero::addPhy() {
    PhysicsMaterial nonBounce(1, 0, 1); // 不会反弹的物理模型
    auto bodyOfHero = PhysicsBody::createBox(this->getContentSize(), nonBounce); // 主角的刚体
    bodyOfHero->setRotationEnable(false); // 锁定主角的旋转
    bodyOfHero->setMass(1); // 设置主角的质量
    this->setPhysicsBody(bodyOfHero);
}
void Hero::addKeyboardListener() {
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        // 处理按下按键
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
void Hero::applyJump()
{
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapHeight = mapSize.height * tileSize.height;
    /*在人物超出上边界时，不跳*/
    if (mapPosition.y >= -mapHeight / 2 + this->getContentSize().height)
    {
        this->getPhysicsBody()->applyImpulse(Vec2(0, JumpHeight));
    }
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
    if (!isJumping && checkBlockCollision(this->getPositionX(), this->getPositionY() - 15) >= BlocksFirstGID_)
    {
        isJumping = true;               // 设置为跳跃状态
    }
}

void Hero::HeroFunc(float delta)
{
    // 获取可见尺寸和地图尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    // 更新跳跃和下落逻辑
    if (isJumping)
    {
        applyJump();
    }

    checkAndFixHeroCollision();

    // 更新左右移动逻辑
    if (moveLeft)
    {
        mapPosition.x += 3; // 向左移动      
        // 限制地图不能超出左边界
        if (mapPosition.x >= visibleSize.width / 2 - 12)
        {
            mapPosition.x = visibleSize.width / 2 - 12;
        }
    }
    if (moveRight)
    {
        mapPosition.x -= 3; // 向右移动
        // 限制地图不能超出右边界
        if (mapPosition.x <= visibleSize.width / 2 - mapWidth + 12)
        {
            mapPosition.x = visibleSize.width / 2 - mapWidth + 12;
        }
    }

    /*阻止人物超出上边界*/
    if (mapPosition.y < -mapHeight / 2 + this->getContentSize().height)
        this->getPhysicsBody()->setVelocity(Vec2(0, -70));

    // 更新地图位置
    Vec2 tmpPosition = this->getPosition();

    mapPosition.x = mapPosition.x - (tmpPosition.x - visibleSize.width / 2);
    mapPosition.y = mapPosition.y - (tmpPosition.y - visibleSize.height / 2);
    map->setPosition(mapPosition);
    this->setPosition(visibleSize.width / 2, visibleSize.height / 2);

    /*// 更新可见范围的瓦片
    RenderManager* renderManager = new RenderManager(map);
    renderManager->updateVisibleTiles(mapPosition, visibleSize);
    delete renderManager;*/

}
void Hero::checkAndFixHeroCollision()
{
    // 获取角色当前的物理碰撞体
    Vec2 heroPos = this->getPosition();

    // 获取角色当前位置左右的瓦片坐标
    int tileGID_left1 = checkBlockCollision(heroPos.x - 8, heroPos.y - 7);
    int tileGID_left2 = checkBlockCollision(heroPos.x - 8, heroPos.y + 7);
    int tileGID_right1 = checkBlockCollision(heroPos.x + 8, heroPos.y - 7);
    int tileGID_right2 = checkBlockCollision(heroPos.x + 8, heroPos.y + 7);

    // 如果碰到方块，则修正角色位置
    if (tileGID_left1 >= BlocksFirstGID_ || tileGID_left2 >= BlocksFirstGID_)
    {
        mapPosition.x -= 3;
    }
    // 如果碰到方块，则修正角色位置
    if (tileGID_right1 >= BlocksFirstGID_ || tileGID_right2 >= BlocksFirstGID_)
    {
        mapPosition.x += 3;
    }
}
