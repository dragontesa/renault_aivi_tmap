#include "socket_client.h"
//#include "gpscallback.h"
#include <stdio.h>
#include <pthread.h>
#include <cassert>
//#include <string.h>
//#include <stdlib.h>

using namespace std;

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX (108)
#endif

#include <sys/syslog.h>

#include <fstream>
#include <iostream>

#define SOCKET_MAX_BUFFER_SIZE		8192

class SocketClient
{
    int socket_fd;

public:

    SocketClient():
        socket_fd(-1)
    {
        openlog("[LocalMessageClient]", LOG_PERROR, LOG_DEBUG);
        syslog(LOG_DEBUG, "Client:SocketClient\n");
    }

    ~SocketClient()
    {
        if (socket_fd >= 0)
            close(socket_fd);

    }

    int connectToServer(const char* socket_name)
    {
        assert(strlen(socket_name)<UNIX_PATH_MAX);
        struct sockaddr_un address = { 0 };
        socket_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (socket_fd < 0)
        {
            syslog(LOG_DEBUG, "Client: socket() failed:%s\n", strerror(errno));
            return errno;
        }
        address.sun_family = AF_UNIX;
        strncpy(address.sun_path, socket_name, UNIX_PATH_MAX-1);
//    address.sun_path[UNIX_PATH_MAX] = 0;

        if (connect(socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) != 0)
        {
            syslog(LOG_DEBUG, "Client: connect() failed:%s\n", strerror(errno));
            return errno;
        }
        else
        {
        	syslog(LOG_DEBUG, "Client: connect() Succeed!!!\n");
        }
        return 0;
    }

    ssize_t getMsg(void* msg, size_t bytes) {
    	ssize_t recvlength = recv(socket_fd, msg, bytes, 0/*MSG_DONTWAIT*/);
        if (recvlength > 0) {
        } else if (recvlength == 0 || errno == ECONNRESET) {
            syslog(LOG_DEBUG, "Client:server disconnected.\n");
            if (socket_fd >= 0) {
                close(socket_fd);
                socket_fd = -1;
            }
            return -1;
        } else if (recvlength < 0) {
            syslog(LOG_DEBUG, "Client:recv error\n");
            return -1;
        }
        return recvlength;
    }
};



string* StringSplit(string strTarget, string strTok);


/*Offset of fields in message from SCC*/
#define SEN_DISP_OFFSET_MSG_ID (0)
#define SEN_DISP_OFFSET_COMPONENT_STATUS (1)
#define SEN_DISP_OFFSET_VERSION_STATUS (SEN_DISP_OFFSET_COMPONENT_STATUS + 1)
#define SEN_DISP_OFFSET_ODO_CONFIG_DATA_INTERVAL (1)
#define SEN_DISP_OFFSET_GYRO_CONFIG_DATA_INTERVAL (3)
#define SEN_DISP_OFFSET_GYRO_CONFIG_TYPE (5)
#define SEN_DISP_OFFSET_ACC_CONFIG_DATA_INTERVAL (6)
#define SEN_DISP_OFFSET_ACC_CONFIG_TYPE (8)
#define SEN_DISP_OFFSET_ABS_CONFIG_DATA_INTERVAL (9)
#define SEN_DISP_OFFSET_REJECT_REASON   (1)
#define SEN_DISP_OFFSET_REJECTED_MSG_ID (2)
#define SEN_DISP_OFFSET_NUM_OF_ENTRIES_IN_DATA_MSG (1)
#define SEN_DISP_OFFSET_FIRST_RECORD_IN_DATA_MSG (3)
#define SEN_DISP_OFFSET_ENTRY_TYPE (0)
#define SEN_DISP_OFFSET_TIME_STAMP (1)
#define SEN_DISP_OFFSET_DIAGNOSIS_MESSAGE (4)
#define SEN_DISP_OFFSET_ODO_WHEEL_CNTR (4)
#define SEN_DISP_OFFSET_ODO_DIRECTION_FLAG (6)
#define SEN_DISP_OFFSET_GYRO_R_DATA (4)
#define SEN_DISP_OFFSET_GYRO_S_DATA (6)
#define SEN_DISP_OFFSET_GYRO_T_DATA (8)
#define SEN_DISP_OFFSET_GYRO_TEMP (4)
#define SEN_DISP_OFFSET_ACC_R_DATA (4)
#define SEN_DISP_OFFSET_ACC_S_DATA (6)
#define SEN_DISP_OFFSET_ACC_T_DATA (8)
#define SEN_DISP_OFFSET_ACC_TEMP (4)
#define SEN_DISP_OFFSET_SELFTEST_DEVICE_ID (1)
#define SEN_DISP_OFFSET_ABS_RR_WHEEL_CNTR (4)
#define SEN_DISP_OFFSET_ABS_RL_WHEEL_CNTR (6)
#define SEN_DISP_OFFSET_ABS_STATUS        (8)
#define SEN_DISP_VERSION_INFO  (0x01)  //macro for version status check


