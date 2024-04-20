#ifndef ADDRESSSEARCHRESULTVO_H
#define ADDRESSSEARCHRESULTVO_H

namespace SKT {

class AddressSearchResultVo
{
    Q_GADGET
    Q_PROPERTY(QString code READ code WRITE setCode)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(int jibunStIdx READ jibunStIdx WRITE setJibunStIdx)
    Q_PROPERTY(int jibunCnt READ jibunCnt WRITE setJibunCnt)
    Q_PROPERTY(int aptStIdx READ aptStIdx WRITE setAptStIdx)
    Q_PROPERTY(short aptCnt READ aptCnt WRITE setAptCnt)
    Q_PROPERTY(int newAddrStIdx READ newAddrStIdx WRITE setNewAddrStIdx)
    Q_PROPERTY(int newAddrCnt READ newAddrCnt WRITE setNewAddrCnt)
    Q_PROPERTY(int jibunRicode READ jibunRicode WRITE setJibunRicode)
public:
    AddressSearchResultVo();

    AddressSearchResultVo(
        const QString& code,
        const QString& name,
        int jibunStIdx,
        int jibunCnt,
        int aptStIdx,
        short aptCnt,
        int newAddrStIdx,
        int newAddrCnt,
        int jibunRicode
        );

    AddressSearchResultVo( const AddressSearchResultVo& other );

    QString code() const;
    QString name() const;
    int jibunStIdx() const;
    int jibunCnt() const;
    int aptStIdx() const;
    short aptCnt() const;
    int newAddrStIdx() const;
    int newAddrCnt() const;
    int jibunRicode() const;

    void setName( const QString& value );
    void setCode( const QString& value );
    void setJibunStIdx( int value );
    void setJibunCnt( int value );
    void setAptStIdx( int value );
    void setAptCnt( short value );
    void setNewAddrStIdx( int value );
    void setNewAddrCnt( int value );
    void setJibunRicode( int value );

private:
    QString mCode;
    QString mName;
    int mJibunStIdx = 0;
    int mJibunCnt = 0;
    int mAptStIdx = 0;
    short mAptCnt = 0;
    int mNewAddrStIdx = 0;
    int mNewAddrCnt = 0;
    int mJibunRicode = 0;

};

}

Q_DECLARE_METATYPE(SKT::AddressSearchResultVo)

#endif // ADDRESSSEARCHRESULTVO_H
