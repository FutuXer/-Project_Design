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
    PhysicsMaterial nonBounce(1, 0, 1); // ���ᷴ��������ģ��
    auto bodyOfHero = PhysicsBody::createBox(this->getContentSize(), nonBounce); // ���ǵĸ���
    bodyOfHero->setRotationEnable(false); // �������ǵ���ת
    bodyOfHero->setMass(1); // �������ǵ�����
    this->setPhysicsBody(bodyOfHero);
}
void Hero::addKeyboardListener() {
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        // �����°���
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_A: // �����ƶ�
            moveLeft = true;
            this->setFlippedX(false);  // ����ʱ������
            setHeroAnimation("man2.png", "man3.png", "man4.png", "man5.png");  // �л������ƶ���
            break;
        case EventKeyboard::KeyCode::KEY_D: // �����ƶ�
            moveRight = true;
           this->setFlippedX(true);  // ����ʱ����
            setHeroAnimation("man2.png", "man3.png", "man4.png", "man5.png");  // �л������ƶ���
            break;
        case EventKeyboard::KeyCode::KEY_SPACE: // ��Ծ
            performJump();
            break;
        default:
            break;
        }
        };

    keyboardListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        // �����ɿ�����
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_A: // ֹͣ�����ƶ�
            moveLeft = false;
           this->stopAllActions();   // ֹͣ���ж���
           this->setSpriteFrame("man1.png"); // ���þ�ֹ֡
            break;
        case EventKeyboard::KeyCode::KEY_D: // ֹͣ�����ƶ�
            moveRight = false;
           this->stopAllActions();   // ֹͣ���ж���
           this->setSpriteFrame("man1.png"); // ���þ�ֹ֡
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
    /*�����ﳬ���ϱ߽�ʱ������*/
    if (mapPosition.y >= -mapHeight / 2 + this->getContentSize().height)
    {
        this->getPhysicsBody()->applyImpulse(Vec2(0, JumpHeight));
    }
    isJumping = false;  // ֹͣ��Ծ����ʼ��������
}
void Hero::setHeroAnimation(const std::string& frame2, const std::string& frame3, const std::string& frame4, const std::string& frame5)
{
    // ��������֡����
    Vector<SpriteFrame*> frames;
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame2));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame3));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame4));
    frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(frame5));

    // ��������
    Animation* animation = Animation::createWithSpriteFrames(frames, 0.1f);  // 0.1f Ϊÿ֮֡��ļ��
    Animate* animate = Animate::create(animation);  // ������������

    // ִ�ж���
    this->runAction(RepeatForever::create(animate));  // �ظ�ִ�ж���
}
void Hero::performJump()
{
    // ����ɫ�Ƿ�վ�ڵ����ϣ�ֻ��վ�ڵ����ϲ�����Ծ
    if (!isJumping && checkBlockCollision(this->getPositionX(), this->getPositionY() - 15) >= BlocksFirstGID_)
    {
        isJumping = true;               // ����Ϊ��Ծ״̬
    }
}

void Hero::HeroFunc(float delta)
{
    // ��ȡ�ɼ��ߴ�͵�ͼ�ߴ�
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto mapSize = map->getMapSize();
    auto tileSize = map->getTileSize();
    float mapWidth = mapSize.width * tileSize.width;
    float mapHeight = mapSize.height * tileSize.height;

    // ������Ծ�������߼�
    if (isJumping)
    {
        applyJump();
    }

    checkAndFixHeroCollision();

    // ���������ƶ��߼�
    if (moveLeft)
    {
        mapPosition.x += 3; // �����ƶ�      
        // ���Ƶ�ͼ���ܳ�����߽�
        if (mapPosition.x >= visibleSize.width / 2 - 12)
        {
            mapPosition.x = visibleSize.width / 2 - 12;
        }
    }
    if (moveRight)
    {
        mapPosition.x -= 3; // �����ƶ�
        // ���Ƶ�ͼ���ܳ����ұ߽�
        if (mapPosition.x <= visibleSize.width / 2 - mapWidth + 12)
        {
            mapPosition.x = visibleSize.width / 2 - mapWidth + 12;
        }
    }

    /*��ֹ���ﳬ���ϱ߽�*/
    if (mapPosition.y < -mapHeight / 2 + this->getContentSize().height)
        this->getPhysicsBody()->setVelocity(Vec2(0, -70));

    // ���µ�ͼλ��
    Vec2 tmpPosition = this->getPosition();

    mapPosition.x = mapPosition.x - (tmpPosition.x - visibleSize.width / 2);
    mapPosition.y = mapPosition.y - (tmpPosition.y - visibleSize.height / 2);
    map->setPosition(mapPosition);
    this->setPosition(visibleSize.width / 2, visibleSize.height / 2);

    /*// ���¿ɼ���Χ����Ƭ
    RenderManager* renderManager = new RenderManager(map);
    renderManager->updateVisibleTiles(mapPosition, visibleSize);
    delete renderManager;*/

}
void Hero::checkAndFixHeroCollision()
{
    // ��ȡ��ɫ��ǰ��������ײ��
    Vec2 heroPos = this->getPosition();

    // ��ȡ��ɫ��ǰλ�����ҵ���Ƭ����
    int tileGID_left1 = checkBlockCollision(heroPos.x - 8, heroPos.y - 7);
    int tileGID_left2 = checkBlockCollision(heroPos.x - 8, heroPos.y + 7);
    int tileGID_right1 = checkBlockCollision(heroPos.x + 8, heroPos.y - 7);
    int tileGID_right2 = checkBlockCollision(heroPos.x + 8, heroPos.y + 7);

    // ����������飬��������ɫλ��
    if (tileGID_left1 >= BlocksFirstGID_ || tileGID_left2 >= BlocksFirstGID_)
    {
        mapPosition.x -= 3;
    }
    // ����������飬��������ɫλ��
    if (tileGID_right1 >= BlocksFirstGID_ || tileGID_right2 >= BlocksFirstGID_)
    {
        mapPosition.x += 3;
    }
}
