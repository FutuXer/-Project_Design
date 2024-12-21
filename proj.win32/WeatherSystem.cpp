#include "cocos2d.h"

class WeatherSystem : public cocos2d::Node
{
public:
    WeatherSystem() : isDay(true), timeOfDay(0), speed(1.0f) {}

    bool init()
    {
        // ���ر�����̫����������ͼƬ
        background = cocos2d::Sprite::create("day_background.png");
        this->addChild(background);

        sun = cocos2d::Sprite::create("sun.png");
        sun->setPosition(cocos2d::Vec2(200, 300));
        this->addChild(sun);

        moon = cocos2d::Sprite::create("moon.png");
        moon->setPosition(cocos2d::Vec2(200, 300));
        moon->setVisible(false);
        this->addChild(moon);

        schedule([=](float deltaTime)
            {
            updateWeather(deltaTime);
            }, 1.0f / 60.0f, "weather_update_key");

        return true;
    }

    void updateWeather(float deltaTime)
    {
        timeOfDay += deltaTime * speed;

        // �л�������ҹ��
        if (timeOfDay >= 24)
        {
            timeOfDay = 0;
            isDay = !isDay;
            changeBackground();
        }

        // ̫�����������˶�
        if (isDay)
        {
            sun->setPositionY(300 + 100 * sinf(timeOfDay / 24.0f * M_PI * 2));  // ̫���켣
            moon->setVisible(false);
            sun->setVisible(true);
        }
        else
        {
            moon->setPositionY(300 + 100 * sinf(timeOfDay / 24.0f * M_PI * 2));  // �����켣
            sun->setVisible(false);
            moon->setVisible(true);
        }
    }

    void changeBackground()
    {
        if (isDay)
        {
            background->setTexture("day_background.png");
        }
        else
        {
            background->setTexture("night_background.png");
        }
    }

    static WeatherSystem* create()
    {
        WeatherSystem* pRet = new WeatherSystem();
        if (pRet && pRet->init())
        {
            pRet->autorelease();
            return pRet;
        }
        else
        {
            delete pRet;
            return nullptr;
        }
    }

private:
    cocos2d::Sprite* background;
    cocos2d::Sprite* sun;
    cocos2d::Sprite* moon;
    bool isDay;
    float timeOfDay;
    float speed;
};
