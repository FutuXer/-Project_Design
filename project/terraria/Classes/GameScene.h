#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"  // 添加 UI 控件头文件，确保能使用 Slider

class GameScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);

private:
    int audioID = -1;  // 初始化 audioID，防止未定义行为
    cocos2d::ui::Slider* volumeSlider = nullptr;  // 音量滑动条指针
    cocos2d::Menu* confirmMenu = nullptr;         // 确定按钮菜单指针

    // 函数声明
    void showSettings(cocos2d::Ref* sender);  // 显示设置菜单
    void exitGame(cocos2d::Ref* sender);      // 退出到主菜单
    void createVolumeSlider();                // 创建音量滑动条和确定按钮
    void removeSettingsMenu();                // 移除设置菜单
    void createSettingsMenu();                // 创建设置菜单
};

#endif // __GAME_SCENE_H__

