#include "CswManager.h"
#include "Csw.h"
#include <QtQuick/QQuickItem>

namespace SKT
{

#ifdef QT_DEBUG
#   define CSW_LOSTFOCUS_INTERVAL  (5*1000) // 디버그 모드에서 필요시 시간을 늘려 디버깅을 용이하게 한다.
#else
#   define CSW_LOSTFOCUS_INTERVAL  (5*1000)
#endif

CswManager::CswManager(QObject *parent)
    : QObject(parent)
    , mCurrent(nullptr)
    , mCurrentIndex(-1)
    , mCurrentSection(-1)
{
    // CSW 타이머는 더 이상 사용하지 않음.
    // 19.04.16 ymhong.
#if (0)
    timer.setInterval(CSW_LOSTFOCUS_INTERVAL);
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, this, &CswManager::lostFocus);
#endif
}

static void traverse(QList<Csw *> &list, QObject *object, int &traverseCount)
{
    ++traverseCount;

    for (auto child: object->children()) {
        QString className = child->metaObject()->className();
        if (className == "SKT::Csw") {
            auto csw = dynamic_cast<Csw*>(child);
            if (csw && !list.contains(csw)) {
                list.push_back(csw);
            }
        } else {
            // QQuickItem의 경우 QObject의 child가 아닌
            // QQuickItem의 childItems()를 뒤져야 한다.
            // 통상 ListView의 delegate들이 이런 케이스.
            auto quickItem = qobject_cast<QQuickItem*>(child);
            if (quickItem) {
                auto children = quickItem->childItems();
                for (auto child : children) {
                    traverse(list, child, traverseCount);
                }
            }
        }

        traverse(list, child, traverseCount);
    }
}

void CswManager::collect(QObject *viewComponent)
{
    int traverseCount = 0;
    traverse(mCsws, viewComponent, traverseCount);

    sort();
    connectAllCsws();
    updateSectionMap();
    mCurrent = nullptr;
    mCurrentIndex = -1;
    mCurrentSection = -1;
}

void CswManager::sort()
{
    qSort(mCsws.begin(), mCsws.end(), [](Csw *l, Csw *r){
        return *l < *r;
    });

#ifdef QT_DEBUG
//    QString str;
//    for (Csw *csw : mCsws) {
//        str += QString("(%1,%2)").arg(csw->section()).arg(csw->sequence());
//    }
//    T_DEBUGV("CSWManager::sort() : %s", str.toLatin1().constData());
#endif
}

void CswManager::updateSectionMap()
{
    mSectionMap.clear();

    int i = 0;
    for (Csw *csw : mCsws) {
        if (csw) {
            auto section = csw->section();
            auto sequence = csw->sequence();

            if (!mSectionMap.contains(section)) {
                mSectionMap[section] = csw;
            } else {
                if (mSectionMap[section]->sequence() > sequence) {
                    mSectionMap[section] = csw;
                }
            }
        }
        ++i;
    }
}

int CswManager::firstSection() const
{
    auto keys = mSectionMap.keys();

    return keys.count() ? keys.at(0) : -1;
}

int CswManager::prevSection() const
{
    if (!mSectionMap.contains(mCurrentSection)) {
        for (Csw *csw : mCsws) {
            if (csw->enable()) {
                return csw->section();
            }
        }
    } else {
        auto keys = mSectionMap.keys();
        for (auto key : keys) {
            if (key == mCurrentSection) {
                auto pos = keys.indexOf(key);
                if (0 < pos)
                    --pos;

                while (0 <= pos) {
                    auto section = keys.at(pos);
                    auto csw = firstSectionItem(section);
                    if (csw != nullptr)
                        return csw->section();
                    --pos;
                }
            }
        }
    }
    return mCurrentSection;
}

int CswManager::nextSection() const
{
    if (!mSectionMap.contains(mCurrentSection)) {
        for (Csw *csw : mCsws) {
            if (csw->enable()) {
                return csw->section();
            }
        }
    } else {
        auto keys = mSectionMap.keys();
        auto pos = keys.indexOf(mCurrentSection);
        if (pos < keys.length()-1)
            ++pos;

        while (pos <= keys.length()-1) {
            auto section = keys.at(pos);
            auto csw = firstSectionItem(section);
            if (csw != nullptr)
                return csw->section();
            ++pos;
        }
    }

    return mCurrentSection;
}

