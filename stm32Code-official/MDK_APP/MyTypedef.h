#ifndef __MYTYPEDEF_H
#define __MYTYPEDEF_H
#include "sys.h"


typedef struct
{
    uint8_t    currentPage;       // Current main page: 0=Home, 1=Threshold, 2=Manual control
    uint8_t    subPage;           // Current sub page
    uint8_t    thresholdField;    // Threshold page: 0=select lower, 1=select upper (Page 1:0-2, Page 2:0-3, Page 3:0-3)
    uint8_t    thresholdIndex;    // Current selected threshold param index 0-6
    uint8_t    alarmEnable;       // Alarm enable: 0=off, 1=on
    uint8_t    feeding;           // Feed status: 0=idle, 1=feeding
    uint8_t    relayHeat;         // Heat relay
    uint8_t    relayFill;         // Fill relay
    uint8_t    relayDrain;        // Drain relay
    uint8_t    relayOxygen;       // Oxygen relay
    uint8_t    sensorError;       // Sensor error flags: bit0=DS18B20, bit1=ADC
    uint16_t   manualTimeout;     // Manual relay safety timeout counter (seconds), 0=disabled
}  Flag_StructType;
extern   Flag_StructType Flag;


typedef struct
{
    float      waterTemp;         // Water temperature C
    float      phValue;           // pH value
    uint8_t    waterLevel;        // Water level 0~100%
    uint16_t   airQuality;        // Air quality
    uint8_t    runMode;           // 0=Auto, 1=Manual
    int16_t    feedCountdown;     // Feed countdown (sec)
    uint16_t   feedInterval;      // Feed interval (sec), default 30

    // Thresholds
    float      tempLower;         // Temperature lower limit
    float      tempUpper;         // Temperature upper limit
    uint16_t   airQualityMax;     // Air quality upper limit
    float      phLower;           // PH lower limit
    float      phUpper;           // PH upper limit
    uint8_t    waterLevelMin;     // Water level lower limit %
    uint8_t    waterLevelMax;     // Water level upper limit %
}  Record_StructType;
extern   Record_StructType Record;


#endif /*__MYTYPEDEF_H*/
