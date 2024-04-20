#ifndef CSWMANAGER_H
#define CSWMANAGER_H

#include <QList>
#include <QMap>
#include <QObject>
#include "Csw.h"

namespace SKT
{

class CswManager : public QObject
{
    Q_OBJECT

public:
    CswManager(QObject *parent = nullptr);

    void collect(QObject *);

    // CSW control 신호를 받아서 처리할 지 무시할 지 설정.
    // View가 activate, deactivate일 때 true, false를 설정하면 된다.
    // todo : 각 뷰에서 처리하지 않고 CSWMediator가 처리하게 되면서 불필요해짐. 향후 삭제.
    void setEnable(bool);

    Csw *at(int) const;
    Csw *spinCwItem(int section, int &retIndex) const;
    Csw *spinCcwItem(int section, int &retIndex) const;
    void clear(bool containUndeletable=false);
    void insert(Csw *);
    void remove(Csw *);
    void removeSection(int section);
    void removeSoftekyMenu();
    void reset(int section, int sequence);

    void setFocus(Csw *);
    void setLostFocus(bool bySection);
    void setLostFocus(Csw *, bool bySection);
    void setSelect(Csw *);
    void setLeftPressed(Csw *);
    void setRightPressed(Csw *);
    void setSpinCcwPressed(Csw *);
    void setSpinCwPressed(Csw *);

    void setCurrent(Csw *);
    void setCurrent(const QString &objectName);

    void blockSection(int section, bool block = true);

    bool hasDialogCsw() const;
    void resetTimer();
    void updateDebugInfo(QVariantMap& m);
    void selectDefaultSoftkeyMenu();

    Csw *currentCsw() const;

public slots:
    // CSW functions.
    bool movePrevSection();
    bool moveNextSection(bool byForce=false);
    void moveNextSectionByForce();
    void moveLeft();
    void moveRight();
    void spinCcw();
    void spinCw();
    void select();
    void goBack();
    void goHome();
    void goOption();
    void lostFocus();
    void selectByObjectName(const QString&, int extra = 0);
    void selectBySequence(int section, int sequence, int extra = 0);

private:
    void sort();
    void updateSectionMap();
    int firstSection() const;
    int prevSection() const;
    int nextSection() const;
    int getCurrentIndex(int section, int sequence) const;
    void connectAllCsws();
    void connectTo(Csw *);
    Csw* getItem(int section, int sequence) const;
    Csw *firstSectionItem(int section) const;
    Csw *lastSectionItem(int section) const;
    Csw *defaultSectionItem(int section) const;
    bool keyProcess(Csw *csw, Csw::Key key);
    bool defaultKeyProcess();

private slots:
    void onCswDeleted(QObject *);
    void onKeyProcessed(bool);

signals:
    void forceLostFocusTriggered();

private:
    typedef QList<Csw*> CSWPtrList;
    CSWPtrList mCsws;
    CSWPtrList mBlockedCsws;
    Csw *mCurrent;
    int mCurrentIndex;

    QMap<int, Csw*> mSectionMap;
    int mCurrentSection;
    QTimer timer;
    bool mBlockSoftkey;
    bool mKeyProcessed = false;
};

} //SKT
#endif // CSWMANAGER_H
