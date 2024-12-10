#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "cocos2d.h"

class AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    // 应用启动时调用
    virtual bool applicationDidFinishLaunching() override;

    // 应用进入后台时调用
    virtual void applicationDidEnterBackground() override;

    // 应用回到前台时调用
    virtual void applicationWillEnterForeground() override;
};

#endif // __APP_DELEGATE_H__
