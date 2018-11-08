#include "GamePlay.h"
#include <ctime>

USING_NS_CC;
int backupNumberChanges;
int backupNumberStart;

GamePlay::GamePlay() :
	_isEndGame(true),
	_numberChanges(25),
	_numberStart(15),
	_titlePressed(nullptr),
	_oldPosTitlePressed(Vec2(0, 0)),
	_result(nullptr)
{
	
}

GamePlay::~GamePlay()
{
	for (auto x : _listTitles)
		CC_SAFE_DELETE(x);

	_listTitles.clear();
	_listPos.clear();

#if CHEAT
	_historyMove.clear();
#endif
}

Scene* GamePlay::createScene()
{
	auto scene = Scene::create();
	auto layer = GamePlay::create();
	scene->addChild(layer);
	return scene;
}

bool GamePlay::init()
{
	if (!Layer::init())
		return false;

	srand(time(NULL));

	createTitles();	
	createMenu();

	auto touchlistener = EventListenerTouchOneByOne::create();
	touchlistener->onTouchBegan = CC_CALLBACK_2(GamePlay::onTouchBegin, this);
	touchlistener->onTouchMoved = CC_CALLBACK_2(GamePlay::onTouchMove, this);
	touchlistener->onTouchEnded = CC_CALLBACK_2(GamePlay::onTouchRelease, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchlistener, this);

	//update
	_listTitles[_numberStart]->setVisible(false);
	backupNumberChanges = _numberChanges;
	backupNumberStart = _numberStart;
	this->schedule(schedule_selector(GamePlay::setupBoard), TIME_SETUP_EACH_TITLE + 0.01, backupNumberChanges, TIME_PRE_PLAY);
	this->scheduleUpdate();

	return true;
}

void GamePlay::update(float dt)
{
	if(!_isEndGame)
		if (checkWin())
			win();

#if CHEAT
	if (_isAuto)
	{
		if (_titleIsAuto != nullptr)
		{
			if (_titleIsAuto->getPosition() == _listPos[_numberStart])
			{
				_oldPosTitlePressed = _listPos[_historyMove[_historyMove.size() - 1]];
				_numberStart = _historyMove[_historyMove.size() - 1];
				_historyMove.pop_back();
				_titleIsAuto = nullptr;
			}
		}
		else
		{
			_titleIsAuto = findTitleByPos(_listPos[_historyMove[_historyMove.size() - 1]]);
			_titleIsAuto->runAction(MoveTo::create(SPEED_MOVE_AUTO, _listPos[_numberStart]));
		}

		if (_historyMove.size() <= 0)_isAuto = false;
	}

#endif
}


void GamePlay::createMenu()
{
	Menu* menu;

	MenuItemFont* buttonReset = MenuItemFont::create("Reset", CC_CALLBACK_0(GamePlay::reset, this));
#if CHEAT
	MenuItemFont* buttonAuto = MenuItemFont::create("Auto", CC_CALLBACK_1(GamePlay::autoPlay, this));
	menu = Menu::create(buttonAuto, buttonReset, nullptr);
#elif
	menu = Menu::create(buttonReset, nullptr);
#endif
	menu->setPosition(buttonAuto->getContentSize().width * 0.75f, _screenSize.height * 0.25);
	menu->alignItemsVertically();
	this->addChild(menu);
}

#if CHEAT
void GamePlay::autoPlay(Ref* pSender)
{
	_isAuto = true;
}
#endif

