#ifndef MULTISENSEMANAGER_H
#define MULTISENSEMANAGER_H

namespace SKT {
	
class MultiSenseManager : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(int colorState READ colorState WRITE setColorState NOTIFY colorStateChanged)
    explicit MultiSenseManager(QObject *parent = nullptr);

    enum ColorState {
        Color_Unavailable = 0,
        Color_1,
        Color_2,
        Color_3,
        Color_4,
        Color_5,
        Color_6,
        Color_7,
        Color_8,
        Color_9,
        Color_10,
        Color_11,
        Color_12,
        Color_13,
        Color_14,
        Color_15,
    };
    enum MexMode {
        MEX_MODE_Unavailable = 0,
        MEX_MODE_ECO,
        MEX_MODE_COMFORT,
        MEX_MODE_NORMAL,
        MEX_MODE_DYNAMIC,
        MEX_MODE_PERSONAL,
        MEX_MODE_RACE,
        MEX_MODE_SNOW
    };

signals:
    void colorStateChanged();

public slots:
public:
    int colorState();
    void setColorState(int colorState);
    void setColorStateFromHiddenMenu(int color);
    QColor color(int colorState) const;

private:
    int mColorState = Color_Unavailable;
    int mMexMode = MEX_MODE_Unavailable;
};

}

#endif // MULTISENSEMANAGER_H
