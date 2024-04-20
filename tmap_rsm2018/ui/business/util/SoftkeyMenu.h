#ifndef SOFTKEYMENU_H
#define SOFTKEYMENU_H

#include <QString>

namespace SKT
{

struct SoftkeyMenu
{
    Q_GADGET
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(QString text READ text)

public:
    int id() { return mId; }
    QString text() { return mText; }

public:
    int mId;
    QString mText;
};

}

Q_DECLARE_METATYPE(SKT::SoftkeyMenu)

#endif // SOFTKEYMENU_H
