#include "NewgroundsBrowserLayer.hpp"
#include "Clip.hpp"

NewgroundsBrowserLayer* NewgroundsBrowserLayer::create(std::string user)
{
    auto pRet = new NewgroundsBrowserLayer();

    if (pRet && pRet->init(user))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

CCScene* NewgroundsBrowserLayer::scene(std::string user)
{
    auto scene = CCScene::create();
    scene->addChild(NewgroundsBrowserLayer::create(user));

    return CCTransitionFade::create(0.5f, scene);
}

bool NewgroundsBrowserLayer::init(std::string user)
{
    if (!CCLayer::init())
        return false;

    this->setKeypadEnabled(true);

    auto bg = CCSprite::create("background.png"_spr);
    bg->setScale(CCDirector::get()->getWinSize().height / bg->getContentHeight());
    bg->setPosition(CCDirector::get()->getWinSize() / 2);

    auto bgBlur = CCSprite::create("background-blur.png"_spr);
    bgBlur->setScale(CCDirector::get()->getWinSize().height / bg->getContentHeight());

    auto backMenu = CCMenu::create();
    backMenu->setPosition(ccp(24, CCDirector::get()->getWinSize().height - 23));

    auto backButton = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"), this, menu_selector(NewgroundsBrowserLayer::onBack));
    backMenu->addChild(backButton);

    auto arrowsMenu = CCMenu::create();
    arrowsMenu->setPosition(CCDirector::get()->getWinSize() / 2);

    leftArrow = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"), this, menu_selector(NewgroundsBrowserLayer::onPrevPage));
    leftArrow->setPositionX(-CCDirector::get()->getWinSize().width / 2 + 24);

    auto rightSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    rightSpr->setFlipX(true);
    rightArrow = CCMenuItemSpriteExtra::create(rightSpr, this, menu_selector(NewgroundsBrowserLayer::onNextPage));
    rightArrow->setPositionX(CCDirector::get()->getWinSize().width / 2 - 24);

    arrowsMenu->addChild(leftArrow);
    arrowsMenu->addChild(rightArrow);

    list = GJListLayer::create(nullptr, "", ccc4(0, 0, 0, 75), 356, 220, 0);
    list->setPosition(this->getContentSize() / 2);
    list->ignoreAnchorPointForPosition(false);
    as<CCSprite*>(list->getChildByID("top-border"))->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName("GJ_table_top02_001.png"));

    searchList = GJListLayer::create(nullptr, "", ccc4(0, 0, 0, 75), 356, 220, 0);
    searchList->setPosition(this->getContentSize() / 2);
    searchList->ignoreAnchorPointForPosition(false);
    searchList->setVisible(false);
    as<CCSprite*>(searchList->getChildByID("top-border"))->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName("GJ_table_top02_001.png"));

    auto genreParent = CCMenu::create();
    genreParent->setContentSize(ccp(320, 80));
    genreParent->setAnchorPoint(ccp(0.5f, 0));
    genreParent->setLayout(RowLayout::create()->setAxis(Axis::Row)->setAxisAlignment(AxisAlignment::Center)->setCrossAxisAlignment(AxisAlignment::End)->setCrossAxisLineAlignment(AxisAlignment::Center)->setGrowCrossAxis(true)->setCrossAxisOverflow(false)->setAutoScale(true));

    for (size_t i = 1; i < 36 + 1; i++)
    {
        genreParent->addChild(NewgroundsGenreLabel::createWithIndex(i));
    }

    genreParent->updateLayout();
    searchList->addChildAtPosition(genreParent, Anchor::Bottom, ccp(0, 15));

    tabsMenu = CCMenu::create();
    tabsMenu->setZOrder(-1);
    tabsMenu->setPosition(list->getPosition() + (list->getContentSize() * ccp(0, 0.5f)) + ccp(0, 12.5f));
    tabsMenu->setAnchorPoint(ccp(0.5f, 0));
    tabsMenu->setContentWidth(370);
    tabsMenu->setLayout(AxisLayout::create()->setAutoScale(false)->setGap(14));
    tabsMenu->setID("tabs-menu");

    tabsMenuSelected = CCMenu::create();
    tabsMenuSelected->setZOrder(1);
    tabsMenuSelected->setPosition(tabsMenu->getPosition());
    tabsMenuSelected->setAnchorPoint(ccp(0.5f, 0));
    tabsMenuSelected->setContentSize(tabsMenu->getContentSize());
    tabsMenuSelected->ignoreAnchorPointForPosition(false);
    tabsMenuSelected->setID("tabs-menu-selected");

    auto pop = TabButton::create("Popular", this, menu_selector(NewgroundsBrowserLayer::onSwitchTab));
    auto tre = TabButton::create("Trending", this, menu_selector(NewgroundsBrowserLayer::onSwitchTab));
    auto sea = TabButton::create("Search", this, menu_selector(NewgroundsBrowserLayer::onSwitchTab));

    buttons.push_back(pop);
    buttons.push_back(tre);
    buttons.push_back(sea);

    tabsMenu->addChild(pop);
    tabsMenu->addChild(tre);
    tabsMenu->addChild(sea);

    tabsMenu->updateLayout();
    tabsMenu->setLayout(nullptr);

    for (auto button : buttons)
    {
        auto tabBG = CCSprite::create("geode.loader/tab-gradient-mask.png");
        tabBG->setAnchorPoint(ccp(0, 0));

        auto clipping = CCClippingNode::create(tabBG);
        clipping->setAlphaThreshold(0.3f);
        
        auto gradientSpr = CCSprite::create("geode.loader/tab-gradient.png");
        clipping->addChild(gradientSpr);

        button->m_onButton->addChild(clipping, -1);

        gradientSpr->setPosition(-button->convertToWorldSpace(ccp(0, 0)) + tabsMenu->getPosition());
    }

    pop->setTag(-1);
    pop->toggleWithCallback(true);
    pop->setTag(0);
    tre->setTag(1);
    sea->setTag(2);

    auto clip = Clip::create();
    clip->setContentSize(list->getContentSize());
    clip->setZOrder(-69);
    bgBlur->setPosition(list->getContentSize() / 2);
    clip->addChild(bgBlur);
    list->addChild(clip);
    searchList->addChild(clip);

    circle = LoadingCircle::create();
    circle->m_sprite->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));
    circle->setVisible(false);

    pageLabel = CCLabelBMFont::create("", "goldFont.fnt");
    pageLabel->setPosition(CCDirector::get()->getWinSize() + ccp(-7, -3));
    pageLabel->setScale(0.6f);
    pageLabel->setAnchorPoint(ccp(1, 1));

    if (!user.empty())
        setTitle(fmt::format("{}'s Songs", user).c_str());

    if (page == 0)
    {
        if (!API::get()->popularSongs.contains(0))
        {
            API::get()->getPopularSongs(0, menu_selector(NewgroundsBrowserLayer::pageFetchCompleted), this);
            circle->setVisible(true);
        }
        else
        {
            updatePageLabel();
        }
    }

    updateList();
    updateArrows();

    this->addChild(bg, -69);
    this->addChild(circle, 69420);
    this->addChild(list);
    this->addChild(searchList);
    this->addChild(pageLabel);
    this->addChild(backMenu);
    this->addChild(arrowsMenu);
    this->addChild(tabsMenu);
    this->addChild(tabsMenuSelected);
    this->addChild(pageLabel);

    return true;
}

