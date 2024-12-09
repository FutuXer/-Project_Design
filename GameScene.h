#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);

private:
    void showSettings(cocos2d::Ref* sender);  // 显示“回到主菜单”按钮
    void exitGame(cocos2d::Ref* sender);      // 回到主菜单
};

#endif // __GAME_SCENE_H__
