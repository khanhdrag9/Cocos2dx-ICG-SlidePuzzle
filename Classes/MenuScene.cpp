//
//  Splash.cpp
//  SlidePuzzle
//
//  Created by Khanh on 1/21/19.
//

#include "MenuScene.h"
#include "Defines.h"
#include "GamePlay.h"

USING_NS_CC;

cocos2d::Scene* MenuScene::createScene()
{
    auto scene = cocos2d::Scene::create();
    auto layer = MenuScene::create();
    scene->addChild(layer);
    return scene;
}

bool MenuScene::init()
{
    if(!Layer::init())
        return false;
    
    auto sz = Director::getInstance()->getVisibleSize();
    auto ori = Director::getInstance()->getVisibleOrigin();
    
    Label* title = Label::createWithTTF("Slide Puzzle by Khanh", LABEL_FONT, sz.height * 0.1);
    title->setColor(Color3B::YELLOW);
    title->setPosition(sz.width / 2.f + ori.x, sz.height * 0.8 + ori.y);
    
    Label* touch = Label::createWithTTF("Touch on screen to play!", LABEL_FONT, sz.height * 0.05);
    touch->setPosition(sz.width / 2.f + ori.x, sz.height / 2.3 + ori.y);
    
    this->addChild(title);
    this->addChild(touch);
    
    auto touchlistener = EventListenerTouchOneByOne::create();
    touchlistener->onTouchBegan = [](Touch*, Event*){return true;};
    touchlistener->onTouchEnded = [](Touch*, Event*){
        auto gameplay = GamePlay::createScene();
        Director::getInstance()->replaceScene(TransitionFlipX::create(0.5, gameplay));
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchlistener, this);
    
    return true;
}
