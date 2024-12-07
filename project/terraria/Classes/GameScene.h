#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    virtual void update(float delta) override;
    CREATE_FUNC(GameScene);

private:
    void addKeyboardListener(cocos2d::Sprite* hero); // Ìí¼Ó¼üÅÌ¼àÌýÆ÷
    void performJump(cocos2d::Sprite* hero);         // Ö´ÐÐÌøÔ¾¶¯×÷
};

#endif // __GAME_SCENE_H__

