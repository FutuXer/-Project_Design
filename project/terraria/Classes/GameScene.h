#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"  // ��� UI �ؼ�ͷ�ļ���ȷ����ʹ�� Slider

class GameScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);

private:
    int audioID = -1;  // ��ʼ�� audioID����ֹδ������Ϊ
    cocos2d::ui::Slider* volumeSlider = nullptr;  // ����������ָ��
    cocos2d::Menu* confirmMenu = nullptr;         // ȷ����ť�˵�ָ��

    // ��������
    void showSettings(cocos2d::Ref* sender);  // ��ʾ���ò˵�
    void exitGame(cocos2d::Ref* sender);      // �˳������˵�
    void createVolumeSlider();                // ����������������ȷ����ť
    void removeSettingsMenu();                // �Ƴ����ò˵�
    void createSettingsMenu();                // �������ò˵�
};

#endif // __GAME_SCENE_H__

