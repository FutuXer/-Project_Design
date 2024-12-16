#ifndef __MY_ITEM_H__
#define __MY_ITEM_H__

#include "cocos2d.h"

class MyItem : public cocos2d::Node {
public:
    static MyItem* create(const std::string& itemName, int num = 1);
    bool init(const std::string& itemName, int num);

    void setItemName(const std::string& name);
    void setNum(int num);
    std::string getItemName() const { return itemName; }
    int getNum() const { return num; }
    void coppyItem(const MyItem* other);

private:
    std::string itemName;
    int num;
    int kind;
    cocos2d::Sprite* itemSprite;
    cocos2d::Label* itemNumLabel;
};

#endif // __MY_ITEM_H__
