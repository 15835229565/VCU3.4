#include  "includes.h"

void Restart_GPS()
{
 GPS_RESET = 1;
 delayms(3000);
 GPS_RESET = 0;  
}
uint8 compare(uint16 a1,uint32 a2, uint16 b1,uint32 b2)
{
    if(( a1>b1 ) || (( a1 == b1 ) && (a2 >= b2 )))
        return 0;
    else
        return 1;
}


/*
*********************************************************************************************************
*                                               GPSfence()
*
* Description : gps围栏判断，车辆当前的位置位于围栏里面，
*                       应该满足左经度<当前经度<右经度，下纬度<当前纬度<上纬度
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
uint8 GPSfence()
{
    //表示目前为超出gps围栏状态，则将围栏缩小
    if((gCarThreshold.errCode & OUT_GPS_FENCE_EXCEPTION) == OUT_GPS_FENCE_EXCEPTION)
    {
      if( compare(gCarThreshold.LTLatitude1,gCarThreshold.LTLatitude2,gCarInfor.lLatitude1,gCarInfor.lLatitude2))
          return 1;
      if( compare(gCarInfor.lLatitude1,gCarInfor.lLatitude2,gCarThreshold.RBLatitude1,gCarThreshold.RBLatitude2))
          return 1;
      if( compare(gCarInfor.lLongitude1,gCarInfor.lLongitude2,gCarThreshold.LTLongitude1,gCarThreshold.LTLongitude2))
          return 1;
      if( compare(gCarThreshold.RBLongitude1,gCarThreshold.RBLongitude2,gCarInfor.lLongitude1,gCarInfor.lLongitude2))
          return 1;
    }
    //表示目前为未超出gps围栏状态，则将围栏扩大
    else
    {
      if( compare(gCarThreshold.LTLatitude1,gCarThreshold.LTLatitude2+GPS_TOLERATE_VALUE,gCarInfor.lLatitude1,gCarInfor.lLatitude2))
          return 1;
      if( compare(gCarInfor.lLatitude1,gCarInfor.lLatitude2,gCarThreshold.RBLatitude1,gCarThreshold.RBLatitude2-GPS_TOLERATE_VALUE))
          return 1;
      if( compare(gCarInfor.lLongitude1,gCarInfor.lLongitude2,gCarThreshold.LTLongitude1,gCarThreshold.LTLongitude2-GPS_TOLERATE_VALUE))
          return 1;
      if( compare(gCarThreshold.RBLongitude1,gCarThreshold.RBLongitude2+GPS_TOLERATE_VALUE,gCarInfor.lLongitude1,gCarInfor.lLongitude2))
          return 1;
    }
    return 0;
}



