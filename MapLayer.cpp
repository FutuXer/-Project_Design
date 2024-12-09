#include "MapLayer.h"
#include "Player.h"  // 引入 Player 头文件
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
    // 清理现有的地图
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

    // 生成一层土块（假设地面位于 y = 1）
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        map[x][1] = Sprite::create("dirt_block.png");
        map[x][1]->setPosition(Vec2(x * 40, 40 + 200));  // y = 1 对应像素位置
        this->addChild(map[x][1]);
    }

    // 在地图上添加一个额外的土块形成障碍物（例如在 x = 5, y = 2 处）
    int obstacleX = 5;
    int obstacleY = 2;
    map[obstacleX][obstacleY] = Sprite::create("dirt_block.png");
    map[obstacleX][obstacleY]->setPosition(Vec2(obstacleX * 40, obstacleY * 40 + 200));
    this->addChild(map[obstacleX][obstacleY]);
}

// 碰撞检测，判断新位置是否与地图上的土块碰撞
bool MapLayer::checkCollisionWithBlocks(Player* player, float newX, float newY)
{
    // 获取玩家的碰撞矩形
    Rect playerRect = player->getBoundingBox();

    // 遍历地图上的每个土块，检查是否发生碰撞
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            if (map[x][y] != nullptr)  // 如果存在土块
            {
                // 获取土块的碰撞矩形
                Rect blockRect = map[x][y]->getBoundingBox();

                // 判断精灵的新位置是否与土块发生碰撞
                if (playerRect.intersectsRect(blockRect))
                {
                    return true;  // 发生碰撞，返回 true
                }
            }
        }
    }
    return false;  // 没有碰撞
}

void MapLayer::movePlayerLeft(Player* player, float screenWidth)
{
    float currentX = player->getPositionX();
    float currentY = player->getPositionY();
    float newX = currentX - MOVE_DISTANCE;

    if (!checkCollisionWithBlocks(player, newX, currentY))  // 检查是否与土块碰撞
    {
        player->setPositionX(newX);  // 没有碰撞，更新位置
    }
}

void MapLayer::movePlayerRight(Player* player, float screenWidth)
{
    float currentX = player->getPositionX();
    float currentY = player->getPositionY();
    float newX = currentX + MOVE_DISTANCE;

    if (!checkCollisionWithBlocks(player, newX, currentY))  // 检查是否与土块碰撞
    {
        player->setPositionX(newX);
    }
}

void MapLayer::movePlayerUp(Player* player, float screenHeight)
{
    float currentX = player->getPositionX();
    float currentY = player->getPositionY();
    float newY = currentY + MOVE_DISTANCE;

    if (!checkCollisionWithBlocks(player, currentX, newY))  // 检查是否与土块碰撞
    {
        player->setPositionY(newY);
    }
}

void MapLayer::movePlayerDown(Player* player, float screenHeight)
{
    float currentX = player->getPositionX();
    float currentY = player->getPositionY();
    float newY = currentY - MOVE_DISTANCE;

    if (!checkCollisionWithBlocks(player, currentX, newY))  // 检查是否与土块碰撞
    {
        player->setPositionY(newY);
    }
}
