/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-08     obito0   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/* defined the LED0 pin: PC13 */
#define LED0_PIN    GET_PIN(C, 13)
#define LED_PRIO 5
#define LED_SIZE 256
#define LED_TIME 5

#define ADC_PRIO 6
#define ADC_SIZE 256
#define ADC_TIME 5


#define ADC_DEV_NAME    "adc1" /* ADC 设 备 名 称 */
#define ADC_DEV_CHANNEL 1 /* ADC 通 道 */
#define REFER_VOLTAGE   330 /* 参 考 电 压 3.3V,数 据 精 度 乘 以 100保 留2位 小 数*/
#define CONVERT_BITS    (1 << 12) /* 转 换 位 数 为12位 */
rt_adc_device_t adc_dev;

static void LED_entry(void *parameter);
static void ADC_entry(void *parameter);

rt_err_t ret = RT_EOK;

int main(void)
{
	  /* set LED0 pin mode to output */
	  static rt_thread_t tid = RT_NULL;
	  rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
	  /* 查 找 设 备 */
		adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
		if (adc_dev == RT_NULL)
		{
			rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);
			return RT_ERROR;
		}
		/* 使 能 设 备 */
		ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);
	  tid = rt_thread_create("LED_thread",LED_entry,RT_NULL,LED_SIZE,LED_PRIO,LED_TIME);
	  if (tid != RT_NULL)	rt_thread_startup(tid);
		tid = rt_thread_create("ADC_thread",ADC_entry,RT_NULL,ADC_SIZE,ADC_PRIO,ADC_TIME);
	  if (tid != RT_NULL)	rt_thread_startup(tid);
//		while(1){}
    return RT_EOK;
}

void LED_entry(void *parameter)
{
	while (1)
	{
		rt_pin_write(LED0_PIN, PIN_HIGH);
    rt_thread_mdelay(500);
    rt_pin_write(LED0_PIN, PIN_LOW);
    rt_thread_mdelay(500);
	}
}

void ADC_entry(void *parameter)
{
	rt_uint32_t value;
	while (1)
	{
		/* 读 取 采 样 值 */
		value = rt_adc_read(adc_dev, ADC_DEV_CHANNEL);
		/* 转 换 为 对 应 电 压 值 */
		value = value * REFER_VOLTAGE / CONVERT_BITS;
		rt_kprintf("the voltage is :%d.%02d \n", value / 100, value % 100);
		rt_thread_mdelay(500);
	}
}

