#ifndef CSWFILTER_H
#define CSWFILTER_H

namespace SKT
{

class CswFilter
{
public:
    enum Type {
        IGNORE_KEYBOARD = 0,
        FORWARD_KEYBOARD,
    };
    void setType(Type type);
    void setKeyboardVisible(bool visible);
    bool processKey(int key, bool pressed);
    bool isBackKey(int key) const;
    bool isCswKey(int key) const;
    void keyForwarding(int key, int state);
    void encoderForwarding(int encCode, int encValue);

private:
    Type mType = IGNORE_KEYBOARD;
    bool mIsKeyboardVisible = false;
};

}

#endif // CSWFILTER_H
