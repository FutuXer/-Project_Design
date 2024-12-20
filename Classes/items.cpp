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
    this->kind = itemName.empty() ? 0 : int(itemName[0] - '0');  // 自动计算出物品的种类

    // 根据kind决定是否要给物品上数量编号
    if (kind == 0 || kind == 2 || kind == 3) {
        this->num = 1;
    }
    else {
        this->num = num;
    }

    // 设置物品的图片
    itemSprite = Sprite::create(itemName);
    this->addChild(itemSprite);

    // 创建显示物品数量的标签
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
    this->kind = name.empty() ? 0 : int(name[0] - '0');  // 更新物品类型
    itemSprite->setTexture(name);
}

void MyItem::setNum(int num) {
    if (itemNumLabel != nullptr)
        itemNumLabel->removeFromParent();            //  清除之前的数字标记

    this->num = num;
    if (this->num >= 1) {
        itemNumLabel = Label::createWithSystemFont(std::to_string(num), "Arial", 12);
        itemNumLabel->setPosition(Vec2(itemSprite->getContentSize().width / 2, itemSprite->getContentSize().height / 2));
        this->addChild(itemNumLabel);
        if (this->num == 1)
            itemNumLabel->setVisible(false);       // 物品数量为1则不显示
    }                                              // 使得Label显现

    // 此物品已经被消耗完了
    if (this->num == 0) {
        if (itemSprite)
            itemSprite->removeFromParent();        // 删去精灵
        this->init("0.png", 1);
    }
}

void MyItem::coppyItem(const MyItem* other) {
    if (itemSprite)
        itemSprite->removeFromParent();
    if (itemNumLabel)
        itemNumLabel->removeFromParent();     //  清除之前的组分

    this->init(other->itemName, other->num);  // 重新初始化
}