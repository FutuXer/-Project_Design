#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "cocos2d.h"

class AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    // Ӧ������ʱ����
    virtual bool applicationDidFinishLaunching() override;

    // Ӧ�ý����̨ʱ����
    virtual void applicationDidEnterBackground() override;

    // Ӧ�ûص�ǰ̨ʱ����
    virtual void applicationWillEnterForeground() override;
};

#endif // __APP_DELEGATE_H__