int CswManager::getCurrentIndex(int section, int sequence) const
{
    Csw *csw = nullptr;
    int index = -1;
    if (mSectionMap.contains(section)) {
        csw = mSectionMap[section];
        if (csw) {
            index = mCsws.indexOf(csw);
            if (0 <= sequence) {
                int i = index;
                while (csw && csw->sequence() != sequence) {
                    ++i;
                    csw = at(i);
                    if (csw->section() != section)
                        return index;
                }

                if (csw)
                    index = i;
            }
        }
    }

    return index;
}

void CswManager::connectAllCsws()
{
    int i=0;
    for (auto iter = mCsws.begin(); iter != mCsws.end(); ++iter) {
        auto csw = *iter;
        connectTo(csw);
        ++i;
    }
}

void CswManager::connectTo(Csw *csw)
{
    connect(csw, SIGNAL(destroyed(QObject*)), this, SLOT(onCswDeleted(QObject*)), Qt::UniqueConnection);
    connect(csw, SIGNAL(forceNextSection()), this, SLOT(moveNextSectionByForce()), Qt::UniqueConnection);
    connect(csw, SIGNAL(forceLostFocus()), this, SLOT(lostFocus()), Qt::UniqueConnection);
    connect(csw, SIGNAL(keyProcessed(bool)), this, SLOT(onKeyProcessed(bool)), Qt::UniqueConnection);
    connect(csw, SIGNAL(forceLostFocus()), this, SIGNAL(forceLostFocusTriggered()), Qt::UniqueConnection);
}

Csw *CswManager::getItem(int section, int sequence) const
{
    Csw *csw = nullptr;
    if (mSectionMap.contains(section)) {
        csw = mSectionMap[section];
        auto index = mCsws.indexOf(csw);
#if (0)
        auto count = mCsws.count();

        for ( ; ; ) {
            if (csw->sequence() == sequence)
                return csw;
            ++index;
            if (index == count) {
                return nullptr;
            }

            csw = mCsws.at(index);
        }
#else
        auto list = mCsws.mid(index);
        for (auto c : list) {
            if (c->section() == section && c->isDefault()) {
                return c;
            }
            if (c->section() > section)
                break;
        }
        for (auto c : list) {
            if (c->section() == section && c->sequence() >= sequence) {
                return c;
            }
        }
#endif
    }
    return csw;
}

Csw *CswManager::firstSectionItem(int section) const
{
    Csw *ret = nullptr;
    if (mSectionMap.contains(section)) {
        auto csw = mSectionMap[section];
        if (csw->enable())
            return csw;

        auto i = mCsws.indexOf(csw) + 1;
        for ( ; i < mCsws.length(); i++) {
            csw = mCsws.at(i);
            if (csw->section() != section)
                break;
            if (csw->enable()) {
                ret = csw;
                break;
            }
        }
    }
    return ret;
}

Csw *CswManager::lastSectionItem(int section) const
{
    Csw *ret = nullptr;
    if (mSectionMap.contains(section)) {
        auto csw = mSectionMap[section];
        int i = mCsws.indexOf(csw);
        for ( ; i<mCsws.length(); i++) {
            csw = mCsws.at(i);
            if (csw->section() != section) {
                break;
            }
            if (csw->enable()) {
                ret = csw;
            }
        }
    }
    return ret;
}

Csw *CswManager::defaultSectionItem(int section) const
{
    Csw *candidate = nullptr;
    if (mSectionMap.contains(section)) {
        auto csw = mSectionMap[section];
        auto index = mCsws.indexOf(csw);
        for ( ; index<mCsws.length(); index++) {
            csw = mCsws.at(index);
            if (csw->enable()) {
                if (csw->isDefault()) {
                    return csw;
                } else {
                    if (candidate == nullptr && csw->section() == Csw::Section_Softkey) {
                        switch (csw->sequence()) {
                        case Csw::Sequence_Back:
                        case Csw::Sequence_Navi:
                        case Csw::Sequence_Option:
                            break;
                        default:
                            candidate = csw;
                            break;
                        }
                    }
                }
            }
        }
    }

    return candidate;
}

bool CswManager::keyProcess(Csw *, Csw::Key key)
{
    mKeyProcessed = false;
    for (auto c : mCsws) {
        if (c->enable() && c->enableKeyProcess()) {
            QMetaObject::invokeMethod(c, "onKeyFunction", Q_ARG(QVariant, key));
            break;
        }
    }

    return mKeyProcessed;
}

