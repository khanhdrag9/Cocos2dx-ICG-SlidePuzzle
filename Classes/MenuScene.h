#pragma once
#include "Cocos2d.h"

class MenuScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    bool init() override;
    
    CREATE_FUNC(MenuScene);
};
