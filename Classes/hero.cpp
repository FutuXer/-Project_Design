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
#define falldamagecontrol 50 //����ˤ���˺�����ֵ

void Hero::addPhy() 
{
    PhysicsMaterial nonBounce(1, 0, 1); // ���ᷴ��������ģ��
    auto bodyOfHero = PhysicsBody::createBox(this->getContentSize(), nonBounce); // ���ǵĸ���
    bodyOfHero->setRotationEnable(false); // �������ǵ���ת
    bodyOfHero->setMass(1); // �������ǵ�����
    bodyOfHero->setCategoryBitmask(0x01);
    bodyOfHero->setContactTestBitmask(0x02 | 0x04 | 0x06 | 0x08); //**********������ײ����
    this->setPhysicsBody(bodyOfHero);
}

void Hero::addKeyboardListener() 
{
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event)
        {
        // �����°���
        auto& children = this->getChildren();
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
        case EventKeyboard::KeyCode::KEY_ESCAPE: // �л�����/�ڴ�
            // ��ֹ��������Ϊ�������ʧ��Ʒ�����������е���Ʒ���ڵ�һ����λ����
            if (itemInMove)
                itemInMove->setTexture("0.png");
            if (movingItem->getItemName() != "0.png") {
                for (int i = 0; i < items.size(); ++i) {
                    if (items.at(i)->getItemName() == "0.png") {
                        // ����Ʒ���ڵ�һ����λ����
                        items.at(i)->coppyItem(movingItem);
                        items.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                        // ����Ķ��˵�һ�е�ͼ�꣬ͬ���޸Ŀڴ��е���Ʒ
                        if (i < 8) {
                            pocketItems.at(i)->coppyItem(movingItem);
                            pocketItems.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                        }
                        break;
                    }
                }
                movingItem = MyItem::create("0.png");  // ����ҹرձ���ʱ
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

void Hero::addTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        touchStartFrame = Director::getInstance()->getTotalFrames();  // ��ȡ������ʼ��֡��
        touchStartPosition = touch->getLocation();
        return true;
        };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        touchEndFrame = Director::getInstance()->getTotalFrames();  // ��ȡ����������֡��
        float duration = (touchEndFrame - touchStartFrame) * Director::getInstance()->getAnimationInterval();  // �������ʱ��
        Vec2 touchEndPosition = touch->getLocation();

        // �������ʱ�䳬����ֵ��ߣ�����
        // ֻ���ù��ߣ�����ߣ�����
        int flag = 0;                       
        if (usingItem >= 0 && usingItem < pocketItems.size() && items.at(usingItem)->getItemKind() == 2) {
            flag = 1;
            if (items.at(usingItem)->getItemName()[1] == '1')
                longPressThreshold = 0.2f;                    // �ֳ�������Լӿ��ھ��ٶ�
        }
        if (duration >= longPressThreshold && flag == 1)
        {
            removeBlockAtPosition(touchEndPosition);
            longPressThreshold = 0.5f;                        // �ھ���ϣ��ָ�0.5s����ֹ��ƽʱ�ĵ���ж���ɸ���
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
    /*�����ﳬ���ϱ߽�ʱ������*/
    //if (mapPosition.y >= -mapHeight / 2 + this->getContentSize().height)
    //{
    this->getPhysicsBody()->applyImpulse(Vec2(0, JumpHeight));
    //}
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
    if (!isJumping && (checkBlockCollision(this->getPositionX(), this->getPositionY() - 25) >= BlocksFirstGID_ || checkBlockCollision(this->getPositionX() - 9, this->getPositionY() - 25) >= BlocksFirstGID_ || checkBlockCollision(this->getPositionX() + 9, this->getPositionY() - 25) >= BlocksFirstGID_))
    {
        isJumping = true;               // ����Ϊ��Ծ״̬
    }
}

void Hero::HeroFunc(float delta)
{
    // ��ȡ�ɼ��ߴ��Ӣ�۵�λ�ú�Ӣ�۵Ĵ�С
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 Pos = this->getPosition();
    Size HeroSize = this->getContentSize();

    // ������Ծ�������߼�
    if (isJumping)
    {
        applyJump();
    }

    checkAndFixHeroCollision(delta);

    auto moveR = MoveBy::create(delta, Vec2(3, 0));
    auto moveL = MoveBy::create(delta, Vec2(-3, 0));
    // ���������ƶ��߼�
    if (moveLeft && Pos.x > (visibleSize.width - mapWidth) / 2 + HeroSize.width / 2)
    {
        this->runAction(moveL);
    }
    if (moveRight && Pos.x < (visibleSize.width + mapWidth) / 2 - HeroSize.width / 2)
    {
        this->runAction(moveR);
    }

    // ���µ�ͼλ��
   /*Vec2 tmpPosition = this->getPosition();

    mapPosition.x = mapPosition.x - (tmpPosition.x - visibleSize.width / 2);
    mapPosition.y = mapPosition.y - (tmpPosition.y - visibleSize.height / 2);
    map->setPosition(mapPosition);
    this->setPosition(visibleSize.width / 2, visibleSize.height / 2);*/

    /*// ���¿ɼ���Χ����Ƭ
    RenderManager* renderManager = new RenderManager(map);
    renderManager->updateVisibleTiles(mapPosition, visibleSize);
    delete renderManager;*/

}

void Hero::checkAndFixHeroCollision(float delta)
{
    // ��ȡ��ɫ��ǰ��������ײ��
    Vec2 heroPos = this->getPosition();

    auto moveR = MoveBy::create(delta, Vec2(3, 0));
    auto moveL = MoveBy::create(delta, Vec2(-3, 0));

    // ��ȡ��ɫ��ǰλ�����ҵ���Ƭ����
    int tileGID_left1 = checkBlockCollision(heroPos.x - 8, heroPos.y - 16);
    int tileGID_left2 = checkBlockCollision(heroPos.x - 8, heroPos.y + 12);
    int tileGID_left3 = checkBlockCollision(heroPos.x - 16, heroPos.y + 5);
    int tileGID_right1 = checkBlockCollision(heroPos.x + 8, heroPos.y - 16);
    int tileGID_right2 = checkBlockCollision(heroPos.x + 8, heroPos.y + 12);
    int tileGID_right3 = checkBlockCollision(heroPos.x + 16, heroPos.y + 5);

    // ����������飬��������ɫλ��
    if (tileGID_left1 >= BlocksFirstGID_ || tileGID_left2 >= BlocksFirstGID_ || tileGID_left3 >= BlocksFirstGID_)
    {
        this->runAction(moveR);
    }
    // ����������飬��������ɫλ��
    if (tileGID_right1 >= BlocksFirstGID_ || tileGID_right2 >= BlocksFirstGID_ || tileGID_right3 >= BlocksFirstGID_)
    {
        this->runAction(moveL);
    }
}

// ��ȡ��ǰʱ�䣨�룩
float Hero::getCurrentTime()
{
    // ��ȡ��ǰʱ�������������
    return std::chrono::duration<float>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void Hero::addBlockAtPosition(cocos2d::Vec2 position)
{
    // ����������Χ����Ч�������
    float validRange = 100.0f;  // ��Ч����뾶���ɸ�����Ҫ����
    cocos2d::Vec2 heroPosition = this->getPosition();
    CCLOG("hero(%f, %f)", heroPosition.x, heroPosition.y);

    // ��ȡ��ǰ����
    auto scene = Director::getInstance()->getRunningScene();

    // �ӳ����л�ȡ����ڵ�
    auto camera = scene->getChildByName("Camera");  // �����������ڵ������� "camera"

    // ��ȡ��������꣨�������꣩
    auto cameraPos = camera->getPosition();

    // ��ȡ��ǰ�����Ŀɼ������С
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 VisSize = Vec2(visibleSize.width, visibleSize.height);
    auto posi = map->getPosition();
    auto mapSize = map->getMapSize();

    // �����λ���Ƿ�����Ч��Χ��
    if ((position + heroPosition - VisSize / 2).distance(heroPosition) > validRange)
    {
        CCLOG("Invalid click. Click is outside the valid range.");
        return;
    }
    CCLOG("touch(%f, %f)", position.x, position.y);

    // ���㾫���������Ļ���꣨���������ƫ�ƣ�
    Vec2 finalSpriteScreenPos = (position - posi + cameraPos - VisSize / 2) / 16;
    int tx = finalSpriteScreenPos.x;
    int ty = mapSize.height - finalSpriteScreenPos.y;

    // �����λ��ת��Ϊ��Ƭ����
    cocos2d::Vec2 tileCoord = Vec2(tx, ty);
    //getTileCoordForPosition(position.x, position.y)
    CCLOG("Tile  (%f, %f).", tileCoord.x, tileCoord.y);

    if (usingItem < 0 || usingItem >= pocketItems.size())
        return;
    int itemkind = pocketItems.at(usingItem)->getItemKind();   // ��ȡ��Ʒ������

    if (itemkind != 1 && itemkind != 4)
        return;                                               // ��Ʒ���ɷ��ã�����ԭ�ϻ�Ҿߣ�
    std::string usingItemName = pocketItems.at(usingItem)->getItemName();       // ��ȡ��Ʒ������
   
    //����λ���Ƿ�������Ƭ
    if (blocksLayer && blocksLayer->getTileGIDAt(tileCoord) != 0)
    {
        CCLOG("Tile already exists at (%f, %f).", tileCoord.x, tileCoord.y);
        return;
    }

    // ����һ������� GID����������� GID Ϊ 182��������� TMX �ļ����޸ģ�
    int dirtBlockGID;
    if (itemkind == 1) {
        dirtBlockGID = dictionaryToPlace[usingItemName[1] - '0'][1];            // ���ֵ���ȡ����Ӧ��GID

        blocksLayer->setTileGID(dirtBlockGID, Vec2(tx, ty));                    // ԭ������Ҫ��ӵ�block�У�����Ӹ���

        // ��ȡ�����ɵ���Ƭ
        auto newTile = blocksLayer->getTileAt(Vec2(tx, ty));
        if (newTile)
        {
            // ������Ƭ����������
            cocos2d::PhysicsMaterial body(1, 0, 1); // ���������������ģ��
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
            chest->setTileGID(dirtBlockGID, Vec2(tx, ty));                                 // ����ǼҾߣ��ֱ�ӵ���Ӧ��ͼ����ȥ
    }
    
    pocketItems.at(usingItem)->setNum(pocketItems.at(usingItem)->getItemNum() - 1);        // ÿ����һ����Ʒ����Ӧ����Ʒ��ȥ1
    pocketItems.at(usingItem)->setCameraMask((unsigned short)CameraFlag::USER1);
    items.at(usingItem)->setNum(pocketItems.at(usingItem)->getItemNum() - 1);              // ÿ����һ����Ʒ��bag��Ҳͬ����ȥ1
    items.at(usingItem)->setCameraMask((unsigned short)CameraFlag::USER1);
    if (usingItemName != "0.png" && usingItemName != "10.png" && usingItemName != "11.png")
        this->calculate();                                                                 // ������ĵĲ�������ʯ�飨����ʯ�鲻�����κζ�����,�����¼���ϳ���

    CCLOG("Block added at (%f, %f).", tileCoord.x, tileCoord.y);
}

// ɾ�����ĺ���
void Hero::removeBlockAtPosition(Vec2 position)
{
    // ����������Χ����Ч�������
    float validRange = 100.0f;  // ��Ч����뾶���ɸ�����Ҫ����
    cocos2d::Vec2 heroPosition = this->getPosition();
    CCLOG("hero(%f, %f)", heroPosition.x, heroPosition.y);

    // ��ȡ��ǰ����
    auto scene = Director::getInstance()->getRunningScene();

    // �ӳ����л�ȡ����ڵ�
    auto camera = scene->getChildByName("Camera");  // �����������ڵ������� "camera"

    // ��ȡ��������꣨�������꣩
    auto cameraPos = camera->getPosition();

    // ��ȡ��ǰ�����Ŀɼ������С
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 VisSize = Vec2(visibleSize.width, visibleSize.height);

    // �����λ���Ƿ�����Ч��Χ��
    if ((position + heroPosition - VisSize / 2).distance(heroPosition) > validRange)
    {
        CCLOG("Invalid click. Click is outside the valid range.");
        return;
    }
    CCLOG("touch(%f, %f)", position.x, position.y);

    // ���㾫���������Ļ���꣨���������ƫ�ƣ�
    Vec2 finalSpriteScreenPos = position + cameraPos - VisSize / 2;

    Vec2 tileCoord = getTileCoordForPosition(finalSpriteScreenPos.x, finalSpriteScreenPos.y);

    int gid = blocksLayer->getTileGIDAt(tileCoord);
    if (gid >= 4 && gid <= 9755)
    {
        if (gid == 5729)
            gid = 4;                              // ���ݵ�������������֮���������
        blocksLayer->removeTileAt(tileCoord);
        CCLOG("Block removed at tile (%f, %f).", tileCoord.x, tileCoord.y);
        std::string Name;             // �������Ʒ������
        // ����GID�ֵ䣬�ҵ���Ӧ����Ʒ����
        for (int i = 0; i < 9; i++) {
            if (dictionaryToPlace[i][1] == gid) {
                Name = std::to_string(dictionaryToPlace[i][0]) + ".png";
                break;
            }
        }
        if (Name.empty())
            return;                               // û���ҵ�Ҫ��ӵ���Ʒ���������������Ϊ����Ϸ�Ľ�׳�ԣ�
        this->pickUpItems(Name);
    }
    else if (chair->getTileGIDAt(tileCoord) == 19209)
    {
        chair->removeTileAt(tileCoord);
        this->pickUpItems("41.png");               // ������ʰȡ������
    }
    else if (toptree->getTileGIDAt(tileCoord) == 11167)
    {
        toptree->removeTileAt(tileCoord);          // �������ã������
    }
    else if (wood->getTileGIDAt(tileCoord) == 347)
    {
        wood->removeTileAt(tileCoord);
        this->pickUpItems("12.png");               // ��ľ��ʰȡ������
    }
    else if (chest->getTileGIDAt(tileCoord) == 9757)
    {
        chest->removeTileAt(tileCoord);
        this->pickUpItems("43.png");               // ������ʰȡ������
    }
    else if (door->getTileGIDAt(tileCoord) == 19210)
    {
        door->removeTileAt(tileCoord);
        this->pickUpItems("42.png");               // ����ʰȡ������
    }
    else if (desk->getTileGIDAt(tileCoord) == 19211)
    {
        desk->removeTileAt(tileCoord);
        this->pickUpItems("40.png");               // ������ʰȡ������
    }
    else
    {
        CCLOG("No block to remove at (%f, %f).", tileCoord.x, tileCoord.y);
    }
}

void Hero::iniItems()
{
    auto item1 = MyItem::create("20.png");
    items.pushBack(item1);  // ��Ʒ1��ͭ��
    auto poccketItem1 = MyItem::create("20.png");
    pocketItems.pushBack(poccketItem1);

    auto item2 = MyItem::create("30.png");
    items.pushBack(item2);  // ��Ʒ2��ͭ��
    auto poccketItem2 = MyItem::create("30.png");
    pocketItems.pushBack(poccketItem2);

    auto item3 = MyItem::create("10.png", 5);
    items.pushBack(item3);  // ��Ʒ3�����飨5����
    auto poccketItem3 = MyItem::create("10.png", 5);
    pocketItems.pushBack(poccketItem3);

    // �������
    /*auto item4 = MyItem::create("12.png", 48);
    items.pushBack(item4);  // ��Ʒ4��ľ�ģ�48����
    auto poccketItem4 = MyItem::create("12.png", 48);
    pocketItems.pushBack(poccketItem4);

    auto item5 = MyItem::create("13.png", 48);
    items.pushBack(item5);  // ��Ʒ5��ͭ��48����
    auto poccketItem5 = MyItem::create("13.png", 48);
    pocketItems.pushBack(poccketItem5);

    auto item6 = MyItem::create("14.png", 48);
    items.pushBack(item6);  // ��Ʒ6������48����
    auto poccketItem6 = MyItem::create("14.png", 48);
    pocketItems.pushBack(poccketItem6);*/

    while (items.size() < 32) {
        auto emptyItem = MyItem::create("0.png");
        items.pushBack(emptyItem);  // ���������е���Ʒ����32��ʱ
    }

    while (pocketItems.size() < 8) {
        auto pocketEmptyItem = MyItem::create("0.png");
        pocketItems.pushBack(pocketEmptyItem);  // ���������е���Ʒ����8��ʱ
    }
    movingItem = MyItem::create("0.png");
    movingItem->retain();                       // ��ʼ�������ƶ�����Ʒ
    usingItem = -1;                             // ��ʼ������ʹ�õ���Ʒ
    this->calculate();                          // ��ʼ�����Ժϳɵ���Ʒ

    this->checkPocket();    // ��ʾ�ڴ���Ʒ�����������е���Ʒ��
    this->checkBag();       // ��ʾ��Ʒ����������������������Ʒ��
    this->checkProduction();// ��ʾ��������

    // �������¼�����
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = [=](EventMouse* event) {
        if (movingItem->isVisible() && itemInMove) {
            itemInMove->setPosition(event->getLocation());
            auto visibleSize = Director::getInstance()->getVisibleSize();
            itemInMove->setPositionY(0.5 * visibleSize.height - itemInMove->getPosition().y);
            itemInMove->setPositionX(itemInMove->getPosition().x - 0.5 * visibleSize.width);   // ����ת��
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

//***********�������
void Hero::takedamage(int damage) {
    health -= damage;
    CCLOG("hero take %d damage---health%d\n", damage, health);
    if (isRecovering)
        stopHealthRecovery(); //��������ڻ�Ѫ����ϻ�Ѫ
    healthBar->setPercentage((float)health / total_health * 100); //�޸�Ѫ��
    if (health <= 0)
        die();
    else
        startHealthRecovery(); //��ʼ��Ѫ
}
void Hero::die() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    /*��*/
      // �ӳ�һ֡������λ��
    this->scheduleOnce([this](float dt) {
        auto heroBody = this->getPhysicsBody();
        if (heroBody) {
            heroBody->setVelocity(Vec2::ZERO); // ֹͣ�ٶ�
        }
        // ����λ�õ���Ļ����
        this->setPosition(Director::getInstance()->getVisibleSize() / 2);
        CCLOG("���������õ���Ļ���ģ�");
        }, 0.0f, "reset_hero_position");


    health = total_health;
    healthBar->setPercentage((float)health / total_health * 100);
}
void Hero::startHealthRecovery() {
    if (isRecovering) return; // ����Ѿ��ڻָ��У����ظ�����
    isRecovering = true;

    // �ӳ�3���ʼ�ָ�
    this->scheduleOnce([this](float) {
        this->schedule([this](float) {
            if (health + 5 <= total_health) {
                health += 5; // ÿ�λָ�5��Ѫ��
                healthBar->setPercentage((float)health / total_health * 100);
                CCLOG("recovering... health: %d", health);
            }
            else {
                health = total_health;
                healthBar->setPercentage((float)health / total_health * 100);
                this->unschedule("HealthRecovery"); // ֹͣ��Ѫ
                isRecovering = false;
            }
            }, 1.0f, "HealthRecovery"); // ÿ1��ָ�һ��
        }, 3.0f, "StartRecovery"); // �ӳ�3��������Ѫ
}

void Hero::stopHealthRecovery() {
    this->unschedule("StartRecovery");  // ֹͣ�ӳټ�ʱ
    this->unschedule("HealthRecovery"); // ֹͣ��ʱ��Ѫ
    isRecovering = false;
}

void Hero::addhealthbar() {
    auto currentScene = Director::getInstance()->getRunningScene();
    // ����Ѫ��
    healthBarBG = cocos2d::Sprite::create("health_bar_bg.png"); // Ѫ������
    healthBarBG->setPosition(this->getContentSize().width / 2, this->getContentSize().height + 10);
    this->addChild(healthBarBG);

    auto healthBarFill = cocos2d::Sprite::create("health_bar_fill.png"); // Ѫ�����
    healthBar = cocos2d::ProgressTimer::create(healthBarFill);
    healthBar->setType(cocos2d::ProgressTimer::Type::BAR);
    healthBar->setMidpoint(cocos2d::Vec2(0, 0.5f)); // ��ʼ��Ϊ���
    healthBar->setBarChangeRate(cocos2d::Vec2(1, 0)); // �������
    healthBar->setPosition(healthBarBG->getPosition());
    healthBar->setPercentage(100); // ��ʼ�����
    this->addChild(healthBar);
}

void Hero::AttackMonsters() {
    if (usingItem >= 0 && usingItem < pocketItems.size() && pocketItems.at(usingItem)->getItemKind() == 3)
        canAttack = true;                 // ֻ���ֳ��������ܹ���
    else
        canAttack = false;
    if (canAttack == false || isOnCooldown == true)
        return;
    // �������ǵĹ�����Χ
    auto attackRange = Rect(this->getPositionX() - this->getContentSize().width / 2 - Attackwidth,  // ��߽�
        this->getPositionY() - this->getContentSize().height / 2 - Attackheight,  // �±߽�
        this->getContentSize().width + 2 * Attackwidth,                          // ���
        this->getContentSize().height + 2 * Attackheight);                         // �߶�

    // ʹ�����������ѯ
    auto physicsWorld = this->getScene()->getPhysicsWorld();
    physicsWorld->queryRect([this](PhysicsWorld& world, PhysicsShape& shape, void* data) -> bool {
        auto body = shape.getBody();
        if (body->getCategoryBitmask() == 0x02) { // ���Ŀ����ʲô
            auto node = body->getNode();
            auto monster = dynamic_cast<Silme*>(node);
            if (pocketItems.at(usingItem)->getItemName() == "31.png")
                monster->takeDamage(50); // ���ǹ�������ֳ�������۳�50�˺�
            else
                monster->takeDamage(20); // �ֳ�ͭ����۳�20�˺�
        }
        if (body->getCategoryBitmask() == 0x04) { // ���Ŀ����ʲô
            auto node = body->getNode();
            auto monster = dynamic_cast<Zombie*>(node);
            if (pocketItems.at(usingItem)->getItemName() == "31.png")
                monster->takeDamage(50); // ���ǹ�������ֳ�������۳�50�˺�
            else
                monster->takeDamage(20); // �ֳ�ͭ����۳�20�˺�
        }
        return true; // ������ѯ
        }, attackRange, NULL);
    // ������ȴ
    isOnCooldown = true;
    this->scheduleOnce([this](float) { isOnCooldown = false; }, 1.0f, "AttackCooldown"); // 1����ȴ
}

void Hero::addTouchListenerAttack() {
    auto mouseListener = EventListenerMouse::create();

    // ������¼�
    mouseListener->onMouseDown = [this](EventMouse* event) {
        this->AttackMonsters(); // ���ǹ�����Χ����
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

void Hero::applyFallDamage(float fallSpeed) {
    // �����˺�
    float damage = std::abs(fallSpeed) / falldamagecontrol;
    takedamage(damage);
    CCLOG("hero get falldamage:%f\n", damage);
}//����ˤ���˺