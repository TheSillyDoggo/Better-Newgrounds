#include "NewgroundsArtistPopup.hpp"

NewgroundsArtistPopup* NewgroundsArtistPopup::create(std::string id)
{
    auto pRet = new NewgroundsArtistPopup();

    if (pRet && pRet->init(id))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool NewgroundsArtistPopup::init(std::string id)
{
    if (!FLAlertLayer::init(100))
        return false;

    this->id = id;

    circleBanner = LoadingCircle::create();
    circleBanner->m_sprite->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));
    circleBanner->setVisible(false);
    circleBanner->m_sprite->setPosition(ccp(0, 0));

    circle = LoadingCircle::create();
    circle->m_sprite->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));
    circle->setVisible(false);

    content = CCNode::create();

    auto bg = CCScale9Sprite::create("geode.loader/GE_square01.png");
    bg->setContentSize(ccp(420, 285));
    bg->setPosition(CCDirector::get()->getWinSize() / 2);

    //auto bgDark = CCScale9Sprite::create("square02_small.png");
    //bgDark->setOpacity(100);
    //bgDark->setContentSize(ccp(100, 100));

    auto mask = CCScale9Sprite::create("banner-mask.png"_spr);
    mask->setContentWidth(420 - 2);
    mask->setContentHeight(85 - 2);

    clip = CCClippingNode::create(mask);
    clip->setAlphaThreshold(0.03f);
    clip->setPosition(CCDirector::get()->getWinSize() / 2 + ccp(0, bg->getContentHeight() / 2) + ccp(0, -1));
    clip->setAnchorPoint(ccp(0.5f, 1));
    clip->setContentHeight(mask->getContentHeight() / 2);
    //clip->addChild(bgDark, 30);

    auto outline = CCScale9Sprite::create("banner-outline.png"_spr);
    outline->setPosition(clip->getPosition() + ccp(0, 1));
    outline->setAnchorPoint(ccp(0.5f, 1));
    outline->setContentSize(ccp(420, 85));

    auto defaultBG = CCSprite::create("user-banner-default.png"_spr);
    defaultBG->setScale(420 / defaultBG->getContentWidth());
    clip->addChild(defaultBG);

    profileParent = CCNode::create();
    profileParent->setPosition(clip->getPosition() + ccp((-bg->getContentWidth() / 2) + 50, -outline->getContentHeight() + 1));
    profileParent->setContentHeight(62);

    auto profileMask2 = CCSprite::create("circle-outline-filled.png"_spr);
    profileMask2->setContentSize(ccp(profileParent->getContentHeight(), profileParent->getContentHeight()));
    profileClip = CCClippingNode::create(profileMask2);
    profileClip->setAlphaThreshold(0.3f);

    auto profileMask = CCSprite::create("circle-outline.png"_spr);
    profileMask->setContentSize(ccp(profileParent->getContentHeight(), profileParent->getContentHeight()));
    profileParent->addChild(profileMask, 3);
    profileParent->addChild(profileClip, 2);

    auto name = CCLabelBMFont::create(id.c_str(), "bigFont.fnt");
    name->setScale(0.65f);
    name->setPosition(profileParent->getPosition() + ccp(35.5f, 17));
    name->setAnchorPoint(ccp(0, 0.5f));

    borders = geode::ListBorders::create();
    borders->setContentSize(ccp(270, 170));
    borders->setSpriteFrames("geode.loader/geode-list-top.png", "geode.loader/geode-list-side.png", 2);
    borders->setPosition(CCDirector::get()->getWinSize() / 2 + ccp(45.5f, -40));

    circleSongs = LoadingCircle::create();
    circleSongs->m_sprite->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));
    circleSongs->m_sprite->setPosition(ccp(0, 0));
    circleSongs->setVisible(false);
    circleSongs->setPosition(borders->getContentSize() / 2);
    borders->addChild(circleSongs, 420);

    auto divider = CCLayerColor::create(ccc4(0, 0, 0, 100), 1, 172);
    divider->ignoreAnchorPointForPosition(false);
    divider->setAnchorPoint(ccp(0.5f, 0.5f));
    divider->setPosition(ccp(CCDirector::get()->getWinSize().width / 2 + -114.5f, borders->getPositionY()));

    buttonsMenu = CCMenu::create();
    buttonsMenu->setPosition(borders->getPosition());
    buttonsMenu->setVisible(false);

    auto leftSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    leftSpr->setScale(0.65f);
    buttonLeft = CCMenuItemSpriteExtra::create(leftSpr, this, menu_selector(NewgroundsArtistPopup::onPage));
    buttonLeft->setTag(-1);
    buttonLeft->setPositionX(-borders->getContentWidth() / 2 - 13);
    buttonLeft->setVisible(false);
    buttonsMenu->addChild(buttonLeft);

    auto rightSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    rightSpr->setFlipX(true);
    rightSpr->setScale(0.65f);
    buttonRight = CCMenuItemSpriteExtra::create(rightSpr, this, menu_selector(NewgroundsArtistPopup::onPage));
    buttonRight->setTag(1);
    buttonRight->setPositionX(borders->getContentWidth() / 2 + 13);
    buttonsMenu->addChild(buttonRight);

    auto closeMenu = CCMenu::create();
    closeMenu->setPosition(bg->getPosition() + (bg->getContentSize() * ccp(-0.5f, 0.5f)));

    auto closeSpr = CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", 0.85f, CircleBaseColor::DarkPurple);
    auto closeBtn = CCMenuItemSpriteExtra::create(closeSpr, this, menu_selector(NewgroundsArtistPopup::onClose));
    closeBtn->setPositionY(-8);

    closeMenu->addChild(closeBtn);

    clip->addChild(circleBanner, 420);
    m_mainLayer->addChild(bg);
    m_mainLayer->addChild(circle, 420);
    content->addChild(clip);
    content->addChild(outline);
    content->addChild(profileParent);
    content->addChild(name, 1);
    content->addChild(divider);
    content->addChild(borders, 2);
    content->addChild(buttonsMenu, 2);
    m_mainLayer->addChild(content, 1);
    m_mainLayer->addChild(closeMenu, 1);

    if (!API::get()->artists.contains(id))
    {
        API::get()->getArtist(id, menu_selector(NewgroundsArtistPopup::pageFetchCompleted), this);
        circleBanner->setVisible(true);
        circle->setVisible(true);
        content->setVisible(false);
    }
    else
    {
        if (!API::get()->artists[id].songs.contains(page))
        {
            API::get()->getArtistSongs(id, page, menu_selector(NewgroundsArtistPopup::songFetchCompleted), this);
            circleSongs->setVisible(true);
            buttonsMenu->setVisible(false);
        }
        else
        {
            buttonsMenu->setVisible(true);
            
            buttonLeft->setVisible(page != 0);
            buttonRight->setVisible(page != API::get()->artists[id].lastPage);
        }
    }

    updateList();

    addBanner();
    addProfile();
    addBio();

    cocos::handleTouchPriority(this);

    return true;
}

