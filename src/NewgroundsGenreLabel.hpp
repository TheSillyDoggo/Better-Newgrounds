#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "NewgroundsAPI.hpp"
#include "NewgroundsSongCell.hpp"

using namespace geode::prelude;

class NewgroundsGenreLabel : public CCNode
{
    public:
        std::string label;

        static NewgroundsGenreLabel* create(std::string label);
        static NewgroundsGenreLabel* createWithIndex(int index);

        bool initWithIndex(int index);
        bool init(std::string label);

        ccColor3B colourForGenre();
};