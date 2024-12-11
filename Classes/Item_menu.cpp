#include "GameScene.h"
#include <vector>

USING_NS_CC;
using namespace std;


// ��ʾ�ڴ��е���Ʒ����Ϸ��ʼ���ն�����ʾ�������������е���Ʒ
void GameScene::checkPocket() {
    pocketLayer = Layer::create();  // �����ڴ�ͼ��
    this->addChild(pocketLayer);

    // ��ȡ�ɼ�����Ĵ�С
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // ���ò˵�Layer��λ�ã����Ͻ�
    pocketLayer->setPosition(Vec2(10 * scaleX, visibleSize.height - 60 * scaleY));

    // ��Ĵ�С
    Size boxSize(50 * scaleY, 50 * scaleY);

    // ��Ʒ�˵��ĺ�����
    float padding = 10 * scaleX;

    vector<string> itemImages = {
        "bronzen_pickaxe.png",  // ��ʼ��Ʒ1��ͭ��
        "bronzen_axe.png",      // ��ʼ��Ʒ2��ͭ��
        "bronzen_hammer.png",   // ��ʼ��Ʒ3��ͭ��
        "bronzen_sword.png",    // ��ʼ��Ʒ4��ͭ��
    };

    while (itemImages.size() < 10) {
        itemImages.push_back("item0.png");  // ���ڴ��е���Ʒ����10��ʱ
    }

    // �����˵�
    Vector<MenuItem*> menuItems;

    for (int i = 0; i < 10; ++i) {
        // ������ɫ��͸���Ŀ�
        auto box = LayerColor::create(Color4B(128, 128, 128, 128), boxSize.width, boxSize.height);
        box->setOpacity(50);
        box->setPosition(Vec2(i * (boxSize.width + padding), 0));
        pocketLayer->addChild(box, 1, "box" + to_string(i));  // ��ÿ����һ��Ψһ�����ֱ�ǩ

        // ����MenuItem
        auto menuItem = MenuItemImage::create(
            itemImages[i],  // ����״̬�µ�ͼƬ
            itemImages[i],  // ѡ��״̬�µ�ͼƬ
            [this, i](Ref* sender) {
                this->onItemMenuClicked(sender, i);
            }
        );
        menuItem->setScale(scaleY);
        menuItem->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));
        menuItems.pushBack(menuItem);

        // ���ﲻ�ٽ� MenuItem ֱ����ӵ� box ��
    }

    // �����˵������������ MenuItem
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    pocketLayer->addChild(menu);

    // ��ÿ�� MenuItem �ĸ��ڵ�����Ϊ��Ӧ�� box
    for (int i = 0; i < 10; ++i) {
        auto box = pocketLayer->getChildByName("box" + to_string(i));
        if (box) {
            menuItems.at(i)->setPosition(box->getPosition() + Vec2(boxSize.width / 2, boxSize.height / 2));
        }
    }
}

void GameScene::onItemMenuClicked(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // ȷ����Ϸ��ɫֻ��ͬʱ����һ����Ʒ
    static int currentItem = -1;
    if (currentItem != -1) {
        // ��֮ǰѡ�еĿ�ָ�Ϊ��ɫ
        auto previousBox = dynamic_cast<LayerColor*>(pocketLayer->getChildByName("box" + std::to_string(currentItem)));
        if (previousBox) {
            previousBox->setColor(Color3B(128, 128, 128));
        }
    }

    // ���õ�ǰѡ�е���Ʒ
    currentItem = itemIndex;

    // ��ѡ�еĿ��ɻ�ɫ
    auto selectedBox = dynamic_cast<LayerColor*>(pocketLayer->getChildByName("box" + std::to_string(itemIndex)));
    if (selectedBox) {
        selectedBox->setColor(Color3B(255, 255, 0));
    }

    this->ItemsInHand(itemIndex);
}

void GameScene::ItemsInHand(int itemTag) {
    // ����itemTagʵ������ͬ��Ʒ���߼�
    //auto itemInHand = Sprite::create("")
}

// ��鱳��������Esc������ʾ����ʼ״̬��ʱ����ʾ������������
void GameScene::checkBag() {
    bagLayer = Layer::create();
    this->addChild(bagLayer);

    // ��ȡ�ɼ�����Ĵ�С
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // ������Ʒֱ��ʼ������ű���
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // ���ò˵�Layer��λ�ã����Ͻ�
    bagLayer->setPosition(Vec2(10 * scaleX, visibleSize.height - 60 * scaleY));

    // ��Ĵ�С
    Size boxSize(40 * scaleY, 40 * scaleY);

    // ��Ʒ�˵��ĺ����������
    float paddingX = 8 * scaleX;
    float paddingY = 8 * scaleY;

    vector<string> itemImages = {
        "bronzen_pickaxe.png",  // ��ʼ��Ʒ1��ͭ��
        "bronzen_axe.png",      // ��ʼ��Ʒ2��ͭ��
        "bronzen_hammer.png",   // ��ʼ��Ʒ3��ͭ��
        "bronzen_sword.png",    // ��ʼ��Ʒ4��ͭ��
    };

    while (itemImages.size() < 50) {
        itemImages.push_back("item0.png");  // ���������е���Ʒ����50��ʱ
    }

    // �����˵�
    Vector<MenuItem*> menuItems;

    for (int i = 0; i < 50; ++i) {
        // ������ɫ��͸���Ŀ�
        auto box = LayerColor::create(Color4B(128, 128, 128, 128), boxSize.width, boxSize.height);
        box->setOpacity(50);
        box->setPosition(Vec2((i % 10) * (boxSize.width + paddingX), -(i / 10) * (boxSize.height + paddingY)));
        bagLayer->addChild(box, 1, "box" + to_string(i));  // ��ÿ����һ��Ψһ�����ֱ�ǩ

        // ����MenuItem
        auto menuItem = MenuItemImage::create(
            itemImages[i],  // ����״̬�µ�ͼƬ
            "item0.png",  // ѡ��״̬�µ�ͼƬ
           [this, i](Ref* sender) {
                this->ClickToChangePosition(sender, i);        // ѡȡ֮�����λ��
            }
        );
        menuItem->setScale(scaleY);
        menuItem->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));
        menuItems.pushBack(menuItem);
    }

    // �����˵������������ MenuItem
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    bagLayer->addChild(menu);

    // ��ÿ�� MenuItem �ĸ��ڵ�����Ϊ��Ӧ�� box
    for (int i = 0; i < 50; ++i) {
        auto box = bagLayer->getChildByName("box" + to_string(i));
        if (box) {
            menuItems.at(i)->setPosition(box->getPosition() + Vec2(boxSize.width / 2, boxSize.height / 2));
        }
    }

    bagLayer->setVisible(false);    // ��ʼ��״̬������Ϊ���ɼ�
}

void GameScene::ClickToChangePosition(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // ��������¼�������
    auto mouseListener = EventListenerMouse::create();
}