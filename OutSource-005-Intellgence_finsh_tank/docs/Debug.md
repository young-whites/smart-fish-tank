21:14:11 **** Incremental Build of configuration Debug for project OutSource-005-Intellgence_finsh_tank ****
make -j28 all 
arm-none-eabi-gcc "../applications/app_alarm.c"
arm-none-eabi-gcc "../applications/app_control.c"
arm-none-eabi-gcc "../applications/app_data.c"
arm-none-eabi-gcc "../applications/app_display.c"
arm-none-eabi-gcc "../applications/app_feed.c"
arm-none-eabi-gcc "../applications/app_key.c"
arm-none-eabi-gcc "../applications/app_sensor.c"
arm-none-eabi-gcc "../applications/app_wifi.c"
arm-none-eabi-gcc "../applications/main.c"
In file included from ../applications/app_alarm.c:1:
../applications/app_data.h:13:5: error: unknown type name 'uint8_t'
   13 |     uint8_t  water_level;      // 水位 0~100%
      |     ^~~~~~~
In file included from ../applications/app_control.c:1:
../applications/app_data.h:13:5: error: unknown type name 'uint8_t'
   13 |     uint8_t  water_level;      // 水位 0~100%
      |     ^~~~~~~
../applications/app_data.h:14:5: error: unknown type name 'uint16_t'
   14 |     uint16_t air_quality;      // 空气质量 ADC值
      |     ^~~~~~~~
../applications/app_data.h:15:5: error: unknown type name 'uint8_t'
   15 |     uint8_t  run_mode;         // 0=自动, 1=手动
      |     ^~~~~~~
../applications/app_data.h:14:5: error: unknown type name 'uint16_t'
   14 |     uint16_t air_quality;      // 空气质量 ADC值
      |     ^~~~~~~~
../applications/app_data.h:15:5: error: unknown type name 'uint8_t'
   15 |     uint8_t  run_mode;         // 0=自动, 1=手动
      |     ^~~~~~~
../applications/app_data.h:16:5: error: unknown type name 'uint32_t'
   16 |     uint32_t feed_countdown;   // 喂食倒计时(秒)
      |     ^~~~~~~~
../applications/app_data.h:21:5: error: unknown type name 'uint8_t'
   21 |     uint8_t relay_heat;        // 加热继电器
      |     ^~~~~~~
../applications/app_data.h:22:5: error: unknown type name 'uint8_t'
   22 |     uint8_t relay_fill;        // 加水继电器
      |     ^~~~~~~
../applications/app_data.h:16:5: error: unknown type name 'uint32_t'
   16 |     uint32_t feed_countdown;   // 喂食倒计时(秒)
      |     ^~~~~~~~
../applications/app_data.h:23:5: error: unknown type name 'uint8_t'
   23 |     uint8_t relay_drain;       // 排水继电器
      |     ^~~~~~~
../applications/app_data.h:24:5: error: unknown type name 'uint8_t'
   24 |     uint8_t relay_oxygen;      // 增氧继电器
      |     ^~~~~~~
../applications/app_data.h:25:5: error: unknown type name 'uint8_t'
   25 |     uint8_t alarm_enable;      // 报警使能
      |     ^~~~~~~
../applications/app_data.h:21:5: error: unknown type name 'uint8_t'
   21 |     uint8_t relay_heat;        // 加热继电器
      |     ^~~~~~~
In file included from ../applications/app_data.c:1:
../applications/app_data.h:13:5: error: unknown type name 'uint8_t'
   13 |     uint8_t  water_level;      // 水位 0~100%
      |     ^~~~~~~
../applications/app_data.h:26:5: error: unknown type name 'uint8_t'
   26 |     uint8_t feeding;           // 喂食中
      |     ^~~~~~~
../applications/app_data.h:22:5: error: unknown type name 'uint8_t'
   22 |     uint8_t relay_fill;        // 加水继电器
      |     ^~~~~~~
../applications/app_data.h:14:5: error: unknown type name 'uint16_t'
   14 |     uint16_t air_quality;      // 空气质量 ADC值
      |     ^~~~~~~~
../applications/app_data.h:33:5: error: unknown type name 'uint16_t'
   33 |     uint16_t air_quality_max;  // 空气质量上限
      |     ^~~~~~~~
../applications/app_data.h:23:5: error: unknown type name 'uint8_t'
   23 |     uint8_t relay_drain;       // 排水继电器
      |     ^~~~~~~
../applications/app_data.h:24:5: error: unknown type name 'uint8_t'
   24 |     uint8_t relay_oxygen;      // 增氧继电器
      |     ^~~~~~~
../applications/app_data.h:36:5: error: unknown type name 'uint8_t'
   36 |     uint8_t  water_level_min;  // 水位下限%
      |     ^~~~~~~
../applications/app_data.h:15:5: error: unknown type name 'uint8_t'
   15 |     uint8_t  run_mode;         // 0=自动, 1=手动
      |     ^~~~~~~
../applications/app_data.h:25:5: error: unknown type name 'uint8_t'
   25 |     uint8_t alarm_enable;      // 报警使能
      |     ^~~~~~~
../applications/app_data.h:26:5: error: unknown type name 'uint8_t'
   26 |     uint8_t feeding;           // 喂食中
      |     ^~~~~~~
../applications/app_data.h:37:5: error: unknown type name 'uint8_t'
   37 |     uint8_t  water_level_max;  // 水位上限%
      |     ^~~~~~~
../applications/app_data.h:16:5: error: unknown type name 'uint32_t'
   16 |     uint32_t feed_countdown;   // 喂食倒计时(秒)
      |     ^~~~~~~~
../applications/app_data.h:38:5: error: unknown type name 'uint32_t'
   38 |     uint32_t feed_interval;    // 喂食间隔(秒)
      |     ^~~~~~~~
../applications/app_data.h:33:5: error: unknown type name 'uint16_t'
   33 |     uint16_t air_quality_max;  // 空气质量上限
      |     ^~~~~~~~
../applications/app_data.h:21:5: error: unknown type name 'uint8_t'
   21 |     uint8_t relay_heat;        // 加热继电器
      |     ^~~~~~~
