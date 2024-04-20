#include "cswfilter.h"
#include "common/hardkey.h"
#include "ui/util/PlatformHelper.h"

namespace SKT
{

void CswFilter::setType(Type type)
{
    mType = type;
}

void CswFilter::setKeyboardVisible(bool visible)
{
    mIsKeyboardVisible = visible;
}

bool CswFilter::processKey(int key, bool pressed)
{
    // SWiD-HMIProxy_V1.4
    // 키보드가 표시 될 때 전달 하는 것이 아니라(종전)
    // 키를 사용(consume) 하지 않는 경우에 전달 하도록 수정. isKeyboardVisible은 더 이상 사용하지 않음.
    // 뒤로가기(Back)를 사용 하므로 해당 키 외에 전부 전달
    // 19.09.06 ymhong
    //auto isKeyboardVisible = PlatformHelper::instance()->isKeyboardVisible();
    auto state = pressed ? 1 : 0;
    if (!isCswKey(key)) {
        PlatformHelper::instance()->keyForwarding(key, state);
        return true;
    } else {
        if (mType == FORWARD_KEYBOARD) {
            PlatformHelper::instance()->keyForwarding(key, state);
            return true;
        }
    }
    return false;
}

bool CswFilter::isBackKey(int key) const
{
    return key == HardKey::ItcJogDialBack;
}

bool CswFilter::isCswKey(int key) const
{
    return key == HardKey::ItcJogDialHome ||
           key == HardKey::ItcJogDialBack ||
           key == HardKey::ItcJogDialLeft ||
           key == HardKey::ItcJogDialRight ||
           key == HardKey::ItcJogDialUp ||
           key == HardKey::ItcJogDialDown ||
           key == HardKey::ItcJogDialSelect ||
           key == HardKey::ItcJogDialOption;
}

void CswFilter::keyForwarding(int key, int state)
{
    PlatformHelper::instance()->keyForwarding(key, state);
}

void CswFilter::encoderForwarding(int encCode, int encValue)
{
    PlatformHelper::instance()->encoderForwarding(encCode, encValue);
}

}
