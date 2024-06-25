#pragma once

#include <Geode/Geode.hpp>
#include "NewgroundsAPI.hpp"
#include "NewgroundsSongCell.hpp"

using namespace geode::prelude;

class NewgroundsBrowserLayer : public CCLayer
{
    public:
        GJListLayer* list;
        ScrollLayer* scroll;
        GJListLayer* searchList;
        ScrollLayer* searchScroll;
        LoadingCircle* circle;
        int selectedTab = 0;
        CCMenuItemSpriteExtra* leftArrow;
        CCMenuItemSpriteExtra* rightArrow;
        CCMenu* tabsMenu;
        CCMenu* tabsMenuSelected;
        std::vector<TabButton*> buttons;
        CCLabelBMFont* pageLabel;
        std::string user;
        int page = 0;

        static NewgroundsBrowserLayer* create(std::string user);
        static CCScene* scene(std::string user);

        virtual bool init(std::string user);
        virtual void keyBackClicked();

        void onBack(CCObject*);
        void onNextPage(CCObject*);
        void onPrevPage(CCObject*);
        void onSwitchTab(CCObject* sender);

        void setTitle(const char* title);
        void updateList();
        void updateArrows();
        void updatePageLabel();

        void pageFetchCompleted(CCObject*);
        void pageFetchCompletedFeatured(CCObject*);
        void songFetchCompleted(CCObject*);
};