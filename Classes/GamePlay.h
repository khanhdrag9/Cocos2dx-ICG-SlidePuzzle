#pragma once
#include "cocos2d.h"
#include <vector>

class GamePlay : public cocos2d::Layer
{
	

	void createTitles();
	// use when init game and reset game to setup board play
	void setupBoard();
	void createActions();
	void checkWin();
	void win();
	void lose();
	void reset();

	GamePlay();
	virtual ~GamePlay();
public:
	static cocos2d::Scene* createScene();
	bool init() override;
	void onTouchMove(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchRelease(cocos2d::Touch* touch, cocos2d::Event* event);

	CREATE_FUNC(GamePlay);
};