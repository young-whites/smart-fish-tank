#ifndef __MYTYPEDEF_H
#define __MYTYPEDEF_H
#include "sys.h"


/* Fish Tank Data Structure */
typedef struct
{
    float    WaterTemp;        /* Water temperature (C) */
    float    PH_Value;         /* PH value */
    uint8_t  WaterLevel;       /* Water level (0-100%) */
    uint16_t AirQuality;       /* Air quality */
} FishTank_DataStruct;


/* Fish Tank Flag Structure */
typedef struct
{
    uint8_t TempHigh_Alarm;        /* Temperature high alarm */
    uint8_t TempLow_Alarm;         /* Temperature low alarm */
    uint8_t PHHigh_Alarm;          /* PH high alarm */
    uint8_t PHLow_Alarm;           /* PH low alarm */
    uint8_t WaterLevel_Alarm;      /* Water level alarm */
    uint8_t WaterLevel_High_Alarm; /* Water level high alarm */
    uint8_t AirQuality_Alarm;      /* Air quality alarm */
    uint8_t RunMode;               /* Run mode (0=auto, 1=manual) */
    uint8_t SetPage;               /* Setting page flag */
    uint8_t Relay_Drain_Manual;    /* 排水手动开关 */
    uint8_t Relay_Oxygen_Manual;   /* 增氧手动开关 */
    uint8_t Relay_Fill_Manual;     /* 加水手动开关 */
    uint8_t Relay_Heat_Manual;     /* 加热手动开关 */
    uint8_t Alarm_Enable;          /* 报警使能 (1=允许报警) */
    uint8_t Feeding;               /* 喂食进行中标志 */
} FishTank_FlagStruct;


/* Fish Tank Setting Structure */
typedef struct
{
    float    TempUpper;            /* Temperature upper limit */
    float    TempLower;            /* Temperature lower limit */
    float    PH_Upper;             /* PH upper limit */
    float    PH_Lower;             /* PH lower limit */
    uint8_t  WaterLevel_Min;       /* Water level lower limit (%) */
    uint8_t  WaterLevel_Max;       /* Water level upper limit (%) */
    uint16_t AirQuality_Max;       /* Air quality alarm threshold */
    uint32_t FeedInterval;         /* Feed interval (seconds) */
} FishTank_SetStruct;


/* Fish Tank Feed Structure */
typedef struct
{
    uint32_t Countdown;            /* Feed countdown (seconds) */
    uint8_t  Feeding;              /* Feeding in progress flag */
    uint8_t  Servo_Open;           /* Servo open flag */
    uint16_t Servo_Timer;          /* Servo open timer (seconds) */
} FishTank_FeedStruct;


extern FishTank_DataStruct  FishTank_Data;
extern FishTank_FlagStruct  FishTank_Flag;
extern FishTank_SetStruct   FishTank_Set;
extern FishTank_FeedStruct  FishTank_Feed;


#endif /*__MYTYPEDEF_H*/
