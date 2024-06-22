#include "NewgroundsAPI.hpp"

//https://creomusic.newgrounds.com/audio?page=2&isAjaxRequest=1
//https://www.newgrounds.com/audio/popular?type=1&interval=all&sort=views&genre=0&artist-type=approved&isAjaxRequest=1
//https://www.newgrounds.com/audio/popular?type=1&interval=all&sort=views&genre=0&artist-type=approved&offset=30&inner=1&isAjaxRequest=1

const char* AudioSong::getName()
{
    if (name.empty())
        return "An unknown error has occurred";

    return name.c_str();
}

API* API::get()
{
    if (!instance)
    {
        instance = new API();
    }

    return instance;
}

void FindAllChildrenWithText(XMLElement* element, std::vector<XMLElement*>& result) {
    if (element->GetText() != nullptr && std::string(element->GetText()).length() > 0) {
        result.push_back(element);
    }

    for (XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        FindAllChildrenWithText(child, result);
    }
}

std::string xmlErrorToString(tinyxml2::XMLError error)
{
    if (error != 0)
    {
        std::string e = "";

        switch (error)
        {
            case 1:
                e = "XML_NO_ATTRIBUTE"; break;

            case 2:
                e = "XML_WRONG_ATTRIBUTE_TYPE"; break;

            case 3:
                e = "XML_ERROR_FILE_NOT_FOUND"; break;

            case 4:
                e = "XML_ERROR_FILE_COULD_NOT_BE_OPENED"; break;

            case 5:
                e = "XML_ERROR_FILE_READ_ERROR"; break;

            case 6:
                e = "XML_ERROR_ELEMENT_MISMATCH"; break;

            case 7:
                e = "XML_ERROR_PARSING_ELEMENT"; break;

            case 8:
                e = "XML_ERROR_PARSING_ATTRIBUTE"; break;

            case 9:
                e = "XML_ERROR_IDENTIFYING_TAG"; break;

            case 10:
                e = "XML_ERROR_PARSING_TEXT"; break;

            case 11:
                e = "XML_ERROR_PARSING_CDATA"; break;

            case 12:
                e = "XML_ERROR_PARSING_COMMENT"; break;

            case 13:
                e = "XML_ERROR_PARSING_DECLARATION"; break;

            case 14:
                e = "XML_ERROR_PARSING_UNKNOWN"; break;

            case 15:
                e = "XML_ERROR_EMPTY_DOCUMENT"; break;

            case 16:
                e = "XML_ERROR_MISMATCHED_ELEMENT"; break;

            case 17:
                e = "XML_ERROR_PARSING"; break;

            case 18:
                e = "XML_CAN_NOT_CONVERT_TEXT"; break;

            case 19:
                e = "XML_NO_TEXT_NODE"; break;

            default:
                break;
        }

        return e;
    }

    return "";
}

