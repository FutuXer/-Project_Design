#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"

class Player : public cocos2d::Sprite
{
public:
    static Player* createPlayer();
    bool checkCollisionWithBlock(cocos2d::Sprite* block);
    // �ƶ�����
    void moveLeft(float screenWidth);
    void moveRight(float screenWidth);
    void moveUp(float screenHeight);
    void moveDown(float screenHeight);

private:
    static const int MOVE_DISTANCE = 5; // ÿ���ƶ��ľ���
};




#endif // __PLAYER_H__

