#include "RgUtils.h"
#include "RgProtocol.h"
#include "VSM_Navi_RgData.h"
#include "RgConstants.h"

using namespace SKT;

QString RgUtils::tbtFirstImage( int code )
{
    static QMap<int,QString> FirstImgMap = QMap<int,QString>{
        {RgTbtCode::STR_RIGHT, QStringLiteral("tbt_1st_21.png")},
        {RgTbtCode::STR_LEFT, QStringLiteral("tbt_1st_20.png")},
        {RgTbtCode::TURN_STR, QStringLiteral("tbt_1st_01.png")},
        {RgTbtCode::TURN_STR_LEFT,QStringLiteral("tbt_1st_09.png")},
        {RgTbtCode::TURN_STR_RIGHT,QStringLiteral("tbt_1st_10.png")},
        {RgTbtCode::TURN_LEFT, QStringLiteral("tbt_1st_03.png")},
        {RgTbtCode::TURN_RIGHT, QStringLiteral("tbt_1st_02.png")},
        {RgTbtCode::TURN_UTURN, QStringLiteral("tbt_1st_06.png")},
        {RgTbtCode::TURN_PTURN, QStringLiteral("tbt_1st_11.png")},
        {RgTbtCode::TURN_LEFT_8, QStringLiteral("tbt_1st_07.png")},
        {RgTbtCode::TURN_LEFT_10, QStringLiteral("tbt_1st_05.png")},
        {RgTbtCode::TURN_RIGNT_2, QStringLiteral("tbt_1st_04.png")},
        {RgTbtCode::TURN_RIGNT_4, QStringLiteral("tbt_1st_08.png")},
        {RgTbtCode::LANE_STR, QStringLiteral("tbt_1st_01.png")},
        {RgTbtCode::LANE_LEFT, QStringLiteral("tbt_1st_01.png")},
        {RgTbtCode::LANE_RIGHT, QStringLiteral("tbt_1st_01.png")},
        {RgTbtCode::LANE_1, QStringLiteral("tbt_1st_01.png")},
        {RgTbtCode::LANE_2, QStringLiteral("tbt_1st_01.png")},
        {RgTbtCode::FIRST_LANE_RIGHT, QStringLiteral("tbt_1st_21.png")},
        {RgTbtCode::SEC_LANE_RIGHT, QStringLiteral("tbt_1st_21.png")},
        {RgTbtCode::FIRST_LANE_LEFT, QStringLiteral("tbt_1st_20.png")},
        {RgTbtCode::SEC_LANE_LEFT, QStringLiteral("tbt_1st_20.png")},
        {RgTbtCode::EXP_IN_RIGHT, QStringLiteral("tbt_1st_16.png")},
        {RgTbtCode::EXP_IN_LEFT, QStringLiteral("tbt_1st_14.png")},
        {RgTbtCode::EXP_IN_STR, QStringLiteral("tbt_1st_12.png")},
        {RgTbtCode::EXP_OUT_RIGHT, QStringLiteral("tbt_1st_17.png")},
        {RgTbtCode::EXP_OUT_LEFT, QStringLiteral("tbt_1st_15.png")},
        {RgTbtCode::EXP_OUT_STR, QStringLiteral("tbt_1st_13.png")},
        {RgTbtCode::SAEXP_IN_RIGHT, QStringLiteral("tbt_1st_16.png")},
        {RgTbtCode::SAEXP_IN_LEFT, QStringLiteral("tbt_1st_14.png")},
        {RgTbtCode::SAEXP_IN_STR, QStringLiteral("tbt_1st_12.png")},
        {RgTbtCode::SAEXP_OUT_RIGHT, QStringLiteral("tbt_1st_17.png")},
        {RgTbtCode::SAEXP_OUT_LEFT, QStringLiteral("tbt_1st_15.png")},
        {RgTbtCode::SAEXP_OUT_STR, QStringLiteral("tbt_1st_13.png")},
        {RgTbtCode::ROAD_RIGHT, QStringLiteral("tbt_1st_21.png")},
        {RgTbtCode::ROAD_LEFT, QStringLiteral("tbt_1st_20.png")},
        {RgTbtCode::UNDER_IN, QStringLiteral("tbt_1st_36.png")},
        {RgTbtCode::OVER_IN, QStringLiteral("tbt_1st_33.png")},
        {RgTbtCode::TUNNEL_IN, QStringLiteral("tbt_1st_29.png")},
        {RgTbtCode::UNDER_OUT, QStringLiteral("tbt_1st_34.png")},
        {RgTbtCode::OVER_OUT, QStringLiteral("tbt_1st_31.png")},
        {RgTbtCode::DIR_1, QStringLiteral("tbt_1st_40.png")},
        {RgTbtCode::DIR_2, QStringLiteral("tbt_1st_40.png")},
        {RgTbtCode::DIR_3, QStringLiteral("tbt_1st_37.png")},
        {RgTbtCode::DIR_4, QStringLiteral("tbt_1st_42.png")},
        {RgTbtCode::DIR_5, QStringLiteral("tbt_1st_42.png")},
        {RgTbtCode::DIR_6, QStringLiteral("tbt_1st_43.png")},
        {RgTbtCode::DIR_7, QStringLiteral("tbt_1st_46.png")},
        {RgTbtCode::DIR_8, QStringLiteral("tbt_1st_46.png")},
        {RgTbtCode::DIR_9, QStringLiteral("tbt_1st_44.png")},
        {RgTbtCode::DIR_10, QStringLiteral("tbt_1st_45.png")},
        {RgTbtCode::DIR_11, QStringLiteral("tbt_1st_45.png")},
        {RgTbtCode::DIR_12, QStringLiteral("tbt_1st_47.png")},
        {RgTbtCode::SA, QStringLiteral("tbt_1st_22.png")},
        {RgTbtCode::SA_IN, QStringLiteral("tbt_1st_22.png")},
        {RgTbtCode::TOL_EXP, QStringLiteral("tbt_1st_28.png")},
        {RgTbtCode::TOL_NORMAL, QStringLiteral("tbt_1st_28.png")},
        {RgTbtCode::START, QStringLiteral("tbt_1st_23.png")},
        {RgTbtCode::GOAL, QStringLiteral("tbt_1st_24.png")},
        {RgTbtCode::TEMP, QStringLiteral("tbt_1st_01.png")},
        {RgTbtCode::TOLLGATE, QStringLiteral("tbt_1st_28.png")},
        {RgTbtCode::VIA_1, QStringLiteral("tbt_1st_25.png")},
        {RgTbtCode::VIA_2, QStringLiteral("tbt_1st_26.png")},
        {RgTbtCode::VIA_3, QStringLiteral("tbt_1st_27.png")},
    };
	
	/* TODO hskim 17.10.19 아래 이미지 추가 검토 필요
        tbtGreenResourceMap.put(RGConstant.TbtCode.START, R.drawable.route_rg_bk_24); V
        tbtGreenResourceMap.put(RGConstant.TbtCode.VIA_1, R.drawable.route_rg_bk_25);
        tbtGreenResourceMap.put(RGConstant.TbtCode.VIA_2, R.drawable.route_rg_bk_26);
        tbtGreenResourceMap.put(RGConstant.TbtCode.GOAL, R.drawable.route_rg_bk_23); V
        tbtGreenResourceMap.put(RGConstant.TbtCode.TURN_NOT_SER_EXP, R.drawable.route_rg_gr_01);
        tbtGreenResourceMap.put(RGConstant.TbtCode.TURN_NOT_SER_NORMAL, R.drawable.route_rg_gr_01);
        tbtGreenResourceMap.put(RGConstant.TbtCode.TURN_NOT_SER_SPEC, R.drawable.route_rg_gr_01);	
	*/

    return FirstImgMap[code];
}

