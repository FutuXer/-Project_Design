#include "GameScene.h"
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
    pocketLayer->setPosition(Vec2(10 * scaleX, visibleSize.height - 60 * scaleY));

    // ��Ĵ�С
    Size boxSize(50 * scaleY, 50 * scaleY);

    // ��Ʒ�˵��ĺ�����
    float padding = 10 * scaleX;

    while (itemImages.size() < 8) {
        itemImages.push_back("item0.png");  // ���ڴ��е���Ʒ����8��ʱ
    }

    for (int i = 0; i < 8; ++i) {
        // ������ɫ��͸���Ŀ�
        auto box = LayerColor::create(Color4B(128, 128, 128, 128), boxSize.width, boxSize.height);
        box->setOpacity(50);
        box->setPosition(Vec2(i * (boxSize.width + padding), 0));
        pocketLayer->addChild(box, 1, i);  // ��ÿ����һ��Ψһ��tag

        // ����Button
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

    // ȷ����Ϸ��ɫֻ��ͬʱ����һ����Ʒ
    static int currentItem = -1;
    if (currentItem != -1) {
        // ��֮ǰѡ�еĿ�ָ�Ϊ��ɫ
        auto previousBox = dynamic_cast<LayerColor*>(pocketLayer->getChildByTag(currentItem));
        if (previousBox) {
            previousBox->setColor(Color3B(128, 128, 128));
        }
    }

    // ���õ�ǰѡ�е���Ʒ
    currentItem = itemIndex;

    // ��ѡ�еĿ��ɻ�ɫ
    auto selectedBox = dynamic_cast<LayerColor*>(pocketLayer->getChildByTag(itemIndex));
    if (selectedBox) {
        selectedBox->setColor(Color3B(255, 255, 0));
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
    bagLayer->setPosition(Vec2(10 * scaleX, visibleSize.height - 60 * scaleY));

    // ��Ĵ�С
    Size boxSize(40 * scaleY, 40 * scaleY);

    // ��Ʒ�˵��ĺ����������
    float paddingX = 8 * scaleX;
    float paddingY = 8 * scaleY;

    while (itemImages.size() < 32) {
        itemImages.push_back("item0.png");  // ���������е���Ʒ����32��ʱ
    }

    for (int i = 0; i < 32; ++i) {
        // ������ɫ��͸���Ŀ�
        auto box = LayerColor::create(Color4B(128, 128, 128, 128), boxSize.width, boxSize.height);
        box->setOpacity(50);
        box->setPosition(Vec2((i % 8) * (boxSize.width + paddingX), -(i / 8) * (boxSize.height + paddingY)));
        bagLayer->addChild(box, 1, i);  // ��ÿ����һ��Ψһ��tag

        // ����Button
        auto button = Button::create(itemImages[i]);
        button->setScale(scaleY);
        button->setPosition(Vec2(boxSize.width / 2, boxSize.height / 2));
        button->addTouchEventListener([this, i](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                this->ClickToChangePosition(sender, i);  // ѡȡ֮�����λ��
            }
            });
        box->addChild(button);
    }

    bagLayer->setVisible(false);  // ��ʼ��״̬������Ϊ���ɼ�
}

void GameScene::ClickToChangePosition(Ref* sender, int itemIndex) {
    CCLOG("Item %d menu item clicked", itemIndex);

    static string itemNameInHand = "item0.png";  // ��ʱ���е���Ʒ

    // ֻ�ð�esc�л�pocket����ʱ�Ż���ã���ֹ��������Ϊ�������ʧ��Ʒ
    if (sender == nullptr && itemIndex == -1) {
        itemInMove = nullptr;                  // �ָ������ʽ
        if (itemNameInHand == "item0.png")
            return;                              // �����Ѿ�����
        for (int i = 0; i < itemImages.size(); ++i) {
            if (itemImages[i] == "item0.png") {
                itemImages[i] = itemNameInHand;
                // ���°�ťͼƬ
                auto box = bagLayer->getChildByTag(i);
                if (box) {
                    auto button = dynamic_cast<Button*>(box->getChildren().at(0));
                    if (button) {
                        button->loadTextureNormal(itemImages[i]);
                        button->loadTexturePressed(itemImages[i]);
                    }
                }
                // ����ڵ�һ�У�Ҳͬ�����¿ڴ�
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

        itemNameInHand = "item0.png";            // ����ҹرձ���ʱ��ȷ������һ���ǿյ�
        return;
    }

    // ���°�����Ʒ��λ��
    if (itemIndex >= 0 && itemIndex < itemImages.size()) {
        // �������ƶ��е���Ʒ��ʾ������λ��
        if (itemNameInHand != "item0.png") {
            // ��ʼ����������
            auto mouseListener = EventListenerMouse::create();
            EventMouse* e = (EventMouse*)mouseListener;
            Vec2 mousePosition = e->getLocation();

            auto itemInMove = Sprite::create(itemNameInHand);
            if (itemInMove) {
                itemInMove->setPosition(mousePosition);
            }
            this->addChild(itemInMove, 1);
        }

        // ʵ�����ǽ���ͼƬ
        string temp = itemImages[itemIndex];
        itemImages[itemIndex] = itemNameInHand;
        itemNameInHand = temp;

        // ���°�ťͼƬ
        auto box = bagLayer->getChildByTag(itemIndex);
        if (box) {
            auto button = dynamic_cast<Button*>(box->getChildren().at(0));
            if (button) {
                button->loadTextureNormal(itemImages[itemIndex]);
                button->loadTexturePressed(itemImages[itemIndex]);
            }
        }
        // ����Ķ��˵�һ�е�ͼ�꣬ͬ���޸Ŀڴ��е���Ʒ
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

        // ��������������ƶ��е���Ʒ����
        if (itemInMove == nullptr) {
            itemInMove = Sprite::create(itemNameInHand);
            this->addChild(itemInMove, 1);
        }
        else {
            itemInMove->setTexture(itemNameInHand);
        }
    }
}

// ����ƶ��ص�����
void GameScene::onMouseMove(Event* event) {
    EventMouse* e = static_cast<EventMouse*>(event);
    Vec2 mousePosition = e->getLocationInView();
    mousePosition = Director::getInstance()->convertToGL(mousePosition);
    mousePosition.y = Director::getInstance()->getVisibleSize().height - mousePosition.y; // ת������ 

    // ��ӡ���λ��
    CCLOG("Mouse Position: x = %f, y = %f", mousePosition.x, mousePosition.y);

    // ����Ʒ����������λ��
    if (itemInMove) {
        itemInMove->setPosition(mousePosition);
    }
}