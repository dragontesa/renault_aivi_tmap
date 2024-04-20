#include "zone3helper.h"

namespace SKT
{

void Zone3Helper::setupForShowKeyboard(ContentInfo &ci, const QString& text, int keypadFormat)
{
    Content c0;
    c0.m_nodeIndex = 0;
    c0.m_bgInfo.m_bgBasePath = "";
    c0.m_bgInfo.m_bgColor = NodeBgColor_DEFAULT;
    c0.m_bgInfo.m_textColor = WHITE;
    c0.m_bgInfo.m_textEffect = NOSHADOW;
    c0.m_nodeType = (keypadFormat == NUMERIC) ? SPELLER : NodeType_DEFAULT;
    c0.m_iconPath = "";
    c0.m_buttonState = NORMAL;
    textInfoList til;
    textInfo ti;
    ti.m_textId = 0;
    ti.m_textStr = "";
    ti.m_type = true;
    til.append(ti);
    c0.m_textData.append(til);
    c0.m_speedRestriction.m_speedRestrictionCategory = SRCATEGORY_INVALID;
    c0.m_speedRestriction.m_speedRestrictionBehavior = 255;
    c0.m_editfieldMode = FREEMODE;
    c0.m_sdsType = LISTENING;
    c0.m_floatValue = 0;
    c0.m_nodeLayoutType = NodeLayoutType_INVALID;
    c0.m_keypadFormat = keypadFormat;
    c0.m_inwardExitAnimationStatus = true;
    c0.m_outwardExitAnimationStatus = true;
    c0.m_inwardEntryAnimationStatus = true;
    c0.m_outwardEntryAnimationStatus = true;

    til.clear();

    Content c1;
    c1.m_nodeIndex = 4294967295;
    c1.m_bgInfo.m_bgBasePath = "";
    c1.m_bgInfo.m_bgColor = NodeBgColor_DEFAULT;
    c1.m_bgInfo.m_textColor = WHITE;
    c1.m_bgInfo.m_textEffect = NOSHADOW;
    c1.m_nodeType = ROOT;
    c1.m_iconPath = "";
    c1.m_buttonState = NORMAL;
    ti.m_textId = 0;
    ti.m_textStr = text;
    ti.m_type = false;
    til.append(ti);
    c1.m_textData.append(til);
    c1.m_speedRestriction.m_speedRestrictionCategory = SRCATEGORY_INVALID;
    c1.m_speedRestriction.m_speedRestrictionBehavior = 255;
    c1.m_editfieldMode = FREEMODE;
    c1.m_sdsType = LISTENING;
    c1.m_floatValue = 0;
    c1.m_nodeLayoutType = NodeLayoutType_INVALID;
    c1.m_keypadFormat = keypadFormat;
    c1.m_inwardExitAnimationStatus = true;
    c1.m_outwardExitAnimationStatus = true;
    c1.m_inwardEntryAnimationStatus = true;
    c1.m_outwardEntryAnimationStatus = true;

    // Bosch guide for numeric keyboard setting
    // 10.05.22 ymhong
    if (keypadFormat == NUMERIC) {
        ci.append(c1);
        ci.append(c0);
    } else {
        ci.append(c0);
        ci.append(c1);
    }
}

void Zone3Helper::setupForHideKeyboard(ContentInfo &ci)
{
    Content c0;
    c0.m_nodeIndex = 4294967295;
    c0.m_bgInfo.m_bgBasePath = "";
    c0.m_bgInfo.m_bgColor = TRANSPARENT;
    c0.m_bgInfo.m_textColor = WHITE;
    c0.m_bgInfo.m_textEffect = NOSHADOW;
    c0.m_nodeType = Z3_BACKGROUND;
    c0.m_iconPath = "";
    c0.m_buttonState = NORMAL;
    textInfoList til;
    textInfo ti;
    ti.m_textId = 0;
    ti.m_textStr = "";
    ti.m_type = false;
    til.append(ti);
    c0.m_textData.append(til);
    c0.m_speedRestriction.m_speedRestrictionCategory = SRCATEGORY_INVALID;
    c0.m_speedRestriction.m_speedRestrictionBehavior = 255;
    c0.m_editfieldMode = FREEMODE;
    c0.m_sdsType = LISTENING;
    c0.m_floatValue = 0;
    c0.m_nodeLayoutType = NodeLayoutType_INVALID;
    c0.m_keypadFormat = 1;
    c0.m_inwardExitAnimationStatus = true;
    c0.m_outwardExitAnimationStatus = true;
    c0.m_inwardEntryAnimationStatus = true;
    c0.m_outwardEntryAnimationStatus = true;
    ci.append(c0);
}

}