/*Entry types in data record from SCC */
#define SEN_DISP_ENTRY_TYPE_DIAGNOSIS  (0)
#define SEN_DISP_ENTRY_TYPE_ODO_DATA   (1)
#define SEN_DISP_ENTRY_TYPE_GYRO_DATA  (2)
#define SEN_DISP_ENTRY_TYPE_GYRO_TEMP  (3)
#define SEN_DISP_ENTRY_TYPE_ACC_DATA   (4)
#define SEN_DISP_ENTRY_TYPE_ACC_TEMP   (5)
#define SEN_DISP_ENTRY_TYPE_ABS_DATA   (6)

/* Size of each entry type in sensor data message */
#define MSG_SIZE_SCC_SENSOR_R_DIAG_MESSAGE            (6)
#define MSG_SIZE_SCC_SENSOR_R_ODO_DATA                (7)
#define MSG_SIZE_SCC_SENSOR_R_GYRO_DATA               (10)
#define MSG_SIZE_SCC_SENSOR_R_ACC_DATA                (10)
#define MSG_SIZE_SCC_SENSOR_R_GYRO_TEMP               (6)
#define MSG_SIZE_SCC_SENSOR_R_ACC_TEMP                (6)
#define MSG_SIZE_SCC_SENSOR_R_ABS_DATA                (9)

/* Size of each field type  */

/* In config message */
#define SEN_DISP_FIELD_SIZE_CONFIG_SENSOR_DATA_INTERVALL  (2)
#define SEN_DISP_FIELD_SIZE_CONFIG_SENSOR_TYPE (1)
/* In data message */
#define SEN_DISP_FIELD_SIZE_NUM_ENTRTIES (2)
#define SEN_DISP_FIELD_SIZE_TIME_STAMP (3)
/* Diag data */
#define SEN_DISP_FIELD_SIZE_DIAG_MESSAGE (2)
/* Odo Data */
#define SEN_DISP_FIELD_SIZE_ODO_WHEEL_CNTR (2)
#define SEN_DISP_FIELD_SIZE_ODO_DIRETION_FLAG (1)
/* Gyro data */
#define SEN_DISP_FIELD_SIZE_EACH_GYRO_DATA (2)
/* ACC data */
#define SEN_DISP_FIELD_SIZE_EACH_ACC_DATA (2)
/* Temp Update */
#define SEN_DISP_FIELD_SIZE_TEMP (2)

/* ABS Data */
#define SEN_DISP_FIELD_SIZE_ABS_RR_WHEEL_CNTR (2)
#define SEN_DISP_FIELD_SIZE_ABS_RL_WHEEL_CNTR (2)
#define SEN_DISP_FIELD_SIZE_ABS_STATUS_FLAGS  (1)
/* Bit fields indicating ABS data status */
#define SEN_DISP_ABS_RR_STATUS_BIT_FIELD          (0x01)
#define SEN_DISP_ABS_RL_STATUS_BIT_FIELD          (0x02)
#define SEN_DISP_ABS_BIT_FIELD_DRIV_DIRE_FWD      (0x01 << 2)
#define SEN_DISP_ABS_BIT_FIELD_DRIV_DIRE_REV      (0x02 << 2)
#define SEN_DISP_ABS_BIT_FIELD_MASK_DRIV_DIRE_REV (0x0C)
#define SEN_DISP_ABS_BIT_FIELD_DRIV_DIRE_INVALID  (0x00)

/* -- Status code definitions ------------------------------------------- */
#define SENSOR_C_U8_ABS_STATUS_NORMAL         0    /* Data is trustworthy. */
#define SENSOR_C_U8_ABS_STATUS_DATA_INVALID   0xf9 /* Consistency of data cannot be guaranteed. */
#define SENSOR_C_U8_ABS_STATUS_INTERNAL_ERROR 0xfa /* Internal error in the device driver. */
#define SENSOR_C_U8_ABS_STATUS_NO_INFO        0xfb /* Information not available */
#define SENSOR_C_U8_ABS_STATUS_NO_MOVEMENT    0xfc /* Vehicle is not moving. */
#define SENSOR_C_U8_ABS_STATUS_ERROR          0xfd /* The vehicle has reported an error. */
#define SENSOR_C_U8_ABS_STATUS_DISCONNECTED   0xfe /* The vehicle's CAN bus is inactive. */
#define SENSOR_C_U8_ABS_STATUS_UNKNOWN        0xff /* The status could not be determined. */
#define SENSOR_C_U8_ABS_DIR_UNKNOWN 0 /* The wheel's direction of rotation cannot be determined. */
#define SENSOR_C_U8_ABS_DIR_FORWARD 1 /*  */
#define SENSOR_C_U8_ABS_DIR_REVERSE 2 /*  */



