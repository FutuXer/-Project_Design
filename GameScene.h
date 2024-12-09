#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);

private:
    void showSettings(cocos2d::Ref* sender);  // ��ʾ���ص����˵�����ť
    void exitGame(cocos2d::Ref* sender);      // �ص����˵�
};

#endif // __GAME_SCENE_H__