QString RgUtils::tbtSecondImage( int code )
{
    static QMap<int,QString> SecondImgMap = QMap<int,QString>{
        {RgTbtCode::STR_RIGHT, QStringLiteral("tbt_2nd_21.png")},
        {RgTbtCode::STR_LEFT, QStringLiteral("tbt_2nd_20.png")},
        {RgTbtCode::TURN_STR, QStringLiteral("tbt_2nd_01.png")},
        {RgTbtCode::TURN_LEFT, QStringLiteral("tbt_2nd_03.png")},
        {RgTbtCode::TURN_STR_LEFT,QStringLiteral("tbt_2nd_09.png")},
        {RgTbtCode::TURN_STR_RIGHT,QStringLiteral("tbt_2nd_10.png")},
        {RgTbtCode::TURN_RIGHT, QStringLiteral("tbt_2nd_02.png")},
        {RgTbtCode::TURN_UTURN, QStringLiteral("tbt_2nd_06.png")},
        {RgTbtCode::TURN_PTURN, QStringLiteral("tbt_2nd_11.png")},
        {RgTbtCode::TURN_LEFT_8, QStringLiteral("tbt_2nd_07.png")},
        {RgTbtCode::TURN_LEFT_10, QStringLiteral("tbt_2nd_05.png")},
        {RgTbtCode::TURN_RIGNT_2, QStringLiteral("tbt_2nd_04.png")},
        {RgTbtCode::TURN_RIGNT_4, QStringLiteral("tbt_2nd_08.png")},
        {RgTbtCode::LANE_STR, QStringLiteral("tbt_2nd_01.png")},
        {RgTbtCode::LANE_LEFT, QStringLiteral("tbt_2nd_01.png")},
        {RgTbtCode::LANE_RIGHT, QStringLiteral("tbt_2nd_01.png")},
        {RgTbtCode::LANE_1, QStringLiteral("tbt_2nd_01.png")},
        {RgTbtCode::LANE_2, QStringLiteral("tbt_2nd_01.png")},
        {RgTbtCode::FIRST_LANE_RIGHT, QStringLiteral("tbt_2nd_21.png")},
        {RgTbtCode::SEC_LANE_RIGHT, QStringLiteral("tbt_2nd_21.png")},
        {RgTbtCode::FIRST_LANE_LEFT, QStringLiteral("tbt_2nd_20.png")},
        {RgTbtCode::SEC_LANE_LEFT, QStringLiteral("tbt_2nd_20.png")},
        {RgTbtCode::EXP_IN_RIGHT, QStringLiteral("tbt_2nd_16.png")},
        {RgTbtCode::EXP_IN_LEFT, QStringLiteral("tbt_2nd_14.png")},
        {RgTbtCode::EXP_IN_STR, QStringLiteral("tbt_2nd_12.png")},
        {RgTbtCode::EXP_OUT_RIGHT, QStringLiteral("tbt_2nd_17.png")},
        {RgTbtCode::EXP_OUT_LEFT, QStringLiteral("tbt_2nd_15.png")},
        {RgTbtCode::EXP_OUT_STR, QStringLiteral("tbt_2nd_13.png")},
        {RgTbtCode::SAEXP_IN_RIGHT, QStringLiteral("tbt_2nd_16.png")},
        {RgTbtCode::SAEXP_IN_LEFT, QStringLiteral("tbt_2nd_14.png")},
        {RgTbtCode::SAEXP_IN_STR, QStringLiteral("tbt_2nd_12.png")},
        {RgTbtCode::SAEXP_OUT_RIGHT, QStringLiteral("tbt_2nd_17.png")},
        {RgTbtCode::SAEXP_OUT_LEFT, QStringLiteral("tbt_2nd_15.png")},
        {RgTbtCode::SAEXP_OUT_STR, QStringLiteral("tbt_2nd_13.png")},
        {RgTbtCode::ROAD_RIGHT, QStringLiteral("tbt_2nd_21.png")},
        {RgTbtCode::ROAD_LEFT, QStringLiteral("tbt_2nd_20.png")},
        {RgTbtCode::UNDER_IN, QStringLiteral("tbt_2nd_36.png")},
        {RgTbtCode::OVER_IN, QStringLiteral("tbt_2nd_33.png")},
        {RgTbtCode::TUNNEL_IN, QStringLiteral("tbt_2nd_29.png")},
        {RgTbtCode::UNDER_OUT, QStringLiteral("tbt_2nd_34.png")},
        {RgTbtCode::OVER_OUT, QStringLiteral("tbt_2nd_31.png")},
        {RgTbtCode::DIR_1, QStringLiteral("tbt_2nd_40.png")},
        {RgTbtCode::DIR_2, QStringLiteral("tbt_2nd_40.png")},
        {RgTbtCode::DIR_3, QStringLiteral("tbt_2nd_37.png")},
        {RgTbtCode::DIR_4, QStringLiteral("tbt_2nd_42.png")},
        {RgTbtCode::DIR_5, QStringLiteral("tbt_2nd_42.png")},
        {RgTbtCode::DIR_6, QStringLiteral("tbt_2nd_43.png")},
        {RgTbtCode::DIR_7, QStringLiteral("tbt_2nd_46.png")},
        {RgTbtCode::DIR_8, QStringLiteral("tbt_2nd_46.png")},
        {RgTbtCode::DIR_9, QStringLiteral("tbt_2nd_44.png")},
        {RgTbtCode::DIR_10, QStringLiteral("tbt_2nd_45.png")},
        {RgTbtCode::DIR_11, QStringLiteral("tbt_2nd_45.png")},
        {RgTbtCode::DIR_12, QStringLiteral("tbt_2nd_47.png")},
        {RgTbtCode::SA, QStringLiteral("tbt_2nd_22.png")},
        {RgTbtCode::SA_IN, QStringLiteral("tbt_2nd_22.png")},
        {RgTbtCode::TOL_EXP, QStringLiteral("tbt_2nd_28.png")},
        {RgTbtCode::TOL_NORMAL, QStringLiteral("tbt_2nd_28.png")},
        {RgTbtCode::START, QStringLiteral("tbt_2nd_23.png")},
        {RgTbtCode::GOAL, QStringLiteral("tbt_2nd_24.png")},
        {RgTbtCode::TEMP, QStringLiteral("tbt_2nd_01.png")},
        {RgTbtCode::TOLLGATE, QStringLiteral("tbt_2nd_28.png")},
        {RgTbtCode::VIA_1, QStringLiteral("tbt_2nd_25.png")},
        {RgTbtCode::VIA_2, QStringLiteral("tbt_2nd_26.png")},
        {RgTbtCode::VIA_3, QStringLiteral("tbt_2nd_27.png")},
    };

    return SecondImgMap[code];
}

QString RgUtils::tbtFirstDistanceNumberImage(QChar num)
{
    QString ret;

    switch ( num.toLatin1() ) {
    case '0':
        ret = QStringLiteral( "tbt_1st_num_0.png" );
        break;
    case '1':
        ret = QStringLiteral( "tbt_1st_num_1.png" );
        break;
    case '2':
        ret = QStringLiteral( "tbt_1st_num_2.png" );
        break;
    case '3':
        ret = QStringLiteral( "tbt_1st_num_3.png" );
        break;
    case '4':
        ret = QStringLiteral( "tbt_1st_num_4.png" );
        break;
    case '5':
        ret = QStringLiteral( "tbt_1st_num_5.png" );
        break;
    case '6':
        ret = QStringLiteral( "tbt_1st_num_6.png" );
        break;
    case '7':
        ret = QStringLiteral( "tbt_1st_num_7.png" );
        break;
    case '8':
        ret = QStringLiteral( "tbt_1st_num_8.png" );
        break;
    case '9':
        ret = QStringLiteral( "tbt_1st_num_9.png" );
        break;
    case 'k':
        ret = QStringLiteral( "tbt_1st_num_km.png" );
        break;
    case 'm':
        ret = QStringLiteral( "tbt_1st_num_m.png" );
        break;
    case '.':
        ret = QStringLiteral( "tbt_1st_num_dot.png" );
        break;
    }

    return ret;
}

QString RgUtils::tbtSecondDistanceNumberImage(QChar num)
{
    QString ret;

    switch ( num.toLatin1() ) {
    case '0':
        ret = QStringLiteral( "tbt_2nd_num_0.png" );
        break;
    case '1':
        ret = QStringLiteral( "tbt_2nd_num_1.png" );
        break;
    case '2':
        ret = QStringLiteral( "tbt_2nd_num_2.png" );
        break;
    case '3':
        ret = QStringLiteral( "tbt_2nd_num_3.png" );
        break;
    case '4':
        ret = QStringLiteral( "tbt_2nd_num_4.png" );
        break;
    case '5':
        ret = QStringLiteral( "tbt_2nd_num_5.png" );
        break;
    case '6':
        ret = QStringLiteral( "tbt_2nd_num_6.png" );
        break;
    case '7':
        ret = QStringLiteral( "tbt_2nd_num_7.png" );
        break;
    case '8':
        ret = QStringLiteral( "tbt_2nd_num_8.png" );
        break;
    case '9':
        ret = QStringLiteral( "tbt_2nd_num_9.png" );
        break;
    case 'k':
        ret = QStringLiteral( "tbt_2nd_num_km.png" );
        break;
    case 'm':
        ret = QStringLiteral( "tbt_2nd_num_m.png" );
        break;
    case '.':
        ret = QStringLiteral( "tbt_2nd_num_dot.png" );
        break;
    }

    return ret;
}

