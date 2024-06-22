#include "NewgroundsSongCell.hpp"

NewgroundsSongCell* NewgroundsSongCell::createWithSong(AudioSong song, bool isPopup)
{
    auto pRet = new NewgroundsSongCell();

    if (pRet && pRet->initWithSong(song, isPopup))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool NewgroundsSongCell::initWithSong(AudioSong song, bool isPopup)
{
    if (!CCLayerColor::init())
        return false;

    this->setContentSize(isPopup ? ccp(270, 50) : ccp(356, 70));
    this->song = song;
    this->isPopup = isPopup;

    float mod = isPopup ? 0.8f : 1;

    iconLoading = LoadingCircle::create();
    iconLoading->setPosition(ccp(getContentHeight() / 2, getContentHeight() / 2));
    iconLoading->setContentSize(ccp(0, 0));
    iconLoading->setScale(0.63f);
    iconLoading->m_sprite->setPosition(ccp(0, 0));
    iconLoading->m_sprite->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));
    iconLoading->setID("icon-loading-circle");

    auto id = song.id;

    std::stringstream ss;
    ss << "By ";
    ss << song.author;

    if (auto tex = CCTextureCache::get()->textureForKey(fmt::format("{}/logo-{}", Mod::get()->getID(), id).c_str()) || song.icon.starts_with("https://img.ngfiles.com/defaults/icon-audio-smaller.png"))
    {
        addIcon();
    }
    else
    {
        this->retain();

        listener.bind([this, id] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (res->ok()) {
                    auto imgData = res->data();
                    auto image = Ref(new CCImage());
                    image->initWithImageData(const_cast<uint8_t*>(res->data().data()),res->data().size());
                    std::string theKey = fmt::format("{}/logo-{}", Mod::get()->getID(), id);
                    auto texture = CCTextureCache::get()->addUIImage(image,theKey.c_str());
                    image->release();

                    addIcon();

                    this->release();
                }
            } else if (e->isCancelled()) {
                log::info("The request was cancelled... So sad :(");
                this->release();
            }
        });

        auto req = web::WebRequest();
        
        listener.setFilter(req.get(song.icon));
    }

    auto title = CCLabelBMFont::create(song.getName(), "bigFont.fnt");
    title->limitLabelWidth(isPopup ? 170 : 220, isPopup ? 0.4f : 0.5f, 0);
    title->setAnchorPoint(ccp(0, 1));
    title->setPosition(ccp(getContentHeight() + 2, getContentHeight() - 5));
    title->setID("name-label");

    auto authorMenu = CCMenu::create();
    authorMenu->setPosition(ccp(0, 0));
    authorMenu->setID("author-menu");

    auto author = CCLabelBMFont::create(ss.str().c_str(), "goldFont.fnt");
    author->limitLabelWidth(250, 0.5f, 0);

    auto authorBtn = CCMenuItemSpriteExtra::create(author, this, menu_selector(NewgroundsSongCell::onArtist));
    authorMenu->addChild(authorBtn);

    authorBtn->m_scaleMultiplier = 1.2f;
    authorBtn->setPosition(ccp(getContentHeight() + 2, getContentHeight() - title->getScaledContentHeight() - 7) + (author->getScaledContentSize() * ccp(0.5f, -0.5f)));

    auto starIcon = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    starIcon->setScale(0.6f * mod);
    starIcon->setPosition(ccp(getContentHeight() + 9, 10));
    starIcon->setID("rating-icon");

    auto starLabel = CCLabelBMFont::create(song.score.c_str(), "bigFont.fnt");
    starLabel->setColor(ccc3(247, 247, 44));
    starLabel->setScale(0.4f * mod);
    starLabel->setAnchorPoint(ccp(0, 0.5f));
    starLabel->setPosition(starIcon->getPosition() + ccp(10 * mod, 0.3f));
    starLabel->setID("rating-label");

    auto viewIcon = CCSprite::create("views.png"_spr);
    viewIcon->setScale(0.6f * mod);
    viewIcon->setPosition(starIcon->getPosition() + ccp(0, 15 * mod));
    viewIcon->setScale(starIcon->getScaledContentWidth() / viewIcon->getContentWidth());
    viewIcon->setID("view-icon");

    auto viewLabel = CCLabelBMFont::create(song.views.c_str(), "bigFont.fnt");
    viewLabel->setScale(0.4f * mod);
    viewLabel->setAnchorPoint(ccp(0, 0.5f));
    viewLabel->setPosition(viewIcon->getPosition() + ccp(10 * mod, 0.3f));
    viewLabel->setID("view-label");

    auto menu = CCMenu::create();
    menu->setID("view-menu");
    menu->setAnchorPoint(ccp(0, 0));
    menu->setPosition(ccp(getContentWidth(), getContentHeight() / 2) + ccp(isPopup ? -25 : -31, 0));
    menu->setScale(isPopup ? 0.75f : 1);

    auto viewSpr = CCSprite::create("view.png"_spr);
    viewSpr->setScale(39.5f / viewSpr->getContentHeight());

    auto viewBtn = CCMenuItemSpriteExtra::create(viewSpr, this, menu_selector(NewgroundsSongCell::onView));
    menu->addChild(viewBtn);

    if (!song.category.empty())
    {
        auto genre = NewgroundsGenreLabel::create(song.category);
        genre->setScale(mod);
        genre->setPosition(starLabel->getPosition() + ccp(starLabel->getScaledContentWidth(), 0) + ccp(4, 0));
        genre->setAnchorPoint(ccp(0, 0.5f));
        genre->setID("genre-button");

        this->addChild(genre);
    }

    this->addChild(iconLoading);
    this->addChild(starIcon);
    this->addChild(starLabel);
    this->addChild(viewIcon);
    this->addChild(viewLabel);
    this->addChild(title);
    if (!isPopup)
        this->addChild(authorMenu);
    this->addChild(menu);

    return true;
}

