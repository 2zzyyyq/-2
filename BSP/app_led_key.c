#include "ALL_Var.h"

 uint8_t Holl_Circle_num_B,Motor_Circle_num_B;
volatile uint8_t Holl_Circle_num,Motor_Circle_num;
volatile int B_Motor_Current_Position_pulse,Motor_Current_Position_pulse,Holl_pulse_temp_B,Holl_test_speed_temp;
uint8_t key;
#define NO_KEY 0X01
//PA08		MOR2_H2	电机2霍尔信号
//PA09		MOR2_H1
//PA10		MOR1_H2	电机1霍尔信号
//PA11		MOR1_H1
void button_init( button* handle, uint8_t(*pin_level)(void))
{
    handle->event = (uint8_t)NONE_PRESS;
    handle->hal_button_Level = pin_level;
    handle->button_level = handle->hal_button_Level();
}
// 定义一个匹配的函数
uint8_t ReadButtonLevel(void) {
    return Gpio_GetInputIO(STK_USER_PORT, STK_USER_PIN);
}
/**
  * @brief  button driver core function, driver state machine.
  * @param  handle: the button handle strcut.
  * @retval None
  */
uint8_t button_handler(button* handle)
{
    uint8_t read_gpio_level = handle->hal_button_Level();
    
    // ticks counter working..
    if ((handle->state) > 0) 
    {
        handle->ticks++;
    }

    /* 消抖处理 */
    if ((read_gpio_level == handle->button_level) && (read_gpio_level != NO_KEY))
    { 
        // not equal to prev one
        // continue read 3 times same new level change
        if (++(handle->debounce_cnt) >= DEBOUNCE_TICKS) 
        {
            key = read_gpio_level;
            handle->debounce_cnt = 0;
        }
    } 
    else 
    { 
        // level not change, counter reset.
        handle->debounce_cnt = 0;
        handle->button_level = read_gpio_level;
    }
    
    /*----------------- 状态机 -------------------*/
    switch (handle->state) 
    {
        case 0: // 初始状态
            if ((read_gpio_level == key) && (read_gpio_level != NO_KEY))
            { 
                handle->event = (uint8_t)PRESS_DOWN;
                handle->ticks = 0;
                handle->repeat = 1;
                handle->state = 1; // 有按键按下
            } 
            else 
            {
                handle->event = (uint8_t)NONE_PRESS;
            }
            break;

        case 1:
            if (read_gpio_level != key) // 按键抬起
            { 
                if (handle->ticks > 100)
                {
                    handle->event = (uint8_t)LONG_PRESS_UP;
                    handle->state = 0;
                }
                else
                {
                    handle->state = 2; 
                }
                handle->ticks = 0;
            } 
            else if (handle->ticks > LONG_TICKS / 10) // 按键一直按下一定时长
            {
                handle->event = (uint8_t)LONG_RRESS_START;
                handle->state = 5; // 长按状态
            }
            break;

        case 2:
            if ((read_gpio_level == key) && (read_gpio_level != NO_KEY)) // 连续有按键按下
            { 
                handle->event = (uint8_t)PRESS_DOWN;
                handle->repeat++;
                handle->event = (uint8_t)PRESS_REPEAT;
                handle->ticks = 0;
                handle->state = 3;
            } 
            else if (handle->ticks > SHORT_TICKS) // 短按超时
            { 
                if (handle->repeat == 1) 
                {
                    handle->event = (uint8_t)SINGLE_CLICK;
                } 
                else if (handle->repeat == 2) 
                {
                    handle->event = (uint8_t)DOUBLE_CLICK;
                }
                else if (handle->repeat == 3) 
                {
                    handle->event = (uint8_t)THREE_CLICK;
                }
                else if (handle->repeat == 4) 
                {
                    handle->event = (uint8_t)FORE_CLICK;
                }
                else if (handle->repeat == 5) 
                {
                    handle->event = (uint8_t)FIVE_CLICK;
                }
                else if (handle->repeat == 6) 
                {
                    handle->event = (uint8_t)SIX_CLICK;
                }
                else if (handle->repeat == 7) 
                {
                    handle->event = (uint8_t)SEVEN_CLICK;
                }
                handle->state = 0;
            }
            break;

        case 3:
            if (read_gpio_level != key) // 有按键抬起
            {
                handle->event = (uint8_t)PRESS_UP;
                
                if (handle->ticks < SHORT_TICKS) // 短按范围内
                {
                    handle->ticks = 0;
                    handle->state = 2; 
                } 
                else 
                {
                    handle->state = 0;
                }
            }
            else if (handle->ticks > LONG_TICKS) // 按键一直按下一定时长
            {
                handle->event = (uint8_t)LONG_RRESS_START;
                handle->state = 5; // 长按状态
            }
            break;

        case 5:
            if ((read_gpio_level == key) && (read_gpio_level != NO_KEY)) 
            {
                handle->event = (uint8_t)LONG_PRESS_HOLD;
            } 
            else 
            {
                handle->event = (uint8_t)PRESS_UP;
                handle->state = 0;
            }
            break;
    }
    
    return (handle->event);
}


