#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "NewgroundsBrowserLayer.hpp"
#include "NewgroundsAPI.hpp"

using namespace geode::prelude;

class $modify(MenuLayer)
{
	void onNewgrounds(cocos2d::CCObject* sender)
	{
		CCDirector::get()->pushScene(NewgroundsBrowserLayer::scene(""));
	}
};