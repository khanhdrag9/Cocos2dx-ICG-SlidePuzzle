#include "GamePlay.h"

USING_NS_CC;

GamePlay::GamePlay()
{

}

GamePlay::~GamePlay()
{

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


	return true;
}

void GamePlay::createTitles()
{
	// TODO: Add your implementation code here.
}


// use when init game and reset game to setup board play
void GamePlay::setupBoard()
{
	// TODO: Add your implementation code here.
}


void GamePlay::createActions()
{
	// TODO: Add your implementation code here.
}


void GamePlay::checkWin()
{
	// TODO: Add your implementation code here.
}


void GamePlay::win()
{
	// TODO: Add your implementation code here.
}


void GamePlay::lose()
{
	// TODO: Add your implementation code here.
}


void GamePlay::reset()
{
	// TODO: Add your implementation code here.
}


void GamePlay::onTouchMove(Touch* touch, Event* event)
{
	// TODO: Add your implementation code here.
}


void GamePlay::onTouchRelease(Touch* touch, Event* event)
{
	// TODO: Add your implementation code here.
}
