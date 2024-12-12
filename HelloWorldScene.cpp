/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include"monster.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

void HelloWorld::MonsterFunc(float dt) {
    // 获取屏幕大小
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    if (numberOfMonsters < 6) {
        // 随机生成怪物的坐标
        float x_S = origin.x + RandomHelper::random_real(0.0f, visibleSize.width); //史莱姆的坐标
        float x_Z = origin.x + RandomHelper::random_real(0.0f, visibleSize.width); //僵尸的坐标
        int choice = RandomHelper::random_int(0, 1); //选择生成史莱姆还是僵尸
        if (choice == 0) {
            auto silme = Silme::create("slime.png", 100);
            silme->setPosition(Vec2(x_S, silme->getContentSize().height / 2));
            silme->addPhy();
            silme->addjellyeffect();
            silme->addhealthbar();
            this->addChild(silme);
            silme->schedule(CC_SCHEDULE_SELECTOR(Silme::silme_move), 3);
        }
        if (choice == 1) {
            auto zombie = Zombie::create("zombie1.png", 100);
            zombie->setPosition(Vec2(x_Z, zombie->getContentSize().height / 2));
            zombie->addPhy();
            zombie->addAnimate();
            zombie->addhealthbar();
            this->addChild(zombie);
            zombie->schedule(CC_SCHEDULE_SELECTOR(Zombie::zombie_move));
        }
        numberOfMonsters++;
    }
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    initWithPhysics();
    this->getPhysicsWorld()->setGravity(Vec2(0, -980));

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
   

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    this->schedule(CC_SCHEDULE_SELECTOR(HelloWorld::MonsterFunc), 10.0f);

    auto airWall1 = Node::create();

    // 设置空气墙的大小
    auto wallBody1 = PhysicsBody::createBox(Size(visibleSize.width, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));

    // 配置空气墙属性
    wallBody1->setDynamic(false); // 静态物体，不会移动

    // 将物理体附加到空气墙节点
    airWall1->setPhysicsBody(wallBody1);

    // 设置空气墙位置（例如屏幕中心）
    airWall1->setPosition(Vec2(origin.x + visibleSize.width / 2, -0.5));

    // 添加到场景中
    this->addChild(airWall1);

   // auto airWall2 = Node::create();

    // 设置空气墙的大小
    //auto wallBody2 = PhysicsBody::createBox(Size(visibleSize.width, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));

    // 配置空气墙属性
    //wallBody2->setDynamic(false); // 静态物体，不会移动

    // 将物理体附加到空气墙节点
    //airWall2->setPhysicsBody(wallBody2);

    // 设置空气墙位置（例如屏幕中心）
    //airWall2->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));

    // 添加到场景中
    //this->addChild(airWall2);
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
