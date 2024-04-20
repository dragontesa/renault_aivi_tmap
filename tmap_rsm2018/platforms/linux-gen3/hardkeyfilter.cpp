#include "hardkeyfilter.h"
#include "hardkey.h"

//#define WHEEL_REPEAT_COUNT  14 // 현재 Dial키를 돌리면 같은 이벤트가 14개씩 들어옴. 18.12.27 ymhong
#define WHEEL_REPEAT_COUNT  1   // TmapController::eventFilter() 참고. 19.03.11 ymhong

namespace SKT {

HardKeyFilter::HardKeyFilter()
{
    mKeyInfo.m_key = HardKey::NoKey;
    mKeyInfo.m_state = HardKey::Released;
    mRepeat = 0;
}

bool HardKeyFilter::acceptable(const GCKeyInfo &info)
{
    auto dirty = info.m_key != mKeyInfo.m_key || info.m_state != mKeyInfo.m_state;
    mKeyInfo = info;
    if (dirty) {
        mRepeat = 0;
    } else {
        mRepeat++;
        if (info.m_key == HardKey::ItcJogDialSpinCcw || info.m_key == HardKey::ItcJogDialSpinCw) {
            if (mRepeat == WHEEL_REPEAT_COUNT) {
                mRepeat = 0;
                dirty = true;
            }
        }
    }
    return dirty;
}

}
