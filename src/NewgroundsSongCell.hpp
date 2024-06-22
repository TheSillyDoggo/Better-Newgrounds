#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "NewgroundsAPI.hpp"
#include "NewgroundsGenreLabel.hpp"
#include "Popups/NewgroundsArtistPopup.hpp"
#include "Popups/NewgroundsSongPopup.hpp"

using namespace geode::prelude;

class NewgroundsSongCell : public CCLayerColor
{
    public:
        EventListener<web::WebTask> listener;
        LoadingCircle* iconLoading;
        AudioSong song;
        bool isPopup;

        static NewgroundsSongCell* createWithSong(AudioSong song, bool isPopup);

        bool initWithSong(AudioSong song, bool isPopup);
        void addIcon();

        void onView(CCObject* sender);
        void onArtist(CCObject* sender);
        void onSpin(CCObject* sender);

        virtual void draw();
};