void NewgroundsBrowserLayer::pageFetchCompleted(CCObject*)
{
    if (!this)
        return;

    NewgroundsBrowserLayer::updateList();
    circle->setVisible(false);
    updateArrows();
    updatePageLabel();
}

void NewgroundsBrowserLayer::pageFetchCompletedFeatured(CCObject*)
{
    if (!this)
        return;

    NewgroundsBrowserLayer::updateList();
    circle->setVisible(false);
    updateArrows();
    updatePageLabel();
}

void NewgroundsBrowserLayer::updateList()
{
    if (scroll)
    {
        scroll->removeFromParent();
        scroll = nullptr;
    }

    scroll = ScrollLayer::create(list->getContentSize() + ccp(0, -4));
    scroll->setPositionY(4);
    list->addChild(scroll);

    if (selectedTab == 0)
    {
        if (!API::get()->popularSongs.contains(page))
            return;
    }
    else if (selectedTab == 1)
    {
        if (!API::get()->featuredSongs.contains(page))
            return;
    }

    float height = 0;
    int i = 0;

    if (selectedTab == 0)
    {
        for (auto song : API::get()->popularSongs[page])
        {
            auto cell = NewgroundsSongCell::createWithSong(song, false);
            cell->setPositionY(((API::get()->popularSongs[page].size() - 1) * cell->getContentHeight()) - height);
            cell->setOpacity(i % 2 == 0 ? 50 : 0);
            cell->setTag(i);

            height += cell->getContentHeight();
            scroll->m_contentLayer->addChild(cell);
            i++;
        }
    }
    else if (selectedTab == 1)
    {
        for (auto song : API::get()->featuredSongs[page])
        {
            auto cell = NewgroundsSongCell::createWithSong(song, false);
            cell->setPositionY(((API::get()->featuredSongs[page].size() - 1) * cell->getContentHeight()) - height);
            cell->setOpacity(i % 2 == 0 ? 50 : 0);
            cell->setTag(i);

            height += cell->getContentHeight();
            scroll->m_contentLayer->addChild(cell);
            i++;
        }
    }

    scroll->m_contentLayer->setContentHeight(height > list->getContentHeight() ? height : list->getContentHeight());
    scroll->moveToTop();
}

