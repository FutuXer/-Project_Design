#include "AppDelegate.h"
#include "StartScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

bool AppDelegate::applicationDidFinishLaunching()
{
    // ��ʼ�� Director
    auto director = Director::getInstance();
    auto glView = director->getOpenGLView();

    // ��� glView Ϊ�գ�����һ���µĴ���
    if (!glView)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glView = GLViewImpl::createWithRect("Simple Game", Rect(0, 0, 800, 600));
#else
        glView = GLViewImpl::create("Simple Game");
#endif
        director->setOpenGLView(glView);
    }

    // ������Ʒֱ��ʴ�С
    glView->setDesignResolutionSize(800, 600, ResolutionPolicy::SHOW_ALL);

    // ��ʾ FPS����ѡ��
    director->setDisplayStats(false);

    // ���� FPS ֡��Ϊ 60
    director->setAnimationInterval(1.0f / 60);

    // ���������п�ʼ����
    auto startScene = StartScene::createScene();
    director->runWithScene(startScene);
    return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
    // ���ʹ����Ч��������������ͣ��Ч
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();
    // ���ʹ����Ч������������ָ���Ч
}
