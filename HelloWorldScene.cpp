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

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}


void HelloWorld::silme_move(float dt) {
    Vec2 Pos;
    Pos = silme->getPosition();
    // 创建果冻效果动画
    auto scaleDown = ScaleTo::create(0.2f, 1.2f, 0.8f);  // 压缩
    auto scaleUp = ScaleTo::create(0.2f, 0.8f, 1.2f); // 拉伸
    auto restore = ScaleTo::create(0.2f, 1.0f, 1.0f);  // 恢复  
    auto jumpR = CallFunc::create([this]() {
        this->silme_jump('R'); // 向右跳
        });
    auto jumpL = CallFunc::create([this]() {
        this->silme_jump('L'); // 向左跳
        });
    // 动画序列
    auto jellyEffectR = Sequence::create(scaleDown, scaleUp, jumpR, restore, nullptr);
    auto jellyEffectL = Sequence::create(scaleDown, scaleUp, jumpL, restore, nullptr);
    if (Pos.x < visibleSize.width / 2) {
        silme->setFlippedX(false); // 恢复正常
        silme->runAction(jellyEffectR);
    }
    else {
        silme->setFlippedX(true);  // 左右翻转
        silme->runAction(jellyEffectL);
    }
}

void HelloWorld::silme_jump(char direction) {
    if (direction == 'R')
        silme->getPhysicsBody()->applyImpulse(Vec2(200, 500)); // 向右跳
    if (direction == 'L')
        silme->getPhysicsBody()->applyImpulse(Vec2(-200, 500)); // 向左跳
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

    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
   

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    silme = Sprite::create("slime2r.png");
    silme->setPosition(Vec2(silme->getContentSize().width / 2, silme->getContentSize().height / 2));
    auto silmebody = PhysicsBody::createBox(Size(silme->getContentSize()), PhysicsMaterial(1, 0, 1));
    silmebody->setMass(1);
    silme->setPhysicsBody(silmebody);
    this->addChild(silme);
    auto scaleDown = ScaleTo::create(0.05f, 1.05f, 0.95f);  // 压缩
    auto scaleUp = ScaleTo::create(0.05f, 0.95f, 1.05f); // 拉伸
    auto restore = ScaleTo::create(0.05f, 1.0f, 1.0f);  // 恢复  
    auto jellyEffect = Sequence::create(scaleDown, scaleUp, restore, nullptr);
    silme->runAction(RepeatForever::create(jellyEffect));
    schedule(CC_SCHEDULE_SELECTOR(HelloWorld::silme_move, this), 3);
    
    //PhysicsMaterial nonBounce(1, 0, 1);
    //auto body = PhysicsBody::createBox(sprite->getContentSize(), nonBounce);
    //body->setDynamic(true); // 允许物体受力影响
    //sprite->setPhysicsBody(body);

   
    auto airWall = Node::create();

    // 设置空气墙的大小
    auto wallBody = PhysicsBody::createBox(Size(visibleSize.width, 1), PhysicsMaterial(0.0f, 1.0f, 1.0f));

    // 配置空气墙属性
    wallBody->setDynamic(false); // 静态物体，不会移动

    // 将物理体附加到空气墙节点
    airWall->setPhysicsBody(wallBody);

    // 设置空气墙位置（例如屏幕中心）
    airWall->setPosition(Vec2(origin.x + visibleSize.width / 2, -0.5));

    // 添加到场景中
    this->addChild(airWall);
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
