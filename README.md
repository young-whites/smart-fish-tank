# 智能鱼缸系统 (Smart Fish Tank)

基于 STM32F103 + ESP-01S 的智能鱼缸控制系统，支持传感器实时监测、自动/手动控制、WiFi 远程通信和 Android APP 联动。

## 系统架构

```
┌──────────────┐     UART      ┌──────────┐    WiFi     ┌──────────────┐
│    STM32     │◄─────────────►│ ESP-01S  │◄──────────►│  Android APP │
│  (主控制器)   │    AT 指令    │ (WiFi)   │  TCP:8080   │  (Kotlin)    │
└──────────────┘               └──────────┘             └──────────────┘
       │
       ├── DS18B20 (温度传感器)
       ├── PH 传感器 (ADC)
       ├── 水位传感器 (ADC)
       ├── 空气质量传感器 (ADC)
       ├── OLED 显示屏 (I2C)
       ├── 继电器 x4 (加热/加水/排水/增氧)
       └── 舵机 (喂食)
```

## 硬件清单

| 模块 | 型号 | 接口 | 说明 |
|------|------|------|------|
| 主控 | STM32F103C8T6 | - | ARM Cortex-M3, 72MHz |
| WiFi | ESP-01S | USART1 | AT 指令模式, TCP 服务器 |
| 温度 | DS18B20 | 单总线 | 水温检测, -55~125°C |
| PH | PH 传感器模块 | ADC (PA0) | 0~14 pH |
| 水位 | 水位传感器 | ADC (PA1) | 0~100% |
| 空气 | MQ-135 | ADC (PA4) | 空气质量检测 |
| 显示 | 0.96" OLED | I2C (PB6/PB7) | 128x64, SSD1306 |
| 继电器 | 4路继电器模块 | GPIO | 加热/加水/排水/增氧 |
| 舵机 | SG90 | PWM | 自动喂食 |
| 蜂鸣器 | 有源蜂鸣器 | GPIO | 报警提示 |
| LED | LED 指示灯 | GPIO | 状态指示 |

## 项目结构

```
├── 1.Schematic/                  # 电路原理图
├── 2.stm32Code/
│   ├── MDK_User/                 # 用户代码 (main.c, 中断)
│   ├── MDK_APP/                  # 应用层
│   │   ├── TimingSet.c/h        # 定时任务调度 (1ms/10ms/50ms/500ms/1s)
│   │   ├── GeneralTIM.c/h       # 通用定时器
│   │   └── MyTypedef.c/h        # 数据结构定义
│   ├── MDK_Hardware/             # 硬件驱动
│   │   ├── ESP01S/               # WiFi 模块驱动 + 通信协议
│   │   ├── OLED/                 # OLED 显示屏驱动
│   │   ├── DS18B20/              # 温度传感器驱动
│   │   ├── Relay/                # 继电器控制
│   │   ├── Servo/                # 舵机控制 (喂食)
│   │   ├── Beep/                 # 蜂鸣器报警
│   │   ├── led/                  # LED 指示灯
│   │   ├── Key/                  # 按键扫描
│   │   └── ADC/                  # ADC 采集 (PH/水位/空气质量)
│   ├── MDK_System/               # 系统层
│   │   ├── usart/                # 串口驱动
│   │   ├── adc/                  # ADC 初始化
│   │   ├── systick_delay/        # 延时函数
│   │   └── sys/                  # 系统状态
│   └── MDK_STM32F10x_FWLib/      # STM32 标准外设库
└── README.md
```

## 通信协议

### 帧格式

```
HEAD(0xAA) | CMD(1B) | LEN(1B) | PAYLOAD(NB) | SUM(1B) | END(0x55)
```

- **校验和**: (CMD + LEN + ΣPAYLOAD) & 0xFF

### 命令列表

| 方向 | CMD | 说明 | 数据长度 |
|------|-----|------|----------|
| MCU→APP | 0x01 | 传感器数据 | 14B |
| MCU→APP | 0x02 | 设备状态 | 5B |
| MCU→APP | 0x03 | 报警事件 | 2B |
| APP→MCU | 0x10 | 查询状态 | 0B |
| APP→MCU | 0x11 | 切换模式 | 1B |
| APP→MCU | 0x12 | 继电器控制 | 2B |
| APP→MCU | 0x13 | 阈值设置 | 20B |
| APP→MCU | 0x14 | 触发喂食 | 0B |
| APP→MCU | 0x15 | 报警开关 | 1B |
| APP→MCU | 0x16 | 喂食间隔 | 2B |

### 传感器数据帧 (CMD 0x01, LEN=14)

| 偏移 | 字段 | 类型 | 说明 |
|------|------|------|------|
| 0-3 | WaterTemp | Float LE | 水温 °C |
| 4-7 | PH_Value | Float LE | pH 值 |
| 8 | WaterLevel | UInt8 | 水位 0~100% |
| 9-10 | AirQuality | UInt16 BE | 空气质量 |
| 11 | RunMode | UInt8 | 0=自动, 1=手动 |
| 12-13 | FeedCountdown | UInt16 BE | 喂食倒计时(秒) |

### 设备状态帧 (CMD 0x02, LEN=5)

| 偏移 | 字段 | 说明 |
|------|------|------|
| 0 | relay_state | bit0=加热 bit1=加水 bit2=排水 bit3=增氧 |
| 1 | AlarmEnable | 报警使能 (0/1) |
| 2 | Feeding | 喂食中 (0/1) |
| 3-4 | Reserved | 保留 |

## 功能特性

### 自动模式
- **温度控制**: 低于下限启动加热棒，高于上限关闭
- **水位控制**: 低于下限加水，高于上限排水
- **空气质量**: 超标启动增氧泵
- **自动喂食**: 按设定间隔自动触发舵机喂食
- **报警系统**: 温度/水位/空气/PH 越限触发声光报警

### 手动模式
- 四路继电器独立开关控制
- 手动触发喂食
- 报警使能开关

### WiFi 通信
- ESP-01S 作为 TCP 服务器 (端口 8080)
- 连接失败自动进入离线模式
- 后台每秒自动重试连接
- OLED 主页显示 WiFi 状态 (W+/W-)

### OLED 显示
- **主页**: 温度、空气质量、水位、PH、模式、喂食倒计时、继电器状态、WiFi 状态
- **自动设置页**: 阈值参数调节
- **手动控制页**: 继电器开关、报警开关、喂食状态

## 开发环境

- **IDE**: Keil uVision 5
- **芯片**: STM32F103C8T6
- **外设库**: STM32F10x Standard Peripheral Library
- **编译器**: ARM Compiler V5

## 配置说明

### WiFi 配置

编辑 `2.stm32Code/MDK_Hardware/ESP01S/bsp_esp01s.h`:

```c
#define ESP01S_WIFI_SSID     "你的WiFi名称"
#define ESP01S_WIFI_PASSWORD "你的WiFi密码"
#define ESP01S_TCP_PORT      8080
```

### 默认阈值

编辑 `2.stm32Code/MDK_APP/MyTypedef.c` 中的 `FishTank_Set` 初始化值。

## License

MIT License
