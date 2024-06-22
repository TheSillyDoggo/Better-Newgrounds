#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class AudioWaveformNode : public CCNode
{
    public:
        struct sample_t {
            float value;
            float x;
        };

        std::vector<sample_t> samples;
        float sampleRate = 1.f;
        float position = 0.f;
        std::string file;
        ccColor4B colour = ccc4(255, 255, 255, 255);

        CCRenderTexture* tex;

        static AudioWaveformNode* create(std::string file);

        bool init(std::string file);

        void generateSampleData();

        virtual void draw();
};