#include "Player.h"

USING_NS_CC;

Player* Player::createPlayer()
{
    Player* player = new (std::nothrow) Player();
    if (player && player->initWithFile("player.png"))  // 确保有 "player.png" 资源
    {
        player->autorelease();
        return player;
    }
    CC_SAFE_DELETE(player);
    return nullptr;
}


void Player::moveLeft(float screenWidth) {
    // 获取精灵的尺寸
    float spriteWidth = this->getContentSize().width;
    // 计算可移动的范围
    float minX = spriteWidth / 2;  // 精灵的左边界不能超过屏幕的左边
    float maxX = screenWidth - spriteWidth / 2;  // 精灵的右边界不能超过屏幕的右边

    // 获取当前精灵的位置
    float currentX = this->getPositionX();

    // 计算新的位置，确保不超出边界
    float newX = currentX - MOVE_DISTANCE;
    if (newX < minX) {
        newX = minX;  // 确保不超出左边界
    }
    this->setPositionX(newX);
}

void Player::moveRight(float screenWidth) {
    float spriteWidth = this->getContentSize().width;
    float minX = spriteWidth / 2;
    float maxX = screenWidth - spriteWidth / 2;

    float currentX = this->getPositionX();
    float newX = currentX + MOVE_DISTANCE;
    if (newX > maxX) {
        newX = maxX;  // 确保不超出右边界
    }
    this->setPositionX(newX);
}

void Player::moveUp(float screenHeight) {
    float spriteHeight = this->getContentSize().height;
    float minY = spriteHeight / 2;  // 精灵的下边界不能超过屏幕下边
    float maxY = screenHeight - spriteHeight / 2;  // 精灵的上边界不能超过屏幕上边

    float currentY = this->getPositionY();
    float newY = currentY + MOVE_DISTANCE;
    if (newY > maxY) {
        newY = maxY;  // 确保不超出上边界
    }
    this->setPositionY(newY);
}

void Player::moveDown(float screenHeight) {
    float spriteHeight = this->getContentSize().height;
    float minY = spriteHeight / 2;
    float maxY = screenHeight - spriteHeight / 2;

    float currentY = this->getPositionY();
    float newY = currentY - MOVE_DISTANCE;
    if (newY < minY) {
        newY = minY;  // 确保不超出下边界
    }
    this->setPositionY(newY);
}

bool Player::checkCollisionWithBlock(cocos2d::Sprite* block)
{
    Rect playerRect = this->getBoundingBox();
    Rect blockRect = block->getBoundingBox();

    return playerRect.intersectsRect(blockRect);  // 判断是否相交
}