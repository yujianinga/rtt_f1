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
#include "mpu6xxx.h"

/* defined the LED0 pin: PC13 */
#define LED0_PIN    GET_PIN(C, 13)
#define LED_PRIO 5
#define LED_SIZE 256
#define LED_TIME 5

#define ADC_PRIO 6
#define ADC_SIZE 256
#define ADC_TIME 5

#define MPU_PRIO 7
#define MPU_SIZE 512
#define MPU_TIME 5


#define ADC_DEV_NAME    "adc1"    /* ADC设备名称 */
#define ADC_DEV_CHANNEL 1         /* ADC通道 */
#define REFER_VOLTAGE   330       /* 参考电压3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS    (1 << 12) /* 转换位数为12位 */

#define MPU6XXX_DEVICE_NAME  "i2c2"

rt_adc_device_t adc_dev;
struct mpu6xxx_device *mpu_dev;

static void LED_entry(void *parameter);
static void ADC_entry(void *parameter);
static void MPU_entry(void *parameter);

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
		/* Initialize mpu6xxx, The parameter is RT_NULL, means auto probing for i2c*/
    mpu_dev = mpu6xxx_init(MPU6XXX_DEVICE_NAME, RT_NULL);
		if (mpu_dev == RT_NULL)
    {
        rt_kprintf("mpu6xxx init failed\n");
        return -1;
    }
    rt_kprintf("mpu6xxx init succeed\n");
		/* 使 能 设 备 */
		ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);
	  tid = rt_thread_create("LED_thread",LED_entry,RT_NULL,LED_SIZE,LED_PRIO,LED_TIME);
	  if (tid != RT_NULL)	rt_thread_startup(tid);
		tid = rt_thread_create("ADC_thread",ADC_entry,RT_NULL,ADC_SIZE,ADC_PRIO,ADC_TIME);
	  if (tid != RT_NULL)	rt_thread_startup(tid);
		tid = rt_thread_create("MPU_thread",MPU_entry,RT_NULL,MPU_SIZE,MPU_PRIO,MPU_TIME);
	  if (tid != RT_NULL)	rt_thread_startup(tid);
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
//		rt_kprintf("the voltage is :%d.%02d \n", value / 100, value % 100);
		rt_thread_mdelay(500);
	}
}

void MPU_entry(void *parameter)
{
	struct mpu6xxx_3axes accel, gyro;
	while (1)
	{
		mpu6xxx_get_accel(mpu_dev, &accel);
    mpu6xxx_get_gyro(mpu_dev, &gyro);
    rt_kprintf("accel.x = %3d, accel.y = %3d, accel.z = %3d ", accel.x, accel.y, accel.z);
    rt_kprintf("gyro.x = %3d gyro.y = %3d, gyro.z = %3d\n", gyro.x, gyro.y, gyro.z);
		rt_thread_mdelay(500);
	}
}