bool CswManager::defaultKeyProcess()
{
    if (mCurrentSection < 0) {
        if (hasDialogCsw()) {
            auto csw = firstSectionItem(Csw::Section_Dialog);
            if (!csw) {
                csw = firstSectionItem(Csw::Section_DialogBottom);
            }
            if (csw) {
                setCurrent(csw);
                resetTimer();
            }
            return true;
        }
        for (auto iter=mSectionMap.begin(); iter!=mSectionMap.end(); iter++) {
            auto csw = *iter;
            if (csw) {
                if (csw->section() == Csw::Section_IntegratedSearch) {
                    continue;
                }
                if (csw->section() == Csw::Section_Softkey) {
                    auto csw2 = defaultSectionItem(Csw::Section_Softkey);
                    if (csw2 != nullptr) {
                        csw = csw2;
                    }
                }
                if (csw->enable()) {
                    setCurrent(csw);
                    resetTimer();
                    return true;
                }
            }
        }
    }
    return false;
}

void CswManager::onCswDeleted(QObject *object)
{
    Csw *csw = dynamic_cast<Csw*>(object);
    if (csw) {
        remove(csw);
        if (csw == mCurrent) {
            mCurrent = nullptr;
            mCurrentIndex = -1;
            mCurrentSection = -1;
            timer.stop();
        }
    }
}

void CswManager::onKeyProcessed(bool b)
{
    mKeyProcessed = b;
    if (b) {
        // Qml내에서 키 처리가 이루어진 경우 타이머를 리셋한다.
        resetTimer();
    }
}

void CswManager::setEnable(bool)
{
}

Csw *CswManager::at(int index) const
{
    Csw *ret = nullptr;
    if (0 <= index && index < mCsws.count()) {
        ret = mCsws.at(index);
    }
    return ret;
}

Csw *CswManager::spinCwItem(int section, int &retIndex) const
{
    int index = retIndex;
    Csw *csw = nullptr;
    do {
        ++index;
        csw = at(index);
        if (csw) {
            if (section < 0)
                section = csw->section();

            if (section != csw->section()) {
                csw = nullptr;
                break;
            }
        }
    } while (csw && !csw->enable());

    if (csw) {
        retIndex = index;
    } else {
        // 다음 섹션의 아이템이 선택 되었다면 현 섹션의 첫번째 아이템을 설정(looping)
        csw = firstSectionItem(section);
        if (csw) {
            retIndex = mCsws.indexOf(csw);
        }
    }

    return csw;
}

Csw *CswManager::spinCcwItem(int section, int &retIndex) const
{
    int index = retIndex;
    Csw *csw = nullptr;
    do {
        --index;
        csw = at(index);
        if (csw) {
            if (section < 0)
                section = csw->section();

            if (section != csw->section()) {
                csw = nullptr;
                break;
            }
        }
    } while (csw && !csw->enable());

    if (csw) {
        retIndex = index;
    } else {
        // 이전 섹션으로 이동 했다면 현 섹션의 마지막 아이템으로 설정(looping)
        csw = lastSectionItem(section);
        if (csw) {
            retIndex = mCsws.indexOf(csw);
        }
    }

    return csw;
}

void clearCswContainer(QList<Csw*>& container, bool deleteUndeletable)
{
    auto iter = container.begin();
    for ( ; iter != container.end(); ) {
        auto csw = *iter;
        if (deleteUndeletable || csw->deletable()) {
            iter = container.erase(iter);
        } else {
            ++iter;
        }
    }
}
void CswManager::clear(bool deleteUndeletable)
{
    clearCswContainer(mCsws, deleteUndeletable);
    clearCswContainer(mBlockedCsws, deleteUndeletable);
    mCurrent = nullptr;
    mCurrentIndex = -1;
    mCurrentSection = -1;
    timer.stop();
    updateSectionMap();
}

void CswManager::insert(Csw *csw)
{
    if (csw == nullptr) {
        qDebug() << "CswManager::insert(nullptr)";
        return;
    }
    if (mCsws.contains(csw)) {
        qDebug() << "CswManager::insert("
                 << csw->objectName()
                 << ", sec:"
                 << csw->section()
                 << ", seq:"
                 << csw->sequence()
                 << ") is already registered.";
        return;
    }
    connectTo(csw);

    for (int i=0; i<mCsws.count(); i++) {
        auto item = mCsws.at(i);
        if (*csw < *item) {
            mCsws.insert(i, csw);
            updateSectionMap();
            return;
        }
    }

    mCsws.append(csw);
    updateSectionMap();
}