unsigned int GetOdoCanCounter(unsigned int u32TimeStamp, unsigned short int u16WheelCounter, unsigned char enDirection)
{
	static unsigned int u32PreTimeStamp = 0;
	static unsigned short u16PreWheelCounter = 0;
	static bool bInited = false;

	unsigned short int odometerCanValue = 0;

	if(!bInited)
	{
		u32PreTimeStamp = u32TimeStamp;
		u16PreWheelCounter = (u16WheelCounter);
		odometerCanValue = u16PreWheelCounter;
		bInited = true;
	}
	else
	{
		short int diffCounter = 0;
		unsigned short int newWrapCount = (u16WheelCounter);

		diffCounter = newWrapCount - u16PreWheelCounter;
		u16PreWheelCounter = newWrapCount;


		if(diffCounter < 0)
		{
		   diffCounter += 0x400;  //wrap around
		}

		if(diffCounter < 0)
		{
			syslog(LOG_DEBUG,"GetOdoCanCounter: Wrong wrap around value!!!!");
		}

		odometerCanValue = abs(diffCounter);

//		if (1 == enDirection)
//		{
//			if(diffCounter < 0)
//			{
//			   diffCounter += 0x400;  //wrap around
//			}
//
//			if(diffCounter < 0)
//			{
//				syslog(LOG_DEBUG,"GetOdoCanCounter: Wrong wrap around value!!!!");
//			}
//
//			odometerCanValue = abs(diffCounter);
//		}
//		else
//		{
//			//T_e8_OdometerDirection__ODOMDIR_REVERSE, the odometer counter decrease, so
//			if(diffCounter > 0)
//			{
//				diffCounter = (0x400 - diffCounter);
//			}
//
//			odometerCanValue = abs(diffCounter);
//		}

	}

	return odometerCanValue;
}

void SenDisp_vDispatchOdoData(unsigned char* u8RawOdoData)
{
   unsigned int u32TimeStamp = 0;
   unsigned short int u16WheelCounter = 0;
   unsigned char enDirection = 0; //1: forward; 2: REVERSE

   /* Extract Time stamp */
   memcpy( &u32TimeStamp,
                      &u8RawOdoData[SEN_DISP_OFFSET_TIME_STAMP],
                      SEN_DISP_FIELD_SIZE_TIME_STAMP );

   /* Extract wheel counter */
   memcpy( &u16WheelCounter,
                      &u8RawOdoData[SEN_DISP_OFFSET_ODO_WHEEL_CNTR],
                      SEN_DISP_FIELD_SIZE_ODO_WHEEL_CNTR );

   /* Extract direction flag */
   memcpy( &enDirection,
                      &u8RawOdoData[SEN_DISP_OFFSET_ODO_DIRECTION_FLAG],
                      SEN_DISP_FIELD_SIZE_ODO_DIRETION_FLAG );

   u32TimeStamp &= 0x00FFFFFF;

   syslog(LOG_DEBUG,"Odo Data Recvd: whl cntr %u -- TS: %u -- Dir %d",
                      u16WheelCounter, u32TimeStamp,
                      enDirection);

   unsigned short int u16CANCounter = 0; //this used for calc vehicle speed
   static unsigned int u32PreTimeStamp = 0;
   unsigned int diffTimestamp = 0;

		if(u32TimeStamp > u32PreTimeStamp)
		{
			diffTimestamp = u32TimeStamp - u32PreTimeStamp;
		}
		else
		{
			diffTimestamp = (unsigned int)((u32TimeStamp - u32PreTimeStamp) + 0xFFFFFFFF );
		}
		u32PreTimeStamp = u32TimeStamp;

		u16CANCounter = GetOdoCanCounter(u32TimeStamp, u16WheelCounter, enDirection);
		//convert wrap around odometer counter into i/p CAN value and vehicle speed
		// 1. i/p CAN value -> KMH(PoS_OdoCanSpeed) * 9/160
		// 2. KMH -> m/s * 5/18
		// 3. take it every SI i.e; 50ms(POS_CFG_ODO_SAMPLING_INTERVAL_MS) or 150ms (POS_CFG_ODO_INIT_SAMPLING_INTERVAL_MS) (time)
		// 4. Pulse per revolution -> 100(POS_CFG_ODO_CAN_SIGNAL_PULSES_PER_REVOLUTION)
		// 5. Wheel circumference -> 2(POS_CFG_ODO_CAN_SIGNAL_WHEEL_CIRCUMFERENCE)
		// 6. calculation:
		/*
		  * C : counter
		  * cv: CAN value
		  * SI: Sample interval
		  * vs: vehicle speed(in km/h)
		  *
		         cv            cv * 9/160 * 5/18  * SI/1000 * 100          cv * SI
		 	 C = -- * P * time = ----------------------------------   =  ------------
		         U                             2                         160 * 8

		 	 cv = 1280*C/SI

		 	 vs = cv*9/160

			 Counter = speedinkmph * PPR*time/wheel_circumference
			 u16CANCounter = speed * 100 * SI ( in ms)/2
			 speed = (u16CANCounter * 2 (in mtr))/(100 * SI(in ms))
			 speed in kmph = (u16CANCounter * 72)/(SI in ms)
		*/

   //Trace out the vehicle speed for testing, in KM/H

      	double dblVehicleSpeed = (double)(u16CANCounter* 72.0)/(diffTimestamp);
  // 	double dblVehicleSpeed = (double)u16CANCounter*9.0/160.0;

   	syslog(LOG_DEBUG,"Odo CanValue: %u, VehicleSpeed: %f", u16CANCounter, dblVehicleSpeed);
}