void NewgroundsArtistPopup::pageFetchCompleted(CCObject*)
{
    content->setVisible(true);
    circleSongs->setVisible(true);
    circle->setVisible(false);

    getBanner();
    getProfile();
    addBio();

    API::get()->getArtistSongs(id, 0, menu_selector(NewgroundsArtistPopup::songFetchCompleted), this);
}

void NewgroundsArtistPopup::songFetchCompleted(CCObject*)
{
    buttonLeft->setVisible(page != 0);
    buttonRight->setVisible(page != API::get()->artists[id].lastPage);
    buttonsMenu->setVisible(true);

    circleSongs->setVisible(false);
    updateList();
}

void NewgroundsArtistPopup::updateList()
{
    if (scroll)
        scroll->removeFromParent();

    scroll = ScrollLayer::create(ccp(270, 170 + 1));
    scroll->setPosition(borders->getPosition() - (borders->getContentSize() * 0.5f));

    content->addChild(scroll);

    cocos::handleTouchPriority(this);

    if (!API::get()->artists.contains(id))
        return;

    if (!API::get()->artists[id].songs.contains(page))
        return;

    if (circleSongs->isVisible())
        return;

    float height = 0;
    int i = 0;

    for (auto song : API::get()->artists[id].songs[page])
    {
        auto cell = NewgroundsSongCell::createWithSong(song, true);
        cell->setPositionY(((API::get()->artists[id].songs[page].size() - 1) * cell->getContentHeight()) - height);
        cell->setOpacity(i % 2 == 0 ? 50 : 0);
        cell->setTag(i);

        height += cell->getContentHeight();
        scroll->m_contentLayer->addChild(cell);
        i++;
    }

    scroll->m_contentLayer->setContentHeight(height > scroll->getContentHeight() ? height : scroll->getContentHeight());
    scroll->moveToTop();

    cocos::handleTouchPriority(this);
}

