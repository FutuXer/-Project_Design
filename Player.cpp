#include "Player.h"

USING_NS_CC;

Player* Player::createPlayer()
{
    Player* player = new (std::nothrow) Player();
    if (player && player->initWithFile("player.png"))  // ȷ���� "player.png" ��Դ
    {
        player->autorelease();
        return player;
    }
    CC_SAFE_DELETE(player);
    return nullptr;
}


void Player::moveLeft(float screenWidth) {
    // ��ȡ����ĳߴ�
    float spriteWidth = this->getContentSize().width;
    // ������ƶ��ķ�Χ
    float minX = spriteWidth / 2;  // �������߽粻�ܳ�����Ļ�����
    float maxX = screenWidth - spriteWidth / 2;  // ������ұ߽粻�ܳ�����Ļ���ұ�

    // ��ȡ��ǰ�����λ��
    float currentX = this->getPositionX();

    // �����µ�λ�ã�ȷ���������߽�
    float newX = currentX - MOVE_DISTANCE;
    if (newX < minX) {
        newX = minX;  // ȷ����������߽�
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
        newX = maxX;  // ȷ���������ұ߽�
    }
    this->setPositionX(newX);
}

void Player::moveUp(float screenHeight) {
    float spriteHeight = this->getContentSize().height;
    float minY = spriteHeight / 2;  // ������±߽粻�ܳ�����Ļ�±�
    float maxY = screenHeight - spriteHeight / 2;  // ������ϱ߽粻�ܳ�����Ļ�ϱ�

    float currentY = this->getPositionY();
    float newY = currentY + MOVE_DISTANCE;
    if (newY > maxY) {
        newY = maxY;  // ȷ���������ϱ߽�
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
        newY = minY;  // ȷ���������±߽�
    }
    this->setPositionY(newY);
}

bool Player::checkCollisionWithBlock(cocos2d::Sprite* block)
{
    Rect playerRect = this->getBoundingBox();
    Rect blockRect = block->getBoundingBox();

    return playerRect.intersectsRect(blockRect);  // �ж��Ƿ��ཻ
}