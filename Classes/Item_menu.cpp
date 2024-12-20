#include "GameScene.h"
#include "items.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

void Hero::checkPocket() {
    // 获取可见区域的大小
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // 框的大小
    Size boxSize(50 * scaleY, 50 * scaleY);

    // 物品菜单的横向间距
    float padding = 10 * scaleX;

    for (int i = 0; i < 8; ++i) {
        // 创建灰色半透明的框（即按钮）
        auto pocketButton = Button::create("Button0.png");
        this->addChild(pocketButton, 51, i);          // 给每个框一个唯一的tag
        pocketButton->setCameraMask((unsigned short)CameraFlag::USER1);
        pocketButton->ignoreContentAdaptWithSize(false);     // 允许按钮根据 contentSize 进行调整
        pocketButton->setColor(Color3B(128, 128, 128));
        pocketButton->setOpacity(128);
        pocketButton->setContentSize(boxSize);
        pocketButton->setPosition(Vec2((i + 1) * (boxSize.width + padding) - visibleSize.width / 2, visibleSize.height / 2 - boxSize.height / 3));
        pocketButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->onItemMenuClicked(sender, i);
            }
            });

        // 把pocketItems[i]（初始状态下的）添加到pocketButton[i]之下
        pocketItems.at(i)->setScale(scaleY);
        pocketItems.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));

        pocketButton->addChild(pocketItems.at(i));
        pocketItems.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
    }
}

void Hero::onItemMenuClicked(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // 确保游戏角色只能同时拿起一个物品
    static int currentItem = -1;
    if (currentItem != -1) {
        // 将之前选中的框恢复为灰色
        auto previousBox = this->getChildByTag(currentItem);
        if (previousBox) {
            previousBox->setColor(Color3B(128, 128, 128));
            previousBox->setOpacity(128);
        }
    }

    // 设置当前选中的物品
    currentItem = itemIndex;

    // 将选中的框变成黄色
    auto selectedBox = this->getChildByTag(itemIndex);
    if (selectedBox) {
        selectedBox->setColor(Color3B(255, 255, 0));
        selectedBox->setOpacity(128);
    }
    usingItem = itemIndex;     // 改变正在使用的物品
}

void Hero::checkBag() {
    // 获取可见区域的大小
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 根据设计分辨率计算缩放比例
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // 框的大小
    Size boxSize(40 * scaleY, 40 * scaleY);

    // 物品菜单的横向和纵向间距
    float paddingX = 8 * scaleX;
    float paddingY = 8 * scaleY;

    for (int i = 0; i < 32; ++i) {
        // 创建灰色半透明的框（即按钮）
        auto bagButton = Button::create("Button0.png");
        bagButton->ignoreContentAdaptWithSize(false);     // 允许按钮根据 contentSize 进行调整
        bagButton->setColor(Color3B(128, 128, 128));
        bagButton->setOpacity(128);
        bagButton->setContentSize(boxSize);
        bagButton->setPosition(Vec2((i % 8 + 1) * (boxSize.width + paddingX) - visibleSize.width / 2, visibleSize.height / 2 - (i / 8) * (boxSize.height + paddingY) - boxSize.height / 3));
        bagButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->ClickToChangePosition(sender, i);
            }
            });

        this->addChild(bagButton, 1, i + 10);  // 给每个框一个唯一的tag，为了与pocketButton区分，把它设置成负数
        bagButton->setCameraMask((unsigned short)CameraFlag::USER1);

        // 把items[i]（初始状态下的）添加到bagButton[i]之下
        items.at(i)->setScale(scaleY);
        items.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));
        items.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);

        bagButton->addChild(items.at(i));
        bagButton->setCameraMask((unsigned short)CameraFlag::USER1);
        bagButton->setVisible(false);                                      // 初始条件下不可见
    }
    auto bagLabel = Label::create();
    bagLabel->setString(u8"物品栏");
    this->addChild(bagLabel, 1);
    bagLabel->setCameraMask((unsigned short)CameraFlag::USER1);
    bagLabel->setScale(scaleY);
    bagLabel->setPosition(Vec2(boxSize.width + paddingX - visibleSize.width / 2, visibleSize.height / 2 - 4 * (boxSize.height + paddingY / 2) - boxSize.height / 3));

    bagLabel->setVisible(false);  // 初始化状态下设置为不可见
}

