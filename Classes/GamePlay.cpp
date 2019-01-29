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
const Vec2 GamePlay::size4x3 = Vec2(2,2);
const Vec2 GamePlay::size16x9 = Vec2(4,4);
const Vec2 GamePlay::size9x16 = Vec2(4,4);

GamePlay::GamePlay() :
	_isEndGame(true),
	_isOrdering(true),
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
	if(!_isEndGame && !_isOrdering)
		if (checkWin())
			win();

#if CHEAT
	if (_isAuto && _historyMove.size() > 0)
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
			//_titleIsAuto->runAction(MoveTo::create(SPEED_MOVE_AUTO, _listPos[_numberStart]));
            _titleIsAuto->setPosition(_listPos[_numberStart]);
            
		}

		if (_historyMove.size() <= 0)
		{
			this->scheduleOnce(schedule_selector(GamePlay::canPlay), 0.5f);
		}
	}

#endif
}


void GamePlay::createMenu()
{
	MenuItemFont* buttonReset = MenuItemFont::create("Reset", CC_CALLBACK_0(GamePlay::reset, this));
#if CHEAT
	MenuItemFont* buttonAuto = MenuItemFont::create("Auto", CC_CALLBACK_1(GamePlay::autoPlay, this));
	_menu = Menu::create(buttonAuto, buttonReset, nullptr);
#elif
	_menu = Menu::create(buttonReset, nullptr);
#endif
	//menu->setPosition(buttonAuto->getContentSize().width * 0.75f, _screenSize.height * 0.8f);
	float sizemore = _sizeTitleW > _sizeTitleH ? _sizeTitleW : _sizeTitleH;
	Vec2 menupos = Vec2(_screenSize.width / 2.f + _origin.x, _sizeTitleH * _sizeBoard.y + _sizeBoard.y * RANGER_TITLES + sizemore + _origin.y);
	_menu->setPosition(menupos);
	_menu->alignItemsHorizontallyWithPadding(_sizeTitleW);
    
	this->addChild(_menu, 2);
}

#if CHEAT
void GamePlay::autoPlay(Ref* pSender)
{
	if(!_isOrdering && !_isEndGame)
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
		float ratioWHImage = imageSize.width / imageSize.height;
		if (ratioWHImage < (16.f / 9.f) && ratioWHImage >(4.f / 3.f))
		{
			_sizeBoard = GamePlay::size9x16;

		}
		else if (ratioWHImage <= (4.f / 3.f))
		{
			_sizeBoard = GamePlay::size4x3;

		}
    }
    else if(imageSize.height > imageSize.width)
    {
		float ratioWHImage = imageSize.height / imageSize.width;
		if (ratioWHImage < (16.f / 9.f) && ratioWHImage > (4.f / 3.f))
		{
			_sizeBoard = GamePlay::size16x9;

		}
		else if(ratioWHImage <= (4.f / 3.f))
		{
			_sizeBoard = GamePlay::size3x4;

		}
    }
    else //for image 1x1
    {
        _sizeBoard = GamePlay::size1x1;
        /*_sizeTitleW = imageSize.width / _sizeBoard.x;
        _sizeTitleH = imageSize.height / _sizeBoard.y;
        float ratioW = (_screenSize.width / _sizeBoard.x) / _sizeTitleW;
        float ratioH = (_screenSize.height / _sizeBoard.y) / _sizeTitleH;
        
        ratio = ratioW < ratioH ? ratioW : ratioH;
        ratio *= 0.95;*/
    }

	_sizeTitleW = imageSize.width / _sizeBoard.x;
	_sizeTitleH = imageSize.height / _sizeBoard.y;
	float ratioW = (_screenSize.width / _sizeBoard.x) / _sizeTitleW;
	float ratioH = (_screenSize.height / _sizeBoard.y) / _sizeTitleH;

	ratio = ratioW < ratioH ? ratioW : ratioH;
	ratio *= 0.95;
    
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
			number++;
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
		if (x->getBoundingBox().containsPoint(pos))
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
			else if (abs(_listPos[around[j]].x - _listPos[_numberStart].x) > _sizeTitleW + RANGER_TITLES * 2)
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
	{
		this->scheduleOnce(schedule_selector(GamePlay::canPlay), 0.5f);
	}
}

void GamePlay::mergeImage(float)
{
	int r = 0;
	int c = 0;
	for (auto x : _listTitles)
	{
		Vec2 move;
		float duration = 0.5;

		move.x = -1 * RANGER_TITLES * c;
		move.y = -1 * RANGER_TITLES * r;

		++c;
		if (c == _sizeBoard.x)
		{
			c = 0;
			++r;
		}
		x->runAction(MoveBy::create(duration, move));
		x->setVisible(true);
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

void GamePlay::canPlay(float)
{
	_isEndGame = false;
	_isOrdering = false;
}

void GamePlay::win()
{
	CCLOG("Win");
	_isEndGame = true;
	this->scheduleOnce(schedule_selector(GamePlay::mergeImage), 0.5);
	if (_result == nullptr)
	{
		_result = Label::createWithTTF("WIN", LABEL_FONT, _screenSize.height * 0.1);
		_result->setPosition(_screenSize.width / 2.f + _origin.x, _menu->getPositionY() + _sizeTitleH / 2.f);
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
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->playEffect(SOUND_RESET);
    
    auto gameplay = GamePlay::createScene();
    Director::getInstance()->replaceScene(TransitionFlipY::create(0.5, gameplay));
}

bool GamePlay::onTouchBegin(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (!_isEndGame && !_isOrdering)
	{
		//get number around;
		std::vector<int> around{ _numberStart - (int)_sizeBoard.x , _numberStart - 1, _numberStart + 1, _numberStart + (int)_sizeBoard.x };
		for (int j = 0; j < around.size();)
		{
			if (around[j] < 0 || around[j] >= _listTitles.size())
				around.erase(around.begin() + j);
			//else if (abs(_listPos[around[j]].x - _listPos[_numberStart].x) > (_sizeTitleW + RANGER_TITLES + _sizeTitleW / 2.f))
			else if ((_listPos[around[j]].x != _listPos[_numberStart].x) && (_listPos[around[j]].y != _listPos[_numberStart].y))
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
				_titlePressed = x;

				return true;
			}
		}
	}

	return true;
}

void GamePlay::onTouchMove(Touch* touch, Event* event)
{
	
}

void GamePlay::onTouchRelease(Touch* touch, Event* event)
{
	if (!_isEndGame && !_isOrdering)
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
	_oldPosTitlePressed = Vec2(0, 0);
}

/*
	_directionMove : to determine direction of title which is pressed
	_spaceTouchAndTitle : space between touch and title which is pressd if have

	in order to make slide effect when use touch move.
*/
