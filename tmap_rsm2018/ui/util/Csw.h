#ifndef CSW_H
#define CSW_H

#include <QObject>
#include <QtQuick/QQuickItem>

namespace SKT
{

class Csw : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enable READ enable WRITE setEnable NOTIFY enableChanged)
    Q_PROPERTY(int section READ section WRITE setSection NOTIFY sectionChanged)
    Q_PROPERTY(int sequence READ sequence WRITE setSequence NOTIFY sequenceChanged)
    Q_PROPERTY(Csw *left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(Csw *right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(bool deletable READ deletable WRITE setDeletable NOTIFY deletableChanged)
    Q_PROPERTY(bool enableKeyProcess READ enableKeyProcess WRITE setEnableKeyProcess NOTIFY enableKeyProcessChanged)
    Q_PROPERTY(bool isDefault READ isDefault WRITE setDefault NOTIFY defaultChanged)
    Q_PROPERTY(bool isFixSection READ isFixSection WRITE setFixSection NOTIFY fixSectionChanged)
    Q_PROPERTY(int extra READ extra WRITE setExtra NOTIFY extraChanged)

    Q_ENUMS(Section)
    Q_ENUMS(Sequence)
    Q_ENUMS(MapViewSequence)
    Q_ENUMS(Key)

public:
    Csw(QObject *object = 0);
    Csw(const Csw &);
    Csw &operator=(const Csw &);
    ~Csw();

    enum Section {
        Section_Dialog = 0,
        Section_DialogBottom,
        Section_IntegratedSearch,
        Section_GridMenu,
        Section_Tbt,
        Section_Hiway,
        Section_RouteCandidate,
        Section_RouteSetting,
        Section_RouteSettingControl,
        Section_RouteSummary,

        Section_Map,
        Section_MapControl,
        Section_MapCursor,

        Section_Setting,
        Section_AddressSearchSiDo,
        Section_NaviMenuGrid,
        Section_SimulationControl,
        Section_SearchResult,
        Section_Tab,
        Section_List,

        Section_Popup,
        Section_VolumeControl,
        Section_Ime = 0x10000,
        Section_Softkey,
        Section_Hardkey,

        Section_Test,
        Section_Test2,
        Section_Test3
    };

    enum Sequence {
        Sequence_Back,
        Sequence_Navi,
        Sequence_SoftkeyMenuBase = Sequence_Navi + 1,
        Sequence_SoftMenuEnd = 65535-2,
        Sequence_ToggleMenu = Sequence_SoftMenuEnd,
        Sequence_Option,
    };

    enum MapViewSequence {
        Sequence_Map = 0,
        Sequence_TbtArrow,
        Sequence_SummaryTbtArrow,
        Sequence_HightwayTbtDestPanel,
        Sequence_MapScalePlus,
        Sequence_MapScaleMinus,
        Sequence_MapScaleReturn,
        Sequence_Reroute,
        Sequence_SkipVia,
        Sequence_SimulationSpeed,
        Sequence_SimulationPause,
        Sequence_SimulationNext,
        Sequence_SimulationPrev,
        Sequence_Volume,
    };

    enum Key {
        Left = 0,
        Right,
        Up,
        Down,
        SpinCcw,
        SpinCw,
        VolumeUp,
        VolumeDown,
        Back,
        Option,
        Home,
        Select
    };

    bool operator<(const Csw &);
    bool operator==(const Csw &);

    void copyFrom(const Csw &);

    bool enable() const;
    void setEnable(bool);

    bool deletable() const;
    void setDeletable(bool);

    int section() const;
    void setSection(int);

    int sequence() const;
    void setSequence(int);

    void setLeft(Csw *);
    Csw* left() const;

    void setRight(Csw *);
    Csw *right() const;

    bool isSameSection(Csw *);
    bool isDifferentSection(Csw *);

    bool enableKeyProcess() const;
    void setEnableKeyProcess(bool);

    bool isDefault() const;
    void setDefault(bool);

    bool isFixSection() const;
    void setFixSection(bool);

    void setExtra(int extra);
    int extra() const;

signals:
    void enableChanged();
    void deletableChanged();
    void sectionChanged();
    void sequenceChanged();
    void leftChanged();
    void rightChanged();

    void focused();
    void lostFocusd(bool bySection);
    void selected();
    void leftPressed();
    void rightPressed();
    void spinCcwPressed();
    void spinCwPressed();

    void forceNextSection();
    void forceLostFocus();
    void selectByObjectName(const QString);
    void selectBySequence(int section, int sequence);
    void keyPressed(Key key);
    void keyProcessed(bool);
    void enableKeyProcessChanged();
    void defaultChanged();
    void fixSectionChanged();
    void extraChanged();

private:
    bool mEnable;
    bool mDeletable;
    bool mEnableKeyProcess = false;
    bool mDefault = false;
    bool mFixSection = false;
    int mSection;
    int mSequence;
    Csw *mLeft;
    Csw *mRight;
    int mExtra;
};

}

#endif // CSW_H