void SenDisp_vDispatchGyroData(unsigned char* u8RawGyroData)
{
   unsigned int	u32TimeStamp = 0;
   unsigned short int u16Gyro_r = 0;
   unsigned short int u16Gyro_s = 0;
   unsigned short int u16Gyro_t = 0;

   /* Extract Time stamp */
   memcpy( &u32TimeStamp, &u8RawGyroData[SEN_DISP_OFFSET_TIME_STAMP], SEN_DISP_FIELD_SIZE_TIME_STAMP );

   /* Extract gyro R value */
   memcpy( &u16Gyro_r, &u8RawGyroData[ SEN_DISP_OFFSET_GYRO_R_DATA], SEN_DISP_FIELD_SIZE_EACH_GYRO_DATA );

   /* Extract gyro S value */
   memcpy( &u16Gyro_s, &u8RawGyroData[SEN_DISP_OFFSET_GYRO_S_DATA], SEN_DISP_FIELD_SIZE_EACH_GYRO_DATA );

   /* Extract gyro T value */
   memcpy( &u16Gyro_t, &u8RawGyroData[SEN_DISP_OFFSET_GYRO_T_DATA], SEN_DISP_FIELD_SIZE_EACH_GYRO_DATA );



   syslog(LOG_DEBUG,"GYRO data:  R: %u -- S: %u -- T: %u -- TS: %u", u16Gyro_r,u16Gyro_s,u16Gyro_t, u32TimeStamp );

	/*	Note:
	 * 		R, S, T coordinate into raw data of gyro sensor
	 * 		1. The offset in vd_sesnor:
	 * 			R_AccVal = R_Rawdata + (0x10000/2)
	 * 		2. Convert back from vd_sensor value to raw data of gyro sensor
	 * 			R_Rawdata = R_AccVal - (0x10000/2)
	 *
	 *	So according the interface define, gyro accl data is unsigned value, so need third party navigation convert back to raw data
	 * */

}


void SenDisp_vDispatchAccData(unsigned char* u8RawAccData)
{

   unsigned int u32TimeStamp = 0;
   unsigned short int u16Acc_x = 0;
   unsigned short int u16Acc_y = 0;
   unsigned short int u16Acc_z = 0;


   /* Extract Time stamp */
   memcpy( &u32TimeStamp,
                      &u8RawAccData[SEN_DISP_OFFSET_TIME_STAMP],
                      SEN_DISP_FIELD_SIZE_TIME_STAMP);

   /* Extract ACC R value */
   memcpy( &u16Acc_x,
                      &u8RawAccData[SEN_DISP_OFFSET_ACC_R_DATA],
                      SEN_DISP_FIELD_SIZE_EACH_ACC_DATA);

   /* Extract ACC S value */
   memcpy( &u16Acc_y,
                      &u8RawAccData[SEN_DISP_OFFSET_ACC_S_DATA],
                      SEN_DISP_FIELD_SIZE_EACH_ACC_DATA);

   /* Extract ACC T value */
   memcpy( &u16Acc_z,
                      &u8RawAccData[SEN_DISP_OFFSET_ACC_T_DATA],
                      SEN_DISP_FIELD_SIZE_EACH_ACC_DATA);

//   syslog(LOG_DEBUG, "ACC data: : X: %u -- Y: %u -- Z: %u -- TS: %u", u16Acc_x,u16Acc_y,u16Acc_z, u32TimeStamp);
}

