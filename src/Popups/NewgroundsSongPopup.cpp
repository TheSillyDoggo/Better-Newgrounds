#include "NewgroundsArtistPopup.hpp"

NewgroundsSongPopup* NewgroundsSongPopup::create(std::string id)
{
    auto pRet = new NewgroundsSongPopup();

    if (pRet && pRet->init(id))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool NewgroundsSongPopup::init(std::string id)
{
    if (!FLAlertLayer::init(100))
        return false;

    this->id = id;
    auto mdm = MusicDownloadManager::sharedState();

    bool downloaded = mdm->isSongDownloaded(utils::numFromString<int>(id).unwrapOr(0));
    auto object = MusicDownloadManager::sharedState()->getSongInfoObject(utils::numFromString<int>(id).unwrapOr(0));

    auto bg = CCScale9Sprite::create("geode.loader/GE_square01.png");
    bg->setContentSize(ccp(420, 265));
    bg->setPosition(CCDirector::get()->getWinSize() / 2);

    text = MDTextArea::create("", ccp(210, bg->getContentHeight() - 25));
    text->setPosition(CCDirector::get()->getWinSize() / 2 + ccp(bg->getContentWidth() / 2 - 17, 0));
    text->setAnchorPoint(ccp(1, 0.5f));

    descriptionCircle = LoadingCircle::create();
    descriptionCircle->setPosition(text->getPosition() + (text->getContentSize() * ccp(-0.5f, 0)));
    descriptionCircle->m_sprite->setPosition(ccp(0, 0));
    descriptionCircle->setContentSize(ccp(0, 0));
    descriptionCircle->m_sprite->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));

    infoBG = CCScale9Sprite::create("square02_small.png");
    infoBG->setOpacity(75);
    infoBG->setContentSize(ccp(185, 150));
    infoBG->setPosition(CCDirector::get()->getWinSize() / 2 + ccp(-bg->getContentWidth() / 2 + 101, bg->getContentHeight() / 2 + -47.5f - 40));

    if (!object)
    {
        this->schedule(schedule_selector(NewgroundsSongPopup::waitForSongInfoFinish));
        this->retain();
        MusicDownloadManager::sharedState()->getSongInfo(utils::numFromString<int>(id).unwrapOr(0), false);
    }
    else
    {
        addWidget();
    }

    auto songName = CCLabelBMFont::create(API::get()->getSong(id).name.c_str(), "bigFont.fnt");
    songName->limitLabelWidth(145, 0.7f, 0);
    songName->setAnchorPoint(ccp(0, 1));
    infoBG->addChildAtPosition(songName, Anchor::TopLeft, ccp(5, -5));

    auto authorName = CCLabelBMFont::create(("By " + API::get()->getSong(id).author).c_str(), "goldFont.fnt");
    authorName->limitLabelWidth(135, 0.5f, 0);

    auto authorBtn = CCMenuItemSpriteExtra::create(authorName, this, nullptr);
    infoBG->addChildAtPosition(authorBtn, Anchor::TopLeft, ccp(5, -5 - songName->getScaledContentHeight() - 2) + (authorBtn->getContentSize() * ccp(0.5f, -0.5f)));

    buttonsMenu = CCMenu::create();
    buttonsMenu->ignoreAnchorPointForPosition(false);
    buttonsMenu->setContentWidth(178);
    buttonsMenu->setLayout(RowLayout::create()->setCrossAxisOverflow(true));
    buttonsMenu->setVisible(!downloaded);

    buttonsMenuInstalled = CCMenu::create();
    buttonsMenuInstalled->ignoreAnchorPointForPosition(false);
    buttonsMenuInstalled->setContentWidth(178);
    buttonsMenuInstalled->setLayout(RowLayout::create()->setCrossAxisOverflow(true));
    buttonsMenuInstalled->setVisible(downloaded);

    buttonsMenuInstalling = CCMenu::create();
    buttonsMenuInstalling->ignoreAnchorPointForPosition(false);
    buttonsMenuInstalling->setContentWidth(178);
    buttonsMenuInstalling->setLayout(RowLayout::create()->setCrossAxisOverflow(true));
    buttonsMenuInstalling->setVisible(false);

    auto webSpr = geode::IconButtonSprite::create("geode.loader/GE_button_01.png", CCSprite::createWithSpriteFrameName("geode.loader/globe.png"), "Open in Web", "bigFont.fnt");
    webSpr->setScale(0.55f);

    auto webBtn = CCMenuItemSpriteExtra::create(webSpr, this, menu_selector(NewgroundsSongPopup::onOpenURL));
    webBtn->setID(fmt::format("https://www.newgrounds.com/audio/listen/{}", id));

    auto webBtnInstalled = CCMenuItemSpriteExtra::create(webSpr, this, menu_selector(NewgroundsSongPopup::onOpenURL));
    webBtnInstalled->setID(fmt::format("https://www.newgrounds.com/audio/listen/{}", id));

    auto webBtnInstalling = CCMenuItemSpriteExtra::create(webSpr, this, menu_selector(NewgroundsSongPopup::onOpenURL));
    webBtnInstalling->setID(fmt::format("https://www.newgrounds.com/audio/listen/{}", id));

    buttonsMenu->addChild(webBtn);
    buttonsMenuInstalled->addChild(webBtnInstalled);
    buttonsMenuInstalling->addChild(webBtnInstalling);

    auto downloadSpr = geode::IconButtonSprite::create("GJ_button_01.png", CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png"), "Download", "bigFont.fnt");
    downloadSpr->setScale(0.55f);

    auto downloadBtn = CCMenuItemSpriteExtra::create(downloadSpr, this, menu_selector(NewgroundsSongPopup::onDownload));
    buttonsMenu->addChild(downloadBtn);

    auto deleteSpr = geode::IconButtonSprite::create("GJ_button_06.png", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"), "Delete", "bigFont.fnt");
    deleteSpr->setScale(0.55f);

    auto deleteBtn = CCMenuItemSpriteExtra::create(deleteSpr, this, menu_selector(NewgroundsSongPopup::onDelete));
    buttonsMenuInstalled->addChild(deleteBtn);

    auto cancelSpr = geode::IconButtonSprite::create("GJ_button_04.png", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"), "Cancel", "bigFont.fnt");
    cancelSpr->setScale(0.55f);

    auto cancelBtn = CCMenuItemSpriteExtra::create(cancelSpr, this, menu_selector(NewgroundsSongPopup::onCancel));
    buttonsMenuInstalling->addChild(cancelBtn);

    buttonsMenu->updateLayout();
    buttonsMenuInstalled->updateLayout();
    buttonsMenuInstalling->updateLayout();
    infoBG->addChildAtPosition(buttonsMenu, Anchor::Bottom, ccp(0, 16));
    infoBG->addChildAtPosition(buttonsMenuInstalled, Anchor::Bottom, ccp(0, 16));
    infoBG->addChildAtPosition(buttonsMenuInstalling, Anchor::Bottom, ccp(0, 16));

    auto tagBG = CCScale9Sprite::create("square02_small.png");
    tagBG->setOpacity(75);
    tagBG->setContentSize(ccp(185, 83));
    tagBG->setPosition(CCDirector::get()->getWinSize() / 2 + ccp(-bg->getContentWidth() / 2 + 101, -bg->getContentHeight() / 2 + 67.5f - 13.5f));

    auto genre = API::get()->getSong(id).category;
    auto tag = NewgroundsGenreLabel::create(genre.empty() ? "No Tag Provided" : genre);
    tag->setAnchorPoint(ccp(0, 0.5f));
    tagBG->addChildAtPosition(tag, Anchor::BottomLeft, ccp(5, 12));

    auto starIcon = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    starIcon->setPosition(ccp(getContentHeight() + 9, 10));
    starIcon->setID("rating-icon");
    starIcon->setScale(0.75f);
    starIcon->setAnchorPoint(ccp(0, 0.5f));
    tagBG->addChildAtPosition(starIcon, Anchor::BottomLeft, ccp(5, 32));

    auto starLabel = CCLabelBMFont::create(API::get()->getSong(id).score.c_str(), "bigFont.fnt");
    starLabel->setScale(0.4f);
    starLabel->setID("rating-label");
    starLabel->setAnchorPoint(ccp(0, 0.5f));
    starLabel->setColor(ccc3(247, 247, 44));
    tagBG->addChildAtPosition(starLabel, Anchor::BottomLeft, ccp(24, 32));

    auto viewsIcon = CCSprite::create("views.png"_spr);
    viewsIcon->setPosition(ccp(getContentHeight() + 9, 10));
    viewsIcon->setID("views-icon");
    viewsIcon->setScale(0.16f);
    viewsIcon->setAnchorPoint(ccp(0, 0.5f));
    tagBG->addChildAtPosition(viewsIcon, Anchor::BottomLeft, ccp(5, 50));

    auto viewsLabel = CCLabelBMFont::create((API::get()->getSong(id).views + " Views").c_str(), "bigFont.fnt");
    viewsLabel->setScale(0.4f);
    viewsLabel->setID("views-label");
    viewsLabel->setAnchorPoint(ccp(0, 0.5f));
    tagBG->addChildAtPosition(viewsLabel, Anchor::BottomLeft, ccp(24, 50));

    auto downloadsIcon = CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png");
    downloadsIcon->setPosition(ccp(getContentHeight() + 9, 10));
    downloadsIcon->setID("downloads-icon");
    downloadsIcon->setScale(0.75f);
    downloadsIcon->setAnchorPoint(ccp(0, 0.5f));
    tagBG->addChildAtPosition(downloadsIcon, Anchor::BottomLeft, ccp(5, 68));

    downloadsLabel = CCLabelBMFont::create("Loading...", "bigFont.fnt");
    downloadsLabel->setScale(0.4f);
    downloadsLabel->setID("downloads-label");
    downloadsLabel->setAnchorPoint(ccp(0, 0.5f));
    tagBG->addChildAtPosition(downloadsLabel, Anchor::BottomLeft, ccp(24, 68));
    
    if (!API::get()->additionalData.contains(utils::numFromString<int>(id).unwrapOr(0)))
        API::get()->getAdditionalSongInfo(utils::numFromString<int>(id).unwrapOr(0), menu_selector(NewgroundsSongPopup::getSongInfoCompleted), this);
    else
        updateSongData();

    auto closeMenu = CCMenu::create();
    closeMenu->setPosition(bg->getPosition() + (bg->getContentSize() * ccp(-0.5f, 0.5f)));

    auto closeSpr = CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", 0.85f, CircleBaseColor::DarkPurple);
    auto closeBtn = CCMenuItemSpriteExtra::create(closeSpr, this, menu_selector(NewgroundsArtistPopup::onClose));
    //closeBtn->setPositionY(-8);

    closeMenu->addChild(closeBtn);

    m_mainLayer->addChild(bg);
    m_mainLayer->addChild(text);
    m_mainLayer->addChild(descriptionCircle);
    m_mainLayer->addChild(infoBG);
    m_mainLayer->addChild(tagBG);
    m_mainLayer->addChild(closeMenu, 1);

    this->schedule(schedule_selector(NewgroundsSongPopup::updateProgress));

    cocos::handleTouchPriority(this);

    return true;
}

void NewgroundsSongPopup::onClose(CCObject*)
{
    this->removeFromParent();
}

void NewgroundsSongPopup::onOpenURL(CCObject* sender)
{
    CCApplication::get()->openURL(as<CCNode*>(sender)->getID().c_str());
}

void NewgroundsSongPopup::onDownload(CCObject* sender)
{
    widger->onDownload(nullptr);

    buttonsMenuInstalling->setVisible(true);
    buttonsMenuInstalled->setVisible(false);
    buttonsMenu->setVisible(false);

    this->schedule(schedule_selector(NewgroundsSongPopup::waitForSongDownloadFinish));
    this->retain();
}

void NewgroundsSongPopup::onDelete(CCObject* sender)
{
    auto alert = geode::createQuickPopup(
        "Delete song",
        "Do you want to <cr>delete</c> this song?",
        "Cancel", "Delete",
        [this, sender](FLAlertLayer* tis, bool right) {
            widger->FLAlert_Clicked(tis, right);

            if (right)
            {
                bool downloaded = false;

                buttonsMenu->setVisible(!downloaded);
                buttonsMenuInstalled->setVisible(downloaded);
            }
        }
    );

    alert->setTag(2);
}

void NewgroundsSongPopup::onCancel(CCObject*)
{
    if (widger)
        widger->m_cancelDownloadBtn->activate();
    
    waitForSongDownloadFinish(0);

    buttonsMenu->setVisible(true);
    buttonsMenuInstalled->setVisible(false);
    buttonsMenuInstalling->setVisible(false);
}

void NewgroundsSongPopup::updateSongData()
{
    auto data = API::get()->additionalData[utils::numFromString<int>(id).unwrapOr(0)];

    text->setString(data.description.empty() ? "**No Description Provided**" : data.description.c_str());

    descriptionCircle->setVisible(false);

    downloadsLabel->setString(data.downloads.empty() ? "Unknown" : (data.downloads + " Downloads").c_str());
}

void NewgroundsSongPopup::getSongInfoCompleted(CCObject*)
{
    updateSongData();
}

void NewgroundsSongPopup::addWidget()
{
    if (widger)
        widger->removeFromParent();

    if (waveParent)
        waveParent->removeFromParent();
        
    waveParent = nullptr;
    clip = nullptr;

    auto object = MusicDownloadManager::sharedState()->getSongInfoObject(utils::numFromString<int>(id).unwrapOr(0));

    widger = CustomSongWidget::create(object, nullptr, false, true, true, false, false, false, 0);
    widger->setVisible(false);
    widger->setID(""_spr);
    this->addChild(widger);

    FMOD::Sound* sound;
    const FMOD_MODE mode = FMOD_DEFAULT | FMOD_CREATESAMPLE | FMOD_OPENONLY;
    FMODAudioEngine::sharedEngine()->m_system->createSound(MusicDownloadManager::sharedState()->pathForSong(utils::numFromString<int>(id).unwrapOr(0)).c_str(), mode, nullptr, &sound);

    if (sound)
    {
        unsigned int l;
        sound->getLength(&l, FMOD_TIMEUNIT_MS);
		songLength = as<float>(l) / 1000.f;
    }

    widger->m_sliderGroove->setScale(0.75f);
    infoBG->addChildAtPosition(widger->m_sliderGroove, Anchor::Bottom, ccp(0, 34));

    if (widger->m_playbackBtn)
    {
        auto playMenu = CCMenu::create();
        playMenu->setScale(0.8f);
        playMenu->setAnchorPoint(ccp(0, 0));
        playMenu->addChild(widger->m_playbackBtn);
        widger->m_playbackBtn->setPosition(ccp(0, 0));

        waveParent = CCScale9Sprite::create("pixel.png");
        waveParent->setColor(ccc3(0, 0, 0));
        waveParent->setContentSize(ccp(170, 50));

        if (false)//if (MusicDownloadManager::sharedState()->isSongDownloaded(utils::numFromString<int>(id).unwrapOr(0)) || (buttonsMenuInstalling && buttonsMenuInstalling->isVisible()))
        {
            auto x = MusicDownloadManager::sharedState()->pathForSong(utils::numFromString<int>(id).unwrapOr(0));

            auto form = AudioWaveformNode::create(x);
            form->colour = ccc4(255, 173, 49, 255);
            form->setAnchorPoint(ccp(0.5f, 0.5f));
            form->setContentSize(ccp(170, 50));
            waveParent->addChildAtPosition(form, Anchor::Center);

            clip = Clip::create();
            clip->setContentSize(form->getContentSize());
            clip->setContentWidth(0);

            clip->addChild(form);
            clip->addChild(form);
            clip->addChild(form);
            clip->addChild(form);
            clip->addChild(form);

            waveParent->addChildAtPosition(clip, Anchor::BottomLeft);
        }
        else
        {
            bool v = (MusicDownloadManager::sharedState()->isSongDownloaded(utils::numFromString<int>(id).unwrapOr(0)) || (buttonsMenuInstalling && buttonsMenuInstalling->isVisible()));

            auto label = CCLabelBMFont::create(v ? "Audio Spectrum is not\nyet supported" : "Download the song to\nplay the song", "bigFont.fnt");
            label->setScale(0.4f);
            label->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
            waveParent->addChildAtPosition(label, Anchor::Center);
        }

        infoBG->addChildAtPosition(waveParent, Anchor::Bottom, ccp(0, 80));
        infoBG->addChildAtPosition(playMenu, Anchor::Bottom, ccp(0, 47.5f));
    }

    cocos::handleTouchPriority(this);
}

void NewgroundsSongPopup::waitForSongInfoFinish(float)
{
    auto obj = MusicDownloadManager::sharedState()->getDLObject(fmt::format("i_{}", utils::numFromString<int>(id).unwrapOr(0)).c_str());

    if (!obj)
    {
        addWidget();

        this->unschedule(schedule_selector(NewgroundsSongPopup::waitForSongInfoFinish));

        bool downloaded = MusicDownloadManager::sharedState()->isSongDownloaded(utils::numFromString<int>(id).unwrapOr(0));

        buttonsMenu->setVisible(!downloaded);
        buttonsMenuInstalled->setVisible(downloaded);

        return this->release();
    }

    bool downloaded = false;

    buttonsMenu->setVisible(false);
    buttonsMenuInstalled->setVisible(false);
}

void NewgroundsSongPopup::waitForSongDownloadFinish(float)
{
    if (widger && widger->m_sliderGroove)
        widger->m_sliderGroove->setPosition(ccp(infoBG->getContentWidth() / 2, 34));

    if (widger && widger->m_sliderGroove && !widger->m_sliderGroove->isVisible())
    {
        this->unschedule(schedule_selector(NewgroundsSongPopup::waitForSongDownloadFinish));

        buttonsMenu->setVisible(false);
        buttonsMenuInstalled->setVisible(true);
        buttonsMenuInstalling->setVisible(false);

        return this->release();
    }

    buttonsMenu->setVisible(false);
    buttonsMenuInstalled->setVisible(false);
    buttonsMenuInstalling->setVisible(true);
}

void NewgroundsSongPopup::updateProgress(float)
{
    if (widger && clip)
    {
        if (FMODAudioEngine::sharedEngine()->isMusicPlaying(0) && songLength != 0 && currentSong == id)
        {
            clip->setContentWidth(170.0f * ((FMODAudioEngine::sharedEngine()->getMusicTimeMS(0) / 1000.0f) / songLength));
        }
        else
        {
            clip->setContentWidth(0);
        }
    }
}

void NewgroundsSongPopup::onAuthor(CCObject*)
{
    NewgroundsArtistPopup::create(API::get()->getSong(id).author)->show();
}

class $modify (CustomSongWidget)
{
    void onPlayback(CCObject* sender)
    {
        CustomSongWidget::onPlayback(sender);

        if (getID() == ""_spr)
        {
            NewgroundsSongPopup::currentSong = (cocos::isSpriteFrameName(m_playbackBtn->getNormalImage(), "GJ_playMusicBtn_001.png")) ? "" : fmt::format("{}", m_songInfoObject->m_songID);
        }
    }
};