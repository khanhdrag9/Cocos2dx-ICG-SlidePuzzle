#include "SplashScreen.h"
//#include "HomeScene.h"
#include "MenuScene.h"

cocos2d::Scene* SplashScreen::createScene()
{
    auto scene = cocos2d::Scene::create();
    auto layer = SplashScreen::create();
    scene->addChild(layer);
    return scene;
}

bool SplashScreen::init()
{
    if(!Layer::init())
        return false;

    cocos2d::Size sc = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    auto background = cocos2d::Sprite::create(SPLASH);

	auto sizeImg = background->getContentSize();
	float scaleSide = (sc.width > sc.height ? sc.width : sc.height);

	background->setScale(scaleSide / sizeImg.width);

    background->setPosition(origin + sc * 0.5f);
    this->addChild(background);
	schedule(schedule_selector(SplashScreen::changeScene), TIME_SPLASH);

    return true;
}

void SplashScreen::changeScene(float)
{
	auto scene = MenuScene::createScene();
	cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(TIME_FADE, scene));
}
