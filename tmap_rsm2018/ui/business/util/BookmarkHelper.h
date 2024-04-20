#ifndef BOOKMARKHELPER_H
#define BOOKMARKHELPER_H

namespace SKT
{

class BookmarkHelper
{
public:
    static void setAddFavoriteDestType(const QString &, QVariantMap &);
    static void setAddFavoriteDestType(bool, bool, QVariantMap &);
};

}//SKT

#endif // BOOKMARKHELPER_H