QString RgUtils::sdiSignNumberImage(QChar num)
{
    QString ret;

    switch ( num.toLatin1() ) {
    case '0':
        ret = QStringLiteral( "tmap_speed_sign_num_0.png" );
        break;
    case '1':
        ret = QStringLiteral( "tmap_speed_sign_num_1.png" );
        break;
    case '2':
        ret = QStringLiteral( "tmap_speed_sign_num_2.png" );
        break;
    case '3':
        ret = QStringLiteral( "tmap_speed_sign_num_3.png" );
        break;
    case '4':
        ret = QStringLiteral( "tmap_speed_sign_num_4.png" );
        break;
    case '5':
        ret = QStringLiteral( "tmap_speed_sign_num_5.png" );
        break;
    case '6':
        ret = QStringLiteral( "tmap_speed_sign_num_6.png" );
        break;
    case '7':
        ret = QStringLiteral( "tmap_speed_sign_num_7.png" );
        break;
    case '8':
        ret = QStringLiteral( "tmap_speed_sign_num_8.png" );
        break;
    case '9':
        ret = QStringLiteral( "tmap_speed_sign_num_9.png" );
        break;
    }

    return ret;
}

QString RgUtils::sdiSignNumberSmallImage(QChar num)
{
    QString ret;

    switch ( num.toLatin1() ) {
    case '0':
        ret = QStringLiteral( "tmap_speed_num_s_0.png" );
        break;
    case '1':
        ret = QStringLiteral( "tmap_speed_num_s_1.png" );
        break;
    case '2':
        ret = QStringLiteral( "tmap_speed_num_s_2.png" );
        break;
    case '3':
        ret = QStringLiteral( "tmap_speed_num_s_3.png" );
        break;
    case '4':
        ret = QStringLiteral( "tmap_speed_num_s_4.png" );
        break;
    case '5':
        ret = QStringLiteral( "tmap_speed_num_s_5.png" );
        break;
    case '6':
        ret = QStringLiteral( "tmap_speed_num_s_6.png" );
        break;
    case '7':
        ret = QStringLiteral( "tmap_speed_num_s_7.png" );
        break;
    case '8':
        ret = QStringLiteral( "tmap_speed_num_s_8.png" );
        break;
    case '9':
        ret = QStringLiteral( "tmap_speed_num_s_9.png" );
        break;
    }

    return ret;
}

QString RgUtils::sdiSectionSpeedAverageNumberImage(RgConstants::SdiSectionSpeedNumberType numberType, QChar num)
{
    QString ret;

    if ( numberType == RgConstants::SdiNormal ) {
        switch ( num.toLatin1() ) {
        case '0':
            ret = QStringLiteral( "tmap_speed_average_g_num_0.png" );
            break;
        case '1':
            ret = QStringLiteral( "tmap_speed_average_g_num_1.png" );
            break;
        case '2':
            ret = QStringLiteral( "tmap_speed_average_g_num_2.png" );
            break;
        case '3':
            ret = QStringLiteral( "tmap_speed_average_g_num_3.png" );
            break;
        case '4':
            ret = QStringLiteral( "tmap_speed_average_g_num_4.png" );
            break;
        case '5':
            ret = QStringLiteral( "tmap_speed_average_g_num_5.png" );
            break;
        case '6':
            ret = QStringLiteral( "tmap_speed_average_g_num_6.png" );
            break;
        case '7':
            ret = QStringLiteral( "tmap_speed_average_g_num_7.png" );
            break;
        case '8':
            ret = QStringLiteral( "tmap_speed_average_g_num_8.png" );
            break;
        case '9':
            ret = QStringLiteral( "tmap_speed_average_g_num_9.png" );
            break;
        }
    }
    else if ( numberType == RgConstants::SdiExcessive ) {
        switch ( num.toLatin1() ) {
        case '0':
            ret = QStringLiteral( "tmap_speed_average_r_num_0.png" );
            break;
        case '1':
            ret = QStringLiteral( "tmap_speed_average_r_num_1.png" );
            break;
        case '2':
            ret = QStringLiteral( "tmap_speed_average_r_num_2.png" );
            break;
        case '3':
            ret = QStringLiteral( "tmap_speed_average_r_num_3.png" );
            break;
        case '4':
            ret = QStringLiteral( "tmap_speed_average_r_num_4.png" );
            break;
        case '5':
            ret = QStringLiteral( "tmap_speed_average_r_num_5.png" );
            break;
        case '6':
            ret = QStringLiteral( "tmap_speed_average_r_num_6.png" );
            break;
        case '7':
            ret = QStringLiteral( "tmap_speed_average_r_num_7.png" );
            break;
        case '8':
            ret = QStringLiteral( "tmap_speed_average_r_num_8.png" );
            break;
        case '9':
            ret = QStringLiteral( "tmap_speed_average_r_num_9.png" );
            break;
        }
    }
    else if ( numberType == RgConstants::SdiWarning ) {
        switch ( num.toLatin1() ) {
        case '0':
            ret = QStringLiteral( "tmap_speed_average_y_num_0.png" );
            break;
        case '1':
            ret = QStringLiteral( "tmap_speed_average_y_num_1.png" );
            break;
        case '2':
            ret = QStringLiteral( "tmap_speed_average_y_num_2.png" );
            break;
        case '3':
            ret = QStringLiteral( "tmap_speed_average_y_num_3.png" );
            break;
        case '4':
            ret = QStringLiteral( "tmap_speed_average_y_num_4.png" );
            break;
        case '5':
            ret = QStringLiteral( "tmap_speed_average_y_num_5.png" );
            break;
        case '6':
            ret = QStringLiteral( "tmap_speed_average_y_num_6.png" );
            break;
        case '7':
            ret = QStringLiteral( "tmap_speed_average_y_num_7.png" );
            break;
        case '8':
            ret = QStringLiteral( "tmap_speed_average_y_num_8.png" );
            break;
        case '9':
            ret = QStringLiteral( "tmap_speed_average_y_num_9.png" );
            break;
        }
    }

    return ret;
}

QString RgUtils::sdiSectionSpeedTimeImage(RgConstants::SdiSectionSpeedNumberType numberType, QChar num)
{
    QString ret;

    if ( numberType == RgConstants::SdiNormal ) {
        switch ( num.toLatin1() ) {
        case '0':
            ret = QStringLiteral( "tmap_speed_average_g_num_0.png" );
            break;
        case '1':
            ret = QStringLiteral( "tmap_speed_average_g_num_1.png" );
            break;
        case '2':
            ret = QStringLiteral( "tmap_speed_average_g_num_2.png" );
            break;
        case '3':
            ret = QStringLiteral( "tmap_speed_average_g_num_3.png" );
            break;
        case '4':
            ret = QStringLiteral( "tmap_speed_average_g_num_4.png" );
            break;
        case '5':
            ret = QStringLiteral( "tmap_speed_average_g_num_5.png" );
            break;
        case '6':
            ret = QStringLiteral( "tmap_speed_average_g_num_6.png" );
            break;
        case '7':
            ret = QStringLiteral( "tmap_speed_average_g_num_7.png" );
            break;
        case '8':
            ret = QStringLiteral( "tmap_speed_average_g_num_8.png" );
            break;
        case '9':
            ret = QStringLiteral( "tmap_speed_average_g_num_9.png" );
            break;
        case ':':
            ret = QStringLiteral( "tmap_speed_average_g_dot.png" );
            break;
        }
    }
    else if ( numberType == RgConstants::SdiExcessive ) {
        switch ( num.toLatin1() ) {
        case '0':
            ret = QStringLiteral( "tmap_speed_average_r_num_0.png" );
            break;
        case '1':
            ret = QStringLiteral( "tmap_speed_average_r_num_1.png" );
            break;
        case '2':
            ret = QStringLiteral( "tmap_speed_average_r_num_2.png" );
            break;
        case '3':
            ret = QStringLiteral( "tmap_speed_average_r_num_3.png" );
            break;
        case '4':
            ret = QStringLiteral( "tmap_speed_average_r_num_4.png" );
            break;
        case '5':
            ret = QStringLiteral( "tmap_speed_average_r_num_5.png" );
            break;
        case '6':
            ret = QStringLiteral( "tmap_speed_average_r_num_6.png" );
            break;
        case '7':
            ret = QStringLiteral( "tmap_speed_average_r_num_7.png" );
            break;
        case '8':
            ret = QStringLiteral( "tmap_speed_average_r_num_8.png" );
            break;
        case '9':
            ret = QStringLiteral( "tmap_speed_average_r_num_9.png" );
            break;
        case ':':
            ret = QStringLiteral( "tmap_speed_average_r_dot.png" );
            break;
        }
    }
    else if ( numberType == RgConstants::SdiWarning ) {
        switch ( num.toLatin1() ) {
        case '0':
            ret = QStringLiteral( "tmap_speed_average_y_num_0.png" );
            break;
        case '1':
            ret = QStringLiteral( "tmap_speed_average_y_num_1.png" );
            break;
        case '2':
            ret = QStringLiteral( "tmap_speed_average_y_num_2.png" );
            break;
        case '3':
            ret = QStringLiteral( "tmap_speed_average_y_num_3.png" );
            break;
        case '4':
            ret = QStringLiteral( "tmap_speed_average_y_num_4.png" );
            break;
        case '5':
            ret = QStringLiteral( "tmap_speed_average_y_num_5.png" );
            break;
        case '6':
            ret = QStringLiteral( "tmap_speed_average_y_num_6.png" );
            break;
        case '7':
            ret = QStringLiteral( "tmap_speed_average_y_num_7.png" );
            break;
        case '8':
            ret = QStringLiteral( "tmap_speed_average_y_num_8.png" );
            break;
        case '9':
            ret = QStringLiteral( "tmap_speed_average_y_num_9.png" );
            break;
        case ':':
            ret = QStringLiteral( "tmap_speed_average_y_dot.png" );
            break;
        }
    }

    return ret;
}

