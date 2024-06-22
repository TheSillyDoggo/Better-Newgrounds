#include "NewgroundsGenreLabel.hpp"

NewgroundsGenreLabel* NewgroundsGenreLabel::create(std::string label)
{
    auto pRet = new NewgroundsGenreLabel();

    if (pRet && pRet->init(label))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

NewgroundsGenreLabel* NewgroundsGenreLabel::createWithIndex(int index)
{
    auto pRet = new NewgroundsGenreLabel();

    if (pRet && pRet->initWithIndex(index))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool NewgroundsGenreLabel::initWithIndex(int index)
{
    std::string l;

    if (index == 1) l = "Jazz";
    if (index == 2) l = "Solo Instrument";
    if (index == 3) l = "Ambient";
    if (index == 4) l = "Chipstep";
    if (index == 5) l = "Dance";
    if (index == 6) l = "Drum N Bass";
    if (index == 7) l = "Dubstep";
    if (index == 8) l = "House";
    if (index == 9) l = "Industrial";
    if (index == 10) l = "New Wave";
    if (index == 11) l = "Synthwave";
    if (index == 12) l = "Techno";
    if (index == 13) l = "Trance";
    if (index == 14) l = "Video Game";
    if (index == 15) l = "Hip Hop - Modern";
    if (index == 16) l = "Hip Hop - Olskool";
    if (index == 17) l = "Nerdcore";
    if (index == 18) l = "R&B";
    if (index == 19) l = "Brit Pop";
    if (index == 20) l = "Classic Rock";
    if (index == 21) l = "General Rock";
    if (index == 22) l = "Grunge";
    if (index == 23) l = "Heavy Metal";
    if (index == 24) l = "Indie";
    if (index == 25) l = "Pop";
    if (index == 26) l = "Punk";
    if (index == 27) l = "Cinematic";
    if (index == 28) l = "Experimental";
    if (index == 29) l = "Fusion";
    if (index == 30) l = "Goth";
    if (index == 31) l = "Miscellaneous";
    if (index == 32) l = "Ska";
    if (index == 33) l = "World";
    if (index == 34) l = "Bluegrass";
    if (index == 35) l = "Blues";
    if (index == 36) l = "Country";

    return NewgroundsGenreLabel::init(l);
}

bool NewgroundsGenreLabel::init(std::string label)
{
    if (!CCNode::init())
        return false;

    this->label = label;

    float scale = 0.45f;

    auto cat = CCLabelBMFont::create(label.c_str(), "bigFont.fnt");
    cat->setScale(0.4f / scale);

    auto catBG = CCScale9Sprite::create("geode.loader/white-square.png");
    catBG->setColor(colourForGenre());
    catBG->setContentSize(cat->getScaledContentSize());
    catBG->setScale(scale);
    cat->setScale(cat->getScale() * 0.9f);
    catBG->setContentWidth(cat->getScaledContentWidth() + 6);
    catBG->addChildAtPosition(cat, Anchor::Center);
    catBG->setAnchorPoint(ccp(0, 0));

    this->addChild(catBG);
    this->setContentSize(catBG->getScaledContentSize());

    return true;
}

// Definitely not stolen :3
std::vector<ccColor3B> colours = {
    ccc3(75, 75, 75),
    ccc3(255, 209, 220), // Pastel Pink
    ccc3(174, 198, 207), // Pastel Blue
    ccc3(119, 221, 119), // Pastel Green
    ccc3(253, 253, 150), // Pastel Yellow
    ccc3(195, 177, 225), // Pastel Purple
    ccc3(255, 179, 71),  // Pastel Orange
    ccc3(255, 105, 97),  // Pastel Red
    ccc3(255, 218, 185), // Pastel Peach
    ccc3(230, 230, 250), // Pastel Lavender
    ccc3(152, 255, 152), // Pastel Mint
    ccc3(255, 179, 179), // Pastel Coral
    ccc3(175, 238, 238), // Pastel Turquoise
    ccc3(220, 208, 255), // Pastel Lilac
    ccc3(153, 204, 204), // Pastel Teal
    ccc3(255, 182, 193), // Pastel Rose
    ccc3(178, 251, 165), // Pastel Lime
    ccc3(245, 245, 220), // Pastel Beige
    ccc3(255, 240, 245), // Pastel Blush
    ccc3(240, 248, 255), // Pastel Alice Blue
    ccc3(255, 228, 225), // Pastel Misty Rose
    ccc3(255, 222, 173), // Pastel Navajo White
    ccc3(255, 239, 213), // Pastel Papaya Whip
    ccc3(255, 250, 205), // Pastel Lemon Chiffon
    ccc3(240, 255, 240), // Pastel Honeydew
    ccc3(245, 255, 250), // Pastel Mint Cream
    ccc3(240, 255, 255), // Pastel Azure
    ccc3(240, 248, 255), // Pastel Alice Blue
    ccc3(255, 250, 240), // Pastel Floral White
    ccc3(250, 240, 230), // Pastel Linen
    ccc3(255, 228, 196), // Pastel Bisque
    ccc3(255, 255, 224), // Pastel Light Yellow
    ccc3(253, 245, 230), // Pastel Old Lace
    ccc3(255, 245, 238), // Pastel Seashell
    ccc3(255, 250, 250), // Pastel Snow
    ccc3(245, 245, 245), // Pastel White Smoke
    ccc3(255, 248, 220), // Pastel Cornsilk
};

ccColor3B NewgroundsGenreLabel::colourForGenre()
{
    int i = 0;

    if (label == "Jazz") i = 1;
    if (label == "Solo Instrument") i = 2;
    if (label == "Ambient") i = 3;
    if (label == "Chipstep") i = 4;
    if (label == "Dance") i = 5;
    if (label == "Drum N Bass") i = 6;
    if (label == "Dubstep") i = 7;
    if (label == "House") i = 8;
    if (label == "Industrial") i = 9;
    if (label == "New Wave") i = 10;
    if (label == "Synthwave") i = 11;
    if (label == "Techno") i = 12;
    if (label == "Trance") i = 13;
    if (label == "Video Game") i = 14;
    if (label == "Hip Hop - Modern") i = 15;
    if (label == "Hip Hop - Olskool") i = 16;
    if (label == "Nerdcore") i = 17;
    if (label == "R&B") i = 18;
    if (label == "Brit Pop") i = 19;
    if (label == "Classic Rock") i = 20;
    if (label == "General Rock") i = 21;
    if (label == "Grunge") i = 22;
    if (label == "Heavy Metal") i = 23;
    if (label == "Indie") i = 24;
    if (label == "Pop") i = 25;
    if (label == "Punk") i = 26;
    if (label == "Cinematic") i = 27;
    if (label == "Experimental") i = 28;
    if (label == "Fusion") i = 29;
    if (label == "Goth") i = 30;
    if (label == "Miscellaneous") i = 31;
    if (label == "Ska") i = 32;
    if (label == "World") i = 33;
    if (label == "Bluegrass") i = 34;
    if (label == "Blues") i = 35;
    if (label == "Country") i = 36;

    return colours[i];
}