void GamePlay::createTitles()
{
	Texture2D* textureParent = new Texture2D(); 
	auto image = new Image();
	image->initWithImageFile(IMAGE_PARENT);
	textureParent->initWithImage(image);

	textureParent->autorelease();
	image->autorelease();

	_screenSize = Director::getInstance()->getVisibleSize();
	float minSide = _screenSize.width < _screenSize.height ? _screenSize.width : _screenSize.height;
	//create size of title
	auto sizeImage = textureParent->getContentSize();
	float ratio = minSide / (sizeImage.width);
	ratio *= 0.9;	//to not touch the screen border
	float sizeTitleOnImage = sizeImage.width / NUMBER_TITLES;
	float sizeTitleOnGame = sizeTitleOnImage * ratio;
	_sizeTitle = sizeTitleOnGame;

	//create sprite
	float x = 0, y = 0;
	float startx = _screenSize.width * 0.5f + sizeTitleOnGame * 0.5 - RANGER_TITLES * 0.5 - sizeTitleOnGame - (NUMBER_TITLES / 2 - 1) * (sizeTitleOnGame + RANGER_TITLES);
	float px = startx;
	float py = _screenSize.height * 0.5f - sizeTitleOnGame * 0.5 + RANGER_TITLES * 0.5 + sizeTitleOnGame + (NUMBER_TITLES / 2 - 1) * (sizeTitleOnGame + RANGER_TITLES);
	for (int i = 0; i < NUMBER_TITLES * NUMBER_TITLES; i++)
	{
		_listPos.push_back(Vec2(px, py));	//list right position

		Title* title = Title::createTitle(i + 1, textureParent, Rect(x, y, sizeTitleOnImage, sizeTitleOnImage));
		title->setScale(ratio);
		title->setPosition(px, py);
		this->addChild(title);
		_listTitles.push_back(title);

		//next postion
		px += (sizeTitleOnGame + 5);
		if (px + (sizeTitleOnGame + 5) > _screenSize.width)
		{
			px = startx;
			py -= (sizeTitleOnGame + 5);
		}

		//next rect on image
		x += sizeTitleOnImage;
		if (x == sizeImage.width)
		{
			x = 0;
			y += sizeTitleOnImage;
		}

	}
}

Title* GamePlay::findTitleByPos(const Vec2& pos)
{
	for (auto x : _listTitles)
	{
		if (x->getPosition() == pos)
			return x;
	}
	return nullptr;
}

Vec2& GamePlay::findPosByTitle(const Title* title) const
{
	for (auto x : _listPos)
	{
		if (x == title->getPosition())
			return x;
	}
	return Vec2();
}

// use when init game and reset game to setup board play
void GamePlay::setupBoard(float)
{
#if CHEAT
	CCLOG("play: %d", _numberStart);
	_historyMove.push_back(_numberStart);
#endif
	//get number around;
		std::vector<int> around { _numberStart - NUMBER_TITLES , _numberStart - 1, _numberStart + 1, _numberStart + NUMBER_TITLES };
		for (int j = 0; j < around.size();)
		{
			if (around[j] == _justnumber)
				around.erase(around.begin() + j);
			else if (around[j] < 0 || around[j] >= _listTitles.size())
				around.erase(around.begin() + j);
			else if (abs(_listPos[around[j]].x - _listPos[_numberStart].x) > _sizeTitle + RANGER_TITLES)
				around.erase(around.begin() + j);
			else
				j++;
		}

		//slide title random
		int random = around[rand() % around.size()];
		auto x = findTitleByPos(_listPos[random]);
		if(x)
		{
				x->runAction(MoveTo::create(TIME_SETUP_EACH_TITLE, _listPos[_numberStart]));
				_justnumber = _numberStart;
				_numberStart = random;
		}
	_numberChanges--;
	if (_numberChanges <= 0)
		_isEndGame = false;
}

void GamePlay::mergeImage(float)
{
	for (auto x : _listTitles)
	{
		Vec2 move;
		
		int nx = abs((_screenSize.width * 0.5 - x->getPositionX()) / _sizeTitle) + 1;
		int ny = abs((_screenSize.height * 0.5 - x->getPositionY()) / _sizeTitle) + 1;
		float duration = 0.5;

		if(nx==1) move.x = RANGER_TITLES * 0.5 * nx;
		else move.x = RANGER_TITLES * nx;

		if(ny==1) move.y = RANGER_TITLES * 0.5 * ny;
		else move.y = RANGER_TITLES * ny;

		if (x->getPositionX() > _screenSize.width * 0.5)move.x *= -1;
		if (x->getPositionY() > _screenSize.height * 0.5)move.y *= -1;

		x->runAction(MoveBy::create(duration, move));
	}
}


bool GamePlay::checkWin()
{
	for (int i = 0; i < _listTitles.size(); i++)
	{
		if (_listTitles[i]->getPosition() != _listPos[i])
			return false;
	}
	return true;
}


void GamePlay::win()
{
	CCLOG("Win");
	_isEndGame = true;
	this->scheduleOnce(schedule_selector(GamePlay::mergeImage), 0.5);
	if (_result == nullptr)
	{
		_result = Label::createWithTTF("WIN", LABEL_FONT, _screenSize.height * 0.1);
		_result->setPosition(_result->getContentSize().width * 0.6, _screenSize.height - _result->getContentSize().height * 1.5f);
		_result->setColor(Color3B::YELLOW);
		this->addChild(_result);
	}
	else
	{
		_result->setVisible(true);
	}
	//reset();
}


void GamePlay::lose()
{
	CCLOG("Lose");
	_isEndGame = true;


}