QString RgUtils::sdiSpeedSectionAverageNumberKmhImage(RgConstants::SdiSectionSpeedNumberType numberType)
{
    QString ret;

    if ( numberType == RgConstants::SdiNormal ) {
        ret = QStringLiteral( "tmap_speed_average_g_kmh.png" );
    }
    else if ( numberType == RgConstants::SdiExcessive ) {
        ret = QStringLiteral( "tmap_speed_average_r_kmh.png" );
    }
    else if ( numberType == RgConstants::SdiWarning ) {
        ret = QStringLiteral( "tmap_speed_average_y_kmh.png" );
    }

    return ret;
}

QString RgUtils::sdiDistanceNumberImage(QChar num)
{
    QString ret;

    switch ( num.toLatin1() ) {
    case '0':
        ret = QStringLiteral( "tmap_speed_remain_num_0.png" );
        break;
    case '1':
        ret = QStringLiteral( "tmap_speed_remain_num_1.png" );
        break;
    case '2':
        ret = QStringLiteral( "tmap_speed_remain_num_2.png" );
        break;
    case '3':
        ret = QStringLiteral( "tmap_speed_remain_num_3.png" );
        break;
    case '4':
        ret = QStringLiteral( "tmap_speed_remain_num_4.png" );
        break;
    case '5':
        ret = QStringLiteral( "tmap_speed_remain_num_5.png" );
        break;
    case '6':
        ret = QStringLiteral( "tmap_speed_remain_num_6.png" );
        break;
    case '7':
        ret = QStringLiteral( "tmap_speed_remain_num_7.png" );
        break;
    case '8':
        ret = QStringLiteral( "tmap_speed_remain_num_8.png" );
        break;
    case '9':
        ret = QStringLiteral( "tmap_speed_remain_num_9.png" );
        break;
    case 'k':
        ret = QStringLiteral( "tmap_speed_remain_km.png" );
        break;
    case 'm':
        ret = QStringLiteral( "tmap_speed_remain_m.png" );
        break;
    case '.':
        ret = QStringLiteral( "tmap_speed_remain_dot.png" );
        break;
    }

    return ret;
}

QString RgUtils::sdiCurrentSpeedNumberImage(QChar num, bool alert)
{
    static QStringList images = QStringList{
        {QStringLiteral("tmap_speed_num_me_0.png")},
        {QStringLiteral("tmap_speed_num_me_1.png")},
        {QStringLiteral("tmap_speed_num_me_2.png")},
        {QStringLiteral("tmap_speed_num_me_3.png")},
        {QStringLiteral("tmap_speed_num_me_4.png")},
        {QStringLiteral("tmap_speed_num_me_5.png")},
        {QStringLiteral("tmap_speed_num_me_6.png")},
        {QStringLiteral("tmap_speed_num_me_7.png")},
        {QStringLiteral("tmap_speed_num_me_8.png")},
        {QStringLiteral("tmap_speed_num_me_9.png")}
    };

    static QStringList alertImages = QStringList{
        {QStringLiteral("tmap_speed_num_me_red_0.png")},
        {QStringLiteral("tmap_speed_num_me_red_1.png")},
        {QStringLiteral("tmap_speed_num_me_red_2.png")},
        {QStringLiteral("tmap_speed_num_me_red_3.png")},
        {QStringLiteral("tmap_speed_num_me_red_4.png")},
        {QStringLiteral("tmap_speed_num_me_red_5.png")},
        {QStringLiteral("tmap_speed_num_me_red_6.png")},
        {QStringLiteral("tmap_speed_num_me_red_7.png")},
        {QStringLiteral("tmap_speed_num_me_red_8.png")},
        {QStringLiteral("tmap_speed_num_me_red_9.png")}
    };

    QString ret;

    int index = num.toLatin1() - '0';
    if ( index >= 0 && index <= 9 ) {
        ret = alert ? alertImages[index] : images[index];
    }
    else {
        T_DEBUG( QString( "Unexpected character '%c'" ).arg( num ) );
    }

    return ret;
}

