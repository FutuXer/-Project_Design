#pragma once
#include"cocos2d.h"
#include "items.h"
#define JumpHeight 500 //人物向上跳跃的冲量大小
class Hero :public cocos2d::Sprite
{
    int health; //当前血量
    int total_health; //总血量
    // 控制地图移动的变量
    bool moveLeft = false;
    bool moveRight = false;
    // 跳跃相关
    bool isJumping = false;

    //********怪物相关
    bool isRecovering = false;  //是否在回血
    cocos2d::ProgressTimer* healthBar; // 血条
    cocos2d::Sprite* healthBarBG; //血条背景
    //********攻击相关
    bool canAttack = false;
    bool isOnCooldown = false;
public:
    static Hero* create(const std::string& filename, int initialHealth)
    {
        Hero* sprite = new (std::nothrow) Hero();
        if (sprite && sprite->initWithFile(filename))
        {
            sprite->autorelease();
            sprite->total_health = initialHealth; // 设置初始血量
            sprite->health = sprite->total_health;
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    void addPhy(); //为主角添加物理刚体性质
    void addKeyboardListener();  //添加键盘监听器
    void applyJump(); //主角跳跃
    void performJump();
    void HeroFunc(float delta);
    void setHeroAnimation(const std::string& frame2, const std::string& frame3, const std::string& frame4, const std::string& frame5);
    void checkAndFixHeroCollision(float delta);

    //物品
    /*物品相关*/
    /*物品相关*/
    void iniItems();                                                            // 初始化所有物品界面
    cocos2d::Vector<MyItem*> items;                                             // 存放所有物品的信息数组
    cocos2d::Vector<MyItem*> itemsToBeMade;                                     // 存放所有能被制作的物品
    cocos2d::Vector<MyItem*> pocketItems;                                       // 存放口袋中物品的信息数组（即items的前8个）
    void checkPocket();                                                         // 检查口袋(只显示前10个常用物品，用于更换手里拿的物品)
    void checkBag();                                                            // 检查背包(显示所有随身物品，用于更换口袋中的物品、整理背包)
    void checkProduction();                                                     // 检查可以被制作的物品，显示出来，并可以与背包互动（完成物品制作）
    void onItemMenuClicked(Ref* sender, int itemIndex);                         // 选择物品
    void ClickToChangePosition(Ref* sender, int itemIndex);                     // 更换物品在背包中的位置
    void ClickToProduce(Ref* sender, int itemIndex);                            // 按下按钮来合成物品（按一次合成一个）
    void pickUpItems(std::string Name);                                         // 拾取物品
    MyItem* movingItem;                                                         // 正在移动的物品
    int usingItem;                                                              // 正在被使用的物品（只是序号，根据序号的改编实时调用）
    cocos2d::Sprite* itemInMove;                                                // 显示正在移动的物品（跟随鼠标）
    void calculate();                                                           // 计算目前可以合成的装备                       
    int findItemCount(const std::string& itemName, int& Index) {
        for (int i = 0; i < items.size(); i++) {
            if (items.at(i)->getItemName() == itemName) {
                Index = i;
                return items.at(i)->getItemNum();
            }
        }
        return 0; // 没有找到时返回 0
    }
    // 合成物品的字典：每行首字代表产品编号，随后依次是原料1的个数、原料1的编号、原料2的个数、原料2的编号
    int dictionary[12][5] = {
        20,4,12,8,13,
        21,3,12,8,14,
        30,6,13,0,0,
        31,8,14,0,0,
        40,10,12,0,0,
        41,4,12,0,0,
        42,6,12,0,0,
        43,8,12,2,14,
        51,20,50,0,0,
        52,20,51,0,0,
        0,0,0,0,            // 用来补位，以备后来添加合成的物品
    };
    // 每一行代表一个物品，第一个是物品编号，第二个是gid
    int dictionaryToPlace[9][2]{
        10,4,
        11,1912,
        12,347,
        13,9650,
        14,6788,
        40,19211,
        41,19209,
        42,19210,
        43,9757,
    };

    void addTouchListener();                            // 读入鼠标点击
    void addBlockAtPosition(cocos2d::Vec2 position);
    void removeBlockAtPosition(cocos2d::Vec2 position); // 删除泥块的函数
    float getCurrentTime();                             // 获取当前时间的函数声明


    // 长按相关变量
    int touchStartFrame;
    int touchEndFrame;
    cocos2d::Vec2 touchStartPosition;
    float longPressThreshold = 0.5f; // 长按时间阈值（秒）*/

    //*******怪物相关
    void takedamage(int damage); //受到伤害
    void die(); //死亡逻辑
    void startHealthRecovery(); //回血
    void stopHealthRecovery(); //在受到攻击时打断回血
    void addhealthbar(); //添加血条
    //*******攻击相关
    void AttackMonsters(); //攻击敌怪
    void addTouchListenerAttack(); // 读入鼠标点击(攻击)
    void applyFallDamage(float fallSpeed);  //计算摔落伤害
};