void CswManager::remove(Csw *csw)
{
    auto index = mCsws.indexOf(csw);
    if (0 <= index) {
        mCsws.removeAt(index);
        updateSectionMap();
    } else {
        index = mBlockedCsws.indexOf(csw);
        if (0 <= index) {
            mBlockedCsws.removeAt(index);
        }
    }
}

bool removeSectionCswContainer(QList<Csw*>& container, int section) {
    bool hasUndeletable = false;

    for (int i=container.count()-1; 0<=i; --i) {
        auto csw = container.at(i);
        if (csw->section() == section) {
            if (csw->deletable()) {
                container.removeAt(i);
            } else {
                hasUndeletable = true;
            }
        }
    }

    return hasUndeletable;
}
void CswManager::removeSection(int section)
{
    if (mCurrentSection == section) {
        setLostFocus(mCurrent);
    }

    auto hasUndeletable = removeSectionCswContainer(mCsws, section);
    auto blockedCswHasUndeletagle = removeSectionCswContainer(mBlockedCsws, section);

    if (!hasUndeletable) {
        mSectionMap.remove(section);
    } else {
        updateSectionMap();
        if (mCurrentSection == Csw::Section_Softkey) {
            mCurrent = mSectionMap[Csw::Section_Softkey];
            auto sequence = mCurrent->sequence();
            mCurrentIndex = getCurrentIndex(mCurrentSection, sequence);
        }
    }
}

void CswManager::removeSoftekyMenu()
{
    auto section = Csw::Section_Softkey;
    if (mCurrentSection == section) {
        setLostFocus(mCurrent);
    }
    for (int i=mCsws.count()-1; 0<=i; --i) {
        auto csw = mCsws.at(i);
        if (csw->section() == section) {
            auto seq = csw->sequence();
            switch (seq) {
            case Csw::Sequence_Back:
            case Csw::Sequence_Navi:
            case Csw::Sequence_Option:
            case Csw::Sequence_ToggleMenu:
                break;
            default:
                if (Csw::Sequence_SoftkeyMenuBase <= seq && seq < Csw::Sequence_SoftMenuEnd) {
                    mCsws.removeAt(i);
                } else {
                    qDebug() << "Invalid softkey csw(" << seq << ")";
                }
                break;
            }
        }
    }
}

void CswManager::reset(int section, int sequence)
{
    timer.stop();

    setLostFocus(mCurrent, true);
    mCurrent = nullptr;
    mCurrentIndex = section >= 0 ? getCurrentIndex(section, sequence) : -1;
    mCurrentSection = section;
    updateSectionMap();
}

void CswManager::setFocus(Csw *csw)
{
    if (csw)
        QMetaObject::invokeMethod(csw, "focused");
}

void CswManager::setLostFocus(bool bySection)
{
    if (mCurrent) {
        setLostFocus(mCurrent, bySection);
    }
}

void CswManager::setLostFocus(Csw *csw, bool bySection)
{
    if (csw)
        QMetaObject::invokeMethod(csw, "lostFocusd", Q_ARG(bool, bySection));
}

void CswManager::setSelect(Csw *csw)
{
    if (csw)
        QMetaObject::invokeMethod(csw, "selected");
    timer.start();
}

void CswManager::setLeftPressed(Csw *csw)
{
    if (csw)
        QMetaObject::invokeMethod(csw, "leftPressed");
}

void CswManager::setRightPressed(Csw *csw)
{
    if (csw)
        QMetaObject::invokeMethod(csw, "rightPressed");
}

void CswManager::setSpinCcwPressed(Csw *csw)
{
    if (csw)
        QMetaObject::invokeMethod(csw, "spinCcwPressed");
}

void CswManager::setSpinCwPressed(Csw *csw)
{
    if (csw)
        QMetaObject::invokeMethod(csw, "spinCwPressed");
}

void CswManager::setCurrent(Csw *csw)
{
    if (mCurrent == csw)
        return;

    if (mCurrent)
        setLostFocus(mCurrent, mCurrent->isDifferentSection(csw));

    mCurrent = csw;
    mCurrentIndex = mCsws.indexOf(csw);
    mCurrentSection = csw->section();

    setFocus(mCurrent);
}

void CswManager::setCurrent(const QString &objectName)
{
    if (objectName.isEmpty())
        return;

    for (auto csw : mCsws) {
        if (csw->objectName() == objectName) {
            setCurrent(csw);
            break;
        }
    }
}

