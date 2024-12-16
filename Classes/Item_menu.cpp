#include "GameScene.h"
#include "items.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

void GameScene::checkPocket() {
    pocketLayer = Layer::create();  // 创建口袋图层
    this->addChild(pocketLayer);

    // 获取可见区域的大小
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // 设置菜单Layer的位置，左上角
    pocketLayer->setPosition(Vec2(40 * scaleX, visibleSize.height - 40 * scaleY));

    // 框的大小
    Size boxSize(50 * scaleY, 50 * scaleY);

    // 物品菜单的横向间距
    float padding = 10 * scaleX;

    for (int i = 0; i < 8; ++i) {
        // 创建灰色半透明的框（即按钮）
        auto pocketButton = Button::create("Button0.png");
        pocketButton->ignoreContentAdaptWithSize(false);     // 允许按钮根据 contentSize 进行调整
        pocketButton->setColor(Color3B(128, 128, 128));
        pocketButton->setOpacity(128);
        pocketButton->setContentSize(boxSize);
        pocketButton->setPosition(Vec2(i * (boxSize.width + padding), 0));
        pocketButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->onItemMenuClicked(sender, i);
            }
            });

        pocketLayer->addChild(pocketButton, 1, i);  // 给每个框一个唯一的tag

        // 把pocketItems[i]（初始状态下的）添加到pocketButton[i]之下
      
        pocketItems.at(i)->setScale(scaleY);
        pocketItems.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));

        pocketButton->addChild(pocketItems.at(i));
    }
}

void GameScene::onItemMenuClicked(Ref* sender, int itemIndex) {
   CCLOG("Item %d menu item clicked", itemIndex);

    // 确保游戏角色只能同时拿起一个物品
    static int currentItem = -1;
    if (currentItem != -1) {
        // 将之前选中的框恢复为灰色
        auto previousBox = pocketLayer->getChildByTag(currentItem);
        if (previousBox) {
            previousBox->setColor(Color3B(128, 128, 128));
            previousBox->setOpacity(128);
        }
    }

    // 设置当前选中的物品
    currentItem = itemIndex;

    // 将选中的框变成黄色
    auto selectedBox = pocketLayer->getChildByTag(itemIndex);
    if (selectedBox) {
        selectedBox->setColor(Color3B(255, 255, 0));
        selectedBox->setOpacity(128);
    }

    this->ItemsInHand(itemIndex);
}

void GameScene::ItemsInHand(int itemTag) {
    // 根据itemTag实现拿起不同物品的逻辑
    // auto itemInHand = Sprite::create("");
}

void GameScene::checkBag() {
    bagLayer = Layer::create();
    this->addChild(bagLayer);

    // 获取可见区域的大小
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 根据设计分辨率计算缩放比例
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // 设置菜单Layer的位置，左上角
    bagLayer->setPosition(Vec2(30 * scaleX, visibleSize.height - 30 * scaleY));

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
        bagButton->setPosition(Vec2((i % 8) * (boxSize.width + paddingX), -(i / 8) * (boxSize.height + paddingY)));
        bagButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->ClickToChangePosition(sender, i);
            }
            });

        bagLayer->addChild(bagButton, 1, i);  // 给每个框一个唯一的tag

        // 把items[i]（初始状态下的）添加到bagButton[i]之下
        items.at(i)->setScale(scaleY);
        items.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));

        bagButton->addChild(items.at(i));
    }
    

    bagLayer->setVisible(false);  // 初始化状态下设置为不可见
}

