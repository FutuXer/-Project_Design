#include "GameScene.h"
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
    pocketLayer->setPosition(Vec2(10 * scaleX, visibleSize.height - 60 * scaleY));

    // 框的大小
    Size boxSize(50 * scaleY, 50 * scaleY);

    // 物品菜单的横向间距
    float padding = 10 * scaleX;

    while (itemImages.size() < 8) {
        itemImages.push_back("item0.png");  // 当口袋中的物品少于8个时
    }

    for (int i = 0; i < 8; ++i) {
        // 创建灰色半透明的框
        auto box = LayerColor::create(Color4B(128, 128, 128, 128), boxSize.width, boxSize.height);
        box->setOpacity(50);
        box->setPosition(Vec2(i * (boxSize.width + padding), 0));
        pocketLayer->addChild(box, 1, i);  // 给每个框一个唯一的tag

        // 创建Button
        auto button = Button::create(itemImages[i]);
        button->setScale(scaleY);
        button->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));
        button->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->onItemMenuClicked(sender, i);
            }
            });
        box->addChild(button);
    }
}

void GameScene::onItemMenuClicked(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // 确保游戏角色只能同时拿起一个物品
    static int currentItem = -1;
    if (currentItem != -1) {
        // 将之前选中的框恢复为灰色
        auto previousBox = dynamic_cast<LayerColor*>(pocketLayer->getChildByTag(currentItem));
        if (previousBox) {
            previousBox->setColor(Color3B(128, 128, 128));
        }
    }

    // 设置当前选中的物品
    currentItem = itemIndex;

    // 将选中的框变成黄色
    auto selectedBox = dynamic_cast<LayerColor*>(pocketLayer->getChildByTag(itemIndex));
    if (selectedBox) {
        selectedBox->setColor(Color3B(255, 255, 0));
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
    bagLayer->setPosition(Vec2(10 * scaleX, visibleSize.height - 60 * scaleY));

    // 框的大小
    Size boxSize(40 * scaleY, 40 * scaleY);

    // 物品菜单的横向和纵向间距
    float paddingX = 8 * scaleX;
    float paddingY = 8 * scaleY;

    while (itemImages.size() < 32) {
        itemImages.push_back("item0.png");  // 当背包袋中的物品少于32个时
    }

    for (int i = 0; i < 32; ++i) {
        // 创建灰色半透明的框
        auto box = LayerColor::create(Color4B(128, 128, 128, 128), boxSize.width, boxSize.height);
        box->setOpacity(50);
        box->setPosition(Vec2((i % 8) * (boxSize.width + paddingX), -(i / 8) * (boxSize.height + paddingY)));
        bagLayer->addChild(box, 1, i);  // 给每个框一个唯一的tag

        // 创建Button
        auto button = Button::create(itemImages[i]);
        button->setScale(scaleY);
        button->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));
        button->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->ClickToChangePosition(sender, i);  // 选取之后更换位置
            }
            });
        box->addChild(button);
    }

    bagLayer->setVisible(false);  // 初始化状态下设置为不可见
}

void GameScene::ClickToChangePosition(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    static string itemNameInHand = "item0.png";  // 此时手中的物品

    // 只用按esc切回pocket界面时才会调用，防止背包中因为整理而丢失物品
    if (sender == nullptr && itemIndex == -1) {
        itemInMove = nullptr;                  // 恢复鼠标样式
        if (itemNameInHand == "item0.png")
            return;                              // 手中已经空了
        for (int i = 0; i < itemImages.size(); ++i) {
            if (itemImages[i] == "item0.png") {
                itemImages[i] = itemNameInHand;
                // 更新按钮图片
                auto box = bagLayer->getChildByTag(i);
                if (box) {
                    auto button = dynamic_cast<Button*>(box->getChildren().at(0));
                    if (button) {
                        button->loadTextureNormal(itemImages[i]);
                        button->loadTexturePressed(itemImages[i]);
                    }
                }
                // 如果在第一行，也同步更新口袋
                if (i < 8) {
                    auto box = pocketLayer->getChildByTag(i);
                    if (box) {
                        auto button = dynamic_cast<Button*>(box->getChildren().at(0));
                        if (button) {
                            button->loadTextureNormal(itemImages[i]);
                            button->loadTexturePressed(itemImages[i]);
                        }
                    }
                }
                break;
            }
        }

        itemNameInHand = "item0.png";            // 当玩家关闭背包时，确保手中一定是空的
        return;
    }

    // 重新安排物品的位置
    if (itemIndex >= 0 && itemIndex < itemImages.size()) {
        // 将正在移动中的物品显示在鼠标的位置
        if (itemNameInHand != "item0.png") {
            // 初始化鼠标监听器
            auto mouseListener = EventListenerMouse::create();
            EventMouse* e = (EventMouse*)mouseListener;
            Vec2 mousePosition = e->getLocation();

            auto itemInMove = Sprite::create(itemNameInHand);
            if (itemInMove) {
                itemInMove->setPosition(mousePosition);
            }
            this->addChild(itemInMove, 1);
        }

        // 实质上是交换图片
        string temp = itemImages[itemIndex];
        itemImages[itemIndex] = itemNameInHand;
        itemNameInHand = temp;

        // 更新按钮图片
        auto box = bagLayer->getChildByTag(itemIndex);
        if (box) {
            auto button = dynamic_cast<Button*>(box->getChildren().at(0));
            if (button) {
                button->loadTextureNormal(itemImages[itemIndex]);
                button->loadTexturePressed(itemImages[itemIndex]);
            }
        }
        // 如果改动了第一行的图标，同步修改口袋中的物品
        if (itemIndex < 8) {
            auto box = pocketLayer->getChildByTag(itemIndex);
            if (box) {
                auto button = dynamic_cast<Button*>(box->getChildren().at(0));
                if (button) {
                    button->loadTextureNormal(itemImages[itemIndex]);
                    button->loadTexturePressed(itemImages[itemIndex]);
                }
            }
        }

        // 创建或更新正在移动中的物品精灵
        if (itemInMove == nullptr) {
            itemInMove = Sprite::create(itemNameInHand);
            this->addChild(itemInMove, 1);
        }
        else {
            itemInMove->setTexture(itemNameInHand);
        }
    }
}

// 鼠标移动回调函数
void GameScene::onMouseMove(Event* event) {
    EventMouse* e = static_cast<EventMouse*>(event);
    Vec2 mousePosition = e->getLocationInView();
    mousePosition = Director::getInstance()->convertToGL(mousePosition);
    mousePosition.y = Director::getInstance()->getVisibleSize().height - mousePosition.y; // 转换坐标 

    // 打印鼠标位置
    CCLOG("Mouse Position: x = %f, y = %f", mousePosition.x, mousePosition.y);

    // 让物品精灵跟随鼠标位置
    if (itemInMove) {
        itemInMove->setPosition(mousePosition);
    }
}