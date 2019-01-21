#pragma once
#include "cocos2d.h"

#define SPLASH "splashKW.jpg"
#define TIME_SPLASH 1.5
#define TIME_FADE 0.5

class SplashScreen : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	bool init() override;
	void changeScene(float);

	CREATE_FUNC(SplashScreen);
};
