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
    void addKeyboardListener(cocos2d::Sprite* hero); // ��Ӽ��̼�����
    void performJump(cocos2d::Sprite* hero);         // ִ����Ծ����
};

#endif // __GAME_SCENE_H__

