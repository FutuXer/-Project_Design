#ifndef __MAP_LAYER_H__
#define __MAP_LAYER_H__
#include "cocos2d.h"
#include "Player.h"  // ���� Player ͷ�ļ�

class MapLayer : public cocos2d::Layer
{
public:
    virtual bool init();
    CREATE_FUNC(MapLayer);

    void generateMap();
    void movePlayerLeft(Player* player, float screenWidth);
    void movePlayerRight(Player* player, float screenWidth);
    void movePlayerUp(Player* player, float screenHeight);
    void movePlayerDown(Player* player, float screenHeight);

private:
    static const int MAP_WIDTH = 50;
    static const int MAP_HEIGHT = 20;
    cocos2d::Sprite* map[MAP_WIDTH][MAP_HEIGHT];

    // ��ײ���
    bool checkCollisionWithBlocks(Player* player, float newX, float newY);
};

#endif // __MAP_LAYER_H__