void CswManager::blockSection(int section, bool block)
{
    if (block) {
        auto iter = mCsws.begin();
        for ( ; iter != mCsws.end(); ) {
            auto csw = *iter;
            if (csw->section() == section) {
                mBlockedCsws.push_back(csw);
                iter = mCsws.erase(iter);
            } else {
                ++iter;
            }
        }
    } else {
        auto iter = mBlockedCsws.begin();
        for ( ; iter != mBlockedCsws.end(); ) {
            auto csw = *iter;
            if (csw->section() == section) {
                insert(csw);
                iter = mBlockedCsws.erase(iter);
            } else {
                ++iter;
            }
        }
    }
}

bool CswManager::hasDialogCsw() const
{
    for (auto csw : mCsws) {
        if (csw->section() == Csw::Section_Dialog || csw->section() == Csw::Section_DialogBottom)
            return true;
    }
    return false;
}

void CswManager::resetTimer()
{
    timer.start();
}

void CswManager::updateDebugInfo(QVariantMap &m)
{
    auto isValid = mCsws.contains(mCurrent);
    m["section"] = mCurrentSection;
    m["index"] = mCurrentIndex;
    m["valid"] = isValid;
    m["count"] = mCsws.count();
    if (mCurrent && isValid) {
        m["sequence"] = mCurrent->sequence();
        m["objectName"] = mCurrent->objectName();
    }
}

void CswManager::selectDefaultSoftkeyMenu()
{
    if (mSectionMap.contains(Csw::Section_Softkey)) {
        auto csw = mSectionMap[Csw::Section_Softkey];
        Csw *candidate = nullptr;
        auto index = mCsws.indexOf(csw);
        for ( ; index<mCsws.length(); index++) {
            csw = mCsws.at(index);
            if (csw->section() != Csw::Section_Softkey) {
                break;
            }
            auto seq = csw->sequence();
            if (seq == Csw::Sequence_Back ||
                seq == Csw::Sequence_Navi ||
                seq == Csw::Sequence_Option)
                continue;
            if (csw->enable() && csw->isDefault()) {
                return;
            }
            if (candidate == nullptr && csw->enable()) {
                candidate = csw;
            }
        }
        if (candidate) {
            candidate->setDefault(true);
        }
    }
}

Csw *CswManager::currentCsw() const
{
    return mCurrent;
}

bool CswManager::movePrevSection()
{
    if (defaultKeyProcess())
        return true;

    resetTimer();

    if (mCurrentSection == Csw::Section_Dialog) {
        return true;
    }
    // Qml내에서 onKeyFunction를 처리하면 더 이상 진행하지 않음.
    // 맵 이동 처리에서 사용
    if (keyProcess(mCurrent, Csw::Up)) {
        return true;
    }
    // 섹션 이동 금지가 설정 되어 있으면 리턴
    // 경유지 이동시 사용
    if (mCurrent && mCurrent->isFixSection()) {
        return true;
    }
    auto section = prevSection();
    if (mCurrentSection != section) {
        mCurrentSection = section;

        if (mCurrent)
            setLostFocus(mCurrent, true);

        mCurrent = mSectionMap[mCurrentSection];
        mCurrentIndex = mCsws.indexOf(mCurrent);

        if (!mCurrent->enable()) {
            int index = mCurrentIndex;
            mCurrent = spinCwItem(mCurrentSection, index);

            mCurrentIndex = mCurrent ? index : -1;
        }
        setFocus(mCurrent);

        return true;
    } else {
        return false;
    }
}

void CswManager::moveNextSectionByForce()
{
    Csw* csw = qobject_cast<Csw*>(sender());
    if (csw && csw->enableKeyProcess()) {
        // forceNextSection을 요청한 Csw의 다음 섹션으로 이동해야 하기 위해선
        // mCurrent가 nullptr일 경우가 있으므로 요청한 Csw를 current로 먼저 만들어 준다.
        setCurrent(csw);
    }
    bool byForce = true;
    moveNextSection(byForce);
}

