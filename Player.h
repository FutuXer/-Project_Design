#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"

class Player : public cocos2d::Sprite
{
public:
    static Player* createPlayer();
    bool checkCollisionWithBlock(cocos2d::Sprite* block);
    // 移动方法
    void moveLeft(float screenWidth);
    void moveRight(float screenWidth);
    void moveUp(float screenHeight);
    void moveDown(float screenHeight);

private:
    static const int MOVE_DISTANCE = 5; // 每次移动的距离
};




#endif // __PLAYER_H__

