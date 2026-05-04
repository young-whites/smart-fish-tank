# 智能鱼缸系统 — WiFi 通信协议规范

> 版本：V2.0 | 日期：2026-05-02 | 裸机版

---

## 1. 通信架构

```
┌──────────┐     WiFi (TCP)     ┌──────────┐    UART (115200)    ┌──────────┐
│ Android  │ ◄────────────────► │ ESP-01S  │ ◄──────────────────► │  STM32   │
│   APP    │  TCP Socket 直连   │ TCP Server│   AT 指令透传       │  MCU     │
└──────────┘   Port: 8080       └──────────┘                     └──────────┘
```

- ESP-01S 工作在 **TCP Server** 模式，监听端口 **8080**
- APP 作为 **TCP Client** 连接到 ESP-01S 的 IP + Port
- MCU 通过 USART2（PA2=TX, PA3=RX）向 ESP-01S 发送 AT 指令配置和数据透传
- 串口接收采用中断 + 环形缓冲区方式，中断回调中置位 `g_wifi_rx_flag`，主循环中检查标志位处理数据

## 2. 数据帧格式

所有通信采用统一帧格式：

```
┌───────┬──────────┬───────────┬────────────────┬───────┬───────┐
│ HEAD  │ CMD_TYPE │ DATA_LEN  │    PAYLOAD     │  SUM  │  END  │
│ 0xAA  │  1 byte  │  1 byte   │  0~255 bytes   │ 1byte │ 0x55  │
└───────┴──────────┴───────────┴────────────────┴───────┴───────┘
```

| 字段 | 长度 | 说明 |
|------|------|------|
| HEAD | 1B | 帧头 `0xAA` |
| CMD_TYPE | 1B | 命令类型（见下表） |
| DATA_LEN | 1B | PAYLOAD 长度（0~255） |
| PAYLOAD | N B | 数据载荷 |
| SUM | 1B | 校验和：CMD_TYPE + DATA_LEN + 所有 PAYLOAD 字节的低 8 位 |
| END | 1B | 帧尾 `0x55` |

## 3. 命令类型定义

### 3.1 MCU → APP（上报）

| CMD | 名称 | PAYLOAD | 说明 |
|-----|------|---------|------|
| 0x01 | 上报传感器数据 | 见 3.1.1 | 每 1 秒主动推送 |
| 0x02 | 上报设备状态 | 见 3.1.2 | 状态变化时推送 |
| 0x03 | 上报报警事件 | 见 3.1.3 | 报警触发/解除时推送 |

#### 3.1.1 传感器数据上报 (0x01), LEN=14

```
偏移  长度  字段           类型      说明
0     4    WaterTemp      float     水温 (°C)
4     4    PH_Value       float     PH 值
8     1    WaterLevel     uint8     水位 (0~100%)
9     2    AirQuality     uint16    空气质量 (大端序)
11    1    RunMode        uint8     0=自动, 1=手动
12    2    FeedCountdown  uint16    喂食倒计时 (秒, 大端序)
```

#### 3.1.2 设备状态上报 (0x02), LEN=5

```
偏移  长度  字段           类型      说明
0     1    RelayState     uint8     bit0=加热 bit1=加水 bit2=排水 bit3=增氧
1     1    AlarmEnable    uint8     0=关闭, 1=开启
2     1    Feeding        uint8     0=空闲, 1=喂食中
3     2    Reserved       uint16    保留
```

#### 3.1.3 报警事件上报 (0x03), LEN=2

```
偏移  长度  字段           类型      说明
0     1    AlarmType      uint8     见报警类型表
1     1    AlarmStatus    uint8     0=解除, 1=触发
```

报警类型表：
| 值 | 含义 |
|----|------|
| 0x01 | 温度过低 |
| 0x02 | 温度过高 |
| 0x03 | 水位过低 |
| 0x04 | 水位过高 |
| 0x05 | 空气质量超标 |
| 0x06 | PH 过低 |
| 0x07 | PH 过高 |

### 3.2 APP → MCU（控制）

| CMD | 名称 | PAYLOAD | 说明 |
|-----|------|---------|------|
| 0x10 | 查询状态 | 无 (LEN=0) | 请求 MCU 上报当前状态 |
| 0x11 | 切换模式 | 1B: mode | 0=自动, 1=手动 |
| 0x12 | 设备控制 | 2B: relay+state | 手动模式下控制继电器 |
| 0x13 | 设置阈值 | 见 3.2.1 | 自动模式下修改阈值 |
| 0x14 | 触发喂食 | 无 (LEN=0) | 立即触发一次喂食 |
| 0x15 | 设置报警 | 1B: enable | 0=关闭报警, 1=开启报警 |
| 0x16 | 设置喂食间隔 | 2B: seconds | uint16 大端序 (秒) |

#### 3.2.1 设置阈值 (0x13), LEN=20

```
偏移  长度  字段           类型      说明
0     4    TempLower      float     温度下限
4     4    TempUpper      float     温度上限
8     2    AirQualityMax  uint16    空气质量阈值 (大端序)
10    4    PH_Lower       float     PH 下限
14    4    PH_Upper       float     PH 上限
18    1    WaterLevelMin  uint8     水位下限 (%)
19    1    WaterLevelMax  uint8     水位上限 (%)
```

#### 3.2.2 设备控制 (0x12), LEN=2

```
偏移  长度  字段           类型      说明
0     1    RelayID        uint8     0=加热 1=加水 2=排水 3=增氧
1     1    State          uint8     0=关, 1=开
```

## 4. USART2 串口配置与接收机制