bool CswManager::moveNextSection(bool byForce)
{
    if (defaultKeyProcess())
        return true;

    resetTimer();

    auto section = nextSection();
    if (section < 0 || (hasDialogCsw() && Csw::Section_DialogBottom < section))
        return true;

    // Qml내에서 onKeyFunction를 처리하면 더 이상 진행하지 않음.
    // 맵 이동 처리에서 사용
    if (!byForce && keyProcess(mCurrent, Csw::Down)) {
        return true;
    }
    // 섹션 이동 금지가 설정 되어 있으면 리턴
    // 경유지 이동시 사용
    if (!byForce && mCurrent && mCurrent->isFixSection()) {
        return true;
    }
    if (mCurrentSection != section) {
        Csw *csw = nullptr;

        if (Csw::Section_Softkey == section) {
            csw = defaultSectionItem(section);
#if (0)
            if (csw) {
                int startMenuSequence = 0;//2;
                csw = getItem(section, startMenuSequence);
                if (csw) {
                    if (!csw->enable())
                        csw = spinCwItem(section, startMenuSequence);
                    if (csw && csw->enable()) {
                        if (csw->objectName() == "option")
                            csw = nullptr;
                    }
                }
            }
#endif
        }

        if (!csw) {
            csw = mSectionMap[section];
            int index = mCsws.indexOf(csw);

            if (!csw->enable()) {
                csw = spinCwItem(section, index);
            }
        }

        if (csw) {
            setLostFocus(mCurrent);
            mCurrent = csw;
            mCurrentIndex = mCsws.indexOf(csw);
            mCurrentSection = section;

            setFocus(mCurrent);
        }
        return true;
    } else {
        return false;
    }
    //timer.start();
}

void CswManager::moveLeft()
{
    if (defaultKeyProcess())
        return;

    resetTimer();

    if (keyProcess(mCurrent, Csw::Left)) {
        return;
    }
    setLeftPressed(mCurrent);
    if (mCurrent) {
        Csw *csw = mCurrent->left();
        while (csw && !csw->enable()) {
            csw = csw->left();
        }
        if (csw) {
            setLostFocus(mCurrent);
            mCurrent = csw;
            setFocus(mCurrent);
        }
    }
}

void CswManager::moveRight()
{
    if (defaultKeyProcess())
        return;

    resetTimer();

    if (keyProcess(mCurrent, Csw::Right)) {
        return;
    }
    setRightPressed(mCurrent);
    if (mCurrent) {
        Csw *csw = mCurrent->right();
        while (csw && !csw->enable())
            csw = csw->right();

        if (csw) {
            setLostFocus(mCurrent);
            mCurrent = csw;
            setFocus(mCurrent);
        }
    }
}

void CswManager::spinCcw()
{
    if (defaultKeyProcess())
        return;

    resetTimer();

    if (keyProcess(mCurrent, Csw::SpinCcw)) {
        return;
    }
    setSpinCcwPressed(mCurrent);

    int index = mCurrentIndex;
    Csw *csw = spinCcwItem(mCurrentSection, index);
    if (csw) {
        setCurrent(csw);
    }
}

void CswManager::spinCw()
{
    if (defaultKeyProcess())
        return;

    resetTimer();

    if (keyProcess(mCurrent, Csw::SpinCw)) {
        return;
    }
    setSpinCwPressed(mCurrent);

    int index = mCurrentIndex;
    Csw *csw = spinCwItem(mCurrentSection, index);
    if (csw) {
        setCurrent(csw);
    }
}

void CswManager::select()
{
    if (defaultKeyProcess())
        return;
    if (keyProcess(mCurrent, Csw::Select)) {
        return;
    }
    if (mCurrent)
        setSelect(mCurrent);
}

void CswManager::goBack()
{
    if (keyProcess(mCurrent, Csw::Back)) {
        return;
    }
}

void CswManager::goHome()
{
}

void CswManager::goOption()
{
    if (keyProcess(mCurrent, Csw::Option)) {
        return;
    }
}

void CswManager::lostFocus()
{
    if (mCurrent) {
        bool bySection = false;
        setLostFocus(mCurrent, bySection);
    }

    mCurrentSection = -1;
    mCurrentIndex = -1;
    mCurrent = nullptr;
}

void CswManager::selectByObjectName(const QString &objectName, int extra)
{
    Csw *current = mCurrent;
    for (Csw *csw : mCsws) {
        if (csw->objectName() == objectName) {
            if (current != csw) {
                csw->setExtra(extra);
                setCurrent(csw);
            }
            break;
        }
    }
}

void CswManager::selectBySequence(int section, int sequence, int extra)
{
    if (mSectionMap.contains(section)) {
        Csw *csw = mSectionMap[section];
        auto index = mCsws.indexOf(csw);
        auto count = mCsws.count();

        do {
            if (sequence <= csw->sequence()) {
                if (csw->enable()) {
                    csw->setExtra(extra);
                    setCurrent(csw);
                    break;
                }
            }
            ++index;
            csw = mCsws.at(index);

            if (csw->section() != section)
                break;
        } while (index < count);
    }
}

} //SKT
