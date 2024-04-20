#include "KeyboardKeyInputProxy.h"
#include "PlatformHelper.h"

namespace SKT
{

void KeyboardKeyInputProxy::setMaxStringLength(int length)
{
    maxLength = length;
}

void KeyboardKeyInputProxy::setString(const QString &string)
{
    mString = string.left(maxLength);
    setEditFieldUpdate();
}

bool KeyboardKeyInputProxy::keyPressed(const QString &key)
{
    if (key == "0x232B") {
        backspace();
        setEditFieldUpdate();
    } else if (key == "0x2327") {
        mString.clear();
        setEditFieldUpdate();
    } else {
        if (mEnableChars.isEmpty() || mEnableChars.contains(key)) {
            mString += key;
            auto left = mString.left(maxLength);
            if (left != mString) {
                mString = left;
                setEditFieldUpdate();
            } else {
                return false;
            }
        } else {
            setEditFieldUpdate();
        }
    }
    return true;
}

void KeyboardKeyInputProxy::backspace()
{
    mString.chop(1);
}

void KeyboardKeyInputProxy::clear()
{
    mString.clear();
}

QString KeyboardKeyInputProxy::string() const
{
    qDebug() << "KeyboardKeyInputProxy::string()" << mString;
    return mString;
}

void KeyboardKeyInputProxy::setEnableChars(const QString &enableChars)
{
    mEnableChars = enableChars;
}

void KeyboardKeyInputProxy::setEditFieldUpdate()
{
    uchar position = mString.length();
    PlatformHelper::instance()->setEditFieldUpdate(position, mString);
}

}
