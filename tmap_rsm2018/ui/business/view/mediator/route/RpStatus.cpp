#include "RpStatus.h"

namespace SKT
{

RpStatus::RpStatus()
{
    init();
}

void RpStatus::init()
{
    for (int i=0; i<3; i++)
    {
        mData[i].status = Ready;
        mData[i].option = 0;
        mData[i].isServerData = false;
    }
}

void RpStatus::reset()
{
    init();
}

void RpStatus::setOptions(QList<int> options)
{
    auto i = 0;
    for (auto option : options) {
        mData[i].option = option;
        i++;
    }
}

int RpStatus::option(int index) const
{
    Q_ASSERT(0<=index && index<optionCount());

    return mData[index].option;
}

int RpStatus::optionIndex(TeRouteOption option) const
{
    for (auto i=0; i<optionCount(); i++) {
        if (mData[i].option == option) {
            return i;
        }
    }
    return -1;
}

int RpStatus::optionCount() const
{
    int count = 0;
    for (auto i=0; i<3; i++) {
        if (0 < mData[i].option) {
            ++count;
        }
    }
    return count;
}

// 코드 내용 다시한번 검토
// 18.12.03
int RpStatus::routeIdByOption(TeRouteOption option) const
{
    int index = -1;
    for (auto i=0; i<optionCount(); i++) {
        if (mData[i].status == Done) {
            ++index;
        }
        if (mData[i].option == option) {
            return index;
        }
    }
    return -1;
}

int RpStatus::routeIdByIndex(int index) const
{
    int newIndex = -1;
    for (auto i=0; i<=index; i++) {
        if (mData[i].status == Done)
            ++newIndex;
    }
    return newIndex;
}

int RpStatus::doneCount() const
{
    int count = 0;
    for (int i=0; i<3; i++) {
        if (mData[i].status == Done)
            ++count;
    }
    return count;
}

void RpStatus::request(const QList<TeRouteOption> &options, bool isServer)
{
    for (auto i=0; i<optionCount(); i++) {
        auto option = static_cast<TeRouteOption>(mData[i].option);
        if (options.contains(option)) {
            if (mData[i].status == Ready) {
                mData[i].status = Request;
                mData[i].isServerData = isServer;
            }
        } else {
            qDebug() << "RpStatus::request() mData[" << i << "].status != Ready, " << toString(mData[i].status);
        }
    }
}

void RpStatus::updateRpDone(QStringListPtr ptr)
{
    auto filenames = *ptr.data();
    for (auto filename : filenames) {
        auto ch = filename.at(10);
        auto number = ch.digitValue();
        if (0 <= number) {
            auto index = number - 1;
            if (mData[index].status == Request) {
                mData[index].status = Done;
            } else {
                qDebug() << "mData[" << index << "].status = " << toString(mData[index].status);
            }
        } else {
            qDebug() << "invalid digit value : " << filename << "[10] = " << ch;
        }
    }
}

void RpStatus::updateRpDone(TeRouteOption option)
{
    auto done = false;
    for (auto i=0; i<3; i++) {
        if (mData[i].option == option) {
            mData[i].status = Done;
            done = true;
            break;
        }
    }
    if (!done) {
        qDebug() << "updateRpDone(" << option << ") : failed.";
    }
}

QString RpStatus::toString(int status) const
{
    switch (status) {
    case Ready: return "Ready";
    case Request: return "Request";
    case Done: return "Done";
    }
    return "Invalid";
}

QVariantList RpStatus::getTvasDataList(bool isOnline) const
{
    QVariantList ret;
    bool swapOption = isSwapOption(isOnline);

    for (auto i=0; i<optionCount(); i++) {
        if (mData[i].status == Done) {
            auto i2 = i;
            if (swapOption) {
                switch (i2) {
                case 1: i2 = 2; break;
                case 2: i2 = 1; break;
                default:
                    break;
                }
            }
            auto option = mData[i].option;
            ret.append(option);
            auto index = i2 + 1;
            ret.append(QString("tvas_route%1.db").arg(index));
        }
    }
    return ret;
}

QStringList RpStatus::getRouteColorList(bool isOnline) const
{
    QStringList ret;
    auto swapOption = isSwapOption(isOnline);

    for (auto i=0; i<3; i++) {
        if (mData[i].status == Done) {
            auto i2 = i;
            if (swapOption) {
                switch (i2) {
                case 1: i2 = 2; break;
                case 2: i2 = 1; break;
                default:
                    break;
                }
            }
            switch (i2) {
            case 0: ret.append("routeRed");     break;
            case 1: ret.append("routeBlue");    break;
            case 2: ret.append("routeGreen");   break;
            }
        }
    }

    return ret;
}

bool RpStatus::isValidRouteIndex(int routeId) const
{
    if (routeId < 0) {
        qDebug() << "isValidRouteIndex(" << routeId << ") fail.";
        return false;
    }

    if (optionCount() <= routeId)
        return false;

    return mData[routeId].status == Done;
}

QString RpStatus::getTvasFilename(int routeId, bool isOnline) const
{
    Q_UNUSED(isOnline)

    int number = 1;
    auto opt1 = mData[1].option;
    auto opt2 = mData[2].option;
    auto optionOrdered = opt1 < opt2;
    auto isSwap = !optionOrdered/* && isOnline*/;

    switch (routeId) {
    case 1:
        if (isSwap)
            number = 3;
        else
            number = 2;
        break;

    case 2:
        if (isSwap)
            number = 2;
        else
            number = 3;
        break;
    }
    return QString("tvas_route%1.db").arg(number);
}

bool RpStatus::isServerData(int index) const
{
    return mData[index].isServerData;
}

bool RpStatus::hasRequest() const
{
    for (int i=0; i<3; i++) {
        if (mData[i].status == Request) {
            return true;
        }
    }
    return false;
}

bool RpStatus::isSwapOption(bool isOnline) const
{
    Q_UNUSED(isOnline)

    bool swapOption = false;
    if (/*isOnline && */3 == optionCount()) {
        auto opt1 = mData[1].option;
        auto opt2 = mData[2].option;
        swapOption = opt1 > opt2;
    }
    return swapOption;
}

void RpStatus::switchToLocal()
{
    init();
    mData[0].status = Done;
}

}//SKT
