#include "AppDelegate.h"
#include "StartScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

bool AppDelegate::applicationDidFinishLaunching()
{
    // 初始化 Director
    auto director = Director::getInstance();
    auto glView = director->getOpenGLView();

    // 如果 glView 为空，创建一个新的窗口
    if (!glView)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glView = GLViewImpl::createWithRect("Simple Game", Rect(0, 0, 800, 600));
#else
        glView = GLViewImpl::create("Simple Game");
#endif
        director->setOpenGLView(glView);
    }

    // 设置设计分辨率大小
    glView->setDesignResolutionSize(800, 600, ResolutionPolicy::SHOW_ALL);

    // 显示 FPS（可选）
    director->setDisplayStats(true);

    // 设置 FPS 帧率为 60
    director->setAnimationInterval(1.0f / 60);

    // 创建并运行开始场景
    auto startScene = StartScene::createScene();
    director->runWithScene(startScene);
    return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
    // 如果使用音效，可以在这里暂停音效
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();
    // 如果使用音效，可以在这里恢复音效
}
