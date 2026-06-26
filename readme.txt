1./*设置了上下限需要保存，断电LVD电压保存flash数据，不然设置不成功*/

2./*晾干一 样机出现重置下限位情况*/ :问题一

3./*wifi拉取位置有1%误差*/，通过提前计算停止时到结束时的走的霍尔值，在走固定值+-差值

4.没有指示灯，需要修改成照明灯

5.默认520ms 开启电机10khz 占空比从70% 到100%   ，关闭电机250ms 从99到0%. 修改PWM_STOP_SPEED 值关闭暂停需要的时间。

6.默认750ms 开启灯光 占空比从0% 到100%     20khz

7.开灯 灯不亮，LED有24V，可能是设置的最大值为0，开启不亮。默认初始化为100；  Time.Light_key_space_time 是设置色温转换时间

8.初始化PWM不能先给高电平，低电平初始化。

9.目前只有一路LED，后续配成一样

10.LVD要基准电压要配置3.3V

11.加看门狗中断

12. 直接关闭PWM 。

13.ADC串口发送开启，会降系统卡顿bug?   低压串口一直发送是否会导致系统卡顿   波特率太慢，发送太快

14.采不同情况下电流曲线，             负载20kg，空杆，收合，负载收合

15.按上升会下降 ，二级限位没有了或者变更。 

16.wifi刚刚连接上offine 会闪烁一下

17.修改map 0x3000 位置 需要Linker \  HC32_L2MAX.sct 程序    

18. 11/19 语音要有蜂鸣器，长度下降1.28多m， 亮度切换在3秒内按，二级限为速度变慢。 

19. 2个杆子同时上升，分别触发超重。

20.修改二级限位到上限位逻辑，在平板灯以下为趋势上升检测遇阻，以上为电流趋势遇阻，并且加大触发电流避免误触发。
 
21.修改遇阻逻辑为趋势，按线性效率区分
 
22.位置偏移，继电器干扰 

BUG修复：
1.操作删码，需要在遇阻状态下按3下暂停删码，再对码，当按下2下时复位状态


2.在到达上限位，检测时间如果到达500ms才算



夜灯逻辑错误：
System_init()
  └─ read_data()                          // app_flash.c:558
  │    └─ Device_State_Data.Light_Bright = Flash_Data.Max_Light_Value;  // 无条件设为90%
  │    └─ Flag.Brightness_time_flag = 0;  // 初始化为0（非夜灯时段）
  │
  └─ variable_init()                      // _hour=0, _min=0（无网络时间）
  │
  └─ 照明打开 → 读到 Light_Bright = 90% → **以90%亮度点亮**
  
  ... WiFi连接中（数秒后）...
  
  收到网络时间响应 → data_handle()          // app_wifi.c:361
    └─ _hour=9, _min=31  （假设9:31）
    └─ Judge_Nightlight_Mode()            // 根据网络时间判断是否在夜灯时段内
    │    └─ 9:00≤9:31<10:00 → Brightness_time_flag = 1（在夜灯时段内）
    │
    └─ if(Flag.Brightness_time_flag)
    │    └─ Device_State_Data.Light_Bright = Flash_Data.Light_Bright_Value;  // 20%
    │
    └─ → **亮度从90%变成20%**（用户看到闪烁）
修复思路
核心矛盾是：上电点亮时，设备还不知道现在是几点。解决方向有几种：

掉电前保存当前实际亮度 — 在 _220AC_Power_down_save() 中将 Device_State_Data.Light_Bright 保存到 Flash，read_data() 中恢复。前提是掉电和来电间隔不长，夜灯时段没变。即使时段变了，网络时间到达后也会纠正。

上电时不立即开灯，等网络时间到达后再开 — 但需要确认硬件层面是否支持延迟开灯（有些 LED 驱动电路上电即亮）。

上电后默认为夜灯亮度（如果夜灯功能已开启） — 在 read_data() 中判断 Flash_Data.Light_Bright_Enable，如果夜灯功能开启，默认用夜灯亮度而非最大亮度。不在夜灯时段的话，网络时间到达后会纠正为最大亮度。


首次上电出现重复触发电机错误
Bug 根因
当霍尔检测故障（或电流堵转检测）发生时，alarm_signal_test_and_beep() 中的流程是：

设置 Err.err_bit.Motor_Err = 1，Time.Up_Data_Delay_time = 1000
函数末尾执行 Device_State_Data.Err_Byte = Err.data（含 Motor_Err 位）
properties_up() 被调用时，检测到 Err_Byte 变了，立即把 7 1 3 和 7 4 0 打包在同一个 properties_changed 包中发送
设计意图是 "延时1s上报故障"（代码注释明确写了），1s 后由 isr_timer.c:167 的 Up_State_Data.Err_Byte &= 0xfb 机制重新触发上报。但当前代码没有阻止第一次立即上报，导致 7 1 3 在第一包中就发出去了。

修复方案
在 app_limit.c 的两处（A杆和B杆的 Device_State_Data.Err_Byte = Err.data 之后），新增：

A杆（alarm_signal_test_and_beep 末尾）：

if(Err.err_bit.Motor_Err && Time.Up_Data_Delay_time > 0)
{
    Up_State_Data.Err_Byte |= 0x04;  // 预同步 Motor_Err 位
}
B杆（alarm_signal_test_and_beep_B 末尾）：

if(Err_B.err_bit.Motor_Err && Time.B_Up_Data_Delay_time > 0)
{
    Up_State_Data.B_Err_Byte |= 0x04;
}
修复后的时序
霍尔故障发生 → Err.err_bit.Motor_Err = 1，Up_Data_Delay_time = 1000
Up_State_Data.Err_Byte |= 0x04 → 上报状态已同步 Motor_Err 位
properties_up() 检查 Err_Byte：相同 → 不发送 7 1 3 ✓
properties_up() 检查 Motor_State_Byte：变化 → 单独发送 7 4 0 ✓
1s 后 ISR 清除 Up_State_Data.Err_Byte 的 bit2 → 产生差异 → 上报 7 1 3 ✓
不影响其他逻辑
|= 0x04 只操作 bit2（Motor_Err），不影响遇阻(bit0)、超重(bit1)、遇障碍(bit3)等其他故障的立即上报
对 B杆的逻辑完全对称
如果 1s 内故障被清除（Err.err_bit.Motor_Err = 0），ISR 清除 bit2 后两边都不含 bit2，不会产生误报


































