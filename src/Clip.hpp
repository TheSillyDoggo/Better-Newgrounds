#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class Clip : public CCClippingNode {
protected:
    CCLayerColor* m_stencil;

    bool init(bool invert) {
        m_stencil = CCLayerColor::create({ 255, 255, 255, 255 });

        if (!CCClippingNode::init(m_stencil))
            return false;

        this->setInverted(invert);
        this->updateClippingRect();

        return true;
    }

    void updateClippingRect() {
        m_stencil->setContentSize(m_obContentSize);
    }

public:
    /**
     * Create a Clip. All ares of children that exceed the clip's content size 
     * are not rendered (clipped out)
     * @param invert Invert the clipping behaviour, so only parts of children 
     * outside the clip's content size are rendered
     */
    static Clip* create(bool invert = false) {
        auto ret = new Clip();
        if (ret && ret->init(invert)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void setContentSize(CCSize const& size) override {
        CCClippingNode::setContentSize(size);
        this->updateClippingRect();
    }
};