### 4.1 USART2 寄存器配置

```c
// USART2 基本配置
// 波特率: 115200, 8N1
// PA2 = USART2_TX (复用推挽输出)
// PA3 = USART2_RX (浮空输入)

// 使能时钟
RCC->APB1ENR |= RCC_APB1ENR_USART2EN;   // USART2 时钟
RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;     // GPIOA 时钟

// PA2 复用推挽输出, PA3 浮空输入
GPIOA->CRL &= ~(0xFF << 8);             // 清除 PA2/PA3 配置
GPIOA->CRL |=  (0x0B << 8);             // PA2: AF 推挽输出 50MHz
GPIOA->CRL |=  (0x04 << 12);            // PA3: 浮空输入

// USART2 配置
USART2->BRR = 0x271;                     // 72MHz / 115200 = 625 = 0x271
USART2->CR1 = USART_CR1_UE              // USART 使能
            | USART_CR1_TE              // 发送使能
            | USART_CR1_RE              // 接收使能
            | USART_CR1_RXNEIE;         // 接收中断使能
USART2->CR2 = 0;
USART2->CR3 = 0;

// 使能 NVIC 中断
NVIC_SetPriority(USART2_IRQn, 2);
NVIC_EnableIRQ(USART2_IRQn);
```

### 4.2 环形缓冲区 + 中断接收

```
USART2 RX 中断 (每收到1字节触发):
│
├── uint8_t data = USART2->DR;               # 读取数据寄存器(自动清RXNE)
│
├── g_wifi_rx_buf[head++] = data;            # 写入环形缓冲区
│   └── head &= (RX_BUF_SIZE - 1);           # 环形回绕
│
└── g_wifi_rx_flag = 1;                      # 置位接收标志

主循环中检查:
│
├── if(g_wifi_rx_flag)
│   ├── g_wifi_rx_flag = 0;
│   └── WiFi_RxProcess()
│       ├── 检查 head/tail 计算待处理字节数
│       ├── 从环形缓冲区逐字节读取
│       ├── 帧头 0xAA 检测 → 帧解析
│       ├── 校验和验证
│       └── 根据 CMD_TYPE 执行对应操作
```

### 4.3 环形缓冲区定义

```c
#define RX_BUF_SIZE  256    // 必须为2的幂

static uint8_t  g_wifi_rx_buf[RX_BUF_SIZE];
static volatile uint16_t g_wifi_rx_head = 0;   // 中断写入位置
static volatile uint16_t g_wifi_rx_tail = 0;   // 主循环读取位置
static volatile uint8_t  g_wifi_rx_flag = 0;    // 接收标志位
```

## 5. ESP-01S AT 指令序列

MCU 通过 USART2 向 ESP-01S 发送以下 AT 指令完成配置：

```
步骤1: 测试通信
  → AT\r\n
  ← OK

步骤2: 设置 Station 模式
  → AT+CWMODE=1\r\n
  ← OK

步骤3: 连接 WiFi
  → AT+CWJAP="SSID","PASSWORD"\r\n
  ← WIFI CONNECTED
  ← WIFI GOT IP
  ← OK

步骤4: 获取 IP 地址
  → AT+CIFSR\r\n
  ← +CIFSR:STAIP,"192.168.x.x"

步骤5: 创建 TCP Server
  → AT+CIPMUX=1\r\n         (多连接模式)
  ← OK
  → AT+CIPSERVER=1,8080\r\n  (开启服务器, 端口8080)
  ← OK

步骤6: 收发数据 (透传模式, 当有客户端连接时)
  → AT+CIPSEND=<link_id>,<len>\r\n
  ← >
  → <data>
  ← SEND OK

  接收数据:
  ← +IPD,<link_id>,<len>:<data>
```

**USART2 发送函数**：直接写 `USART2->DR` 寄存器，等待 `USART_SR_TXE` 标志。

```c
void USART2_SendByte(uint8_t byte) {
    while (!(USART2->SR & USART_SR_TXE));  // 等待发送缓冲区空
    USART2->DR = byte;
}

void USART2_SendString(const char *str) {
    while (*str) {
        USART2_SendByte(*str++);
    }
}
```

## 6. 心跳机制

- MCU 每 1 秒主动上报一次传感器数据 (0x01)，由 `WiFi_Report()` 在 Timing_1s 中调用
- APP 端接收数据后更新界面
- 若 APP 超过 10 秒未收到数据，显示"连接断开"
- APP 向 MCU 发送 0x10 (查询状态) 时，MCU 应立即回复 0x01 + 0x02

## 7. 连接流程

```
APP 端:                                  MCU 端:
  │                                        │
  │  1. 用户输入 ESP-01S 的 IP 地址         │
  │  2. TCP Connect(ip, 8080)              │
  │  ─────────────────────────────────────► │
  │                                        │  3. ESP-01S 收到连接
  │                                        │  4. MCU 发送 0x01 (传感器数据)
  │  ◄───────────────────────────────────── │
  │  5. APP 显示实时数据                    │
  │                                        │
  │  6. 用户点击"切换手动"                  │
  │  7. APP 发送 0x11 0x01                 │
  │  ─────────────────────────────────────► │
  │                                        │  8. MCU 切换模式
  │                                        │  9. MCU 回复 0x02 (设备状态)
  │  ◄───────────────────────────────────── │
  │  10. APP 更新界面                      │
```

---

*协议结束。裸机版 — USART2 直接寄存器操作，中断+环形缓冲区接收，主循环中检查 g_wifi_rx_flag 处理数据。*