void Hero::ClickToChangePosition(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // 重新安排物品的位置
    if (itemIndex >= 0 && itemIndex < items.size()) {
        // 实质上是交换items向量中的位置
        auto temp = MyItem::create(items.at(itemIndex)->getItemName(), items.at(itemIndex)->getItemNum());
        items.at(itemIndex)->coppyItem(movingItem);
        items.at(itemIndex)->setCameraMask((unsigned short)CameraFlag::USER1);

        // 如果改动了第一行的图标，同步修改口袋中的物品
        if (itemIndex < 8) {
            pocketItems.at(itemIndex)->coppyItem(movingItem);
            pocketItems.at(itemIndex)->setCameraMask((unsigned short)CameraFlag::USER1);
        }

        movingItem->coppyItem(temp);                // 更新正在移动的物品

        // 创更新正在移动中的物品精灵
        if (itemInMove == nullptr) {
            itemInMove = Sprite::create(movingItem->getItemName());
            this->addChild(itemInMove, 1);
            itemInMove->setCameraMask((unsigned short)CameraFlag::USER1);
        }
        else {
            itemInMove->setTexture(movingItem->getItemName());
            itemInMove->setCameraMask((unsigned short)CameraFlag::USER1);
        }
    }
}

void Hero::calculate() {
    for (int i = 0; i < 12; i++) {
        int Index;
        int ingredient1 = findItemCount(std::to_string(dictionary[i][2]) + ".png", Index);
        int ingredient2 = findItemCount(std::to_string(dictionary[i][4]) + ".png", Index);
        // 若原料充足，则在itemsToBeMade的相应位置显示该物品，表示它可以被合成，则显示出来
        if (dictionary[i][0] != 0 && ingredient1 >= dictionary[i][1] && ingredient2 >= dictionary[i][3]) {
            auto CanBeMade = MyItem::create(std::to_string(dictionary[i][0]) + ".png");
            if (itemsToBeMade.size() < 12)              // 初始化时pushBack
                itemsToBeMade.pushBack(CanBeMade);
            else {                                      // 后续更新
                if (itemsToBeMade.at(i)->getItemName() == "0.png") {
                    itemsToBeMade.at(i)->coppyItem(CanBeMade);
                    itemsToBeMade.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                }
            }
        }
        else {
            auto CannotBe = MyItem::create("0.png");
            if (itemsToBeMade.size() < 12)              // 初始化时pushBack
                itemsToBeMade.pushBack(CannotBe);
            else {                                      // 后续更新
                if (itemsToBeMade.at(i)->getItemName() != "0.png") {
                    itemsToBeMade.at(i)->coppyItem(CannotBe);
                    itemsToBeMade.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                }
            }
        }
        // 否则原料不足，显示空图
    }
}

void Hero::checkProduction() {
    // 获取可见区域的大小
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 根据设计分辨率计算缩放比例
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // 框的大小
    Size boxSize(40 * scaleY, 40 * scaleY);

    // 物品菜单的横向和纵向间距
    float paddingX = 8 * scaleX;
    float paddingY = 8 * scaleY;

    for (int i = 0; i < 12; ++i) {
        // 创建灰色半透明的框（即按钮）
        auto ProduceButton = Button::create("Button0.png");
        ProduceButton->ignoreContentAdaptWithSize(false);     // 允许按钮根据 contentSize 进行调整
        ProduceButton->setColor(Color3B(128, 128, 128));
        ProduceButton->setOpacity(128);
        ProduceButton->setContentSize(boxSize);
        this->addChild(ProduceButton, 1, (i + 1) * 100);     // 给每个框一个唯一的tag（tag从100开始）
        ProduceButton->setCameraMask((unsigned short)CameraFlag::USER1);
        ProduceButton->setVisible(false);
        ProduceButton->setPosition(Vec2((i % 6 + 1) * (boxSize.width + paddingX) - visibleSize.width / 2, -visibleSize.height / 4 - (i / 6 + 1) * (boxSize.height + paddingY) - paddingY));
        ProduceButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->ClickToProduce(sender, i * 10);
            }
            });

        // 把items[i]（初始状态下的）添加到ProduceButton[i]之下
        itemsToBeMade.at(i)->setScale(scaleY);
        itemsToBeMade.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));

        ProduceButton->addChild(itemsToBeMade.at(i));
        itemsToBeMade.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
    }
    auto productionLabel = Label::create();
    productionLabel->setString(u8"制作栏");
    this->addChild(productionLabel, 1);
    productionLabel->setCameraMask((unsigned short)CameraFlag::USER1);
    productionLabel->setVisible(false);
    productionLabel->setScale(scaleY);
    productionLabel->setPosition(Vec2(boxSize.width + paddingX - visibleSize.width / 2, -visibleSize.height / 4 - paddingY));
}