../applications/app_data.h:96:15: error: unknown type name 'GPIO_TypeDef'
   96 | static inline GPIO_TypeDef* _pin_to_port(rt_base_t pin) {
      |               ^~~~~~~~~~~~
../applications/app_data.h:22:5: error: unknown type name 'uint8_t'
   22 |     uint8_t relay_fill;        // 加水继电器
      |     ^~~~~~~
../applications/app_data.h:36:5: error: unknown type name 'uint8_t'
   36 |     uint8_t  water_level_min;  // 水位下限%
      |     ^~~~~~~
../applications/app_data.h:23:5: error: unknown type name 'uint8_t'
   23 |     uint8_t relay_drain;       // 排水继电器
      |     ^~~~~~~
../applications/app_data.h: In function '_pin_to_port':
../applications/app_data.h:37:5: error: unknown type name 'uint8_t'
   37 |     uint8_t  water_level_max;  // 水位上限%
      |     ^~~~~~~
../applications/app_data.h:24:5: error: unknown type name 'uint8_t'
   24 |     uint8_t relay_oxygen;      // 增氧继电器
      |     ^~~~~~~
../applications/app_data.h:38:5: error: unknown type name 'uint32_t'
   38 |     uint32_t feed_interval;    // 喂食间隔(秒)
      |     ^~~~~~~~
../applications/app_data.h:25:5: error: unknown type name 'uint8_t'
   25 |     uint8_t alarm_enable;      // 报警使能
      |     ^~~~~~~
../applications/app_data.h:97:5: error: unknown type name 'uint16_t'; did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |     ^~~~~~~~
      |     rt_uint16_t
../applications/app_data.h:26:5: error: unknown type name 'uint8_t'
   26 |     uint8_t feeding;           // 喂食中
      |     ^~~~~~~
../applications/app_data.h:33:5: error: unknown type name 'uint16_t'
   33 |     uint16_t air_quality_max;  // 空气质量上限
      |     ^~~~~~~~
../applications/app_data.h:96:15: error: unknown type name 'GPIO_TypeDef'
   96 | static inline GPIO_TypeDef* _pin_to_port(rt_base_t pin) {
      |               ^~~~~~~~~~~~
../applications/app_data.h:97:26: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |                          ^~~~~~~~
      |                          rt_uint16_t
../applications/app_data.h:36:5: error: unknown type name 'uint8_t'
   36 |     uint8_t  water_level_min;  // 水位下限%
      |     ^~~~~~~
../applications/app_data.h:97:26: note: each undeclared identifier is reported only once for each function it appears in
../applications/app_data.h: In function '_pin_to_port':
../applications/app_data.h:37:5: error: unknown type name 'uint8_t'
   37 |     uint8_t  water_level_max;  // 水位上限%
      |     ^~~~~~~
../applications/app_data.h:38:5: error: unknown type name 'uint32_t'
   38 |     uint32_t feed_interval;    // 喂食间隔(秒)
      |     ^~~~~~~~
../applications/app_data.h:97:5: error: unknown type name 'uint16_t'; did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |     ^~~~~~~~
      |     rt_uint16_t
../applications/app_data.h:96:15: error: unknown type name 'GPIO_TypeDef'
   96 | static inline GPIO_TypeDef* _pin_to_port(rt_base_t pin) {
      |               ^~~~~~~~~~~~
../applications/app_data.h:99:24: error: 'GPIOA' undeclared (first use in this function)
   99 |         case 0: return GPIOA;
      |                        ^~~~~
../applications/app_data.h:100:24: error: 'GPIOB' undeclared (first use in this function)
  100 |         case 1: return GPIOB;
      |                        ^~~~~
../applications/app_data.h: In function '_pin_to_port':
../applications/app_data.h:97:26: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |                          ^~~~~~~~
      |                          rt_uint16_t
../applications/app_data.h:101:24: error: 'GPIOC' undeclared (first use in this function)
  101 |         case 2: return GPIOC;
      |                        ^~~~~
../applications/app_data.h:97:26: note: each undeclared identifier is reported only once for each function it appears in
../applications/app_data.h:97:5: error: unknown type name 'uint16_t'; did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |     ^~~~~~~~
      |     rt_uint16_t
../applications/app_data.h: At top level:
../applications/app_data.h:105:15: error: unknown type name 'uint16_t'
  105 | static inline uint16_t _pin_to_gpio(rt_base_t pin) {
      |               ^~~~~~~~
../applications/app_data.h:99:24: error: 'GPIOA' undeclared (first use in this function)
   99 |         case 0: return GPIOA;
      |                        ^~~~~
../applications/app_data.h:97:26: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |                          ^~~~~~~~
      |                          rt_uint16_t
../applications/app_data.h: In function '_pin_to_gpio':
../applications/app_data.h:97:26: note: each undeclared identifier is reported only once for each function it appears in
../applications/app_data.h:106:13: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
  106 |     return (uint16_t)(1 << (pin & 0x0F));
      |             ^~~~~~~~
      |             rt_uint16_t
../applications/app_data.h:100:24: error: 'GPIOB' undeclared (first use in this function)
  100 |         case 1: return GPIOB;
      |                        ^~~~~
../applications/app_data.h:99:24: error: 'GPIOA' undeclared (first use in this function)
   99 |         case 0: return GPIOA;
      |                        ^~~~~
../applications/app_data.h:100:24: error: 'GPIOB' undeclared (first use in this function)
  100 |         case 1: return GPIOB;
      |                        ^~~~~
../applications/app_data.h:101:24: error: 'GPIOC' undeclared (first use in this function)
  101 |         case 2: return GPIOC;
      |                        ^~~~~
../applications/app_data.h: At top level:
../applications/app_data.h:101:24: error: 'GPIOC' undeclared (first use in this function)
  101 |         case 2: return GPIOC;
      |                        ^~~~~
In file included from ../applications/app_control.c:2:
../applications/app_control.h: At top level:
../applications/app_data.h: At top level:
../applications/app_data.h:105:15: error: unknown type name 'uint16_t'
  105 | static inline uint16_t _pin_to_gpio(rt_base_t pin) {
      |               ^~~~~~~~
../applications/app_data.h:105:15: error: unknown type name 'uint16_t'
  105 | static inline uint16_t _pin_to_gpio(rt_base_t pin) {
      |               ^~~~~~~~
../applications/app_data.h: In function '_pin_to_gpio':
../applications/app_data.h:106:13: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
  106 |     return (uint16_t)(1 << (pin & 0x0F));
      |             ^~~~~~~~
      |             rt_uint16_t
../applications/app_control.h:4:31: error: unknown type name 'uint8_t'; did you mean 'rt_uint8_t'?
    4 | void relay_set(rt_base_t pin, uint8_t state);
      |                               ^~~~~~~
      |                               rt_uint8_t
../applications/app_data.h: In function '_pin_to_gpio':
../applications/app_data.h:106:13: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
  106 |     return (uint16_t)(1 << (pin & 0x0F));
      |             ^~~~~~~~
      |             rt_uint16_t
../applications/app_data.c: In function 'app_data_init':
../applications/app_data.c:36:5: warning: implicit declaration of function 'rt_semaphore_init'; did you mean 'rt_sem_init'? [-Wimplicit-function-declaration]
   36 |     rt_semaphore_init(&sem_key, "s_key", 0, RT_IPC_FLAG_FIFO);
      |     ^~~~~~~~~~~~~~~~~
      |     rt_sem_init
make: *** [applications/subdir.mk:45: applications/app_data.o] Error 1
make: *** Waiting for unfinished jobs....
In file included from ../applications/app_key.c:1:
../applications/app_data.h:13:5: error: unknown type name 'uint8_t'
   13 |     uint8_t  water_level;      // 水位 0~100%
      |     ^~~~~~~
../applications/app_data.h:14:5: error: unknown type name 'uint16_t'
   14 |     uint16_t air_quality;      // 空气质量 ADC值
      |     ^~~~~~~~
../applications/app_data.h:15:5: error: unknown type name 'uint8_t'
   15 |     uint8_t  run_mode;         // 0=自动, 1=手动
      |     ^~~~~~~
../applications/app_data.h:16:5: error: unknown type name 'uint32_t'
   16 |     uint32_t feed_countdown;   // 喂食倒计时(秒)
      |     ^~~~~~~~
../applications/app_data.h:21:5: error: unknown type name 'uint8_t'
   21 |     uint8_t relay_heat;        // 加热继电器
      |     ^~~~~~~
../applications/app_data.h:22:5: error: unknown type name 'uint8_t'
   22 |     uint8_t relay_fill;        // 加水继电器
      |     ^~~~~~~
../applications/app_data.h:23:5: error: unknown type name 'uint8_t'
   23 |     uint8_t relay_drain;       // 排水继电器
      |     ^~~~~~~
../applications/app_data.h:24:5: error: unknown type name 'uint8_t'
   24 |     uint8_t relay_oxygen;      // 增氧继电器
      |     ^~~~~~~
../applications/app_data.h:25:5: error: unknown type name 'uint8_t'
   25 |     uint8_t alarm_enable;      // 报警使能
      |     ^~~~~~~
../applications/app_data.h:26:5: error: unknown type name 'uint8_t'
   26 |     uint8_t feeding;           // 喂食中
      |     ^~~~~~~
../applications/app_data.h:33:5: error: unknown type name 'uint16_t'
   33 |     uint16_t air_quality_max;  // 空气质量上限
      |     ^~~~~~~~
../applications/app_data.h:36:5: error: unknown type name 'uint8_t'
   36 |     uint8_t  water_level_min;  // 水位下限%
      |     ^~~~~~~
../applications/app_data.h:37:5: error: unknown type name 'uint8_t'
   37 |     uint8_t  water_level_max;  // 水位上限%
      |     ^~~~~~~
../applications/app_data.h:38:5: error: unknown type name 'uint32_t'
   38 |     uint32_t feed_interval;    // 喂食间隔(秒)
      |     ^~~~~~~~
../applications/app_data.h:96:15: error: unknown type name 'GPIO_TypeDef'
   96 | static inline GPIO_TypeDef* _pin_to_port(rt_base_t pin) {
      |               ^~~~~~~~~~~~
../applications/app_data.h: In function '_pin_to_port':
../applications/app_data.h:97:5: error: unknown type name 'uint16_t'; did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |     ^~~~~~~~
      |     rt_uint16_t
../applications/app_data.h:97:26: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |                          ^~~~~~~~
      |                          rt_uint16_t
../applications/app_data.h:97:26: note: each undeclared identifier is reported only once for each function it appears in
../applications/app_data.h:99:24: error: 'GPIOA' undeclared (first use in this function)
   99 |         case 0: return GPIOA;
      |                        ^~~~~
../applications/app_data.h:100:24: error: 'GPIOB' undeclared (first use in this function)
  100 |         case 1: return GPIOB;
      |                        ^~~~~
../applications/app_data.h:101:24: error: 'GPIOC' undeclared (first use in this function)
  101 |         case 2: return GPIOC;
      |                        ^~~~~
../applications/app_data.h: At top level:
../applications/app_data.h:105:15: error: unknown type name 'uint16_t'
  105 | static inline uint16_t _pin_to_gpio(rt_base_t pin) {
      |               ^~~~~~~~
../applications/app_data.h: In function '_pin_to_gpio':
../applications/app_data.h:106:13: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
  106 |     return (uint16_t)(1 << (pin & 0x0F));
      |             ^~~~~~~~
      |             rt_uint16_t
In file included from ../applications/app_key.c:2:
../applications/app_key.h: At top level:
../applications/app_key.h:4:8: error: unknown type name 'uint8_t'
    4 | extern uint8_t g_key_event;  // 1=KEY1, 2=KEY2, 3=KEY3, 4=KEY4, 5=KEY5
      |        ^~~~~~~
In file included from ../applications/app_display.c:1:
../applications/app_data.h:13:5: error: unknown type name 'uint8_t'
   13 |     uint8_t  water_level;      // 水位 0~100%
      |     ^~~~~~~
../applications/app_data.h:14:5: error: unknown type name 'uint16_t'
   14 |     uint16_t air_quality;      // 空气质量 ADC值
      |     ^~~~~~~~
../applications/app_data.h:15:5: error: unknown type name 'uint8_t'
   15 |     uint8_t  run_mode;         // 0=自动, 1=手动
      |     ^~~~~~~
../applications/app_data.h:16:5: error: unknown type name 'uint32_t'
   16 |     uint32_t feed_countdown;   // 喂食倒计时(秒)
      |     ^~~~~~~~
../applications/app_data.h:21:5: error: unknown type name 'uint8_t'
   21 |     uint8_t relay_heat;        // 加热继电器
      |     ^~~~~~~
../applications/app_data.h:22:5: error: unknown type name 'uint8_t'
   22 |     uint8_t relay_fill;        // 加水继电器
      |     ^~~~~~~
../applications/app_data.h:23:5: error: unknown type name 'uint8_t'
   23 |     uint8_t relay_drain;       // 排水继电器
      |     ^~~~~~~
../applications/app_data.h:24:5: error: unknown type name 'uint8_t'
   24 |     uint8_t relay_oxygen;      // 增氧继电器
      |     ^~~~~~~
../applications/app_data.h:25:5: error: unknown type name 'uint8_t'
   25 |     uint8_t alarm_enable;      // 报警使能
      |     ^~~~~~~
../applications/app_data.h:26:5: error: unknown type name 'uint8_t'
   26 |     uint8_t feeding;           // 喂食中
      |     ^~~~~~~
../applications/app_data.h:33:5: error: unknown type name 'uint16_t'
   33 |     uint16_t air_quality_max;  // 空气质量上限
      |     ^~~~~~~~
../applications/app_data.h:36:5: error: unknown type name 'uint8_t'
   36 |     uint8_t  water_level_min;  // 水位下限%
      |     ^~~~~~~
../applications/app_data.h:37:5: error: unknown type name 'uint8_t'
   37 |     uint8_t  water_level_max;  // 水位上限%
      |     ^~~~~~~
../applications/app_data.h:38:5: error: unknown type name 'uint32_t'
   38 |     uint32_t feed_interval;    // 喂食间隔(秒)
      |     ^~~~~~~~
../applications/app_data.h:96:15: error: unknown type name 'GPIO_TypeDef'
   96 | static inline GPIO_TypeDef* _pin_to_port(rt_base_t pin) {
      |               ^~~~~~~~~~~~
../applications/app_data.h: In function '_pin_to_port':
../applications/app_data.h:97:5: error: unknown type name 'uint16_t'; did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |     ^~~~~~~~
      |     rt_uint16_t
../applications/app_data.h:97:26: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |                          ^~~~~~~~
      |                          rt_uint16_t
../applications/app_data.h:97:26: note: each undeclared identifier is reported only once for each function it appears in
../applications/app_data.h:99:24: error: 'GPIOA' undeclared (first use in this function)
   99 |         case 0: return GPIOA;
      |                        ^~~~~
../applications/app_data.h:100:24: error: 'GPIOB' undeclared (first use in this function)
  100 |         case 1: return GPIOB;
      |                        ^~~~~
../applications/app_data.h:101:24: error: 'GPIOC' undeclared (first use in this function)
  101 |         case 2: return GPIOC;
      |                        ^~~~~
../applications/app_data.h: At top level:
../applications/app_data.h:105:15: error: unknown type name 'uint16_t'
  105 | static inline uint16_t _pin_to_gpio(rt_base_t pin) {
      |               ^~~~~~~~
../applications/app_data.h: In function '_pin_to_gpio':
../applications/app_data.h:106:13: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
  106 |     return (uint16_t)(1 << (pin & 0x0F));
      |             ^~~~~~~~
      |             rt_uint16_t
In file included from ../applications/app_sensor.c:1:
../applications/app_data.h:13:5: error: unknown type name 'uint8_t'
   13 |     uint8_t  water_level;      // 水位 0~100%
      |     ^~~~~~~
../applications/app_data.h:14:5: error: unknown type name 'uint16_t'
   14 |     uint16_t air_quality;      // 空气质量 ADC值
      |     ^~~~~~~~
../applications/app_data.h:15:5: error: unknown type name 'uint8_t'
   15 |     uint8_t  run_mode;         // 0=自动, 1=手动
      |     ^~~~~~~
../applications/app_data.h:16:5: error: unknown type name 'uint32_t'
   16 |     uint32_t feed_countdown;   // 喂食倒计时(秒)
      |     ^~~~~~~~
../applications/app_data.h:21:5: error: unknown type name 'uint8_t'
   21 |     uint8_t relay_heat;        // 加热继电器
      |     ^~~~~~~
../applications/app_data.h:22:5: error: unknown type name 'uint8_t'
   22 |     uint8_t relay_fill;        // 加水继电器
      |     ^~~~~~~
../applications/app_data.h:23:5: error: unknown type name 'uint8_t'
   23 |     uint8_t relay_drain;       // 排水继电器
      |     ^~~~~~~
../applications/app_data.h:24:5: error: unknown type name 'uint8_t'
   24 |     uint8_t relay_oxygen;      // 增氧继电器
      |     ^~~~~~~
../applications/app_data.h:25:5: error: unknown type name 'uint8_t'
   25 |     uint8_t alarm_enable;      // 报警使能
      |     ^~~~~~~
../applications/app_data.h:26:5: error: unknown type name 'uint8_t'
   26 |     uint8_t feeding;           // 喂食中
      |     ^~~~~~~
../applications/app_data.h:33:5: error: unknown type name 'uint16_t'
   33 |     uint16_t air_quality_max;  // 空气质量上限
      |     ^~~~~~~~
../applications/app_data.h:36:5: error: unknown type name 'uint8_t'
   36 |     uint8_t  water_level_min;  // 水位下限%
      |     ^~~~~~~
../applications/app_data.h:37:5: error: unknown type name 'uint8_t'
   37 |     uint8_t  water_level_max;  // 水位上限%
      |     ^~~~~~~
../applications/app_data.h:38:5: error: unknown type name 'uint32_t'
   38 |     uint32_t feed_interval;    // 喂食间隔(秒)
      |     ^~~~~~~~
../applications/app_data.h:96:15: error: unknown type name 'GPIO_TypeDef'
   96 | static inline GPIO_TypeDef* _pin_to_port(rt_base_t pin) {
      |               ^~~~~~~~~~~~
../applications/app_data.h: In function '_pin_to_port':
../applications/app_data.h:97:5: error: unknown type name 'uint16_t'; did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |     ^~~~~~~~
      |     rt_uint16_t
../applications/app_data.h:97:26: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |                          ^~~~~~~~
      |                          rt_uint16_t
../applications/app_data.h:97:26: note: each undeclared identifier is reported only once for each function it appears in
../applications/app_data.h:99:24: error: 'GPIOA' undeclared (first use in this function)
   99 |         case 0: return GPIOA;
      |                        ^~~~~
../applications/app_data.h:100:24: error: 'GPIOB' undeclared (first use in this function)
  100 |         case 1: return GPIOB;
      |                        ^~~~~
../applications/app_data.h:101:24: error: 'GPIOC' undeclared (first use in this function)
  101 |         case 2: return GPIOC;
      |                        ^~~~~
../applications/app_data.h: At top level:
../applications/app_data.h:105:15: error: unknown type name 'uint16_t'
  105 | static inline uint16_t _pin_to_gpio(rt_base_t pin) {
      |               ^~~~~~~~
../applications/app_data.h: In function '_pin_to_gpio':
../applications/app_data.h:106:13: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
  106 |     return (uint16_t)(1 << (pin & 0x0F));
      |             ^~~~~~~~
      |             rt_uint16_t
In file included from ../applications/app_feed.c:1:
../applications/app_data.h:13:5: error: unknown type name 'uint8_t'
   13 |     uint8_t  water_level;      // 水位 0~100%
      |     ^~~~~~~
../applications/app_data.h:14:5: error: unknown type name 'uint16_t'
   14 |     uint16_t air_quality;      // 空气质量 ADC值
      |     ^~~~~~~~
../applications/app_data.h:15:5: error: unknown type name 'uint8_t'
   15 |     uint8_t  run_mode;         // 0=自动, 1=手动
      |     ^~~~~~~
../applications/app_data.h:16:5: error: unknown type name 'uint32_t'
   16 |     uint32_t feed_countdown;   // 喂食倒计时(秒)
      |     ^~~~~~~~
../applications/app_data.h:21:5: error: unknown type name 'uint8_t'
   21 |     uint8_t relay_heat;        // 加热继电器
      |     ^~~~~~~
../applications/app_data.h:22:5: error: unknown type name 'uint8_t'
   22 |     uint8_t relay_fill;        // 加水继电器
      |     ^~~~~~~
../applications/app_data.h:23:5: error: unknown type name 'uint8_t'
   23 |     uint8_t relay_drain;       // 排水继电器
      |     ^~~~~~~
../applications/app_data.h:24:5: error: unknown type name 'uint8_t'
   24 |     uint8_t relay_oxygen;      // 增氧继电器
      |     ^~~~~~~
../applications/app_data.h:25:5: error: unknown type name 'uint8_t'
   25 |     uint8_t alarm_enable;      // 报警使能
      |     ^~~~~~~
../applications/app_data.h:26:5: error: unknown type name 'uint8_t'
   26 |     uint8_t feeding;           // 喂食中
      |     ^~~~~~~
../applications/app_data.h:33:5: error: unknown type name 'uint16_t'
   33 |     uint16_t air_quality_max;  // 空气质量上限
      |     ^~~~~~~~
../applications/app_data.h:36:5: error: unknown type name 'uint8_t'
   36 |     uint8_t  water_level_min;  // 水位下限%
      |     ^~~~~~~
../applications/app_data.h:37:5: error: unknown type name 'uint8_t'
   37 |     uint8_t  water_level_max;  // 水位上限%
      |     ^~~~~~~
../applications/app_data.h:38:5: error: unknown type name 'uint32_t'
   38 |     uint32_t feed_interval;    // 喂食间隔(秒)
      |     ^~~~~~~~
../applications/app_data.h:96:15: error: unknown type name 'GPIO_TypeDef'
   96 | static inline GPIO_TypeDef* _pin_to_port(rt_base_t pin) {
      |               ^~~~~~~~~~~~
../applications/app_data.h: In function '_pin_to_port':
../applications/app_data.h:97:5: error: unknown type name 'uint16_t'; did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |     ^~~~~~~~
      |     rt_uint16_t
../applications/app_data.h:97:26: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |                          ^~~~~~~~
      |                          rt_uint16_t
../applications/app_data.h:97:26: note: each undeclared identifier is reported only once for each function it appears in
../applications/app_data.h:99:24: error: 'GPIOA' undeclared (first use in this function)
   99 |         case 0: return GPIOA;
      |                        ^~~~~
../applications/app_data.h:100:24: error: 'GPIOB' undeclared (first use in this function)
  100 |         case 1: return GPIOB;
      |                        ^~~~~
../applications/app_data.h:101:24: error: 'GPIOC' undeclared (first use in this function)
  101 |         case 2: return GPIOC;
      |                        ^~~~~
../applications/app_data.h: At top level:
../applications/app_data.h:105:15: error: unknown type name 'uint16_t'
  105 | static inline uint16_t _pin_to_gpio(rt_base_t pin) {
      |               ^~~~~~~~
../applications/app_data.h: In function '_pin_to_gpio':
../applications/app_data.h:106:13: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
  106 |     return (uint16_t)(1 << (pin & 0x0F));
      |             ^~~~~~~~
      |             rt_uint16_t
In file included from ../applications/app_wifi.c:1:
../applications/app_data.h:13:5: error: unknown type name 'uint8_t'
   13 |     uint8_t  water_level;      // 水位 0~100%
      |     ^~~~~~~
../applications/app_data.h:14:5: error: unknown type name 'uint16_t'
   14 |     uint16_t air_quality;      // 空气质量 ADC值
      |     ^~~~~~~~
../applications/app_data.h:15:5: error: unknown type name 'uint8_t'
   15 |     uint8_t  run_mode;         // 0=自动, 1=手动
      |     ^~~~~~~
../applications/app_data.h:16:5: error: unknown type name 'uint32_t'
   16 |     uint32_t feed_countdown;   // 喂食倒计时(秒)
      |     ^~~~~~~~
../applications/app_data.h:21:5: error: unknown type name 'uint8_t'
   21 |     uint8_t relay_heat;        // 加热继电器
      |     ^~~~~~~
../applications/app_data.h:22:5: error: unknown type name 'uint8_t'
   22 |     uint8_t relay_fill;        // 加水继电器
      |     ^~~~~~~
../applications/app_data.h:23:5: error: unknown type name 'uint8_t'
   23 |     uint8_t relay_drain;       // 排水继电器
      |     ^~~~~~~
../applications/app_data.h:24:5: error: unknown type name 'uint8_t'
   24 |     uint8_t relay_oxygen;      // 增氧继电器
      |     ^~~~~~~
../applications/app_data.h:25:5: error: unknown type name 'uint8_t'
   25 |     uint8_t alarm_enable;      // 报警使能
      |     ^~~~~~~
../applications/app_data.h:26:5: error: unknown type name 'uint8_t'
   26 |     uint8_t feeding;           // 喂食中
      |     ^~~~~~~
../applications/app_data.h:33:5: error: unknown type name 'uint16_t'
   33 |     uint16_t air_quality_max;  // 空气质量上限
      |     ^~~~~~~~
../applications/app_data.h:36:5: error: unknown type name 'uint8_t'
   36 |     uint8_t  water_level_min;  // 水位下限%
      |     ^~~~~~~
../applications/app_data.h:37:5: error: unknown type name 'uint8_t'
   37 |     uint8_t  water_level_max;  // 水位上限%
      |     ^~~~~~~
../applications/app_data.h:38:5: error: unknown type name 'uint32_t'
   38 |     uint32_t feed_interval;    // 喂食间隔(秒)
      |     ^~~~~~~~
../applications/app_data.h:96:15: error: unknown type name 'GPIO_TypeDef'
   96 | static inline GPIO_TypeDef* _pin_to_port(rt_base_t pin) {
      |               ^~~~~~~~~~~~
../applications/app_data.h: In function '_pin_to_port':
../applications/app_data.h:97:5: error: unknown type name 'uint16_t'; did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |     ^~~~~~~~
      |     rt_uint16_t
../applications/app_data.h:97:26: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |                          ^~~~~~~~
      |                          rt_uint16_t
../applications/app_data.h:97:26: note: each undeclared identifier is reported only once for each function it appears in
../applications/app_data.h:99:24: error: 'GPIOA' undeclared (first use in this function)
   99 |         case 0: return GPIOA;
      |                        ^~~~~
../applications/app_data.h:100:24: error: 'GPIOB' undeclared (first use in this function)
  100 |         case 1: return GPIOB;
      |                        ^~~~~
../applications/app_data.h:101:24: error: 'GPIOC' undeclared (first use in this function)
  101 |         case 2: return GPIOC;
      |                        ^~~~~
../applications/app_data.h: At top level:
../applications/app_data.h:105:15: error: unknown type name 'uint16_t'
  105 | static inline uint16_t _pin_to_gpio(rt_base_t pin) {
      |               ^~~~~~~~
../applications/app_data.h: In function '_pin_to_gpio':
../applications/app_data.h:106:13: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
  106 |     return (uint16_t)(1 << (pin & 0x0F));
      |             ^~~~~~~~
      |             rt_uint16_t
In file included from ../applications/app_wifi.c:2:
../applications/app_wifi.h: At top level:
../applications/app_wifi.h:4:17: error: unknown type name 'uint8_t'
    4 | extern volatile uint8_t g_wifi_connected;
      |                 ^~~~~~~
In file included from ../applications/main.c:3:
../applications/app_data.h:13:5: error: unknown type name 'uint8_t'
   13 |     uint8_t  water_level;      // 水位 0~100%
      |     ^~~~~~~
../applications/app_data.h:14:5: error: unknown type name 'uint16_t'
   14 |     uint16_t air_quality;      // 空气质量 ADC值
      |     ^~~~~~~~
../applications/app_data.h:15:5: error: unknown type name 'uint8_t'
   15 |     uint8_t  run_mode;         // 0=自动, 1=手动
      |     ^~~~~~~
../applications/app_data.h:16:5: error: unknown type name 'uint32_t'
   16 |     uint32_t feed_countdown;   // 喂食倒计时(秒)
      |     ^~~~~~~~
../applications/app_data.h:21:5: error: unknown type name 'uint8_t'
   21 |     uint8_t relay_heat;        // 加热继电器
      |     ^~~~~~~
../applications/app_data.h:22:5: error: unknown type name 'uint8_t'
   22 |     uint8_t relay_fill;        // 加水继电器
      |     ^~~~~~~
../applications/app_data.h:23:5: error: unknown type name 'uint8_t'
   23 |     uint8_t relay_drain;       // 排水继电器
      |     ^~~~~~~
../applications/app_data.h:24:5: error: unknown type name 'uint8_t'
   24 |     uint8_t relay_oxygen;      // 增氧继电器
      |     ^~~~~~~
../applications/app_data.h:25:5: error: unknown type name 'uint8_t'
   25 |     uint8_t alarm_enable;      // 报警使能
      |     ^~~~~~~
../applications/app_data.h:26:5: error: unknown type name 'uint8_t'
   26 |     uint8_t feeding;           // 喂食中
      |     ^~~~~~~
../applications/app_data.h:33:5: error: unknown type name 'uint16_t'
   33 |     uint16_t air_quality_max;  // 空气质量上限
      |     ^~~~~~~~
../applications/app_data.h:36:5: error: unknown type name 'uint8_t'
   36 |     uint8_t  water_level_min;  // 水位下限%
      |     ^~~~~~~
../applications/app_data.h:37:5: error: unknown type name 'uint8_t'
   37 |     uint8_t  water_level_max;  // 水位上限%
      |     ^~~~~~~
../applications/app_data.h:38:5: error: unknown type name 'uint32_t'
   38 |     uint32_t feed_interval;    // 喂食间隔(秒)
      |     ^~~~~~~~
../applications/app_data.h:96:15: error: unknown type name 'GPIO_TypeDef'
   96 | static inline GPIO_TypeDef* _pin_to_port(rt_base_t pin) {
      |               ^~~~~~~~~~~~
../applications/app_data.h: In function '_pin_to_port':
../applications/app_data.h:97:5: error: unknown type name 'uint16_t'; did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |     ^~~~~~~~
      |     rt_uint16_t
../applications/app_data.h:97:26: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
   97 |     uint16_t port_idx = (uint16_t)(pin >> 4);
      |                          ^~~~~~~~
      |                          rt_uint16_t
../applications/app_data.h:97:26: note: each undeclared identifier is reported only once for each function it appears in
../applications/app_data.h:99:24: error: 'GPIOA' undeclared (first use in this function)
   99 |         case 0: return GPIOA;
      |                        ^~~~~
../applications/app_data.h:100:24: error: 'GPIOB' undeclared (first use in this function)
  100 |         case 1: return GPIOB;
      |                        ^~~~~
../applications/app_data.h:101:24: error: 'GPIOC' undeclared (first use in this function)
  101 |         case 2: return GPIOC;
      |                        ^~~~~
../applications/app_data.h: At top level:
../applications/app_data.h:105:15: error: unknown type name 'uint16_t'
  105 | static inline uint16_t _pin_to_gpio(rt_base_t pin) {
      |               ^~~~~~~~
../applications/app_data.h: In function '_pin_to_gpio':
../applications/app_data.h:106:13: error: 'uint16_t' undeclared (first use in this function); did you mean 'rt_uint16_t'?
  106 |     return (uint16_t)(1 << (pin & 0x0F));
      |             ^~~~~~~~
      |             rt_uint16_t
In file included from ../applications/main.c:5:
../applications/app_control.h: At top level:
../applications/app_control.h:4:31: error: unknown type name 'uint8_t'; did you mean 'rt_uint8_t'?
    4 | void relay_set(rt_base_t pin, uint8_t state);
      |                               ^~~~~~~
      |                               rt_uint8_t
In file included from ../applications/main.c:7:
../applications/app_key.h:4:8: error: unknown type name 'uint8_t'
    4 | extern uint8_t g_key_event;  // 1=KEY1, 2=KEY2, 3=KEY3, 4=KEY4, 5=KEY5
      |        ^~~~~~~
In file included from ../applications/main.c:10:
../applications/app_wifi.h:4:17: error: unknown type name 'uint8_t'
    4 | extern volatile uint8_t g_wifi_connected;
      |                 ^~~~~~~
make: *** [applications/subdir.mk:45: applications/main.o] Error 1
../applications/app_feed.c: At top level:
In file included from ../applications/app_control.c:1:
../applications/app_control.c: In function 'relay_set':
../applications/app_feed.c:5:8: error: unknown type name 'TIM_HandleTypeDef'
    5 | static TIM_HandleTypeDef htim3;
      |        ^~~~~~~~~~~~~~~~~
../applications/app_key.c:5:9: error: conflicting types for 'g_key_event'
    5 | uint8_t g_key_event = 0;
      |         ^~~~~~~~~~~
In file included from ../applications/app_key.c:2:
../applications/app_key.h:4:16: note: previous declaration of 'g_key_event' was here
    4 | extern uint8_t g_key_event;  // 1=KEY1, 2=KEY2, 3=KEY3, 4=KEY4, 5=KEY5
      |                ^~~~~~~~~~~
../applications/app_data.h:108:25: warning: passing argument 1 of 'HAL_GPIO_WritePin' from incompatible pointer type [-Wincompatible-pointer-types]
  108 | #define GET_PORT(p)     _pin_to_port(p)
      |                         ^~~~~~~~~~~~~~~
      |                         |
      |                         int *
../applications/app_control.c:6:23: note: in expansion of macro 'GET_PORT'
    6 |     HAL_GPIO_WritePin(GET_PORT(pin), GET_PIN_NUM(pin), state ? GPIO_PIN_SET : GPIO_PIN_RESET);
      |                       ^~~~~~~~
../applications/app_feed.c: In function 'servo_init':
../applications/app_sensor.c: At top level:
../applications/app_feed.c:17:10: error: request for member 'Instance' in something not a structure or union
   17 |     htim3.Instance = TIM3;
      |          ^
../applications/app_feed.c:18:10: error: request for member 'Init' in something not a structure or union
   18 |     htim3.Init.Prescaler = 71;
      |          ^
../applications/app_feed.c:19:10: error: request for member 'Init' in something not a structure or union
   19 |     htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
      |          ^
../applications/app_sensor.c:97:8: error: unknown type name 'ADC_HandleTypeDef'
   97 | static ADC_HandleTypeDef hadc1;
      |        ^~~~~~~~~~~~~~~~~
In file included from I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\cubemx\Inc/stm32f1xx_hal_conf.h:242,
                 from I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\STM32F1xx_HAL_Driver\Inc/stm32f1xx_hal.h:30,
                 from ../applications/app_control.c:3:
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\STM32F1xx_HAL_Driver\Inc/stm32f1xx_hal_gpio.h:235:38: note: expected 'GPIO_TypeDef *' but argument is of type 'int *'
  235 | void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
      |                        ~~~~~~~~~~~~~~^~~~~
../applications/app_sensor.c: In function 'adc_init':
../applications/app_sensor.c:108:10: error: request for member 'Instance' in something not a structure or union
  108 |     hadc1.Instance = ADC1;
      |          ^
../applications/app_sensor.c:109:10: error: request for member 'Init' in something not a structure or union
  109 |     hadc1.Init.ScanConvMode = DISABLE;
      |          ^
../applications/app_sensor.c:110:10: error: request for member 'Init' in something not a structure or union
  110 |     hadc1.Init.ContinuousConvMode = DISABLE;
      |          ^
In file included from ../applications/app_control.c:1:
../applications/app_control.c: In function 'auto_control':
../applications/app_sensor.c:111:10: error: request for member 'Init' in something not a structure or union
  111 |     hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
      |          ^
../applications/app_data.h:66:27: warning: implicit declaration of function 'GET_PIN' [-Wimplicit-function-declaration]
   66 | #define PIN_RELAY_HEAT    GET_PIN(B, 12)
      |                           ^~~~~~~
../applications/app_control.c:43:19: note: in expansion of macro 'PIN_RELAY_HEAT'
   43 |         relay_set(PIN_RELAY_HEAT, 1);
      |                   ^~~~~~~~~~~~~~
../applications/app_data.h:66:35: error: 'B' undeclared (first use in this function); did you mean 'OB'?
   66 | #define PIN_RELAY_HEAT    GET_PIN(B, 12)
      |                                   ^
../applications/app_control.c:43:19: note: in expansion of macro 'PIN_RELAY_HEAT'
   43 |         relay_set(PIN_RELAY_HEAT, 1);
      |                   ^~~~~~~~~~~~~~
../applications/app_data.h:69:35: error: 'A' undeclared (first use in this function)
   69 | #define PIN_RELAY_OXYGEN  GET_PIN(A, 15)
      |                                   ^
../applications/app_control.c:61:19: note: in expansion of macro 'PIN_RELAY_OXYGEN'
   61 |         relay_set(PIN_RELAY_OXYGEN, 1);
      |                   ^~~~~~~~~~~~~~~~
../applications/app_data.h: In function '_pin_to_port':
../applications/app_data.h:104:1: warning: control reaches end of non-void function [-Wreturn-type]
  104 | }
      | ^
../applications/app_data.h: In function '_pin_to_gpio':
../applications/app_data.h:107:1: warning: control reaches end of non-void function [-Wreturn-type]
  107 | }
      | ^
make: *** [applications/subdir.mk:45: applications/app_key.o] Error 1
make: *** [applications/subdir.mk:45: applications/app_alarm.o] Error 1
../applications/app_feed.c:19:30: error: 'TIM_COUNTERMODE_UP' undeclared (first use in this function)
   19 |     htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
      |                              ^~~~~~~~~~~~~~~~~~
../applications/app_feed.c:20:10: error: request for member 'Init' in something not a structure or union
   20 |     htim3.Init.Period = 19999;
      |          ^
../applications/app_feed.c:21:10: error: request for member 'Init' in something not a structure or union
   21 |     htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      |          ^
make: *** [applications/subdir.mk:45: applications/app_control.o] Error 1
../applications/app_sensor.c:111:28: error: 'ADC_DATAALIGN_RIGHT' undeclared (first use in this function)
  111 |     hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
      |                            ^~~~~~~~~~~~~~~~~~~
../applications/app_sensor.c:112:10: error: request for member 'Init' in something not a structure or union
  112 |     hadc1.Init.NbrOfConversion = 1;
      |          ^
../applications/app_sensor.c:113:5: warning: implicit declaration of function 'HAL_ADC_Init'; did you mean 'HAL_DeInit'? [-Wimplicit-function-declaration]
  113 |     HAL_ADC_Init(&hadc1);
      |     ^~~~~~~~~~~~
      |     HAL_DeInit
../applications/app_feed.c:21:32: error: 'TIM_CLOCKDIVISION_DIV1' undeclared (first use in this function)
   21 |     htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      |                                ^~~~~~~~~~~~~~~~~~~~~~
../applications/app_sensor.c: In function 'adc_read_channel':
../applications/app_sensor.c:117:5: error: unknown type name 'ADC_ChannelConfTypeDef'; did you mean 'DMA_Channel_TypeDef'?
  117 |     ADC_ChannelConfTypeDef s = {0};
      |     ^~~~~~~~~~~~~~~~~~~~~~
      |     DMA_Channel_TypeDef
../applications/app_sensor.c:118:6: error: request for member 'Channel' in something not a structure or union
  118 |     s.Channel = channel;
      |      ^
../applications/app_sensor.c:119:6: error: request for member 'Rank' in something not a structure or union
  119 |     s.Rank = ADC_REGULAR_RANK_1;
      |      ^
../applications/app_feed.c:22:5: warning: implicit declaration of function 'HAL_TIM_PWM_Init'; did you mean 'HAL_LIN_Init'? [-Wimplicit-function-declaration]
   22 |     HAL_TIM_PWM_Init(&htim3);
      |     ^~~~~~~~~~~~~~~~
      |     HAL_LIN_Init
../applications/app_wifi.c:7:18: error: conflicting types for 'g_wifi_connected'
    7 | volatile uint8_t g_wifi_connected = 0;
      |                  ^~~~~~~~~~~~~~~~
In file included from ../applications/app_wifi.c:2:
../applications/app_wifi.h:4:25: note: previous declaration of 'g_wifi_connected' was here
    4 | extern volatile uint8_t g_wifi_connected;
      |                         ^~~~~~~~~~~~~~~~
../applications/app_sensor.c:119:14: error: 'ADC_REGULAR_RANK_1' undeclared (first use in this function)
  119 |     s.Rank = ADC_REGULAR_RANK_1;
      |              ^~~~~~~~~~~~~~~~~~
../applications/app_sensor.c:120:6: error: request for member 'SamplingTime' in something not a structure or union
  120 |     s.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
      |      ^
../applications/app_feed.c:24:5: error: unknown type name 'TIM_OC_InitTypeDef'; did you mean 'DMA_InitTypeDef'?
   24 |     TIM_OC_InitTypeDef oc = {0};
      |     ^~~~~~~~~~~~~~~~~~
      |     DMA_InitTypeDef
../applications/app_feed.c:25:7: error: request for member 'OCMode' in something not a structure or union
   25 |     oc.OCMode = TIM_OCMODE_PWM1;
      |       ^
make: *** [applications/subdir.mk:45: applications/app_display.o] Error 1
make: *** [applications/subdir.mk:45: applications/app_wifi.o] Error 1
../applications/app_feed.c:25:17: error: 'TIM_OCMODE_PWM1' undeclared (first use in this function)
   25 |     oc.OCMode = TIM_OCMODE_PWM1;
      |                 ^~~~~~~~~~~~~~~
../applications/app_feed.c:26:7: error: request for member 'Pulse' in something not a structure or union
   26 |     oc.Pulse = 1500;
      |       ^
../applications/app_feed.c:27:7: error: request for member 'OCPolarity' in something not a structure or union
   27 |     oc.OCPolarity = TIM_OCPOLARITY_HIGH;
      |       ^
../applications/app_sensor.c:120:22: error: 'ADC_SAMPLETIME_55CYCLES_5' undeclared (first use in this function)
  120 |     s.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
      |                      ^~~~~~~~~~~~~~~~~~~~~~~~~
../applications/app_feed.c:27:21: error: 'TIM_OCPOLARITY_HIGH' undeclared (first use in this function)
   27 |     oc.OCPolarity = TIM_OCPOLARITY_HIGH;
      |                     ^~~~~~~~~~~~~~~~~~~
../applications/app_feed.c:28:7: error: request for member 'OCFastMode' in something not a structure or union
   28 |     oc.OCFastMode = TIM_OCFAST_DISABLE;
      |       ^
../applications/app_sensor.c:121:5: warning: implicit declaration of function 'HAL_ADC_ConfigChannel' [-Wimplicit-function-declaration]
  121 |     HAL_ADC_ConfigChannel(&hadc1, &s);
      |     ^~~~~~~~~~~~~~~~~~~~~
../applications/app_sensor.c:122:5: warning: implicit declaration of function 'HAL_ADC_Start'; did you mean 'HAL_DMA_Start'? [-Wimplicit-function-declaration]
  122 |     HAL_ADC_Start(&hadc1);
      |     ^~~~~~~~~~~~~
      |     HAL_DMA_Start
../applications/app_feed.c:28:21: error: 'TIM_OCFAST_DISABLE' undeclared (first use in this function)
   28 |     oc.OCFastMode = TIM_OCFAST_DISABLE;
      |                     ^~~~~~~~~~~~~~~~~~
../applications/app_sensor.c:123:5: warning: implicit declaration of function 'HAL_ADC_PollForConversion' [-Wimplicit-function-declaration]
  123 |     HAL_ADC_PollForConversion(&hadc1, 100);
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~
../applications/app_feed.c:29:5: warning: implicit declaration of function 'HAL_TIM_PWM_ConfigChannel' [-Wimplicit-function-declaration]
   29 |     HAL_TIM_PWM_ConfigChannel(&htim3, &oc, TIM_CHANNEL_1);
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~
../applications/app_sensor.c:124:12: warning: implicit declaration of function 'HAL_ADC_GetValue' [-Wimplicit-function-declaration]
  124 |     return HAL_ADC_GetValue(&hadc1);
      |            ^~~~~~~~~~~~~~~~
../applications/app_feed.c:29:44: error: 'TIM_CHANNEL_1' undeclared (first use in this function)
   29 |     HAL_TIM_PWM_ConfigChannel(&htim3, &oc, TIM_CHANNEL_1);
      |                                            ^~~~~~~~~~~~~
../applications/app_sensor.c: In function 'sensor_thread_entry':
../applications/app_sensor.c:145:49: error: 'ADC_CHANNEL_4' undeclared (first use in this function)
  145 |         g_sensor.air_quality = adc_read_channel(ADC_CHANNEL_4);
      |                                                 ^~~~~~~~~~~~~
../applications/app_sensor.c:146:59: error: 'ADC_CHANNEL_1' undeclared (first use in this function)
  146 |         g_sensor.water_level = (uint8_t)(adc_read_channel(ADC_CHANNEL_1) * 100 / 4095);
      |                                                           ^~~~~~~~~~~~~
../applications/app_feed.c:30:5: warning: implicit declaration of function 'HAL_TIM_PWM_Stop' [-Wimplicit-function-declaration]
   30 |     HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
      |     ^~~~~~~~~~~~~~~~
../applications/app_sensor.c:147:53: error: 'ADC_CHANNEL_0' undeclared (first use in this function)
  147 |         g_sensor.ph_value = (float)adc_read_channel(ADC_CHANNEL_0) * 14.0f / 4095.0f;
      |                                                     ^~~~~~~~~~~~~
../applications/app_feed.c: In function 'servo_open':
../applications/app_feed.c:34:5: warning: implicit declaration of function '__HAL_TIM_SET_COMPARE' [-Wimplicit-function-declaration]
   34 |     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 2500);
      |     ^~~~~~~~~~~~~~~~~~~~~
make: *** [applications/subdir.mk:45: applications/app_sensor.o] Error 1
../applications/app_feed.c:34:35: error: 'TIM_CHANNEL_1' undeclared (first use in this function)
   34 |     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 2500);
      |                                   ^~~~~~~~~~~~~
../applications/app_feed.c:35:5: warning: implicit declaration of function 'HAL_TIM_PWM_Start'; did you mean 'HAL_DMA_Start'? [-Wimplicit-function-declaration]
   35 |     HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
      |     ^~~~~~~~~~~~~~~~~
      |     HAL_DMA_Start
../applications/app_feed.c: In function 'servo_close':
../applications/app_feed.c:39:30: error: 'TIM_CHANNEL_1' undeclared (first use in this function)
   39 |     HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
      |                              ^~~~~~~~~~~~~
make: *** [applications/subdir.mk:45: applications/app_feed.o] Error 1
"make -j28 all" terminated with exit code 2. Build might be incomplete.

21:14:12 Build Failed. 253 errors, 15 warnings. (took 1s.414ms)

