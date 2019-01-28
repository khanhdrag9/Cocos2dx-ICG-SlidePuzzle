#pragma once
#include "cocos2d.h"
#include "Defines.h"
#include <vector>

class Title;

typedef enum listAction {
	LEFT,
	RIGHT,
	UP,
	DOWN,
	LEFT_NEAR,
	RIGHT_NEAR,
	UP_NEAR,
	DOWN_NEAR
} listAction;

class GamePlay : public cocos2d::Layer
{
    
public:
    static const cocos2d::Vec2 size1x1;
    static const cocos2d::Vec2 size3x4;
    static const cocos2d::Vec2 size4x3;
    static const cocos2d::Vec2 size16x9;
    static const cocos2d::Vec2 size9x16;
    
private:
	std::vector<Title*> _listTitles;
	std::vector<cocos2d::Vec2> _listPos;
	bool _isEndGame;
	bool _isOrdering;

	cocos2d::Size _screenSize;
    cocos2d::Vec2 _origin;
    cocos2d::Vec2 _sizeBoard;
    
	float _sizeTitleW;
    float _sizeTitleH;
	int _numberStart;	//index of empty title
	int _justnumber;
	int _numberChanges;

	//for slide title
	Title* _titlePressed;
	//int _directionMove;
	//cocos2d::Vec2 _spaceTouchAndTitle;
	cocos2d::Vec2 _oldPosTitlePressed;

	cocos2d::Label* _result;
	cocos2d::Menu* _menu;

#if CHEAT
	//cheat auto play
	Title* _titleIsAuto = nullptr;
	bool _isAuto = false;
	void autoPlay(cocos2d::Ref* pSender);
	std::vector<int> _historyMove;	
#endif

	void createTitles();
	void createMenu();
	// use when init game and reset game to setup board play
	void setupBoard(float);
	bool checkWin();

	void mergeImage(float);
	Title* findTitleByPos(const cocos2d::Vec2& pos);
	const cocos2d::Vec2& findPosByTitle(const Title* title) const;

	void win();
	void lose();
	void reset();

	GamePlay();
	virtual ~GamePlay();
public:
	static cocos2d::Scene* createScene();
	bool init() override;
	void update(float dt) override;
	bool onTouchBegin(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMove(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchRelease(cocos2d::Touch* touch, cocos2d::Event* event);

	CREATE_FUNC(GamePlay);
};
