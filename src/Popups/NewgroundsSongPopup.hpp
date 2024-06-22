#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/utils/web.hpp>
#include "../NewgroundsAPI.hpp"
#include "../NewgroundsSongCell.hpp"
#include "../NewgroundsBrowserLayer.hpp"
#include "../AudioWaveformNode.hpp"
#include "../Clip.hpp"

using namespace geode::prelude;

class NewgroundsSongPopup : public FLAlertLayer, public CustomSongDelegate
{
    public:
        std::string id;
        MDTextArea* text;
        CCScale9Sprite* infoBG;
        LoadingCircle* descriptionCircle;
        CCLabelBMFont* downloadsLabel;
        CCMenu* buttonsMenu;
        CCMenu* buttonsMenuInstalled;
        CCMenu* buttonsMenuInstalling;
        CustomSongWidget* widger;
        CCScale9Sprite* waveParent;
        Clip* clip;
        float songLength;
        static inline std::string currentSong = "";
        EventListener<web::WebTask> listener;

        static NewgroundsSongPopup* create(std::string id);

        bool init(std::string id);

        void onClose(CCObject*);
        void onOpenURL(CCObject* sender);
        void onDownload(CCObject* sender);
        void onDelete(CCObject* sender);
        void onAuthor(CCObject*);
        void onCancel(CCObject*);

        void addWidget();
        void updateSongData();
        void getSongInfoCompleted(CCObject*);

        void waitForSongInfoFinish(float);
        void waitForSongDownloadFinish(float);

        void updateProgress(float);
};