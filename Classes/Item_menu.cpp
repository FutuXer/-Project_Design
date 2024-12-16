#include "GameScene.h"
#include "items.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

void GameScene::checkPocket() {
    pocketLayer = Layer::create();  // �����ڴ�ͼ��
    this->addChild(pocketLayer);

    // ��ȡ�ɼ�����Ĵ�С
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // ���ò˵�Layer��λ�ã����Ͻ�
    pocketLayer->setPosition(Vec2(40 * scaleX, visibleSize.height - 40 * scaleY));

    // ��Ĵ�С
    Size boxSize(50 * scaleY, 50 * scaleY);

    // ��Ʒ�˵��ĺ�����
    float padding = 10 * scaleX;

    for (int i = 0; i < 8; ++i) {
        // ������ɫ��͸���Ŀ򣨼���ť��
        auto pocketButton = Button::create("Button0.png");
        pocketButton->ignoreContentAdaptWithSize(false);     // ����ť���� contentSize ���е���
        pocketButton->setColor(Color3B(128, 128, 128));
        pocketButton->setOpacity(128);
        pocketButton->setContentSize(boxSize);
        pocketButton->setPosition(Vec2(i * (boxSize.width + padding), 0));
        pocketButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->onItemMenuClicked(sender, i);
            }
            });

        pocketLayer->addChild(pocketButton, 1, i);  // ��ÿ����һ��Ψһ��tag

        // ��pocketItems[i]����ʼ״̬�µģ���ӵ�pocketButton[i]֮��
      
        pocketItems.at(i)->setScale(scaleY);
        pocketItems.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));

        pocketButton->addChild(pocketItems.at(i));
    }
}

void GameScene::onItemMenuClicked(Ref* sender, int itemIndex) {
   CCLOG("Item %d menu item clicked", itemIndex);

    // ȷ����Ϸ��ɫֻ��ͬʱ����һ����Ʒ
    static int currentItem = -1;
    if (currentItem != -1) {
        // ��֮ǰѡ�еĿ�ָ�Ϊ��ɫ
        auto previousBox = pocketLayer->getChildByTag(currentItem);
        if (previousBox) {
            previousBox->setColor(Color3B(128, 128, 128));
            previousBox->setOpacity(128);
        }
    }

    // ���õ�ǰѡ�е���Ʒ
    currentItem = itemIndex;

    // ��ѡ�еĿ��ɻ�ɫ
    auto selectedBox = pocketLayer->getChildByTag(itemIndex);
    if (selectedBox) {
        selectedBox->setColor(Color3B(255, 255, 0));
        selectedBox->setOpacity(128);
    }

    this->ItemsInHand(itemIndex);
}

void GameScene::ItemsInHand(int itemTag) {
    // ����itemTagʵ������ͬ��Ʒ���߼�
    // auto itemInHand = Sprite::create("");
}

void GameScene::checkBag() {
    bagLayer = Layer::create();
    this->addChild(bagLayer);

    // ��ȡ�ɼ�����Ĵ�С
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // ������Ʒֱ��ʼ������ű���
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // ���ò˵�Layer��λ�ã����Ͻ�
    bagLayer->setPosition(Vec2(30 * scaleX, visibleSize.height - 30 * scaleY));

    // ��Ĵ�С
    Size boxSize(40 * scaleY, 40 * scaleY);

    // ��Ʒ�˵��ĺ����������
    float paddingX = 8 * scaleX;
    float paddingY = 8 * scaleY;

    for (int i = 0; i < 32; ++i) {
        // ������ɫ��͸���Ŀ򣨼���ť��
        auto bagButton = Button::create("Button0.png");
        bagButton->ignoreContentAdaptWithSize(false);     // ����ť���� contentSize ���е���
        bagButton->setColor(Color3B(128, 128, 128));
        bagButton->setOpacity(128);
        bagButton->setContentSize(boxSize);
        bagButton->setPosition(Vec2((i % 8) * (boxSize.width + paddingX), -(i / 8) * (boxSize.height + paddingY)));
        bagButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->ClickToChangePosition(sender, i);
            }
            });

        bagLayer->addChild(bagButton, 1, i);  // ��ÿ����һ��Ψһ��tag

        // ��items[i]����ʼ״̬�µģ���ӵ�bagButton[i]֮��
        items.at(i)->setScale(scaleY);
        items.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));

        bagButton->addChild(items.at(i));
    }

    bagLayer->setVisible(false);  // ��ʼ��״̬������Ϊ���ɼ�
}

void GameScene::ClickToChangePosition(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // ֻ�ð�esc�л�pocket����ʱ�Ż���ã���ֹ��������Ϊ�������ʧ��Ʒ
    if (sender == nullptr && itemIndex == -1) {
        if (movingItem->getItemName() == "0.png")
            return;                         // �����Ѿ����ˣ����谲��
        for (int i = 0; i < items.size(); ++i) {
            if (items.at(i)->getItemName() == "0.png") {
                // ����Ʒ���ڵ�һ����λ����
                items.at(i)->coppyItem(movingItem);           
                // ����Ķ��˵�һ�е�ͼ�꣬ͬ���޸Ŀڴ��е���Ʒ
                if (i < 8) {
                    pocketItems.at(i)->coppyItem(movingItem);
                }
                break;
            }
        }

        movingItem = MyItem::create("0.png");  // ����ҹرձ���ʱ
        movingItem->retain();
        return;
    }

    // ���°�����Ʒ��λ��
    if (itemIndex >= 0 && itemIndex < items.size()) {
        // ʵ�����ǽ���items�����е�λ��
        auto temp = MyItem::create(items.at(itemIndex)->getItemName(), items.at(itemIndex)->getNum());
        items.at(itemIndex)->coppyItem(movingItem);

        // ����Ķ��˵�һ�е�ͼ�꣬ͬ���޸Ŀڴ��е���Ʒ
        if (itemIndex < 8) {
           pocketItems.at(itemIndex)->coppyItem(movingItem);
        }

        movingItem->coppyItem(temp);                // ���������ƶ�����Ʒ
    
        // �����������ƶ��е���Ʒ����
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