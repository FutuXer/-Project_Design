#ifndef __HELLO_WORLD_SCENE_H__
#define __HELLO_WORLD_SCENE_H__

#include "cocos2d.h"
#include "Player.h"
#include "MapLayer.h"
#include <set>

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(HelloWorld);

    void update(float delta) override;
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

private:
    Player* player;
    std::set<cocos2d::EventKeyboard::KeyCode> keysPressed;  // 追踪当前按下的按键
    cocos2d::Size screenSize;  // 屏幕大小，用于边界检测
};

#endif // __HELLO_WORLD_SCENE_H__
