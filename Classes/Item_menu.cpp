#include "GameScene.h"
#include "items.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

void Hero::checkPocket() {
    // ��ȡ�ɼ�����Ĵ�С
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float scaleX = visibleSize.width / 960;
    float scaleY = visibleSize.height / 640;

    // ��Ĵ�С
    Size boxSize(50 * scaleY, 50 * scaleY);

    // ��Ʒ�˵��ĺ�����
    float padding = 10 * scaleX;

    for (int i = 0; i < 8; ++i) {
        // ������ɫ��͸���Ŀ򣨼���ť��
        auto pocketButton = Button::create("Button0.png");
        this->addChild(pocketButton, 51, i);          // ��ÿ����һ��Ψһ��tag
        pocketButton->setCameraMask((unsigned short)CameraFlag::USER1);
        pocketButton->ignoreContentAdaptWithSize(false);     // ����ť���� contentSize ���е���
        pocketButton->setColor(Color3B(128, 128, 128));
        pocketButton->setOpacity(128);
        pocketButton->setContentSize(boxSize);
        pocketButton->setPosition(Vec2((i + 1) * (boxSize.width + padding) - visibleSize.width / 2, visibleSize.height / 2 - boxSize.height / 3));
        pocketButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->onItemMenuClicked(sender, i);
            }
            });

        // ��pocketItems[i]����ʼ״̬�µģ���ӵ�pocketButton[i]֮��
        pocketItems.at(i)->setScale(scaleY);
        pocketItems.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));

        pocketButton->addChild(pocketItems.at(i));
        pocketItems.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
    }
}

void Hero::onItemMenuClicked(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // ȷ����Ϸ��ɫֻ��ͬʱ����һ����Ʒ
    static int currentItem = -1;
    if (currentItem != -1) {
        // ��֮ǰѡ�еĿ�ָ�Ϊ��ɫ
        auto previousBox = this->getChildByTag(currentItem);
        if (previousBox) {
            previousBox->setColor(Color3B(128, 128, 128));
            previousBox->setOpacity(128);
        }
    }

    // ���õ�ǰѡ�е���Ʒ
    currentItem = itemIndex;

    // ��ѡ�еĿ��ɻ�ɫ
    auto selectedBox = this->getChildByTag(itemIndex);
    if (selectedBox) {
        selectedBox->setColor(Color3B(255, 255, 0));
        selectedBox->setOpacity(128);
    }
    usingItem = itemIndex;     // �ı�����ʹ�õ���Ʒ
}

void Hero::checkBag() {
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

    for (int i = 0; i < 32; ++i) {
        // ������ɫ��͸���Ŀ򣨼���ť��
        auto bagButton = Button::create("Button0.png");
        bagButton->ignoreContentAdaptWithSize(false);     // ����ť���� contentSize ���е���
        bagButton->setColor(Color3B(128, 128, 128));
        bagButton->setOpacity(128);
        bagButton->setContentSize(boxSize);
        bagButton->setPosition(Vec2((i % 8 + 1) * (boxSize.width + paddingX) - visibleSize.width / 2, visibleSize.height / 2 - (i / 8) * (boxSize.height + paddingY) - boxSize.height / 3));
        bagButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->ClickToChangePosition(sender, i);
            }
            });

        this->addChild(bagButton, 1, i + 10);  // ��ÿ����һ��Ψһ��tag��Ϊ����pocketButton���֣��������óɸ���
        bagButton->setCameraMask((unsigned short)CameraFlag::USER1);

        // ��items[i]����ʼ״̬�µģ���ӵ�bagButton[i]֮��
        items.at(i)->setScale(scaleY);
        items.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));
        items.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);

        bagButton->addChild(items.at(i));
        bagButton->setCameraMask((unsigned short)CameraFlag::USER1);
        bagButton->setVisible(false);                                      // ��ʼ�����²��ɼ�
    }
    auto bagLabel = Label::create();
    bagLabel->setString(u8"��Ʒ��");
    this->addChild(bagLabel, 1);
    bagLabel->setCameraMask((unsigned short)CameraFlag::USER1);
    bagLabel->setScale(scaleY);
    bagLabel->setPosition(Vec2(boxSize.width + paddingX - visibleSize.width / 2, visibleSize.height / 2 - 4 * (boxSize.height + paddingY / 2) - boxSize.height / 3));

    bagLabel->setVisible(false);  // ��ʼ��״̬������Ϊ���ɼ�
}

