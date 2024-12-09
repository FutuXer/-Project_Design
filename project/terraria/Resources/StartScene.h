#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include "cocos2d.h"

class StartScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(StartScene);

private:
    void startGame(cocos2d::Ref* sender);
    void exitGame(cocos2d::Ref* sender);  // 添加 exitGame 方法的声明
};

#endif // __START_SCENE_H__