QString RgUtils::laneImage( int laneIndex, const QString& code )
{
    static QMap<QString,QString> LangImgMap = QMap<QString,QString>{
        {RgLaneCode::instance()->L0100(), QStringLiteral("1_0.png")}, //유턴 비활성화
        {RgLaneCode::instance()->L0101(), QStringLiteral("1_1.png")}, //유턴 활성화
        {RgLaneCode::instance()->L0200(), QStringLiteral("2_0.png")}, //좌회전 비활성화
        {RgLaneCode::instance()->L0202(), QStringLiteral("2_1.png")}, //좌회전 활성화
        {RgLaneCode::instance()->L0300(), QStringLiteral("12_00.png")}, //좌회전+유턴 : 비활성화
        {RgLaneCode::instance()->L0301(), QStringLiteral("12_01.png")}, //좌회전+유턴 : 유턴 활성화
        {RgLaneCode::instance()->L0302(), QStringLiteral("12_10.png")}, //좌회전+유턴 : 좌회전 활성화
        {RgLaneCode::instance()->L0303(), QStringLiteral("12_11.png")}, //좌회전+유턴 : 활성화
        {RgLaneCode::instance()->L0400(), QStringLiteral("3_0.png")}, //10시 방향 좌회전 비활성화
        {RgLaneCode::instance()->L0404(), QStringLiteral("3_1.png")}, //10시 방향 좌회전 활성화
        {RgLaneCode::instance()->L0500(), QStringLiteral("13_00.png")}, //10시 방향 좌회전 + 유턴 : 비활성화
        {RgLaneCode::instance()->L0501(), QStringLiteral("13_01.png")}, //10시 방향 좌회전 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L0504(), QStringLiteral("13_10.png")}, //10시 방향 좌회전 + 유턴 : 10시 방향 좌회전 활성화
        {RgLaneCode::instance()->L0505(), QStringLiteral("13_11.png")}, //10시 방향 좌회전 + 유턴 : 활성화
        {RgLaneCode::instance()->L0600(), QStringLiteral("23_00.png")}, //10시 방향 좌회전 + 좌회전 : 비활성화
        {RgLaneCode::instance()->L0602(), QStringLiteral("23_01.png")}, //10시 방향 좌회전 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L0604(), QStringLiteral("23_10.png")}, //10시 방향 좌회전 + 좌회전 : 10시 방향 좌회전 활성화
        {RgLaneCode::instance()->L0606(), QStringLiteral("23_11.png")}, //10시 방향 좌회전 + 좌회전 : 활성화
        {RgLaneCode::instance()->L0700(), QStringLiteral("123_000.png")}, //10시 방향 좌회전 + 좌회전 + 유턴 : 비활성화
        {RgLaneCode::instance()->L0701(), QStringLiteral("123_001.png")}, //10시 방향 좌회전 + 좌회전 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L0702(), QStringLiteral("123_010.png")}, //10시 방향 좌회전 + 좌회전 + 유턴 : 좌회전 활성화
        {RgLaneCode::instance()->L0704(), QStringLiteral("123_100.png")}, //10시 방향 좌회전 + 좌회전 + 유턴 : 10시 방향 좌회전 활성화
        {RgLaneCode::instance()->L0707(), QStringLiteral("123_111.png")}, //10시 방향 좌회전 + 좌회전 + 유턴 : 활성화
        {RgLaneCode::instance()->L0800(), QStringLiteral("4_0.png")}, //직진 비활성화
        {RgLaneCode::instance()->L0808(), QStringLiteral("4_1.png")}, //직진 활성화
        {RgLaneCode::instance()->L0900(), QStringLiteral("14_00.png")}, //직진 + 유턴 비활성화
        {RgLaneCode::instance()->L0901(), QStringLiteral("14_01.png")}, //직진 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L0908(), QStringLiteral("14_10.png")}, //직진 + 유턴 : 직진 활성화
        {RgLaneCode::instance()->L0909(), QStringLiteral("14_11.png")}, //직진 + 유턴 활성화
        {RgLaneCode::instance()->L1000(), QStringLiteral("24_00.png")}, //직진 + 좌회전 비활성화
        {RgLaneCode::instance()->L1002(), QStringLiteral("24_01.png")}, //직진 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L1008(), QStringLiteral("24_10.png")}, //직진 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L1010(), QStringLiteral("24_11.png")}, //직진 + 좌회전 활성화
        {RgLaneCode::instance()->L1100(), QStringLiteral("124_000.png")}, //직진 + 유턴 + 좌회전  비활성화
        {RgLaneCode::instance()->L1101(), QStringLiteral("124_001.png")}, //직진 + 유턴 + 좌회전 : 유턴 활성화
        {RgLaneCode::instance()->L1102(), QStringLiteral("124_010.png")}, //직진 + 유턴 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L1108(), QStringLiteral("124_100.png")}, //직진 + 유턴 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L1111(), QStringLiteral("124_111.png")}, //직진 + 유턴 + 좌회전 활성화
        {RgLaneCode::instance()->L1200(), QStringLiteral("34_00.png")}, //직진 + 10시방향 좌회전 : 비활성화
        {RgLaneCode::instance()->L1204(), QStringLiteral("34_01.png")}, //직진 + 10시방향 좌회전 : 10시 방향 좌회전 활성화
        {RgLaneCode::instance()->L1208(), QStringLiteral("34_10.png")}, //직진 + 10시방향 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L1212(), QStringLiteral("34_11.png")}, //직진 + 10시방향 좌회전 : 활성화
        {RgLaneCode::instance()->L1300(), QStringLiteral("134_000.png")}, //직진 + 10시방향 좌회전 + 유턴 : 비활성화
        {RgLaneCode::instance()->L1301(), QStringLiteral("134_001.png")}, //직진 + 10시방향 좌회전 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L1304(), QStringLiteral("134_010.png")}, //직진 + 10시방향 좌회전 + 유턴 : 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L1308(), QStringLiteral("134_100.png")}, //직진 + 10시방향 좌회전 + 유턴 : 직진 활성화
        {RgLaneCode::instance()->L1313(), QStringLiteral("134_111.png")}, //직진 + 10시방향 좌회전 + 유턴 : 활성화
        {RgLaneCode::instance()->L1400(), QStringLiteral("234_000.png")}, //직진 + 10시방향 좌회전 + 좌회전 비활성화
        {RgLaneCode::instance()->L1402(), QStringLiteral("234_001.png")}, //직진 + 10시방향 좌회전 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L1404(), QStringLiteral("234_010.png")}, //직진 + 10시방향 좌회전 + 좌회전 : 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L1408(), QStringLiteral("234_100.png")}, //직진 + 10시방향 좌회전 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L1414(), QStringLiteral("234_111.png")}, //직진 + 10시방향 좌회전 + 좌회전 활성화
        {RgLaneCode::instance()->L1500(), QStringLiteral("1234_0000.png")}, //직진 + 10시방향 좌회전 + 좌회전 + 유턴 비활성화
        {RgLaneCode::instance()->L1501(), QStringLiteral("1234_0001.png")}, //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 유턴 활성화
        {RgLaneCode::instance()->L1502(), QStringLiteral("1234_0010.png")}, //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 좌회전 활성화
        {RgLaneCode::instance()->L1504(), QStringLiteral("1234_0100.png")}, //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L1508(), QStringLiteral("1234_1000.png")}, //직진 + 10시방향 좌회전 + 좌회전 + 유턴  : 직진 활성화
        {RgLaneCode::instance()->L1515(), QStringLiteral("1234_1111.png")}, //직진 + 10시방향 좌회전 + 좌회전 + 유턴 활성화
        {RgLaneCode::instance()->L1600(), QStringLiteral("5_0.png")}, //2시방향 우회전 비활성화
        {RgLaneCode::instance()->L1616(), QStringLiteral("5_1.png")}, //2시방향 우회전 활성화
        {RgLaneCode::instance()->L1700(), QStringLiteral("15_00.png")}, //2시방향 우회전 + 유턴 비활성화
        {RgLaneCode::instance()->L1701(), QStringLiteral("15_01.png")}, //2시방향 우회전 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L1716(), QStringLiteral("15_10.png")}, //2시방향 우회전 + 유턴 : 2시방향 우회전 활성화
        {RgLaneCode::instance()->L1717(), QStringLiteral("15_11.png")}, //2시방향 우회전 + 유턴 활성화
        {RgLaneCode::instance()->L1800(), QStringLiteral("25_00.png")}, //2시방향 우회전 + 좌회전 비활성화
        {RgLaneCode::instance()->L1802(), QStringLiteral("25_01.png")}, //2시방향 우회전 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L1816(), QStringLiteral("25_10.png")}, //2시방향 우회전 + 좌회전 : 2시방향 우회전 활성화
        {RgLaneCode::instance()->L1818(), QStringLiteral("25_11.png")}, //2시방향 우회전 + 좌회전 활성화
        {RgLaneCode::instance()->L1900(), QStringLiteral("124_000.png")}, //직진 + 유턴 + 좌회전  비활성화
        {RgLaneCode::instance()->L1901(), QStringLiteral("124_001.png")}, //직진 + 유턴 + 좌회전 : 유턴 활성화
        {RgLaneCode::instance()->L1902(), QStringLiteral("124_010.png")}, //직진 + 유턴 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L1908(), QStringLiteral("124_100.png")}, //직진 + 유턴 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L1911(), QStringLiteral("124_111.png")}, //직진 + 유턴 + 좌회전 활성화
        {RgLaneCode::instance()->L2000(), QStringLiteral("35_00.png")}, //2시방향 우회전 + 10시방향 좌회전 비활성화
        {RgLaneCode::instance()->L2004(), QStringLiteral("35_01.png")}, //2시방향 우회전 + 10시방향 좌회전 : 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L2016(), QStringLiteral("35_10.png")}, //2시방향 우회전 + 10시방향 좌회전 : 2시방향 우회전 활성화
        {RgLaneCode::instance()->L2020(), QStringLiteral("35_11.png")}, //2시방향 우회전 + 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L2100(), QStringLiteral("14_00.png")}, //직진 + 유턴 비활성화
        {RgLaneCode::instance()->L2101(), QStringLiteral("14_01.png")}, //직진 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L2108(), QStringLiteral("14_10.png")}, //직진 + 유턴 : 직진 활성화
        {RgLaneCode::instance()->L2109(), QStringLiteral("14_11.png")}, //직진 + 유턴 활성화
        {RgLaneCode::instance()->L2200(), QStringLiteral("235_000.png")}, //2시방향 우회전 + 10시방향 좌회전 + 좌회전 비활성화
        {RgLaneCode::instance()->L2202(), QStringLiteral("235_001.png")}, //2시방향 우회전 + 10시방향 좌회전 + 좌회전 : 좌회전  활성화
        {RgLaneCode::instance()->L2204(), QStringLiteral("235_010.png")}, //2시방향 우회전 + 10시방향 좌회전 + 좌회전 : 10시방향 좌회전  활성화
        {RgLaneCode::instance()->L2216(), QStringLiteral("235_100.png")}, //2시방향 우회전 + 10시방향 좌회전 + 좌회전 : 2시방향 좌회전 활성화
        {RgLaneCode::instance()->L2222(), QStringLiteral("235_111.png")}, //2시방향 우회전 + 10시방향 좌회전 + 좌회전 활성화
        {RgLaneCode::instance()->L2300(), QStringLiteral("124_000.png")}, //직진 + 유턴 + 좌회전  비활성화
        {RgLaneCode::instance()->L2301(), QStringLiteral("124_001.png")}, //직진 + 유턴 + 좌회전 : 유턴 활성화
        {RgLaneCode::instance()->L2302(), QStringLiteral("124_010.png")}, //직진 + 유턴 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L2308(), QStringLiteral("124_100.png")}, //직진 + 유턴 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L2311(), QStringLiteral("124_111.png")}, //직진 + 유턴 + 좌회전 활성화
        {RgLaneCode::instance()->L2400(), QStringLiteral("45_00.png")}, //2시 방향 우회전 + 직진 비활성화
        {RgLaneCode::instance()->L2408(), QStringLiteral("45_01.png")}, //2시 방향 우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L2416(), QStringLiteral("45_10.png")}, //2시 방향 우회전 + 직진 : 2시 방향 우회전 활성화
        {RgLaneCode::instance()->L2424(), QStringLiteral("45_11.png")}, //2시 방향 우회전 + 직진 활성화
        {RgLaneCode::instance()->L2500(), QStringLiteral("145_000.png")}, //2시 방향 우회전 + 직진 + 유턴 비활성화
        {RgLaneCode::instance()->L2501(), QStringLiteral("145_001.png")}, //2시 방향 우회전 + 직진 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L2508(), QStringLiteral("145_010.png")}, //2시 방향 우회전 + 직진 + 유턴 : 직진 활성화
        {RgLaneCode::instance()->L2516(), QStringLiteral("145_100.png")}, //2시 방향 우회전 + 직진 + 유턴 : 2시 방향 우회전 활성화
        {RgLaneCode::instance()->L2525(), QStringLiteral("145_111.png")}, //2시 방향 우회전 + 직진 + 유턴 활성화
        {RgLaneCode::instance()->L2600(), QStringLiteral("245_000.png")}, //2시 방향 우회전 + 직진 + 좌회전 비활성화
        {RgLaneCode::instance()->L2602(), QStringLiteral("245_001.png")}, //2시 방향 우회전 + 직진 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L2608(), QStringLiteral("245_010.png")}, //2시 방향 우회전 + 직진 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L2616(), QStringLiteral("245_100.png")}, //2시 방향 우회전 + 직진 + 좌회전 : 2시 방향 우회전 활성화
        {RgLaneCode::instance()->L2626(), QStringLiteral("245_111.png")}, //2시 방향 우회전 + 직진 + 좌회전 활성화
        {RgLaneCode::instance()->L2700(), QStringLiteral("124_000.png")}, //직진 + 유턴 + 좌회전  비활성화
        {RgLaneCode::instance()->L2701(), QStringLiteral("124_001.png")}, //직진 + 유턴 + 좌회전 : 유턴 활성화
        {RgLaneCode::instance()->L2702(), QStringLiteral("124_010.png")}, //직진 + 유턴 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L2708(), QStringLiteral("124_100.png")}, //직진 + 유턴 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L2711(), QStringLiteral("124_111.png")}, //직진 + 유턴 + 좌회전 활성화
        {RgLaneCode::instance()->L2800(), QStringLiteral("4_0.png")}, //직진 비활성화
        {RgLaneCode::instance()->L2808(), QStringLiteral("4_1.png")}, //직진 활성화
        {RgLaneCode::instance()->L2900(), QStringLiteral("14_00.png")}, //직진 + 유턴 비활성화
        {RgLaneCode::instance()->L2901(), QStringLiteral("14_01.png")}, //직진 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L2908(), QStringLiteral("14_10.png")}, //직진 + 유턴 : 직진 활성화
        {RgLaneCode::instance()->L2909(), QStringLiteral("14_11.png")}, //직진 + 유턴 활성화
        {RgLaneCode::instance()->L3000(), QStringLiteral("24_00.png")}, //직진 + 좌회전 비활성화
        {RgLaneCode::instance()->L3002(), QStringLiteral("24_01.png")}, //직진 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L3008(), QStringLiteral("24_10.png")}, //직진 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L3010(), QStringLiteral("24_11.png")}, //직진 + 좌회전 활성화
        {RgLaneCode::instance()->L3100(), QStringLiteral("124_000.png")}, //직진 + 유턴 + 좌회전  비활성화
        {RgLaneCode::instance()->L3101(), QStringLiteral("124_001.png")}, //직진 + 유턴 + 좌회전 : 유턴 활성화
        {RgLaneCode::instance()->L3102(), QStringLiteral("124_010.png")}, //직진 + 유턴 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L3108(), QStringLiteral("124_100.png")}, //직진 + 유턴 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L3111(), QStringLiteral("124_111.png")}, //직진 + 유턴 + 좌회전 활성화
        {RgLaneCode::instance()->L3200(), QStringLiteral("6_0.png")}, //우회전 비활성화
        {RgLaneCode::instance()->L3232(), QStringLiteral("6_1.png")}, //우회전 활성화
        {RgLaneCode::instance()->L3300(), QStringLiteral("16_00.png")}, //우회전 + 유턴 비활성화
        {RgLaneCode::instance()->L3301(), QStringLiteral("16_01.png")}, //우회전 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L3332(), QStringLiteral("16_10.png")}, //우회전 + 유턴 : 우회전 활성화
        {RgLaneCode::instance()->L3333(), QStringLiteral("16_11.png")}, //우회전 + 유턴 활성화
        {RgLaneCode::instance()->L3400(), QStringLiteral("26_00.png")}, //우회전 + 좌회전 비활성화
        {RgLaneCode::instance()->L3402(), QStringLiteral("26_01.png")}, //우회전 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L3432(), QStringLiteral("26_10.png")}, //우회전 + 좌회전 : 우회전 활성화
        {RgLaneCode::instance()->L3434(), QStringLiteral("26_11.png")}, //우회전 + 좌회전 활성화
        {RgLaneCode::instance()->L3500(), QStringLiteral("26_00.png")}, //우회전 + 좌회전 비활성화
        {RgLaneCode::instance()->L3502(), QStringLiteral("26_01.png")}, //우회전 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L3532(), QStringLiteral("26_10.png")}, //우회전 + 좌회전 : 우회전 활성화
        {RgLaneCode::instance()->L3534(), QStringLiteral("26_11.png")}, //우회전 + 좌회전 활성화
        {RgLaneCode::instance()->L3600(), QStringLiteral("36_00.png")}, //우회전 + 10시방향 좌회전 비활성화
        {RgLaneCode::instance()->L3602(), QStringLiteral("36_01.png")}, //우회전 + 10시방향 좌회전 : 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L3632(), QStringLiteral("36_10.png")}, //우회전 + 10시방향 좌회전 : 우회전 활성화
        {RgLaneCode::instance()->L3634(), QStringLiteral("36_11.png")}, //우회전 + 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L3700(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L3708(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L3732(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L3740(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L3800(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L3808(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L3832(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L3840(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L3900(), QStringLiteral("1236_0000.png")}, //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 비활성화
        {RgLaneCode::instance()->L3901(), QStringLiteral("1236_0001.png")}, //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L3902(), QStringLiteral("1236_0010.png")}, //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 좌회전 활성화
        {RgLaneCode::instance()->L3904(), QStringLiteral("1236_0100.png")}, //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L3932(), QStringLiteral("1236_1000.png")}, //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 : 우회전 활성화
        {RgLaneCode::instance()->L3939(), QStringLiteral("1236_1111.png")}, //우회전 + 10시방향 좌회전 + 좌회전 + 유턴 활성화
        {RgLaneCode::instance()->L4000(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L4008(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L4032(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L4040(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L4100(), QStringLiteral("146_000.png")}, //우회전 + 직진 + 유턴  비활성화
        {RgLaneCode::instance()->L4101(), QStringLiteral("146_001.png")}, //우회전 + 직진 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L4108(), QStringLiteral("146_010.png")}, //우회전 + 직진 + 유턴 : 직진 활성화
        {RgLaneCode::instance()->L4132(), QStringLiteral("146_100.png")}, //우회전 + 직진 + 유턴 : 우회전 활성화
        {RgLaneCode::instance()->L4141(), QStringLiteral("146_111.png")}, //우회전 + 직진 + 유턴 활성화
        {RgLaneCode::instance()->L4200(), QStringLiteral("246_000.png")}, //우회전 + 직진 + 좌회전  비활성화
        {RgLaneCode::instance()->L4201(), QStringLiteral("246_001.png")}, //우회전 + 직진 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L4208(), QStringLiteral("246_010.png")}, //우회전 + 직진 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L4232(), QStringLiteral("246_100.png")}, //우회전 + 직진 + 좌회전 : 우회전 활성화
        {RgLaneCode::instance()->L4241(), QStringLiteral("246_111.png")}, //우회전 + 직진 + 좌회전 활성화
        {RgLaneCode::instance()->L4300(), QStringLiteral("1246_0000.png")}, //우회전 + 직진 + 좌회전 + 유턴  비활성화
        {RgLaneCode::instance()->L4301(), QStringLiteral("1246_0001.png")}, //우회전 + 직진 + 좌회전 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L4302(), QStringLiteral("1246_0010.png")}, //우회전 + 직진 + 좌회전 + 유턴 : 좌회전 활성화
        {RgLaneCode::instance()->L4308(), QStringLiteral("1246_0100.png")}, //우회전 + 직진 + 좌회전 + 유턴 : 직진 활성화
        {RgLaneCode::instance()->L4332(), QStringLiteral("1246_1000.png")}, //우회전 + 직진 + 좌회전 + 유턴 : 우회전 활성화
        {RgLaneCode::instance()->L4343(), QStringLiteral("1246_1111.png")}, //우회전 + 직진 + 좌회전 + 유턴 활성화
        {RgLaneCode::instance()->L4400(), QStringLiteral("346_000.png")}, //우회전 + 직진 + 10시방향 좌회전  비활성화
        {RgLaneCode::instance()->L4404(), QStringLiteral("346_001.png")}, //우회전 + 직진 + 10시방향 좌회전 : 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L4408(), QStringLiteral("346_010.png")}, //우회전 + 직진 + 10시방향 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L4432(), QStringLiteral("346_100.png")}, //우회전 + 직진 + 10시방향 좌회전 : 우회전 활성화
        {RgLaneCode::instance()->L4444(), QStringLiteral("346_111.png")}, //우회전 + 직진 + 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L4500(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L4508(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L4532(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L4540(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L4600(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L4608(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L4632(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L4640(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L4700(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L4708(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L4732(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L4740(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L4800(), QStringLiteral("56_00.png")}, //우회전 + 2시 방향 우회전 비활성화
        {RgLaneCode::instance()->L4816(), QStringLiteral("56_01.png")}, //우회전 + 2시 방향 우회전 : 2시 방향 우회전 활성화
        {RgLaneCode::instance()->L4832(), QStringLiteral("56_10.png")}, //우회전 + 2시 방향 우회전 : 우회전 활성화
        {RgLaneCode::instance()->L4848(), QStringLiteral("56_11.png")}, //우회전 + 2시 방향 우회전 활성화
        {RgLaneCode::instance()->L4900(), QStringLiteral("156_000.png")}, //우회전 + 2시 방향 우회전 + 유턴  비활성화
        {RgLaneCode::instance()->L4901(), QStringLiteral("156_001.png")}, //우회전 + 2시 방향 우회전 + 유턴 : 유턴 활성화
        {RgLaneCode::instance()->L4916(), QStringLiteral("156_010.png")}, //우회전 + 2시 방향 우회전 + 유턴 : 2시 방향 우회전 활성화
        {RgLaneCode::instance()->L4932(), QStringLiteral("156_100.png")}, //우회전 + 2시 방향 우회전 + 유턴 : 우회전 활성화
        {RgLaneCode::instance()->L4949(), QStringLiteral("156_111.png")}, //우회전 + 2시 방향 우회전 + 유턴 활성화
        {RgLaneCode::instance()->L5000(), QStringLiteral("256_000.png")}, //우회전 + 2시 방향 우회전 + 좌회전  비활성화
        {RgLaneCode::instance()->L5002(), QStringLiteral("256_001.png")}, //우회전 + 2시 방향 우회전 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L5016(), QStringLiteral("256_010.png")}, //우회전 + 2시 방향 우회전 + 좌회전 : 2시 방향 우회전 활성화
        {RgLaneCode::instance()->L5032(), QStringLiteral("256_100.png")}, //우회전 + 2시 방향 우회전 + 좌회전 : 우회전 활성화
        {RgLaneCode::instance()->L5050(), QStringLiteral("256_111.png")}, //우회전 + 2시 방향 우회전 + 좌회전 활성화
        {RgLaneCode::instance()->L5100(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L5108(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L5132(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L5140(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L5200(), QStringLiteral("356_000.png")}, //우회전 + 2시 방향 우회전 + 10시방향 좌회전  비활성화
        {RgLaneCode::instance()->L5204(), QStringLiteral("356_001.png")}, //우회전 + 2시 방향 우회전 + 10시방향 좌회전 : 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L5216(), QStringLiteral("356_010.png")}, //우회전 + 2시 방향 우회전 + 10시방향 좌회전 : 2시 방향 우회전 활성화
        {RgLaneCode::instance()->L5232(), QStringLiteral("356_100.png")}, //우회전 + 2시 방향 우회전 + 10시방향 좌회전 : 우회전 활성화
        {RgLaneCode::instance()->L5252(), QStringLiteral("356_111.png")}, //우회전 + 2시 방향 우회전 + 10시방향 좌회전 활성화
        {RgLaneCode::instance()->L5300(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L5308(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L5332(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L5340(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L5400(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L5408(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L5432(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L5440(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L5500(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L5508(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L5532(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L5540(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L5600(), QStringLiteral("456_000.png")}, //우회전 + 2시 방향 우회전 + 직진  비활성화
        {RgLaneCode::instance()->L5608(), QStringLiteral("456_001.png")}, //우회전 + 2시 방향 우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L5616(), QStringLiteral("456_010.png")}, //우회전 + 2시 방향 우회전 + 직진 : 2시 방향 우회전 활성화
        {RgLaneCode::instance()->L5632(), QStringLiteral("456_100.png")}, //우회전 + 2시 방향 우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L5656(), QStringLiteral("456_111.png")}, //우회전 + 2시 방향 우회전 + 직진 활성화
        {RgLaneCode::instance()->L5700(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L5708(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L5732(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L5740(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L5800(), QStringLiteral("2456_0000.png")}, //우회전 + 2시 방향 우회전 + 직진 + 좌회전  비활성화
        {RgLaneCode::instance()->L5802(), QStringLiteral("2456_0001.png")}, //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 좌회전 활성화
        {RgLaneCode::instance()->L5808(), QStringLiteral("2456_0010.png")}, //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 직진 활성화
        {RgLaneCode::instance()->L5816(), QStringLiteral("2456_0100.png")}, //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 2시 방향 우회전 활성화
        {RgLaneCode::instance()->L5832(), QStringLiteral("2456_1000.png")}, //우회전 + 2시 방향 우회전 + 직진 + 좌회전 : 우회전 활성화
        {RgLaneCode::instance()->L5858(), QStringLiteral("2456_1111.png")}, //우회전 + 2시 방향 우회전 + 직진 + 좌회전 활성화
        {RgLaneCode::instance()->L5900(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L5908(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L5932(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L5940(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L6000(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L6008(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L6032(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L6040(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L6100(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L6108(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L6132(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L6140(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L6200(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L6208(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L6232(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L6240(), QStringLiteral("46_11.png")}, //우회전 + 직진 활성화
        {RgLaneCode::instance()->L6300(), QStringLiteral("46_00.png")}, //우회전 + 직진 비활성화
        {RgLaneCode::instance()->L6308(), QStringLiteral("46_01.png")}, //우회전 + 직진 : 직진 활성화
        {RgLaneCode::instance()->L6332(), QStringLiteral("46_10.png")}, //우회전 + 직진 : 우회전 활성화
        {RgLaneCode::instance()->L6340(), QStringLiteral("46_11.png")} //우회전 + 직진 활성화
    };

    return ( laneIndex == 0 ? QStringLiteral("land_laneguid_") : QStringLiteral("land_laneguid2_") ) + LangImgMap[code];
}


QString RgUtils::laneDistanceNumberImage( int laneIndex, QChar num )
{
    QString ret;

    switch ( num.toLatin1() ) {
    case '0':
        ret = QStringLiteral( "0.png" );
        break;
    case '1':
        ret = QStringLiteral( "1.png" );
        break;
    case '2':
        ret = QStringLiteral( "2.png" );
        break;
    case '3':
        ret = QStringLiteral( "3.png" );
        break;
    case '4':
        ret = QStringLiteral( "4.png" );
        break;
    case '5':
        ret = QStringLiteral( "5.png" );
        break;
    case '6':
        ret = QStringLiteral( "6.png" );
        break;
    case '7':
        ret = QStringLiteral( "7.png" );
        break;
    case '8':
        ret = QStringLiteral( "8.png" );
        break;
    case '9':
        ret = QStringLiteral( "9.png" );
        break;
    case 'k':
        ret = QStringLiteral( "km.png" );
        break;
    case 'm':
        ret = QStringLiteral( "m.png" );
        break;
    case '.':
        ret = QStringLiteral( "dot.png" );
        break;
    default:
        T_DEBUG( QString( "Unexpected Distance Char %1" ).arg( num ) );
        break;
    }

    return ( laneIndex == 0 ? QStringLiteral("laneguid_num_") : QStringLiteral("laneguid2_num_") ) + ret;
}

QString RgUtils::formatDistance(int dist)
{
    QString farStr;

    // 1km 미만의 거리는 m 단위 정수로 표시
    // 1km이상 ~ 10km 미만: 0.0km 형식으로 표시
    // 10km 이상: 소수점 없이 정수로 표시

    if (dist < 1000) { // 0 ~ 999m
        farStr = QString(QStringLiteral("%1")).arg(dist) + QStringLiteral("m");
    }
    else if (dist >= 1000 && dist < 10000 ) { // 1000 ~ 9999m
        auto km = dist / 1000.0;
        km = floor(km * 10.0) / 10.0;
        farStr = QString().sprintf("%.1fkm", km);
    }
    else {
        farStr = QString(QStringLiteral("%1km")).arg(dist / 1000);
    }

    return farStr;
}

QString RgUtils::highwayDistanceNumberImage( QChar num )
{
    QString ret;

    switch ( num.toLatin1() ) {
    case '0':
        ret = QStringLiteral( "highway_num_0.png" );
        break;
    case '1':
        ret = QStringLiteral( "highway_num_1.png" );
        break;
    case '2':
        ret = QStringLiteral( "highway_num_2.png" );
        break;
    case '3':
        ret = QStringLiteral( "highway_num_3.png" );
        break;
    case '4':
        ret = QStringLiteral( "highway_num_4.png" );
        break;
    case '5':
        ret = QStringLiteral( "highway_num_5.png" );
        break;
    case '6':
        ret = QStringLiteral( "highway_num_6.png" );
        break;
    case '7':
        ret = QStringLiteral( "highway_num_7.png" );
        break;
    case '8':
        ret = QStringLiteral( "highway_num_8.png" );
        break;
    case '9':
        ret = QStringLiteral( "highway_num_9.png" );
        break;
    case 'k':
        ret = QStringLiteral( "highway_num_km.png" );
        break;
    case 'm':
        ret = QStringLiteral( "highway_num_m.png" );
        break;
    case '.':
        ret = QStringLiteral( "highway_num_dot.png" );
        break;
    }

    return ret;
}

QString RgUtils::highwayStationImage( int station )
{
    QString ret;

    switch ( station ) {
    case VSM_OilCompanySK:
        ret = QStringLiteral( "highway_station_01.png" );
        break;
    case VSM_OilCompanyGS:
        ret = QStringLiteral( "highway_station_03.png" );
        break;
    case VSM_OilCompanyHyundae:
        ret = QStringLiteral( "highway_station_02.png" );
        break;
    case VSM_OilCompanySOil:
        ret = QStringLiteral( "highway_station_06.png" );
        break;
    case VSM_OilCompanyTiger:
        ret = QStringLiteral( "highway_station_07.png" );
        break;
    case VSM_OilCompanyInCheon:
        ret = QStringLiteral( "highway_station_07.png" );
        break;
    case VSM_OilCompanyNC:
        ret = QStringLiteral( "highway_station_07.png" );
        break;
    case VSM_OilCompanyNH:
        ret = QStringLiteral( "highway_station_05.png" );
        break;
    case VSM_OilCompanySevenD:
        ret = QStringLiteral( "highway_station_07.png" );
        break;
    case VSM_OilCompanyEconomic:
        ret = QStringLiteral( "highway_station_04.png" );
        break;
    case VSM_OilCompanyETC:
        ret = QStringLiteral( "highway_station_07.png" );
        break;
    case VSM_GasCompanySK:
        ret = QStringLiteral( "highway_station_08.png" );
        break;
    case VSM_GasCompanyGS:
        ret = QStringLiteral( "highway_station_10.png" );
        break;
    case VSM_GasCompanyHyundae:
        ret = QStringLiteral( "highway_station_09.png" );
        break;
    case VSM_GasCompanySOil:
        ret = QStringLiteral( "highway_station_13.png" );
        break;
    case VSM_GasCompanyE1:
        ret = QStringLiteral( "highway_station_14.png" );
        break;
    case VSM_GasCompanyInCheon:
        ret = QStringLiteral( "highway_station_14.png" );
        break;
    case VSM_GasCompanyNC:
        ret = QStringLiteral( "highway_station_14.png" );
        break;
    case VSM_GasCompanyNH:
        ret = QStringLiteral( "highway_station_12.png" );
        break;
    case VSM_GasCompanySevenD:
        ret = QStringLiteral( "highway_station_14.png" );
        break;
    case VSM_GasCompanyETC:
        ret = QStringLiteral( "highway_station_14.png" );
        break;
    }

    return ret;
}

QString RgUtils::highwayCongestionImage( int congestion )
{
    QString ret;

    switch ( congestion ) {
    case RgHighwayCongestionCode::NONE:
        ret = QStringLiteral("tbt_progress_null");
        break;
    case RgHighwayCongestionCode::GOOD:
        ret = QStringLiteral("tbt_progress_smooth");
        break;
    case RgHighwayCongestionCode::DELAY:
        ret = QStringLiteral("tbt_progress_slow");
        break;
    case RgHighwayCongestionCode::TIEUP:
        ret = QStringLiteral("tbt_progress_tieup");
        break;
    }

    return ret;
}

QString RgUtils::summaryTbtSecondImage( int code )
{
    return RgUtils::tbtSecondImage( code );
}

QString RgUtils::summaryTbtSecondDistanceNumberImage(QChar num)
{
    return RgUtils::tbtSecondDistanceNumberImage( num );
}

QString RgUtils::summaryTbtListDistanceNumberImage( QChar num )
{
    QString ret;

    switch ( num.toLatin1() ) {
    case '0':
        ret = QStringLiteral( "tbt_3rd_num_0.png" );
        break;
    case '1':
        ret = QStringLiteral( "tbt_3rd_num_1.png" );
        break;
    case '2':
        ret = QStringLiteral( "tbt_3rd_num_2.png" );
        break;
    case '3':
        ret = QStringLiteral( "tbt_3rd_num_3.png" );
        break;
    case '4':
        ret = QStringLiteral( "tbt_3rd_num_4.png" );
        break;
    case '5':
        ret = QStringLiteral( "tbt_3rd_num_5.png" );
        break;
    case '6':
        ret = QStringLiteral( "tbt_3rd_num_6.png" );
        break;
    case '7':
        ret = QStringLiteral( "tbt_3rd_num_7.png" );
        break;
    case '8':
        ret = QStringLiteral( "tbt_3rd_num_8.png" );
        break;
    case '9':
        ret = QStringLiteral( "tbt_3rd_num_9.png" );
        break;
    case 'k':
        ret = QStringLiteral( "tbt_3rd_num_km.png" );
        break;
    case 'm':
        ret = QStringLiteral( "tbt_3rd_num_m.png" );
        break;
    case '.':
        ret = QStringLiteral( "tbt_3rd_num_dot.png" );
        break;
    }

    return ret;
}

QString RgUtils::summaryTbtCongestionImage( int congestion )
{
    return RgUtils::highwayCongestionImage( congestion );
}