void SenDisp_vDispatchGyroTemp(unsigned char* u8RawGyroTemp)
{
   unsigned short int u16GyroTemp = 0;

   /* Extract Temperature */
   memcpy( &u16GyroTemp, &u8RawGyroTemp[SEN_DISP_OFFSET_GYRO_TEMP], SEN_DISP_FIELD_SIZE_TEMP );

   syslog(LOG_DEBUG,"Gyro tmp %u", u16GyroTemp);
}
/********************************************************************************
* FUNCTION     : SenDisp_vDispatchAccTemp
*
* PARAMETER    : tU32 u32Offset: Offset to the record to be extracted
*
* RETURNVALUE  : None
*
* DESCRIPTION  : This will extract one Acc Temp and sends it to driver.
* HISTORY      :  13.JAN.2014| Initial Version    |Ramachandra Madhu Kiran (CM-AI/PJ-CF35)
**********************************************************************************/
void SenDisp_vDispatchAccTemp(unsigned char* u8RawAccTemp)
{
   unsigned short int u16AccTemp = 0;

   /* Extract Temperature */
   memcpy( &u16AccTemp, &u8RawAccTemp[SEN_DISP_OFFSET_ACC_TEMP], SEN_DISP_FIELD_SIZE_TEMP);

   syslog(LOG_DEBUG, "ACC tmp %u", u16AccTemp );
}


void SenDisp_vDispatchAbsData(unsigned char* u8RawAbsTemp)
{

   unsigned int u32TimeStamp = 0;
   unsigned int u32CounterRearRight = 0;
   unsigned int u32CounterRearLeft = 0;
   unsigned char u8StatusRearRight = 0;
   unsigned char u8StatusRearLeft = 0;
   unsigned char u8DirectionRearLeft = 0;
   unsigned char u8DirectionRearRight = 0;

   unsigned char u32CounterFrontLeft = 0;
   unsigned char u32CounterFrontRight = 0;
   unsigned char u8DirectionFrontLeft = 0;
   unsigned char u8DirectionFrontRight = 0;
   unsigned char u8StatusFrontLeft = 0;
   unsigned char u8StatusFrontRight = 0;

   unsigned char u8Status = 0;

   //Extract Time stamp
   memcpy( &u32TimeStamp,
                      &u8RawAbsTemp[SEN_DISP_OFFSET_TIME_STAMP],
                      SEN_DISP_FIELD_SIZE_TIME_STAMP );

   //Extract Rear Right wheel counter
   memcpy( &u32CounterRearRight,
                      &u8RawAbsTemp[SEN_DISP_OFFSET_ABS_RR_WHEEL_CNTR],
                      SEN_DISP_FIELD_SIZE_ABS_RR_WHEEL_CNTR );

   //Extract Rear Left wheel counter
   memcpy( &u32CounterRearLeft,
                      &u8RawAbsTemp[SEN_DISP_OFFSET_ABS_RL_WHEEL_CNTR],
                      SEN_DISP_FIELD_SIZE_ABS_RL_WHEEL_CNTR );

   //Extract status flag
   memcpy( &u8Status,
                      &u8RawAbsTemp[SEN_DISP_OFFSET_ABS_STATUS],
                      SEN_DISP_FIELD_SIZE_ABS_STATUS_FLAGS );

//   syslog(LOG_DEBUG,"Received ABS status = %d",u8Status);

   //Check rear right wheel counter data validity
   if( u8Status & SEN_DISP_ABS_RR_STATUS_BIT_FIELD )
   {
      u8StatusRearRight = SENSOR_C_U8_ABS_STATUS_DATA_INVALID;
   }
   else
   {
      u8StatusRearRight = SENSOR_C_U8_ABS_STATUS_NORMAL;
   }
   //Check rear left wheel counter data validity
   if( u8Status & SEN_DISP_ABS_RL_STATUS_BIT_FIELD )
   {
      u8StatusRearLeft = SENSOR_C_U8_ABS_STATUS_DATA_INVALID;
   }
   else
   {
      u8StatusRearLeft = SENSOR_C_U8_ABS_STATUS_NORMAL;
   }
   //Update driving direction(check for 00-undefined;01-forward;10-reverse;11-invalid)
   if( ( u8Status & SEN_DISP_ABS_BIT_FIELD_MASK_DRIV_DIRE_REV ) ==
                                SEN_DISP_ABS_BIT_FIELD_DRIV_DIRE_FWD )
   {
      u8DirectionRearLeft  = SENSOR_C_U8_ABS_DIR_FORWARD;
      u8DirectionRearRight = SENSOR_C_U8_ABS_DIR_FORWARD;
   }
   else if( ( u8Status & SEN_DISP_ABS_BIT_FIELD_MASK_DRIV_DIRE_REV ) ==
                                SEN_DISP_ABS_BIT_FIELD_DRIV_DIRE_REV )
   {
      u8DirectionRearLeft  = SENSOR_C_U8_ABS_DIR_REVERSE;
      u8DirectionRearRight = SENSOR_C_U8_ABS_DIR_REVERSE;
   }
   else
   {
      u8DirectionRearLeft  = SENSOR_C_U8_ABS_DIR_UNKNOWN;
      u8DirectionRearRight = SENSOR_C_U8_ABS_DIR_UNKNOWN;
   }

   //The below info is not available so update appropriate values indicating unavailability.
   u32CounterFrontLeft  = 0;
   u32CounterFrontRight = 0;
   u8DirectionFrontLeft  = SENSOR_C_U8_ABS_DIR_UNKNOWN;
   u8DirectionFrontRight = SENSOR_C_U8_ABS_DIR_UNKNOWN;
   u8StatusFrontLeft     = SENSOR_C_U8_ABS_STATUS_UNKNOWN;
   u8StatusFrontRight    = SENSOR_C_U8_ABS_STATUS_UNKNOWN;
   //u16ErrorCounter will be updated after validating record.

//   syslog(LOG_DEBUG, "Ts %u,CRR %u,SRR %u,DRR %u,CRL %u,SRL %u,DRL %u",
//                      u32TimeStamp,
//                      u32CounterRearRight,u8StatusRearRight,u8DirectionRearRight,
//                      u32CounterRearLeft,u8StatusRearLeft,u8DirectionRearLeft);
}

