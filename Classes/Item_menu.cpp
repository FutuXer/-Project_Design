#include "GameScene.h"
#include <vector>

USING_NS_CC;
using namespace std;


// 显示口袋中的物品（游戏自始至终都会显示），并更换手中的物品
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

    vector<string> itemImages = {
        "bronzen_pickaxe.png",  // 初始物品1：铜镐
        "bronzen_axe.png",      // 初始物品2：铜斧
        "bronzen_hammer.png",   // 初始物品3：铜锤
        "bronzen_sword.png",    // 初始物品4：铜剑
    };

    while (itemImages.size() < 10) {
        itemImages.push_back("item0.png");  // 当口袋中的物品少于10个时
    }

    // 创建菜单
    Vector<MenuItem*> menuItems;

    for (int i = 0; i < 10; ++i) {
        // 创建灰色半透明的框
        auto box = LayerColor::create(Color4B(128, 128, 128, 128), boxSize.width, boxSize.height);
        box->setOpacity(50);
        box->setPosition(Vec2(i * (boxSize.width + padding), 0));
        pocketLayer->addChild(box, 1, "box" + to_string(i));  // 给每个框一个唯一的名字标签

        // 创建MenuItem
        auto menuItem = MenuItemImage::create(
            itemImages[i],  // 正常状态下的图片
            itemImages[i],  // 选中状态下的图片
            [this, i](Ref* sender) {
                this->onItemMenuClicked(sender, i);
            }
        );
        menuItem->setScale(scaleY);
        menuItem->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));
        menuItems.pushBack(menuItem);

        // 这里不再将 MenuItem 直接添加到 box 中
    }

    // 创建菜单，并添加所有 MenuItem
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    pocketLayer->addChild(menu);

    // 将每个 MenuItem 的父节点设置为对应的 box
    for (int i = 0; i < 10; ++i) {
        auto box = pocketLayer->getChildByName("box" + to_string(i));
        if (box) {
            menuItems.at(i)->setPosition(box->getPosition() + Vec2(boxSize.width / 2, boxSize.height / 2));
        }
    }
}

void GameScene::onItemMenuClicked(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // 确保游戏角色只能同时拿起一个物品
    static int currentItem = -1;
    if (currentItem != -1) {
        // 将之前选中的框恢复为灰色
        auto previousBox = dynamic_cast<LayerColor*>(pocketLayer->getChildByName("box" + std::to_string(currentItem)));
        if (previousBox) {
            previousBox->setColor(Color3B(128, 128, 128));
        }
    }

    // 设置当前选中的物品
    currentItem = itemIndex;

    // 将选中的框变成黄色
    auto selectedBox = dynamic_cast<LayerColor*>(pocketLayer->getChildByName("box" + std::to_string(itemIndex)));
    if (selectedBox) {
        selectedBox->setColor(Color3B(255, 255, 0));
    }

    this->ItemsInHand(itemIndex);
}

void GameScene::ItemsInHand(int itemTag) {
    // 根据itemTag实现拿起不同物品的逻辑
    //auto itemInHand = Sprite::create("")
}

// 检查背包（按下Esc键后显示，初始状态暂时不显示），并整理背包
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

    vector<string> itemImages = {
        "bronzen_pickaxe.png",  // 初始物品1：铜镐
        "bronzen_axe.png",      // 初始物品2：铜斧
        "bronzen_hammer.png",   // 初始物品3：铜锤
        "bronzen_sword.png",    // 初始物品4：铜剑
    };

    while (itemImages.size() < 50) {
        itemImages.push_back("item0.png");  // 当背包袋中的物品少于50个时
    }

    // 创建菜单
    Vector<MenuItem*> menuItems;

    for (int i = 0; i < 50; ++i) {
        // 创建灰色半透明的框
        auto box = LayerColor::create(Color4B(128, 128, 128, 128), boxSize.width, boxSize.height);
        box->setOpacity(50);
        box->setPosition(Vec2((i % 10) * (boxSize.width + paddingX), -(i / 10) * (boxSize.height + paddingY)));
        bagLayer->addChild(box, 1, "box" + to_string(i));  // 给每个框一个唯一的名字标签

        // 创建MenuItem
        auto menuItem = MenuItemImage::create(
            itemImages[i],  // 正常状态下的图片
            "item0.png",  // 选中状态下的图片
           [this, i](Ref* sender) {
                this->ClickToChangePosition(sender, i);        // 选取之后更换位置
            }
        );
        menuItem->setScale(scaleY);
        menuItem->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));
        menuItems.pushBack(menuItem);
    }

    // 创建菜单，并添加所有 MenuItem
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    bagLayer->addChild(menu);

    // 将每个 MenuItem 的父节点设置为对应的 box
    for (int i = 0; i < 50; ++i) {
        auto box = bagLayer->getChildByName("box" + to_string(i));
        if (box) {
            menuItems.at(i)->setPosition(box->getPosition() + Vec2(boxSize.width / 2, boxSize.height / 2));
        }
    }

    bagLayer->setVisible(false);    // 初始化状态下设置为不可见
}

void GameScene::ClickToChangePosition(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // 创建鼠标事件监听器
    auto mouseListener = EventListenerMouse::create();
}