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