/********************************************************************************
* FUNCTION     : vHandleSensorData
*
* PARAMETER    : NONE
*
* RETURNVALUE  : OSAL_OK  on success
*                OSAL_ERROR on Failure
*
* DESCRIPTION  : Parses complete data record entry by entry.
*                Actual sensor data will be added to respective ring buffer.
*                Sensor temperature will be sent to respective sensor driver
*                via proper message queue.
*
* HISTORY      : 19.MAR.2013| Initial Version      |Madhu Kiran Ramachandra (RBEI/ECF5)
**********************************************************************************/
void vClientHandleSensorData(unsigned char* u8RecvBuffer, unsigned int u32RecvdMsgSize)
{

   int s32ErrChk = -1;
   unsigned short int u16NumSenRecords;
   unsigned short int u32OffsetNextRecord = SEN_DISP_OFFSET_FIRST_RECORD_IN_DATA_MSG;
   unsigned short int u32NumRecordsParsed;
   unsigned short int u16DiagMessage;
   bool bErrOccurred = false;

   if( u32RecvdMsgSize >= ( SEN_DISP_OFFSET_NUM_OF_ENTRIES_IN_DATA_MSG + SEN_DISP_FIELD_SIZE_NUM_ENTRTIES ))
   {
      /*Get the number of entries in data record.*/
      memcpy( &u16NumSenRecords,&u8RecvBuffer[SEN_DISP_OFFSET_NUM_OF_ENTRIES_IN_DATA_MSG],SEN_DISP_FIELD_SIZE_NUM_ENTRTIES );

      syslog(LOG_DEBUG,"Number of Entries in Data message %u", u16NumSenRecords );

      //TODO This is a filter for the noice data which buffered in the memory,
      //there are max 10KB sensor data buffer when socket disconnect, so if do not need the buffer data, should filter it
      // I just give a example filter size.....
      if(u16NumSenRecords >= 30)
      {
    	  return;
      }

      /* Parse the record entry by entry */
      for( u32NumRecordsParsed =0; ((u32NumRecordsParsed < u16NumSenRecords) && (false == bErrOccurred)); u32NumRecordsParsed++ )
      {
         switch( u8RecvBuffer[u32OffsetNextRecord + SEN_DISP_OFFSET_ENTRY_TYPE] )
         {
            /* We don't know what to do with diagnosis info. Just trace it out */
            case SEN_DISP_ENTRY_TYPE_DIAGNOSIS :
            {
               if ( u32RecvdMsgSize >= (u32OffsetNextRecord + MSG_SIZE_SCC_SENSOR_R_DIAG_MESSAGE -1) )
               {
                  /* Extract diagnosis message */
                  memcpy( &u16DiagMessage, &u8RecvBuffer[u32OffsetNextRecord + SEN_DISP_OFFSET_DIAGNOSIS_MESSAGE], SEN_DISP_FIELD_SIZE_DIAG_MESSAGE );
                  syslog(LOG_DEBUG,"Diagnosis message: %x", u16DiagMessage );
                  /* Update from where next entry starts */
                  u32OffsetNextRecord += MSG_SIZE_SCC_SENSOR_R_DIAG_MESSAGE;
               }
               else
               {
                  syslog(LOG_DEBUG,"Improper Data msg:line %u", __LINE__ );
                  bErrOccurred = true;
               }
               break;
            }
            /* We got Odo data. Extract and add it to ring buffer */
            case SEN_DISP_ENTRY_TYPE_ODO_DATA :
            {
               if ( u32RecvdMsgSize >= (u32OffsetNextRecord + MSG_SIZE_SCC_SENSOR_R_ODO_DATA -1) )
               {
                  /* Extract, validate and add data to ring buffer */
                  SenDisp_vDispatchOdoData(&u8RecvBuffer[u32OffsetNextRecord]);
                  /* Update from where next entry starts */
                  u32OffsetNextRecord += MSG_SIZE_SCC_SENSOR_R_ODO_DATA;
               }
               else
               {
                  syslog(LOG_DEBUG,"Improper Data msg:line %u", __LINE__ );
                  bErrOccurred = true;
               }

               break;
            }
            /* We got Gyro data. Extract and add it to ring buffer */
            case SEN_DISP_ENTRY_TYPE_GYRO_DATA :
            {

               if ( u32RecvdMsgSize >= (u32OffsetNextRecord + MSG_SIZE_SCC_SENSOR_R_GYRO_DATA -1) )
               {
                  /* Extract, validate and add data to ring buffer */
                  SenDisp_vDispatchGyroData(&u8RecvBuffer[u32OffsetNextRecord]);
                  /* Update from where next entry starts */
                  u32OffsetNextRecord += MSG_SIZE_SCC_SENSOR_R_GYRO_DATA;
               }
               else
               {
                  syslog(LOG_DEBUG,"Improper Data msg:line %u", __LINE__ );
                  bErrOccurred = true;
               }
               break;
            }

            /* We got GYRO temperature update. Send it to driver via message queue */
            case SEN_DISP_ENTRY_TYPE_GYRO_TEMP :
            {
               if ( u32RecvdMsgSize >= (u32OffsetNextRecord + MSG_SIZE_SCC_SENSOR_R_GYRO_TEMP -1) )
               {
                  /* Extract and dispatch Gyro temp to Gyro driver */
                  SenDisp_vDispatchGyroTemp(&u8RecvBuffer[u32OffsetNextRecord]);
                  /* Update from where next entry starts */
                  u32OffsetNextRecord += MSG_SIZE_SCC_SENSOR_R_GYRO_TEMP;
               }
               else
               {
                  syslog(LOG_DEBUG,"Improper Data msg:line %u", __LINE__ );
                  bErrOccurred = true;
               }
               break;
            }
            /* We got ACC data. Extract and add it to ring buffer */
            case SEN_DISP_ENTRY_TYPE_ACC_DATA :
            {

               if ( u32RecvdMsgSize >= (u32OffsetNextRecord + MSG_SIZE_SCC_SENSOR_R_ACC_DATA -1) )
               {
                  /* Extract, validate and add data to ring buffer */
                  SenDisp_vDispatchAccData(&u8RecvBuffer[u32OffsetNextRecord]);
                  /* Update from where next entry starts */
                  u32OffsetNextRecord += MSG_SIZE_SCC_SENSOR_R_ACC_DATA;
               }
               else
               {
                  syslog(LOG_DEBUG,"Improper Data msg:line %u", __LINE__ );
                  bErrOccurred = true;
               }
               break;
            }
            /* We got ACC temperature update. Send it to driver via message queue */
            case SEN_DISP_ENTRY_TYPE_ACC_TEMP :
            {

               if ( u32RecvdMsgSize >= (u32OffsetNextRecord + MSG_SIZE_SCC_SENSOR_R_ACC_TEMP -1) )
               {
                  /* Extract and dispatch Gyro temp to Gyro driver */
                  SenDisp_vDispatchAccTemp(&u8RecvBuffer[u32OffsetNextRecord]);
                  /* Update from where next entry starts */
                  u32OffsetNextRecord += MSG_SIZE_SCC_SENSOR_R_ACC_TEMP;
               }
               else
               {
                  syslog(LOG_DEBUG,"Improper Data msg:line %u", __LINE__ );
                  bErrOccurred = true;
               }
               break;
            }
            /* We got ABS data. Extract and add it to ring buffer */
            case SEN_DISP_ENTRY_TYPE_ABS_DATA :
            {
               if( u32RecvdMsgSize >= (u32OffsetNextRecord + MSG_SIZE_SCC_SENSOR_R_ABS_DATA -1) )
               {
                  /* Extract, validate and add data to ring buffer */
                  SenDisp_vDispatchAbsData(&u8RecvBuffer[u32OffsetNextRecord]);
                  /* Update from where next entry starts */
                  u32OffsetNextRecord += MSG_SIZE_SCC_SENSOR_R_ABS_DATA;
               }
               else
               {
                  syslog(LOG_DEBUG,"Improper Data msg:line %u", __LINE__ );
                  bErrOccurred = true;
               }
               break;
            }

            default:
            {
               /* If we miss even a single entry, we can't proceed further. So we have to drop the whole record. */
               syslog(LOG_DEBUG, "!!! Encountered unknown Entry type" );
               bErrOccurred = true;
            }
         }
      }
   }
   else
   {
      syslog(LOG_DEBUG,"Improper Data msg: size %d line %u", s32ErrChk, __LINE__ );
   }
}

