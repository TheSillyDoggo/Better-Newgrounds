#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/cocos/support/tinyxml2/tinyxml2.h>
#include <regex>

using namespace tinyxml2;
using namespace geode::prelude;

struct AudioSong
{
    public:
        std::string name;
        std::string author;
        std::string id;
        std::string icon;
        std::string score;
        std::string views;
        std::string category;

        const char* getName();
};

struct AdditionalAudioData
{
    std::string id;
    std::string description;
    std::string downloads;
};

struct AudioArtist
{
    public:
        std::string name;
        std::string banner;
        std::string pfp;
        std::string bio;
        std::string location;
        std::string joinTime;
        int lastPage = -1;

        std::map<int, std::vector<AudioSong>> songs;
};

class API : CCNode
{
    private:
        static inline API* instance = nullptr;
    public:
        EventListener<web::WebTask> listener;
        std::map<std::string, AudioArtist> artists;
        int latestResponse = 0;

        std::map<int, std::vector<AudioSong>> popularSongs;
        int totalCount = 0;

        std::map<int, std::vector<AudioSong>> featuredSongs;
        int totalCountFeatured = 0;

        std::map<int, AdditionalAudioData> additionalData;

        static API* get();

        std::string getFormattedText(std::string txt);

        void getFeaturedSongs(int page, SEL_MenuHandler event, CCObject* sender);
        void getPopularSongs(int page, SEL_MenuHandler event, CCObject* sender);
        void getArtist(std::string name, SEL_MenuHandler event, CCObject* sender);
        void getArtistSongs(std::string name, int page, SEL_MenuHandler event, CCObject* sender);
        void getAdditionalSongInfo(int id, SEL_MenuHandler event, CCObject* sender);

        AudioSong getSong(std::string id);
};