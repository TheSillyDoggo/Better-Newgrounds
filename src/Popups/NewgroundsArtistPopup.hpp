#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include "../NewgroundsAPI.hpp"
#include "../NewgroundsSongCell.hpp"
#include "../NewgroundsBrowserLayer.hpp"

using namespace geode::prelude;

class NewgroundsArtistPopup : public FLAlertLayer
{
    public:
        std::string id;
        CCNode* content;
        LoadingCircle* circle;
        LoadingCircle* circleBanner;
        LoadingCircle* circleSongs;
        CCClippingNode* clip;
        CCClippingNode* profileClip;
        CCNode* profileParent;
        ScrollLayer* scroll = nullptr;
        ListBorders* borders;
        CCMenu* buttonsMenu;
        CCMenuItemSpriteExtra* buttonLeft;
        CCMenuItemSpriteExtra* buttonRight;
        int page = 0;

        EventListener<web::WebTask> profileListener;
        EventListener<web::WebTask> bannerListener;
        EventListener<web::WebTask> listener;

        static NewgroundsArtistPopup* create(std::string id);

        bool init(std::string id);

        void addBanner();
        void getBanner();
        void addProfile();
        void getProfile();
        void addBio();
        void updateList();

        void onSongs(CCObject*);
        void onPage(CCObject* sender);
        void onClose(CCObject*);

        void pageFetchCompleted(CCObject*);
        void songFetchCompleted(CCObject*);
};