void Hero::ClickToChangePosition(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    // ���°�����Ʒ��λ��
    if (itemIndex >= 0 && itemIndex < items.size()) {
        // ʵ�����ǽ���items�����е�λ��
        auto temp = MyItem::create(items.at(itemIndex)->getItemName(), items.at(itemIndex)->getItemNum());
        items.at(itemIndex)->coppyItem(movingItem);
        items.at(itemIndex)->setCameraMask((unsigned short)CameraFlag::USER1);

        // ����Ķ��˵�һ�е�ͼ�꣬ͬ���޸Ŀڴ��е���Ʒ
        if (itemIndex < 8) {
            pocketItems.at(itemIndex)->coppyItem(movingItem);
            pocketItems.at(itemIndex)->setCameraMask((unsigned short)CameraFlag::USER1);
        }

        movingItem->coppyItem(temp);                // ���������ƶ�����Ʒ

        // �����������ƶ��е���Ʒ����
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
        // ��ԭ�ϳ��㣬����itemsToBeMade����Ӧλ����ʾ����Ʒ����ʾ�����Ա��ϳɣ�����ʾ����
        if (dictionary[i][0] != 0 && ingredient1 >= dictionary[i][1] && ingredient2 >= dictionary[i][3]) {
            auto CanBeMade = MyItem::create(std::to_string(dictionary[i][0]) + ".png");
            if (itemsToBeMade.size() < 12)              // ��ʼ��ʱpushBack
                itemsToBeMade.pushBack(CanBeMade);
            else {                                      // ��������
                if (itemsToBeMade.at(i)->getItemName() == "0.png") {
                    itemsToBeMade.at(i)->coppyItem(CanBeMade);
                    itemsToBeMade.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                }
            }
        }
        else {
            auto CannotBe = MyItem::create("0.png");
            if (itemsToBeMade.size() < 12)              // ��ʼ��ʱpushBack
                itemsToBeMade.pushBack(CannotBe);
            else {                                      // ��������
                if (itemsToBeMade.at(i)->getItemName() != "0.png") {
                    itemsToBeMade.at(i)->coppyItem(CannotBe);
                    itemsToBeMade.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                }
            }
        }
        // ����ԭ�ϲ��㣬��ʾ��ͼ
    }
}

void Hero::checkProduction() {
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
        this->addChild(ProduceButton, 1, (i + 1) * 100);     // ��ÿ����һ��Ψһ��tag��tag��100��ʼ��
        ProduceButton->setCameraMask((unsigned short)CameraFlag::USER1);
        ProduceButton->setVisible(false);
        ProduceButton->setPosition(Vec2((i % 6 + 1) * (boxSize.width + paddingX) - visibleSize.width / 2, -visibleSize.height / 4 - (i / 6 + 1) * (boxSize.height + paddingY) - paddingY));
        ProduceButton->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->ClickToProduce(sender, i * 10);
            }
            });

        // ��items[i]����ʼ״̬�µģ���ӵ�ProduceButton[i]֮��
        itemsToBeMade.at(i)->setScale(scaleY);
        itemsToBeMade.at(i)->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));

        ProduceButton->addChild(itemsToBeMade.at(i));
        itemsToBeMade.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
    }
    auto productionLabel = Label::create();
    productionLabel->setString(u8"������");
    this->addChild(productionLabel, 1);
    productionLabel->setCameraMask((unsigned short)CameraFlag::USER1);
    productionLabel->setVisible(false);
    productionLabel->setScale(scaleY);
    productionLabel->setPosition(Vec2(boxSize.width + paddingX - visibleSize.width / 2, -visibleSize.height / 4 - paddingY));
}

void Hero::ClickToProduce(Ref* sender, int itemIndex) {
    int productIndex = itemIndex / 10, IndexInbag = -1;               // ����ò�Ʒ�Ѿ����ڣ���IndexInBag�����ڱ����е�λ��
    std::string productName = itemsToBeMade.at(productIndex)->getItemName();
    if (productName == "0.png")
        return;                          // ���ϳɵ���ƷΪ�գ�ֱ�ӷ��ز�����Ӧ

    pickUpItems(productName);            // �Ѳ�Ʒ��ӵ�����

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
    items.at(IndexInbag)->setNum(items.at(IndexInbag)->getItemNum() - dictionary[line][1]);
    items.at(IndexInbag)->setCameraMask((unsigned short)CameraFlag::USER1);
    if (IndexInbag < 8) {
        pocketItems.at(IndexInbag)->setNum(pocketItems.at(IndexInbag)->getItemNum() - dictionary[line][1]);
        pocketItems.at(IndexInbag)->setCameraMask((unsigned short)CameraFlag::USER1);
    }

    // ԭ����2��ȥ�䷽�е�����
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
    int positionInbag = -1;                                // �ѵ������Ʒ�ƶ���������
    if (findItemCount(Name, positionInbag) > 0 && Name[0]!='2' && Name[0] != '3') {          // ����,������һ    // ������������ϣ�����ݺϳ���Ʒʱ�����
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
                // ��δ��ã�������Ʒ���ڵ�һ����λ����
                auto newItem = MyItem::create(Name);
                items.at(i)->coppyItem(newItem);
                items.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                // ����Ķ��˵�һ�е�ͼ�꣬ͬ���޸Ŀڴ��е���Ʒ
                if (i < 8) {
                    pocketItems.at(i)->coppyItem(newItem);
                    pocketItems.at(i)->setCameraMask((unsigned short)CameraFlag::USER1);
                }
                break;
            }
        }
    }
    if (Name != "10.png" && Name != "11.png")
        this->calculate();     // �����������ʯ�飬����¿��Ժϳɵ���Ʒ������ʯ����������ܺϳ��κ���Ʒ��
}