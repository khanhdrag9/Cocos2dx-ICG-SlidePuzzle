#pragma once
#include "cocos2d.h"

class Title : public cocos2d::Sprite
{
public:
	Title();
	virtual ~Title();
	int _number;
	static Title* createTitle(int i_number ,cocos2d::Texture2D* texture, const cocos2d::Rect& rect);
};