void NewgroundsArtistPopup::addBio()
{
    return;

    if (API::get()->artists.contains(id))
    {
        auto text = TextArea::create("<cy>\"</c>" + API::get()->artists[id].bio + "<cy>\"</c>", "chatFont.fnt", 0.55f, 80, ccp(0.5f, 1), 10, false);
        text->setPosition(profileParent->getPosition() + ccp(34, -50));

        m_mainLayer->addChild(text);
    }
}

void NewgroundsArtistPopup::addBanner()
{
    auto tex = CCTextureCache::get()->textureForKey(fmt::format("{}/banner-{}", Mod::get()->getID(), id).c_str());

    if (tex)
    {
        auto spr = CCSprite::createWithTexture(tex);

        if (spr)
        {
            spr->runAction(CCFadeIn::create(0.25f));
            spr->setScale(420 / spr->getContentWidth());

            circleBanner->setVisible(false);
            clip->addChild(spr);
        }
    }
    else
    {
        getBanner();
    }
}

void NewgroundsArtistPopup::getBanner()
{
    if (API::get()->artists.contains(id))
    {
        if (API::get()->artists[id].banner.empty())
            return circleBanner->setVisible(false);

        this->retain();

        bannerListener.bind([this] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (res->ok()) {
                    auto imgData = res->data();
                    auto image = Ref(new CCImage());
                    image->initWithImageData(const_cast<uint8_t*>(res->data().data()),res->data().size());
                    std::string theKey = fmt::format("{}/banner-{}", Mod::get()->getID(), id);
                    auto texture = CCTextureCache::get()->addUIImage(image,theKey.c_str());
                    image->release();

                    addBanner();

                    this->release();
                }
            } else if (e->isCancelled()) {
                log::info("The request was cancelled... So sad :(");
                this->release();
            }
        });

        auto req = web::WebRequest();
        
        bannerListener.setFilter(req.get(API::get()->artists[id].banner));
    }
}

void NewgroundsArtistPopup::addProfile()
{
    auto tex = CCTextureCache::get()->textureForKey(fmt::format("{}/profile-{}", Mod::get()->getID(), id).c_str());

    if (tex)
    {
        auto spr = CCSprite::createWithTexture(tex);

        if (spr)
        {
            profileClip->addChild(spr);
        }
    }
    else
    {
        getProfile();
    }
}

void NewgroundsArtistPopup::getProfile()
{
    if (API::get()->artists.contains(id))
    {
        this->retain();

        profileListener.bind([this] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (res->ok()) {
                    auto imgData = res->data();
                    auto image = Ref(new CCImage());
                    image->initWithImageData(const_cast<uint8_t*>(res->data().data()),res->data().size());
                    std::string theKey = fmt::format("{}/profile-{}", Mod::get()->getID(), id);
                    auto texture = CCTextureCache::get()->addUIImage(image,theKey.c_str());
                    image->release();

                    addProfile();

                    this->release();
                }
            } else if (e->isCancelled()) {
                log::info("The request was cancelled... So sad :(");
                this->release();
            }
        });

        auto req = web::WebRequest();
        
        profileListener.setFilter(req.get(API::get()->artists[id].pfp));
    }
}

void NewgroundsArtistPopup::onSongs(CCObject*)
{
    CCDirector::get()->pushScene(NewgroundsBrowserLayer::scene(id));
}

void NewgroundsArtistPopup::onPage(CCObject* sender)
{
    page += sender->getTag();

    buttonLeft->setVisible(page != 0);
    buttonRight->setVisible(page != API::get()->artists[id].lastPage);
    
    if (!API::get()->artists[id].songs.contains(page))
    {
        API::get()->getArtistSongs(id, page, menu_selector(NewgroundsArtistPopup::songFetchCompleted), this);
        circleSongs->setVisible(true);
        buttonsMenu->setVisible(false);
    }

    updateList();
}

void NewgroundsArtistPopup::onClose(CCObject*)
{
    this->removeFromParent();
}