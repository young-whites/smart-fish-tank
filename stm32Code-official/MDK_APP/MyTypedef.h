#ifndef __MYTYPEDEF_H
#define __MYTYPEDEF_H
#include "sys.h"


typedef struct
{
    uint8_t    currentPage;       // 当前主页面 0=数据,1=阈值,2=手动控制
    uint8_t    subPage;           // 当前子页面
    uint8_t    thresholdField;    // 阈值页面: 0=选中下限, 1=选中上限 (主页面1:0-2, 主页面2:0-3, 主页面3:0-3)
    uint8_t    thresholdIndex;    // 褰撳墠閫変腑鐨勯槇鍊煎弬鏁扮储寮? 0-6
    uint8_t    alarmEnable;       // 鎶ヨ?︿娇鑳? 0=鍏抽棴, 1=寮?鍚?
    uint8_t    feeding;           // 鍠傞?熺姸鎬? 0=绌洪棽, 1=鍠傞?熶腑
    uint8_t    relayHeat;         // 鍔犵儹缁х數鍣?
    uint8_t    relayFill;         // 鍔犳按缁х數鍣?
    uint8_t    relayDrain;        // 鎺掓按缁х數鍣?
    uint8_t    relayOxygen;       // 澧炴哀缁х數鍣?
}  Flag_StructType;
extern   Flag_StructType Flag;


typedef struct
{
    float      waterTemp;         // 姘存俯 掳C
    float      phValue;           // pH鍊?
    uint8_t    waterLevel;        // 姘翠綅 0~100%
    uint16_t   airQuality;        // 绌烘皵璐ㄩ噺
    uint8_t    runMode;           // 0=鑷?鍔?, 1=鎵嬪姩
    uint16_t   feedCountdown;     // 鍠傞?熷?掕?℃椂(绉?)

    // 闃堝??
    float      tempLower;         // 娓╁害涓嬮檺
    float      tempUpper;         // 娓╁害涓婇檺
    uint16_t   airQualityMax;     // 绌烘皵璐ㄩ噺涓婇檺
    float      phLower;           // PH涓嬮檺
    float      phUpper;           // PH涓婇檺
    uint8_t    waterLevelMin;     // 姘翠綅涓嬮檺 %
    uint8_t    waterLevelMax;     // 姘翠綅涓婇檺 %
}  Record_StructType;
extern   Record_StructType Record;


#endif /*__MYTYPEDEF_H*/
