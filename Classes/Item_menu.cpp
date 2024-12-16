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

void GameScene::calculate() {
    for (int i = 0; i < 12; i++) {
        int Index;                                                      
        int ingredient1 = findItemCount(std::to_string(dictionary[i][2]) + ".png", Index);
        int ingredient2 = findItemCount(std::to_string(dictionary[i][4]) + ".png", Index);
        // ��ԭ�ϳ��㣬����itemsToBeMade����Ӧλ����ʾ����Ʒ����ʾ�����Ա��ϳɣ�����ʾ����
        if (dictionary[i][0] != 0 && ingredient1 >= dictionary[i][1] && ingredient2 >= dictionary[i][3]) {
            auto CanBeMade = MyItem::create(std::to_string(dictionary[i][0]) + ".png");
            if (itemsToBeMade.size() < 12)              // ��ʼ��ʱpushBack
                itemsToBeMade.pushBack(CanBeMade);    
            else {                                      // ��������
                if (itemsToBeMade.at(i)->getItemName() == "0.png")
                    itemsToBeMade.at(i)->coppyItem(CanBeMade);
            }
        }
        else {
            auto CannotBe = MyItem::create("0.png");
            if (itemsToBeMade.size() < 12)              // ��ʼ��ʱpushBack
                itemsToBeMade.pushBack(CannotBe);
            else {                                      // ��������
                if (itemsToBeMade.at(i)->getItemName() != "0.png")
                    itemsToBeMade.at(i)->coppyItem(CannotBe);
            }
        }
        // ����ԭ�ϲ��㣬��ʾ��ͼ
    }
}

void GameScene::checkProduction() {
    // ��ȡ�ɼ�����Ĵ�С
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // ������Ʒֱ��ʼ������ű���
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // ��Ĵ�С
    Size boxSize(40 * scaleY, 40 * scaleY);

    // ��Ʒ�˵��ĺ����������
    float paddingX = 8 * scaleX;
    float paddingY = 8 * scaleY;

    for (int i = 0; i < 12; ++i) {
        // ������ɫ��͸���Ŀ򣨼���ť��
        auto ProduceButton = Button::create("Button0.png");
        ProduceButton->ignoreContentAdaptWithSize(false);     // ����ť���� contentSize ���е���
        ProduceButton->setColor(Color3B(128, 128, 128));
        ProduceButton->setOpacity(128);
        ProduceButton->setContentSize(boxSize);
        bagLayer->addChild(ProduceButton, 1, i * 10);  // ��ÿ����һ��Ψһ��tag������Ʒ����10�������ֿ���
        ProduceButton->setPosition(Vec2((i % 6) * (boxSize.width + paddingX), -visibleSize.height + (- i / 6 + 3) * (boxSize.height + paddingY)));
        ProduceButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->ClickToProduce(sender, i * 10);
            }
            });

        // ��items[i]����ʼ״̬�µģ���ӵ�ProduceButton[i]֮��
        itemsToBeMade.at(i)->setScale(scaleY);
        itemsToBeMade.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));

        ProduceButton->addChild(itemsToBeMade.at(i));
    }
}

void GameScene::ClickToProduce(Ref* sender, int itemIndex) {
    int productIndex = itemIndex / 10, IndexInbag = -1;               // ����ò�Ʒ�Ѿ����ڣ���IndexInBag�����ڱ����е�λ��
    std::string productName = itemsToBeMade.at(productIndex)->getItemName();
    if (productName == "0.png")
        return;                          // ���ϳɵ���ƷΪ�գ�ֱ�ӷ��ز�����Ӧ

    // ����ϳɵ���Ʒ�Ѿ����ڣ��Ҳ��ǵ�����Ʒ�����ߺ�����������ֻ��Ҫ������������һ���ɣ�������Ҫ��������һ������
    if (findItemCount(productName,IndexInbag) > 0 && productName[0] != '2' && productName[0] != '3') {
        items.at(IndexInbag)->setNum(items.at(IndexInbag)->getNum() + 1);          // ���в�Ʒ��������һ
        if (IndexInbag < 8) {
            pocketItems.at(IndexInbag)->setNum(pocketItems.at(IndexInbag)->getNum() + 1);
        }
    }
    else {
        for (int i = 0; i < items.size(); ++i) {
            if (items.at(i)->getItemName() == "0.png") {
                // �Ѳ�Ʒ���ڵ�һ����λ����
                items.at(i)->coppyItem(itemsToBeMade.at(productIndex));
                // ����Ķ��˵�һ�е�ͼ�꣬ͬ���޸Ŀڴ��е���Ʒ
                if (i < 8) {
                    pocketItems.at(i)->coppyItem(itemsToBeMade.at(productIndex));
                }
                break;
            }
        }
    }

    // ���ĵ�ԭ���ϣ������¿��Ժϳɵ���Ʒ
    int IndexInDic = 10 * (productName[0] - '0') + (productName[1] - '0'), line;        // �����Ȳ�һ���ֵ䣬�����ֵ���еĵ�һ����
    for (line = 0; line < 12; line++) {
        if (dictionary[line][0] == IndexInDic)
            break;
    }                                         // �ҵ��ֵ����һ��
    IndexInbag = -1;
    // �鵽����ԭ���ϵı��
    std::string ingredient1Name = std::to_string(dictionary[line][2]) + ".png";
    std::string ingredient2Name = std::to_string(dictionary[line][4]) + ".png";

    // ԭ����1��ȥ�䷽�е�����
    findItemCount(ingredient1Name, IndexInbag);
    items.at(IndexInbag)->setNum(items.at(IndexInbag)->getNum() - dictionary[line][1]);         
    if (IndexInbag < 8) {
        pocketItems.at(IndexInbag)->setNum(pocketItems.at(IndexInbag)->getNum() - dictionary[line][1]);
    }

    // ԭ����2��ȥ�䷽�е�����
    IndexInbag = -1;
    findItemCount(ingredient2Name, IndexInbag);
    items.at(IndexInbag)->setNum(items.at(IndexInbag)->getNum() - dictionary[line][3]);
    if (IndexInbag < 8) {
        pocketItems.at(IndexInbag)->setNum(pocketItems.at(IndexInbag)->getNum() - dictionary[line][3]);
    }

    this->calculate();
}