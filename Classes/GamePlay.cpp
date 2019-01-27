#include "GamePlay.h"
#include <ctime>
#include "Title.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

USING_NS_CC;

int backupNumberChanges;
int backupNumberStart;

const Vec2 GamePlay::size1x1 = Vec2(4,4);
const Vec2 GamePlay::size3x4 = Vec2(4,4);
const Vec2 GamePlay::size4x3 = Vec2(4,4);
const Vec2 GamePlay::size16x9 = Vec2(6,4);
const Vec2 GamePlay::size9x16 = Vec2(4,6);

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
    
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(MUSIC_PLAY, true);
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.25f);

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
	//menu->setPosition(buttonAuto->getContentSize().width * 0.75f, _screenSize.height * 0.8f);
    menu->setPosition(_screenSize.width / 2.f + _origin.x, _sizeTitleH * _sizeBoard.y + _sizeBoard.y * RANGER_TITLES + _sizeTitleH * 1.5f);
    menu->alignItemsHorizontallyWithPadding(_sizeTitleW);
    
	this->addChild(menu, 2);
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
    _origin = Director::getInstance()->getVisibleOrigin();
	//float minSide = _screenSize.width < _screenSize.height ? _screenSize.width : _screenSize.height;
    auto imageSize = textureParent->getContentSize();

	//create size of title
    float ratio = 1.f;
    if(imageSize.width > imageSize.height)
    {
        
    }
    else if(imageSize.height > imageSize.width)
    {
        
    }
    else
    {
        _sizeBoard = GamePlay::size1x1;
        _sizeTitleW = imageSize.width / _sizeBoard.x;
        _sizeTitleH = imageSize.height / _sizeBoard.y;
        float ratioW = (_screenSize.width / _sizeBoard.x) / _sizeTitleW;
        float ratioH = (_screenSize.height / _sizeBoard.y) / _sizeTitleH;
        
        ratio = ratioW < ratioH ? ratioW : ratioH;
        ratio *= 0.95;
    }
    
    float sizeInImageW = _sizeTitleW;
    float sizeInImageH = _sizeTitleH;
    _sizeTitleW *= ratio;
    _sizeTitleH *= ratio;
    
	//create sprite
    //float centerX = _screenSize.width / 2.f;
    float startx = RANGER_TITLES + _sizeTitleW / 2.f + _origin.x;
    float px = startx;
    float py = RANGER_TITLES + _sizeTitleH / 2.f + _origin.y + _screenSize.height * 0.1f;
    Rect rect = Rect(0, imageSize.height, sizeInImageW, -sizeInImageH);
    
    
    //create in board
    int number = 1;
    for(int i = 0; i < _sizeBoard.y; i++)
    {
        for(int j = 0; j < _sizeBoard.x; j++)
        {
            _listPos.emplace_back(px, py);
            
            auto title = Title::createTitle(number, textureParent, rect);
            title->setScale(ratio);
            title->setPosition(px, py);
            
            this->addChild(title);
            
            _listTitles.push_back(title);
            px += _sizeTitleW + RANGER_TITLES;
            rect.origin.x += sizeInImageW;
        }
        py += _sizeTitleH + RANGER_TITLES;
        px = startx;
        rect.origin.x = 0;
        rect.origin.y -= sizeInImageH;
    }
    
    _numberStart = _sizeBoard.x * _sizeBoard.y - 1;
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

const Vec2& GamePlay::findPosByTitle(const Title* title) const
{
	for (auto& x : _listPos)
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
		std::vector<int> around { _numberStart - (int)_sizeBoard.x , _numberStart - 1, _numberStart + 1, _numberStart + (int)_sizeBoard.x };
		for (int j = 0; j < around.size();)
		{
			if (around[j] == _justnumber)
				around.erase(around.begin() + j);
			else if (around[j] < 0 || around[j] >= _listTitles.size())
				around.erase(around.begin() + j);
			else if (abs(_listPos[around[j]].x - _listPos[_numberStart].x) > _sizeTitleW + RANGER_TITLES)
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
		_result->setPosition(_result->getContentSize().width * 0.6, _screenSize.height - _result->getContentSize().height * 2.0f);
		_result->setColor(Color3B::YELLOW);
		this->addChild(_result);
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        SimpleAudioEngine::getInstance()->playBackgroundMusic(MUSIC_WIN, true);
        SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(1.f);
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
//#if CHEAT
//    _isAuto = false;
//    _historyMove.clear();
//#endif
//    _numberChanges = backupNumberChanges;
//    _numberStart = backupNumberStart;
//
//    if (_result)
//        _result->setVisible(false);
//
//    for (int i = 0; i < _listTitles.size(); i++)
//    {
//        //_listTitles[i]->setPosition(_listPos[i]);
//        _listTitles[i]->runAction(MoveTo::create(0.2, _listPos[i]));
//    }
//    this->schedule(schedule_selector(GamePlay::setupBoard), TIME_SETUP_EACH_TITLE + 0.01, backupNumberChanges, TIME_PRE_PLAY);
    
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->playEffect(SOUND_RESET);
    
    auto gameplay = GamePlay::createScene();
    Director::getInstance()->replaceScene(TransitionFlipY::create(0.5, gameplay));
}

bool GamePlay::onTouchBegin(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (!_isEndGame)
	{
		//get number around;
		std::vector<int> around{ _numberStart - (int)_sizeBoard.x , _numberStart - 1, _numberStart + 1, _numberStart + (int)_sizeBoard.x };
		for (int j = 0; j < around.size();)
		{
			if (around[j] < 0 || around[j] >= _listTitles.size())
				around.erase(around.begin() + j);
			else if (abs(_listPos[around[j]].x - _listPos[_numberStart].x) > _sizeTitleW + RANGER_TITLES * 2)
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

				return true;
			}
		}
	}

	return false;
}

void GamePlay::onTouchMove(Touch* touch, Event* event)
{
	
}

void GamePlay::onTouchRelease(Touch* touch, Event* event)
{
#if CHEAT
	CCLOG("play: %d", _numberStart);
	_historyMove.push_back(_numberStart);
    
    if(_isAuto)return;
#endif

	if (_titlePressed)
	{
        SimpleAudioEngine::getInstance()->playEffect(SOUND_SLIDE);
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