void *sensor_socket_client_main(void *arg)
{
    SocketClient client;
    bool bConnected = false;

	ssize_t rev_len = 0;
	unsigned char str_rev[SOCKET_MAX_BUFFER_SIZE+1] = {0};

	while(1)
	{
		if(!bConnected)
		{
			if(client.connectToServer("/tmp/sock_sensors") == 0)
			{
				bConnected = true;
			}
			else
			{
				syslog(LOG_DEBUG, "connectToServer error!\n");
				sleep(1);
			}
		}
		else
		{

            rev_len = client.getMsg(str_rev, SOCKET_MAX_BUFFER_SIZE);
			if(rev_len > 0)
			{
				syslog(LOG_DEBUG, "Sensor recv Lenth: %lu", rev_len);
				for(ssize_t i = 0; i < rev_len; i++)
				{
					printf("%02x", str_rev[i]);
				}
				printf("\n");
				syslog(LOG_DEBUG, "Sensor Recv Item End!\n");

				vClientHandleSensorData(str_rev, rev_len);

				//clear the data after processed
				memset(str_rev, 0, SOCKET_MAX_BUFFER_SIZE+1);
			}
			else
			{
				syslog(LOG_DEBUG, "recv error\n");
			}
		}

	}
}

void sensor_socket_client_init()
{
	pthread_t id_1;
	pthread_attr_t attr;

	int ret;

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

	ret=pthread_create(&id_1, &attr,sensor_socket_client_main,NULL);
	if(ret!=0)
	{
		syslog(LOG_DEBUG, "Create sensor_socket_client_main error!\n");
	    return;
	}
}