void API::getPopularSongs(int page, SEL_MenuHandler event, CCObject* sender)
{
    auto url = fmt::format("https://www.newgrounds.com/audio/popular?type=1&interval=all&sort=views&genre=0&artist-type=approved&offset={}&inner=1&isAjaxRequest=1", page * 30);
    sender->retain();

    listener.bind([page, this, event, sender] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            auto json = res->json().unwrap();
            auto str = json.as_object()["content"].as_string();

            totalCount = json.as_object()["total"].as_int();

            if (popularSongs.contains(page))
                popularSongs.erase(page);

            std::vector<AudioSong> songs;

            tinyxml2::XMLDocument doc;
            auto result = doc.Parse(str.c_str());

            for(auto child = doc.FirstChild(); child; child = child->NextSibling() )
            {
                if (child->ToComment())
                {
                    continue;
                }

                if (child->ToElement() && child->ToElement()->Attribute("data-hub-id"))
                {
                    auto song = AudioSong();
                    song.id = child->ToElement()->Attribute("data-hub-id");

                    if (auto wrapper = child->ToElement()->FirstChild())
                    {
                        if (auto flexbox = wrapper->FirstChild())
                        {
                            if (auto submission = flexbox->FirstChild())
                            {
                                song.name = submission->ToElement()->Attribute("title");

                                if (auto icon = submission->FirstChild())
                                {
                                    if (auto icon2 = icon->FirstChild())
                                    {
                                        if (auto icon3 = icon2->FirstChild()->ToElement())
                                        {
                                            song.icon = icon3->Attribute("src");

                                            if (!song.icon.empty())
                                                song.icon = utils::string::replace(song.icon, "_medium", "");
                                        }
                                    }

                                    if (auto details = icon->NextSibling())
                                    {
                                        if (auto detailsmain = details->FirstChild())
                                        {
                                            if (auto details2 = detailsmain->FirstChild())
                                            {
                                                if (auto detailstitle = details2->FirstChild())
                                                {
                                                    for (XMLElement* child = detailstitle->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
                                                    {
                                                        if (std::string(child->Value()).starts_with("span") && child->FirstChild())
                                                        {
                                                            if (auto author = child->ToElement()->FirstChildElement()->GetText())
                                                                song.author = std::string(author);
                                                        }
                                                    }
                                                }
                                            }

                                            if (auto meta = detailsmain->NextSibling())
                                            {
                                                for (XMLElement* child = meta->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
                                                {
                                                    if (auto score = child->Attribute("title"))
                                                        song.score = std::string(score + 7);
                                                }

                                                std::vector<XMLElement*> elementsWithText;
                                                FindAllChildrenWithText(meta->ToElement(), elementsWithText);

                                                bool v = false;

                                                for (auto element : elementsWithText)
                                                {
                                                    if (auto views = std::string(element->GetText()); views.ends_with(" Views"))
                                                    {
                                                        song.views = views.substr(0, views.size() - 6);
                                                    }

                                                    if (v)
                                                    {
                                                        song.category = element->GetText();
                                                        v = false;
                                                    }

                                                    if (auto type = std::string(element->GetText()); type.ends_with("Song"))
                                                    {
                                                        v = true;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        songs.push_back(song);
                    }
                }
            }

            popularSongs[page] = songs;
            latestResponse = 0;
            if (event)
                (sender->*event)(sender);

            sender->release();
        }
        else if (e->isCancelled())
        {
            latestResponse = -1;
            if (event)
                (sender->*event)(sender);

            log::info("The request was cancelled... So sad :(");
            sender->release();
        }
    });

    auto req = web::WebRequest();
    listener.setFilter(req.get(url));
}

void API::getFeaturedSongs(int page, SEL_MenuHandler event, CCObject* sender)
{
    auto url = fmt::format("https://www.newgrounds.com/audio/featured?type=1&offset={}&inner=1&isAjaxRequest=1", page * 30);
    sender->retain();

    listener.bind([page, this, event, sender] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            auto json = res->json().unwrap();
            auto str = json.as_object()["content"].as_string();

            totalCountFeatured = json.as_object()["total"].as_int();

            if (featuredSongs.contains(page))
                featuredSongs.erase(page);

            std::vector<AudioSong> songs;

            tinyxml2::XMLDocument doc;
            auto result = doc.Parse(str.c_str());

            for(auto child = doc.FirstChild(); child; child = child->NextSibling() )
            {
                if (child->ToComment())
                {
                    continue;
                }

                if (child->ToElement() && child->ToElement()->Attribute("data-hub-id"))
                {
                    auto song = AudioSong();
                    song.id = child->ToElement()->Attribute("data-hub-id");

                    if (auto wrapper = child->ToElement()->FirstChild())
                    {
                        if (auto flexbox = wrapper->FirstChild())
                        {
                            if (auto submission = flexbox->FirstChild())
                            {
                                song.name = submission->ToElement()->Attribute("title");

                                if (auto icon = submission->FirstChild())
                                {
                                    if (auto icon2 = icon->FirstChild())
                                    {
                                        if (auto icon3 = icon2->FirstChild()->ToElement())
                                        {
                                            song.icon = icon3->Attribute("src");

                                            if (!song.icon.empty())
                                                song.icon = utils::string::replace(song.icon, "_medium", "");
                                        }
                                    }

                                    if (auto details = icon->NextSibling())
                                    {
                                        if (auto detailsmain = details->FirstChild())
                                        {
                                            if (auto details2 = detailsmain->FirstChild())
                                            {
                                                if (auto detailstitle = details2->FirstChild())
                                                {
                                                    for (XMLElement* child = detailstitle->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
                                                    {
                                                        if (std::string(child->Value()).starts_with("span") && child->FirstChild())
                                                        {
                                                            if (auto author = child->ToElement()->FirstChildElement()->GetText())
                                                                song.author = std::string(author);
                                                        }
                                                    }
                                                }
                                            }

                                            if (auto meta = detailsmain->NextSibling())
                                            {
                                                for (XMLElement* child = meta->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
                                                {
                                                    if (auto score = child->Attribute("title"))
                                                        song.score = std::string(score + 7);
                                                }

                                                std::vector<XMLElement*> elementsWithText;
                                                FindAllChildrenWithText(meta->ToElement(), elementsWithText);

                                                bool v = false;

                                                for (auto element : elementsWithText)
                                                {
                                                    if (auto views = std::string(element->GetText()); views.ends_with(" Views"))
                                                    {
                                                        song.views = views.substr(0, views.size() - 6);
                                                    }

                                                    if (v)
                                                    {
                                                        if (!std::string(element->GetText()).ends_with(" Views"))
                                                            song.category = element->GetText();
                                                        
                                                        v = false;
                                                    }

                                                    if (auto type = std::string(element->GetText()); type.ends_with("Song"))
                                                    {
                                                        v = true;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        songs.push_back(song);
                    }
                }
            }

            featuredSongs[page] = songs;
            latestResponse = 0;
            if (event)
                (sender->*event)(sender);

            sender->release();
        }
        else if (e->isCancelled())
        {
            latestResponse = -1;
            if (event)
                (sender->*event)(sender);

            log::info("The request was cancelled... So sad :(");
            sender->release();
        }
    });

    auto req = web::WebRequest();
    listener.setFilter(req.get(url));
}

void API::getArtistSongs(std::string name, int page, SEL_MenuHandler event, CCObject* sender)
{
    auto url = fmt::format("https://{}.newgrounds.com/audio?page={}&isAjaxRequest=1", name, page + 1);
    sender->retain();

    listener.bind([page, name, this, event, sender] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            auto json = res->json().unwrap();
            auto items = json.as_object()["items"].as_object();

            if (artists[name].songs.contains(page))
                artists[name].songs.erase(page);

            if (json.as_object()["load_more"].as_string() == "\n")
                artists[name].lastPage = page;

            for (auto item : items)
            {
                auto year = item.second.as_array();

                for (auto song2 : year)
                {
                    auto str = song2.as_string();

                    tinyxml2::XMLDocument doc;
                    auto result = doc.Parse(str.c_str());
                    auto song = AudioSong();

                    song.author = name;
                    song.id = std::string(doc.FirstChild()->FirstChild()->FirstChild()->ToElement()->Attribute("href") + 40);

                    if (auto flexbox = doc.FirstChild()->FirstChild())
                    {
                        if (auto submission = flexbox->FirstChild())
                        {
                            song.name = submission->ToElement()->Attribute("title");

                            if (auto icon = submission->FirstChild())
                            {
                                if (auto icon2 = icon->FirstChild())
                                {
                                    if (auto icon3 = icon2->FirstChild()->ToElement())
                                    {
                                        song.icon = icon3->Attribute("src");

                                        if (!song.icon.empty())
                                        {
                                            song.icon = utils::string::replace(song.icon, "_medium", "");
                                            song.icon = utils::string::split(song.icon, "?")[0];
                                        }
                                    }
                                }

                                if (auto details = icon->NextSibling())
                                {
                                    if (auto detailsmain = details->FirstChild())
                                    {
                                        if (auto details2 = detailsmain->FirstChild())
                                        {
                                            if (auto detailstitle = details2->FirstChild())
                                            {
                                                for (XMLElement* child = detailstitle->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
                                                {
                                                    if (std::string(child->Value()).starts_with("span") && child->FirstChild())
                                                    {
                                                        if (auto author = child->ToElement()->FirstChildElement()->GetText())
                                                            song.author = std::string(author);
                                                    }
                                                }
                                            }
                                        }

                                        if (auto meta = detailsmain->NextSibling())
                                        {
                                            for (XMLElement* child = meta->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
                                            {
                                                if (auto score = child->Attribute("title"))
                                                    song.score = std::string(score + 7);
                                            }

                                            std::vector<XMLElement*> elementsWithText;
                                            FindAllChildrenWithText(meta->ToElement(), elementsWithText);

                                            bool v = false;

                                            for (auto element : elementsWithText)
                                            {
                                                if (auto views = std::string(element->GetText()); views.ends_with(" Views"))
                                                {
                                                    song.views = views.substr(0, views.size() - 6);
                                                }

                                                if (v)
                                                {
                                                    song.category = element->GetText();
                                                    v = false;
                                                }

                                                if (auto type = std::string(element->GetText()); type.ends_with("Song"))
                                                {
                                                    v = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    artists[name].songs[page].push_back(song);
                }
            }

            latestResponse = 0;
            if (event)
                (sender->*event)(sender);

            sender->release();
        }
        else if (e->isCancelled())
        {
            latestResponse = -1;
            if (event)
                (sender->*event)(sender);

            log::info("The request was cancelled... So sad :(");
            sender->release();
        }
    });

    auto req = web::WebRequest();
    listener.setFilter(req.get(url));
}

void API::getAdditionalSongInfo(int id, SEL_MenuHandler event, CCObject* sender)
{
    auto url = fmt::format("https://www.newgrounds.com/audio/listen/{}", id);
    sender->retain();

    listener.bind([id, this, event, sender] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            auto str = res->string().unwrap();
            auto lines = utils::string::split(str, "\n");

            if (additionalData.contains(id))
                additionalData.erase(id);

            auto info = AdditionalAudioData();
            info.id = fmt::format("{}", id);

            bool inDesc = false;
            bool inDownloads = false;

            for (auto line : lines)
            {
                if (inDesc)
                {
                    info.description += fmt::format("{}\n", line);
                }

                if (inDownloads)
                {
                    inDownloads = false;
                    info.downloads = utils::string::trim(line).substr(4, utils::string::trim(line).size() - 9);
                }

                if (line.find("id=\"author_comments\"") != std::string::npos)
                {
                    inDesc = true;
                }

                if (line.find("<dt>Downloads</dt>") != std::string::npos)
                {
                    inDownloads = true;
                }

                if (inDesc && line.find("</div>") != std::string::npos)
                {
                    inDesc = false;
                    info.description = info.description.substr(0, info.description.size() - 6 - 1);
                    info.description = utils::string::trimLeft(info.description);
                    info.description = getFormattedText(info.description);
                }
            }

            additionalData[id] = info;
            latestResponse = 0;
            if (event)
                (sender->*event)(sender);

            sender->release();
        }
        else if (e->isCancelled())
        {
            latestResponse = -1;
            if (event)
                (sender->*event)(sender);

            log::info("The request was cancelled... So sad :(");
            sender->release();
        }
    });

    auto req = web::WebRequest();
    listener.setFilter(req.get(url));
}

void API::getArtist(std::string name, SEL_MenuHandler event, CCObject* sender)
{
    auto url = fmt::format("https://{}.newgrounds.com/", name);
    sender->retain();

    listener.bind([name, this, event, sender] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            auto str = res->string().unwrap();
            auto lines = utils::string::split(str, "\n");

            auto artist = AudioArtist();
            artist.name = name;

            for (auto line : lines)
            {
                if (line.find("page_banners") != std::string::npos)
                {
                    auto banner = utils::string::split(utils::string::split(line, "\"src\":\"\\/\\/")[1], "?")[0];
                    banner = utils::string::replace(banner, "\\/", "/");

                    artist.banner = banner;
                }

                if (line.find("href=\"https://uimg.ngfiles.com/profile/") != std::string::npos)
                {
                    auto pfp = utils::string::split(utils::string::split(line, "href=\"")[1], "?")[0];

                    artist.pfp = pfp;
                }

                if (line.find("blockquote class=\"general fill-space text-content\"") != std::string::npos)
                {
                    auto bio = utils::string::split(utils::string::split(line, "<blockquote class=\"general fill-space text-content\">")[1], "</blockquote>")[0];

                    artist.bio = bio;
                }

                //class="fa fa-map-marker"
                //class="fa fa-calendar"
            }

            artists[name] = artist;
            latestResponse = 0;
            if (event)
                (sender->*event)(sender);

            sender->release();
        }
        else if (e->isCancelled())
        {
            latestResponse = -1;
            if (event)
                (sender->*event)(sender);

            log::info("The request was cancelled... So sad :(");
            sender->release();
        }
    });

    auto req = web::WebRequest();
    listener.setFilter(req.get(url));
}

std::string API::getFormattedText(std::string txt)
{
    std::string markdown = txt;

    // Replace <br> tags with newlines
    markdown = std::regex_replace(markdown, std::regex("<br>"), "\n");

    // Replace <p> tags with newlines
    markdown = std::regex_replace(markdown, std::regex("<p>"), "");
    markdown = std::regex_replace(markdown, std::regex("</p>"), "\n");

    // Replace <a href="...">...</a> with [text](url)
    std::regex link_regex("<a href=\"([^\"]*)\"[^>]*>([^<]*)</a>");
    markdown = std::regex_replace(markdown, link_regex, "[$2]($1)");

    // Remove remaining HTML tags
    markdown = std::regex_replace(markdown, std::regex("<[^>]+>"), "");

    return markdown;
}

AudioSong API::getSong(std::string id)
{
    for (auto page : featuredSongs)
    {
        for (auto song : page.second)
        {
            if (song.id == id)
                return song;
        }
    }

    for (auto page : popularSongs)
    {
        for (auto song : page.second)
        {
            if (song.id == id)
                return song;
        }
    }

    for (auto artist : artists)
    {
        for (auto page : artist.second.songs)
        {
            for (auto song : page.second)
            {
                if (song.id == id)
                    return song;
            }
        }
    }

    return AudioSong();
}