void GamePlay::reset()
{
#if CHEAT
	_isAuto = false;
	_historyMove.clear();
#endif
	_numberChanges = backupNumberChanges;
	_numberStart = backupNumberStart;

	if (_result)
		_result->setVisible(false);

	for (int i = 0; i < _listTitles.size(); i++)
	{
		//_listTitles[i]->setPosition(_listPos[i]);
		_listTitles[i]->runAction(MoveTo::create(0.2, _listPos[i]));
	}
	this->schedule(schedule_selector(GamePlay::setupBoard), TIME_SETUP_EACH_TITLE + 0.01, backupNumberChanges, TIME_PRE_PLAY);
}

bool GamePlay::onTouchBegin(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (!_isEndGame)
	{
		//get number around;
		std::vector<int> around{ _numberStart - NUMBER_TITLES , _numberStart - 1, _numberStart + 1, _numberStart + NUMBER_TITLES };
		for (int j = 0; j < around.size();)
		{
			if (around[j] < 0 || around[j] >= _listTitles.size())
				around.erase(around.begin() + j);
			else if (abs(_listPos[around[j]].x - _listPos[_numberStart].x) > _sizeTitle + RANGER_TITLES)
				around.erase(around.begin() + j);
			else
				j++;
		}

		//get list titles can slide
		std::vector<Title*> titlesCanMove;
		for (auto title : _listTitles)
		{
			for (int i = 0; i < around.size();)
			{
				if (_listPos[around[i]] == title->getPosition())
				{
					titlesCanMove.push_back(title);
					around.erase(around.begin() + i);
				}
				else
				{
					i++;
				}
			}
			if (around.size() <= 0)break;
		}

		Vec2 touchPos = touch->getLocation();

		for (auto x : titlesCanMove)
		{
			if (x->getBoundingBox().containsPoint(touchPos))
			{
				_oldPosTitlePressed = x->getPosition();
				//_spaceTouchAndTitle = touchPos - x->getPosition();
				_titlePressed = x;

				/*if (x->getPositionX() > _listTitles[_numberStart]->getPositionX())
					_directionMove = LEFT;
				else if (x->getPositionX() < _listTitles[_numberStart]->getPositionX())
					_directionMove = RIGHT;
				else if (x->getPositionY() > _listTitles[_numberStart]->getPositionY())
					_directionMove = DOWN;
				else if (x->getPositionY() < _listTitles[_numberStart]->getPositionY())
					_directionMove = UP;*/

				return true;
			}
		}
	}

	return false;
}

void GamePlay::onTouchMove(Touch* touch, Event* event)
{
	//for slide title
	/*float newPosX, newPosY;
	if (_titlePressed != nullptr)
	{
		switch (_directionMove)
		{
		case LEFT:
			newPosX = touch->getLocation().x - _spaceTouchAndTitle.x;
			if (newPosX < _oldPosTitlePressed.x)
				_titlePressed->setPositionX(newPosX);
			break;
		case RIGHT:
			newPosX = touch->getLocation().x - _spaceTouchAndTitle.x;
			if (newPosX > _oldPosTitlePressed.x)
				_titlePressed->setPositionX(newPosX);
			break;
		case UP:
			newPosY = touch->getLocation().y - _spaceTouchAndTitle.y;
			if (newPosY > _oldPosTitlePressed.y)
				_titlePressed->setPositionY(newPosY);
			break;
		case DOWN:
			newPosY = touch->getLocation().y - _spaceTouchAndTitle.y;
			if (newPosY < _oldPosTitlePressed.y)
				_titlePressed->setPositionY(newPosY);
			break;
		default:
			break;
		}
	}*/
}

void GamePlay::onTouchRelease(Touch* touch, Event* event)
{
#if CHEAT
	CCLOG("play: %d", _numberStart);
	_historyMove.push_back(_numberStart);
#endif

	if (_titlePressed)
	{
		_titlePressed->runAction(MoveTo::create(SPEED_MOVE_TITLE, _listPos[_numberStart]));

		for (int i = 0; i < _listPos.size(); i++)
			if (_listPos[i] == _oldPosTitlePressed)
			{
				_numberStart = i;
				break;
			}
	}
	_titlePressed = nullptr;
	//_directionMove = -1;
	//_spaceTouchAndTitle = Vec2(0, 0);
	_oldPosTitlePressed = Vec2(0, 0);
}

/*
	_directionMove : to determine direction of title which is pressed
	_spaceTouchAndTitle : space between touch and title which is pressd if have

	in order to make slide effect when use touch move.
*/