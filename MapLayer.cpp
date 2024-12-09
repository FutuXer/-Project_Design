#include "MapLayer.h"
#include "Player.h"  // ���� Player ͷ�ļ�
float MOVE_DISTANCE = 5.0f;
USING_NS_CC;

bool MapLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }

    generateMap();

    return true;
}

void MapLayer::generateMap()
{
    // �������еĵ�ͼ
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            if (map[x][y])
            {
                map[x][y]->removeFromParent();
                map[x][y] = nullptr;
            }
        }
    }

    // ����һ�����飨�������λ�� y = 1��
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        map[x][1] = Sprite::create("dirt_block.png");
        map[x][1]->setPosition(Vec2(x * 40, 40 + 200));  // y = 1 ��Ӧ����λ��
        this->addChild(map[x][1]);
    }

    // �ڵ�ͼ�����һ������������γ��ϰ�������� x = 5, y = 2 ����
    int obstacleX = 5;
    int obstacleY = 2;
    map[obstacleX][obstacleY] = Sprite::create("dirt_block.png");
    map[obstacleX][obstacleY]->setPosition(Vec2(obstacleX * 40, obstacleY * 40 + 200));
    this->addChild(map[obstacleX][obstacleY]);
}

// ��ײ��⣬�ж���λ���Ƿ����ͼ�ϵ�������ײ
bool MapLayer::checkCollisionWithBlocks(Player* player, float newX, float newY)
{
    // ��ȡ��ҵ���ײ����
    Rect playerRect = player->getBoundingBox();

    // ������ͼ�ϵ�ÿ�����飬����Ƿ�����ײ
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            if (map[x][y] != nullptr)  // �����������
            {
                // ��ȡ�������ײ����
                Rect blockRect = map[x][y]->getBoundingBox();

                // �жϾ������λ���Ƿ������鷢����ײ
                if (playerRect.intersectsRect(blockRect))
                {
                    return true;  // ������ײ������ true
                }
            }
        }
    }
    return false;  // û����ײ
}

void MapLayer::movePlayerLeft(Player* player, float screenWidth)
{
    float currentX = player->getPositionX();
    float currentY = player->getPositionY();
    float newX = currentX - MOVE_DISTANCE;

    if (!checkCollisionWithBlocks(player, newX, currentY))  // ����Ƿ���������ײ
    {
        player->setPositionX(newX);  // û����ײ������λ��
    }
}

void MapLayer::movePlayerRight(Player* player, float screenWidth)
{
    float currentX = player->getPositionX();
    float currentY = player->getPositionY();
    float newX = currentX + MOVE_DISTANCE;

    if (!checkCollisionWithBlocks(player, newX, currentY))  // ����Ƿ���������ײ
    {
        player->setPositionX(newX);
    }
}

void MapLayer::movePlayerUp(Player* player, float screenHeight)
{
    float currentX = player->getPositionX();
    float currentY = player->getPositionY();
    float newY = currentY + MOVE_DISTANCE;

    if (!checkCollisionWithBlocks(player, currentX, newY))  // ����Ƿ���������ײ
    {
        player->setPositionY(newY);
    }
}

void MapLayer::movePlayerDown(Player* player, float screenHeight)
{
    float currentX = player->getPositionX();
    float currentY = player->getPositionY();
    float newY = currentY - MOVE_DISTANCE;

    if (!checkCollisionWithBlocks(player, currentX, newY))  // ����Ƿ���������ײ
    {
        player->setPositionY(newY);
    }
}