void NewgroundsSongCell::onView(CCObject* sender)
{
    NewgroundsSongPopup::create(this->song.id)->show();
}

void NewgroundsSongCell::onArtist(CCObject* sender)
{
    NewgroundsArtistPopup::create(this->song.author)->show();
}

void NewgroundsSongCell::onSpin(CCObject* sender)
{
    if (sender->getTag() == 69)
        getChildOfType<CCSprite>(as<CCNode*>(sender), 0)->runAction(CCEaseInOut::create(CCRotateBy::create(1, 360), 2));
    else
        getChildOfType<CCSprite>(getChildOfType<CCSprite>(as<CCNode*>(sender), 0), 0)->runAction(CCEaseInOut::create(CCRotateBy::create(1, 360), 2));
}

void NewgroundsSongCell::addIcon()
{
    iconLoading->setVisible(false);
    bool isBlank = song.icon.starts_with("https://img.ngfiles.com/defaults/icon-audio-smaller.png");
    auto tex = CCTextureCache::get()->textureForKey(fmt::format("{}/logo-{}", Mod::get()->getID(), song.id).c_str());

    if (tex || isBlank)
    {
        CCSprite* spr = nullptr;
        if (!isBlank)
            spr = CCSprite::createWithTexture(tex);

        if (spr || isBlank)
        {
            auto disc = CCSprite::create(isBlank ? "disc.png"_spr : "blank-disc.png"_spr);
            disc->setPosition(ccp(getContentHeight() / 2, getContentHeight() / 2));
            disc->setScale(0.43f);
            
            if (!isBlank)
            {
                disc->addChild(spr);
                spr->setScale((disc->getContentHeight() - 15) / spr->getContentHeight());
                spr->setPosition(disc->getContentSize() / 2);
            }

            auto btn = CCMenuItemSpriteExtra::create(disc, this, menu_selector(NewgroundsSongCell::onSpin));
            btn->setID("disc");

            if (isBlank)
                btn->setEnabled(false); //btn->setTag(69);

            auto menu = CCMenu::create();

            menu->setPosition(ccp(getContentHeight() / 2, getContentHeight() / 2));
            menu->setID("disc-menu");
            menu->addChild(btn);
            menu->setAnchorPoint(ccp(0, 0));
            menu->setScale(isPopup ? 0.8f : 1.15f);
            this->addChild(menu);
        }
    }

    handleTouchPriority(this);
}

void NewgroundsSongCell::draw()
{
    CCLayerColor::draw();

    if (this->getTag() == 0)
        ccDrawSolidRect(ccp(3, getContentHeight() + -1), ccp(getContentWidth() - 3, getContentHeight()), ccc4f(0, 0, 0, 0.5f));
    
    ccDrawSolidRect(ccp(3, -1), ccp(getContentWidth() - 3, 0), ccc4f(0, 0, 0, 0.5f));

    ccDrawSolidRect(ccp(getContentHeight() - 3, 3), ccp(getContentHeight() - 3 + 1.75f, getContentHeight() - 3), ccc4f(0, 0, 0, 0.5f));
}