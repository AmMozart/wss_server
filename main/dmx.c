#include <stdio.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "dmx.h"

#define ECHO_TEST_TXD (GPIO_NUM_4)
#define ECHO_TEST_RXD (GPIO_NUM_5)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

void set_dmx_level(int channel, uint8_t value)
{
  dmx_data_arr[channel] = value;
}

void uart_init(void *arg)
{
  uart_config_t uart_config = {
      .baud_rate = 250000,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_2,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_APB,
  };
  uart_driver_install(UART_NUM_1, 2048, 2048, 0, NULL, 0);
  uart_param_config(UART_NUM_1, &uart_config);
  uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
}

static void timer_tg0_isr(void *arg)
{
  static int flag = 0;
  flag += 1;

  switch (flag)
  {
  case 1:
  {
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 88);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_4, 0);
    break;
  }
  case 2:
  {
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 8);
    gpio_set_level(GPIO_NUM_4, 1);
    break;
  }
  case 3:
  {
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_write_bytes(UART_NUM_1, (const char *)&dmx_data_arr, BUF_SIZE);

    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 30000); // Frequency DMX package 30 uSec
    flag = 0;
    break;
  }
  }
  //Reset irq and set for next time
  TIMERG0.int_clr_timers.t0 = 1;
  TIMERG0.hw_timer[0].config.alarm_en = 1;
}

void timer_tg0_initialise(int timer_period_us)
{
  timer_config_t config = {
      .alarm_en = true,              //Alarm Enable
      .counter_en = false,           //If the counter is enabled it will start incrementing / decrementing immediately after calling timer_init()
      .intr_type = TIMER_INTR_LEVEL, //Is interrupt is triggered on timer’s alarm (timer_intr_mode_t)
      .counter_dir = TIMER_COUNT_UP, //Does counter increment or decrement (timer_count_dir_t)
      .auto_reload = true,           //If counter should auto_reload a specific initial value on the timer’s alarm, or continue incrementing or decrementing.
      .divider = 80                  //Divisor of the incoming 80 MHz (12.5nS) APB_CLK clock. E.g. 80 = 1uS per timer tick
  };

  timer_init(TIMER_GROUP_0, TIMER_0, &config);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, timer_period_us);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_tg0_isr, NULL, 0, NULL);

  timer_start(TIMER_GROUP_0, TIMER_0);
}

void start_dmx(void)
{
  uart_init(NULL);
  dmx_data_arr[0] = 0x0;     // MAB
  timer_tg0_initialise(100); //random us
}