void Hero::ClickToProduce(Ref* sender, int itemIndex) {
    int productIndex = itemIndex / 10, IndexInbag = -1;               // 如果该产品已经存在，则IndexInBag返回在背包中的位置
    std::string productName = itemsToBeMade.at(productIndex)->getItemName();
    if (productName == "0.png")
        return;                          // 所合成的物品为空，直接返回不做反应

    pickUpItems(productName);            // 把产品添加到背包

    // 消耗掉原材料，并更新可以合成的物品
    int IndexInDic = 10 * (productName[0] - '0') + (productName[1] - '0'), line;        // 我们先查一下字典，这是字典该行的第一个字
    for (line = 0; line < 12; line++) {
        if (dictionary[line][0] == IndexInDic)
            break;
    }                                         // 找到字典的这一行
    IndexInbag = -1;
    // 查到两种原材料的编号
    std::string ingredient1Name = std::to_string(dictionary[line][2]) + ".png";
    std::string ingredient2Name = std::to_string(dictionary[line][4]) + ".png";

    // 原材料1减去配方中的数量
    findItemCount(ingredient1Name, IndexInbag);
    items.at(IndexInbag)->setNum(items.at(IndexInbag)->getItemNum() - dictionary[line][1]);
    items.at(IndexInbag)->setCameraMask((unsigned short)CameraFlag::USER1);
    if (IndexInbag < 8) {
        pocketItems.at(IndexInbag)->setNum(pocketItems.at(IndexInbag)->getItemNum() - dictionary[line][1]);
        pocketItems.at(IndexInbag)->setCameraMask((unsigned short)CameraFlag::USER1);
    }

    // 原材料2减去配方中的数量
    if (ingredient2Name != "0.png") {
        IndexInbag = -1;
        findItemCount(ingredient2Name, IndexInbag);
        items.at(IndexInbag)->setNum(items.at(IndexInbag)->getItemNum() - dictionary[line][3]);
        items.at(IndexInbag)->setCameraMask((unsigned short)CameraFlag::USER1);
        if (IndexInbag < 8) {
            pocketItems.at(IndexInbag)->setNum(pocketItems.at(IndexInbag)->getItemNum() - dictionary[line][3]);
            pocketItems.at(IndexInbag)->setCameraMask((unsigned short)CameraFlag::USER1);
        }
    }

    this->calculate();
}

void Hero::pickUpItems(std::string Name) {
    int positionInbag = -1;                                // 把掉落的物品移动到背包中
    if (findItemCount(Name, positionInbag) > 0 && Name[0]!='2' && Name[0] != '3') {          // 已有,数量加一    // 后两个条件是希望兼容合成物品时的添加
        items.at(positionInbag)->setNum(items.at(positionInbag)->getItemNum() + 1);
        items.at(positionInbag)->setCameraMask((unsigned short)CameraFlag::USER1);
        if (positionInbag < 8) {
            pocketItems.at(positionInbag)->setNum(pocketItems.at(positionInbag)->getItemNum() + 1);
            pocketItems.at(positionInbag)->setCameraMask((unsigned short)CameraFlag::USER1);
        }
    }
    else {
        for (int i = 0; i < items.size(); ++i) {
            if (items.at(i)->getItemName() == "0.png") {
                // 尚未获得，把新物品放在第一个空位置上
                auto newItem = MyItem::create(Name);
                items.at(i)->coppyItem(newItem);
                items.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                // 如果改动了第一行的图标，同步修改口袋中的物品
                if (i < 8) {
                    pocketItems.at(i)->coppyItem(newItem);
                    pocketItems.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                }
                break;
            }
        }
    }
    if (Name != "10.png" && Name != "11.png")
        this->calculate();     // 如果不是土、石块，则更新可以合成的物品（土、石块最常见但不能合成任何物品）
}