#include "items.h"

USING_NS_CC;

MyItem* MyItem::create(const std::string& itemName, int num) {
    MyItem* pRet = new (std::nothrow) MyItem();
    if (pRet && pRet->init(itemName, num)) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return nullptr;
}

bool MyItem::init(const std::string& itemName, int num) {
    if (!Node::init()) {
        return false;
    }

    this->itemName = itemName;
    this->kind = itemName.empty() ? 0 : int(itemName[0] - '0');  // �Զ��������Ʒ������

    // ����kind�����Ƿ�Ҫ����Ʒ���������
    if (kind == 0 || kind == 2 || kind == 3) {
        this->num = 1;
    }
    else {
        this->num = num;
    }

    // ������Ʒ��ͼƬ
    itemSprite = Sprite::create(itemName);
    this->addChild(itemSprite);

    // ������ʾ��Ʒ�����ı�ǩ
    if (this->num > 1) {
        itemNumLabel = Label::createWithSystemFont(std::to_string(num), "Arial", 12);
        itemNumLabel->setPosition(Vec2(itemSprite->getContentSize().width / 2, itemSprite->getContentSize().height / 2));
        this->addChild(itemNumLabel);
    }
    else {
        itemNumLabel = nullptr;
    }

    return true;
}

void MyItem::setItemName(const std::string& name) {
    this->itemName = name;
    this->kind = name.empty() ? 0 : int(name[0] - '0');  // ������Ʒ����
    itemSprite->setTexture(name);
}

void MyItem::setNum(int num) {
    if (itemNumLabel != nullptr)
        itemNumLabel->removeFromParent();            //  ���֮ǰ�����ֱ��

    this->num = num;
    if (this->num >= 1) {
        itemNumLabel = Label::createWithSystemFont(std::to_string(num), "Arial", 12);
        itemNumLabel->setPosition(Vec2(itemSprite->getContentSize().width / 2, itemSprite->getContentSize().height / 2));
        this->addChild(itemNumLabel);
        if (this->num == 1)
            itemNumLabel->setVisible(false);       // ��Ʒ����Ϊ1����ʾ
    }                                              // ʹ��Label����

    // ����Ʒ�Ѿ�����������
    if (this->num == 0) {
        if (itemSprite)
            itemSprite->removeFromParent();        // ɾȥ����
        this->init("0.png", 1);
    }
}

void MyItem::coppyItem(const MyItem* other) {
    if (itemSprite)
        itemSprite->removeFromParent();
    if (itemNumLabel)
        itemNumLabel->removeFromParent();     //  ���֮ǰ�����

    this->init(other->itemName, other->num);  // ���³�ʼ��
}