void GameScene::ClickToChangePosition(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // 只用按esc切回pocket界面时才会调用，防止背包中因为整理而丢失物品
    if (sender == nullptr && itemIndex == -1) {
        if (movingItem->getItemName() == "0.png")
            return;                         // 手中已经空了，无需安排
        for (int i = 0; i < items.size(); ++i) {
            if (items.at(i)->getItemName() == "0.png") {
                // 把物品放在第一个空位置上
                items.at(i)->coppyItem(movingItem);           
                // 如果改动了第一行的图标，同步修改口袋中的物品
                if (i < 8) {
                    pocketItems.at(i)->coppyItem(movingItem);
                }
                break;
            }
        }

        movingItem = MyItem::create("0.png");  // 当玩家关闭背包时
        movingItem->retain();
        return;
    }

    // 重新安排物品的位置
    if (itemIndex >= 0 && itemIndex < items.size()) {
        // 实质上是交换items向量中的位置
        auto temp = MyItem::create(items.at(itemIndex)->getItemName(), items.at(itemIndex)->getNum());
        items.at(itemIndex)->coppyItem(movingItem);

        // 如果改动了第一行的图标，同步修改口袋中的物品
        if (itemIndex < 8) {
           pocketItems.at(itemIndex)->coppyItem(movingItem);
        }

        movingItem->coppyItem(temp);                // 更新正在移动的物品
    
        // 创更新正在移动中的物品精灵
        if (itemInMove == nullptr) {
            itemInMove = Sprite::create(movingItem->getItemName());
            this->addChild(itemInMove, 1);
            itemInMove->setVisible(true);
        }
        else {
            itemInMove->setTexture(movingItem->getItemName());
        }
    }
}

void GameScene::calculate() {
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
                if (itemsToBeMade.at(i)->getItemName() == "0.png")
                    itemsToBeMade.at(i)->coppyItem(CanBeMade);
            }
        }
        else {
            auto CannotBe = MyItem::create("0.png");
            if (itemsToBeMade.size() < 12)              // 初始化时pushBack
                itemsToBeMade.pushBack(CannotBe);
            else {                                      // 后续更新
                if (itemsToBeMade.at(i)->getItemName() != "0.png")
                    itemsToBeMade.at(i)->coppyItem(CannotBe);
            }
        }
        // 否则原料不足，显示空图
    }
}

void GameScene::checkProduction() {
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
        bagLayer->addChild(ProduceButton, 1, i * 10);  // 给每个框一个唯一的tag（比物品栏大10倍，区分开）
        ProduceButton->setPosition(Vec2((i % 6) * (boxSize.width + paddingX), -visibleSize.height + (- i / 6 + 3) * (boxSize.height + paddingY)));
        ProduceButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->ClickToProduce(sender, i * 10);
            }
            });

        // 把items[i]（初始状态下的）添加到ProduceButton[i]之下
        itemsToBeMade.at(i)->setScale(scaleY);
        itemsToBeMade.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));

        ProduceButton->addChild(itemsToBeMade.at(i));
    }
}

void GameScene::ClickToProduce(Ref* sender, int itemIndex) {
    int productIndex = itemIndex / 10, IndexInbag = -1;               // 如果该产品已经存在，则IndexInBag返回在背包中的位置
    std::string productName = itemsToBeMade.at(productIndex)->getItemName();
    if (productName == "0.png")
        return;                          // 所合成的物品为空，直接返回不做反应

    // 如果合成的物品已经存在，且不是单体物品（工具和武器），则只需要把它的数量加一即可；否则需要单独开辟一个方块
    if (findItemCount(productName,IndexInbag) > 0 && productName[0] != '2' && productName[0] != '3') {
        items.at(IndexInbag)->setNum(items.at(IndexInbag)->getNum() + 1);          // 已有产品的数量加一
        if (IndexInbag < 8) {
            pocketItems.at(IndexInbag)->setNum(pocketItems.at(IndexInbag)->getNum() + 1);
        }
    }
    else {
        for (int i = 0; i < items.size(); ++i) {
            if (items.at(i)->getItemName() == "0.png") {
                // 把产品放在第一个空位置上
                items.at(i)->coppyItem(itemsToBeMade.at(productIndex));
                // 如果改动了第一行的图标，同步修改口袋中的物品
                if (i < 8) {
                    pocketItems.at(i)->coppyItem(itemsToBeMade.at(productIndex));
                }
                break;
            }
        }
    }

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
    items.at(IndexInbag)->setNum(items.at(IndexInbag)->getNum() - dictionary[line][1]);         
    if (IndexInbag < 8) {
        pocketItems.at(IndexInbag)->setNum(pocketItems.at(IndexInbag)->getNum() - dictionary[line][1]);
    }

    // 原材料2减去配方中的数量
    IndexInbag = -1;
    findItemCount(ingredient2Name, IndexInbag);
    items.at(IndexInbag)->setNum(items.at(IndexInbag)->getNum() - dictionary[line][3]);
    if (IndexInbag < 8) {
        pocketItems.at(IndexInbag)->setNum(pocketItems.at(IndexInbag)->getNum() - dictionary[line][3]);
    }

    this->calculate();
}