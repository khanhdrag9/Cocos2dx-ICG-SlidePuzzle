#include "Title.h"



Title::Title()
{
	_number = 0;
}


Title::~Title()
{
}


Title* Title::createTitle(int i_number, cocos2d::Texture2D* texture, const cocos2d::Rect& rect)
{
	Title* title = new Title();
	if (title && title->initWithTexture(texture, rect))
	{
		title->_number = i_number;
		title->autorelease();
		return title;
	}

	CC_SAFE_DELETE(title);
	return nullptr;
}