static bool gnss_running = false;
static pthread_t gnss_thread_id = 0;
static void* gnss_userdata = NULL;
static gnss_socket_client_callback gnss_callback = NULL;

void *gnss_socket_client_main(void *arg) {
    SocketClient client;
    bool bConnected = false;

    ssize_t rev_len = 0;
    char str_rev[SOCKET_MAX_BUFFER_SIZE+1];
    char str_stc[10];

    while(gnss_running) {
        if(!bConnected) {
            if(client.connectToServer("/tmp/sock_gnss") == 0) {
                bConnected = true;
            } else {
                syslog(LOG_DEBUG, "connectToGnssServer error!\n");
                sleep(1);
            }
        } else {
            rev_len = client.getMsg(str_rev, SOCKET_MAX_BUFFER_SIZE);
            if(rev_len > 0) {
                if (rev_len < 5) {
                    syslog(LOG_DEBUG, "invalid message size\n");
                } else {
                    unsigned int u32TimeStamp;
                    memcpy( &u32TimeStamp, &str_rev[1], 4 );
                    syslog(LOG_DEBUG, "GNSS recv: Len:%lu, 0x%02x, TS: %lu, 0x%02x%02x%02x%02x,\n%s", rev_len, str_rev[0], u32TimeStamp, str_rev[4],str_rev[3],str_rev[2],str_rev[1],str_rev+5);
                    //SKT::Gpscallback *cGpsCallback = SKT::Gpscallback::sharedInstance();
                    //cGpsCallback->sendGpsDataSignal(str_rev);
                    //memset(str_rev, 0, SOCKET_MAX_BUFFER_SIZE + 1);


                    if (gnss_callback) {
                        gnss_callback(u32TimeStamp, str_rev + 5, rev_len - 5, gnss_userdata);
                    }
                }
            } else {
                syslog(LOG_DEBUG, "recv error\n");
            }
        }
    }
}
bool checksum(char *sentence) {
	printf("sentence [%s] = \n", sentence);
	int sum = '\0';
	char c, *p = sentence, csum[3];
	while (*p != NULL && (c = *p++) != '*') { sum ^= c; }
	sprintf(csum, "%02X", sum);
	return (strncmp(csum, p, 2) == 0);
}



string* StringSplit(string strTarget, string strTok) {
	int nCutPos;
	int nIndex = 0;
	string* strResult = new string[20];

	while ((nCutPos = strTarget.find_first_of(strTok)) != strTarget.npos) {
		if (nCutPos > 0) { strResult[nIndex++] = strTarget.substr(0, nCutPos); }
		strTarget = strTarget.substr(nCutPos+1);
	}

	if(strTarget.length() > 0) { strResult[nIndex++] = strTarget.substr(0, nCutPos); }
	return strResult;
}

void gnss_socket_client_init()
{
    gnss_running = false;
    gnss_thread_id = 0;
}

void gnss_socket_client_start(gnss_socket_client_callback callback, void *userdata)
{
    if (gnss_running) {
        return;
    }

    gnss_callback = callback;
    gnss_userdata = userdata;
    gnss_running = true;

    int ret = pthread_create(&gnss_thread_id, NULL, gnss_socket_client_main, NULL);
    if (ret != 0) {
        syslog(LOG_DEBUG, "Create gnss_socket_client_main error!\n");
    }
}

void gnss_socket_client_stop()
{
    if (!gnss_running) {
        return;
    }

    gnss_running = false;
    pthread_join(gnss_thread_id, NULL);
    gnss_thread_id = 0;
}