void NewgroundsBrowserLayer::updateArrows()
{
    leftArrow->setVisible(page != 0);
    rightArrow->setVisible(true);

    if (circle->isVisible())
    {
        leftArrow->setVisible(false);
        rightArrow->setVisible(false);
    }
}

void NewgroundsBrowserLayer::setTitle(const char* title)
{
    //auto titleLbl = getChildOfType<CCLabelBMFont>(list, 0);

    //titleLbl->setString(title);
}

void NewgroundsBrowserLayer::updatePageLabel()
{
    if (circle->isVisible())
        return pageLabel->setString("");

    pageLabel->setString(fmt::format("{} to {} of {}", (page * 30) + 1, ((page + 1) * 30), selectedTab == 0 ? API::get()->totalCount : API::get()->totalCountFeatured).c_str());
    pageLabel->limitLabelWidth(123, 0.6f, 0);
}

void NewgroundsBrowserLayer::onBack(CCObject*)
{
    CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void NewgroundsBrowserLayer::onNextPage(CCObject*)
{
    page++;

    if (selectedTab == 0)
    {
        if (!API::get()->popularSongs.contains(page))
        {
            API::get()->getPopularSongs(page, menu_selector(NewgroundsBrowserLayer::pageFetchCompleted), this);
            circle->setVisible(true);
        }
    }
    else if (selectedTab == 1)
    {
        if (!API::get()->featuredSongs.contains(page))
        {
            API::get()->getFeaturedSongs(page, menu_selector(NewgroundsBrowserLayer::pageFetchCompletedFeatured), this);
            circle->setVisible(true);
        }
    }

    updateList();
    updateArrows();
    updatePageLabel();
}

void NewgroundsBrowserLayer::onPrevPage(CCObject*)
{
    page--;

    if (selectedTab == 0)
    {
        if (!API::get()->popularSongs.contains(page))
        {
            API::get()->getPopularSongs(page, menu_selector(NewgroundsBrowserLayer::pageFetchCompleted), this);
            circle->setVisible(true);
        }
    }
    else if (selectedTab == 1)
    {
        if (!API::get()->featuredSongs.contains(page))
        {
            API::get()->getFeaturedSongs(page, menu_selector(NewgroundsBrowserLayer::pageFetchCompletedFeatured), this);
            circle->setVisible(true);
        }
    }

    updateList();
    updateArrows();
    updatePageLabel();
}

void NewgroundsBrowserLayer::onSwitchTab(CCObject* sender)
{
    if (sender->getTag() == 2)
    {
        for (auto button : buttons)
        {
            button->toggle(selectedTab == button->getTag());
        }

        Loader::get()->queueInMainThread([this, sender] {
            as<CCMenuItemToggler*>(sender)->toggle(false);
        });

        auto layer = DialogLayer::createDialogLayer(DialogObject::create("The Shopkeeper", "The search tab is <cl>Coming Soon!</c>", 5, 1, false, ccc3(255, 255, 255)), nullptr, 2);
        layer->animateInRandomSide();

        CCScene::get()->addChild(layer, 420);
        return;
    }

    if (sender->getTag() == selectedTab)
        return;

    for (auto button : buttons)
    {
        button->retain();

        button->removeFromParentAndCleanup(false);

        if (button != sender)
            button->toggle(false);

        (as<CCNode*>(sender) == button ? tabsMenuSelected : tabsMenu)->addChild(button);

        button->release();
    }

    if (sender->getTag() == -1)
        return;

    page = 0;
    selectedTab = sender->getTag();

    list->setVisible(selectedTab != 2);
    searchList->setVisible(selectedTab == 2);

    if (selectedTab == 0)
    {
        if (!API::get()->popularSongs.contains(page))
        {
            API::get()->getPopularSongs(page, menu_selector(NewgroundsBrowserLayer::pageFetchCompleted), this);
            circle->setVisible(true);
        }
    }
    else if (selectedTab == 1)
    {
        if (!API::get()->featuredSongs.contains(page))
        {
            API::get()->getFeaturedSongs(page, menu_selector(NewgroundsBrowserLayer::pageFetchCompletedFeatured), this);
            circle->setVisible(true);
        }
    }

    updateList();
    updateArrows();
    updatePageLabel();
}

void NewgroundsBrowserLayer::keyBackClicked()
{
    onBack(nullptr);
}