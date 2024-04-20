#include "Csw.h"
#include <QtQml/QQmlComponent>

namespace SKT
{

Csw::Csw(QObject *object)
    : QObject(object)
    , mSection(0)
    , mSequence(-1)
    , mLeft(nullptr)
    , mRight(nullptr)
    , mEnable(true)
    , mDeletable(true)
    , mExtra(0)
{
}

Csw::Csw(const Csw &other)
{
    if (this != &other) {
        copyFrom(other);
    }
}

Csw &Csw::operator=(const Csw &other)
{
    if (this != &other) {
        copyFrom(other);
    }
    return *this;
}

Csw::~Csw()
{
    emit destroyed(this);
}

bool Csw::operator<(const Csw &other)
{
    if (mSection < other.mSection)
        return true;
    else if (mSection > other.mSection)
        return false;

    return mSequence < other.mSequence;
}

bool Csw::operator==(const Csw &other)
{
    return mSequence == other.mSequence && mSection == other.mSection;
}

void Csw::copyFrom(const Csw &other)
{
    mEnable = other.mEnable;
    mSection = other.mSection;
    mSequence = other.mSequence;
    mLeft = other.mLeft;
    mRight = other.mRight;
}

bool Csw::enable() const
{
    return mEnable;
}

void Csw::setEnable(bool enable)
{
    if (mEnable != enable) {
        mEnable = enable;
        emit enableChanged();
    }
}

bool Csw::deletable() const
{
    return mDeletable;
}

void Csw::setDeletable(bool b)
{
    if (mDeletable != b) {
        mDeletable = b;
        emit deletableChanged();
    }
}

int Csw::section() const
{
    return mSection;
}

void Csw::setSection(int section)
{
    if (mSection != section) {
        mSection = section;
        emit sectionChanged();
    }
}

int Csw::sequence() const
{
    return mSequence;
}

void Csw::setSequence(int sequence)
{
    if (mSequence != sequence) {
        mSequence = sequence;
        emit sequenceChanged();
    }
}

void Csw::setLeft(Csw *left)
{
    if (left != mLeft) {
        mLeft= left;
        emit leftChanged();
    }
}

Csw* Csw::left() const
{
    return mLeft;
}

void Csw::setRight(Csw *right)
{
    if (right != mRight) {
        mRight = right;
        emit rightChanged();
    }
}

Csw *Csw::right() const
{
    return mRight;
}

bool Csw::isSameSection(Csw *csw)
{
    return mSection == csw->section();
}

bool Csw::isDifferentSection(Csw *csw)
{
    return !isSameSection(csw);
}

bool Csw::enableKeyProcess() const
{
    return mEnableKeyProcess;
}

void Csw::setEnableKeyProcess(bool b)
{
    if (mEnableKeyProcess != b) {
        mEnableKeyProcess = b;
        emit enableKeyProcessChanged();
    }
}

bool Csw::isDefault() const
{
    return mDefault;
}

void Csw::setDefault(bool b)
{
    if (mDefault != b) {
        mDefault = b;
        emit defaultChanged();
    }
}

bool Csw::isFixSection() const
{
    return mFixSection;
}

void Csw::setFixSection(bool b)
{
    if (mFixSection != b) {
        mFixSection = b;
        emit fixSectionChanged();
    }
}

void Csw::setExtra(int extra)
{
    if (mExtra != extra) {
        mExtra = extra;
        emit extraChanged();
    }
}

int Csw::extra() const
{
    return mExtra;
}

}
