#ifndef KEYBOARDKEYINPUTPROXY_H
#define KEYBOARDKEYINPUTPROXY_H

#include <QString>

namespace SKT {

class KeyboardKeyInputProxy
{
public:
    void setMaxStringLength(int length);
    void setString(const QString &string);
    bool keyPressed(const QString &key);
    void backspace();
    void clear();
    QString string() const;
    void setEnableChars(const QString &enableChars);

    void setEditFieldUpdate();

private:
    QString mString;
    QString mEnableChars;
    int maxLength = 65535;
};

}//SKT

#endif // KEYBOARDKEYINPUTPROXY_H
