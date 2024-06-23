#include "AudioWaveformNode.hpp"

AudioWaveformNode* AudioWaveformNode::create(std::string file)
{
    auto pRet = new AudioWaveformNode();

    if (pRet && pRet->init(file))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool AudioWaveformNode::init(std::string file)
{
    if (!CCNode::init())
        return false;

    this->file = file;

    std::thread generateThread([this]{ this->generateSampleData(); });
    generateThread.detach();

    return true;
}

void AudioWaveformNode::generateSampleData()
{
    auto engine = FMODAudioEngine::sharedEngine();

    if (!engine || !engine->m_system)
        return;

    FMOD::Sound* sound;
    const FMOD_MODE mode = FMOD_DEFAULT | FMOD_CREATESAMPLE | FMOD_OPENONLY;
    engine->m_system->createSound(file.c_str(), mode, nullptr, &sound);

    if (!sound)
        return;

    FMOD_SOUND_FORMAT format;
    int channels;
    sound->getFormat(nullptr, &format, &channels, nullptr);

    if(format != FMOD_SOUND_FORMAT_PCM8 && format != FMOD_SOUND_FORMAT_PCM16 && format != FMOD_SOUND_FORMAT_PCM24 &&
        format != FMOD_SOUND_FORMAT_PCM32 && format != FMOD_SOUND_FORMAT_PCMFLOAT) {
        log::error("Unsupported format 3:");
        sound->release();
        sound = nullptr;
        samples.clear();
        return;
    }

    unsigned int length;
    unsigned int sampleCount;
    sound->getLength(&length, FMOD_TIMEUNIT_PCMBYTES);
    sound->getLength(&sampleCount, FMOD_TIMEUNIT_PCM);
    unsigned int bytesPerSample = length / sampleCount;
    int8_t* data = new int8_t[length];
    sound->readData(data, length, &length);

    FMOD::ChannelGroup* channelGroup;
    engine->m_globalChannel->getChannelGroup(&channelGroup);
    engine->m_system->playSound(sound, channelGroup, true, &engine->m_globalChannel);
    engine->m_globalChannel->getFrequency(&sampleRate);

    sound->release();
    sound = nullptr;

    sampleCount = length / bytesPerSample;
    samples.clear();
    samples.reserve(sampleCount);
    size_t dataIndex = 0;
    for(size_t i = 0; i < sampleCount; i++) {
        float sample = 0.f;
        for(int j = 0; j < channels; j++) {
            int32_t rawChannelSample;
            switch(format) {
                case FMOD_SOUND_FORMAT_PCM8:
                    rawChannelSample = data[dataIndex++];
                    sample += rawChannelSample / 255.f;
                    break;
                case FMOD_SOUND_FORMAT_PCM16:
                    rawChannelSample = (data[dataIndex++] << 0 |
                                        data[dataIndex++] << 8);
                    sample += rawChannelSample / (255.f * 255.f);
                    break;
                case FMOD_SOUND_FORMAT_PCM24:
                    rawChannelSample = (data[dataIndex++] << 0 |
                                        data[dataIndex++] << 8 |
                                        data[dataIndex++] << 16);
                    sample += rawChannelSample / (255.f * 255.f * 255.f);
                    break;
                case FMOD_SOUND_FORMAT_PCM32:
                    rawChannelSample = (data[dataIndex++] << 0  |
                                        data[dataIndex++] << 8  |
                                        data[dataIndex++] << 16 |
                                        data[dataIndex++] << 24);
                    sample += rawChannelSample / (255.f * 255.f * 255.f * 255.f);
                    break;
                case FMOD_SOUND_FORMAT_PCMFLOAT:
                    rawChannelSample = (data[dataIndex++] << 0  |
                                        data[dataIndex++] << 8  |
                                        data[dataIndex++] << 16 |
                                        data[dataIndex++] << 24);
                    sample += *reinterpret_cast<float*>(&rawChannelSample);
                    break;
            }
        }
        sample /= channels;
        samples.push_back({sample, 0.f});
    }
    delete[] data;
}

void AudioWaveformNode::draw()
{
    if (samples.empty())
        return;

    if (!tex)
    {
        tex = CCRenderTexture::create(this->getContentWidth(), this->getContentHeight());
        tex->getSprite()->setAnchorPoint(ccp(0, 1));
        tex->getSprite()->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        tex->begin();

        CCNode::draw();

        ccDrawColor4B(colour.r, colour.g, colour.b, colour.a);

        float width = this->getContentSize().width;
        float height = this->getContentSize().height;
        size_t numSamples = samples.size();
        float stepX = width / (numSamples - 1);

        for (size_t i = 1; i < numSamples; ++i)
        {
            float x1 = (i - 1) * stepX;
            float y1 = (samples[i - 1].value + 1.0f) * 0.5f * height;
            float x2 = i * stepX;
            float y2 = (samples[i].value + 1.0f) * 0.5f * height;

            ccDrawLine(ccp(x1, y1), ccp(x2, y2));
        }

        tex->end();

        this->addChild(tex);
    }
}