/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-03-13     Liuguang     the first version.
 * 2018-03-19     Liuguang     add GPIO interrupt mode support.
 * 2019-03-11     JiCheng      Adapt RT1020's IO MAP
 */
#include "drv_pin.h" 

#include "fsl_common.h" 
#include "fsl_iomuxc.h" 
#include "fsl_gpio.h" 

#ifdef RT_USING_PIN

#if defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL
    #error "Please don't define 'FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL'!"
#endif

struct rt1021_pin
{
    rt_uint16_t   pin; 
    GPIO_Type    *gpio; 
    rt_uint32_t   gpio_pin; 
}; 

struct rt1021_irq
{
    rt_uint16_t           enable; 
    struct rt_pin_irq_hdr irq_info; 
};

#define __ARRAY_LEN(array) (sizeof(array)/sizeof(array[0])) 
#define __RT1021_PIN_DEFAULT {0, 0, 0} 
#define __RT1021_PIN(INDEX, PORT, PIN) {INDEX, PORT, PIN} 

static struct rt_pin_ops rt1021_pin_ops; 

static struct rt1021_pin rt1021_pin_map[] = 
{
    __RT1021_PIN_DEFAULT, 
    
    /* GPIO2 */
    __RT1021_PIN( 1, GPIO2,  0),    /* GPIO_EMC_00 */
    __RT1021_PIN( 2, GPIO2,  1),    /* GPIO_EMC_01 */
    __RT1021_PIN( 3, GPIO2,  2),    /* GPIO_EMC_02 */
    __RT1021_PIN( 4, GPIO2,  3),    /* GPIO_EMC_03 */
    __RT1021_PIN( 5, GPIO2,  4),    /* GPIO_EMC_04 */
    __RT1021_PIN( 6, GPIO2,  5),    /* GPIO_EMC_05 */
    __RT1021_PIN( 7, GPIO2,  6),    /* GPIO_EMC_06 */
    __RT1021_PIN( 8, GPIO2,  7),    /* GPIO_EMC_07 */
    __RT1021_PIN( 9, GPIO2,  8),    /* GPIO_EMC_08 */
    __RT1021_PIN(10, GPIO2,  9),    /* GPIO_EMC_09 */
    __RT1021_PIN(11, GPIO2, 10),    /* GPIO_EMC_10 */
    __RT1021_PIN(12, GPIO2, 11),    /* GPIO_EMC_11 */
    __RT1021_PIN(13, GPIO2, 12),    /* GPIO_EMC_12 */
    __RT1021_PIN(14, GPIO2, 13),    /* GPIO_EMC_13 */
    __RT1021_PIN(15, GPIO2, 14),    /* GPIO_EMC_14 */
    __RT1021_PIN(16, GPIO2, 15),    /* GPIO_EMC_15 */
    __RT1021_PIN(17, GPIO2, 16),    /* GPIO_EMC_16 */
    __RT1021_PIN(18, GPIO2, 17),    /* GPIO_EMC_17 */
    __RT1021_PIN(19, GPIO2, 18),    /* GPIO_EMC_18 */
    __RT1021_PIN(20, GPIO2, 19),    /* GPIO_EMC_19 */
    __RT1021_PIN(21, GPIO2, 20),    /* GPIO_EMC_20 */
    __RT1021_PIN(22, GPIO2, 21),    /* GPIO_EMC_21 */
    __RT1021_PIN(23, GPIO2, 22),    /* GPIO_EMC_22 */
    __RT1021_PIN(24, GPIO2, 23),    /* GPIO_EMC_23 */
    __RT1021_PIN(25, GPIO2, 24),    /* GPIO_EMC_24 */
    __RT1021_PIN(26, GPIO2, 25),    /* GPIO_EMC_25 */
    __RT1021_PIN(27, GPIO2, 26),    /* GPIO_EMC_26 */
    __RT1021_PIN(28, GPIO2, 27),    /* GPIO_EMC_27 */
    __RT1021_PIN(29, GPIO2, 28),    /* GPIO_EMC_28 */
    __RT1021_PIN(30, GPIO2, 29),    /* GPIO_EMC_29 */
    __RT1021_PIN(31, GPIO2, 30),    /* GPIO_EMC_30 */
    __RT1021_PIN(32, GPIO2, 31),    /* GPIO_EMC_31 */
    
    __RT1021_PIN(33, GPIO3, 0),    /* GPIO_EMC_32 */
    __RT1021_PIN(34, GPIO3, 1),    /* GPIO_EMC_33 */
    __RT1021_PIN(35, GPIO3, 2),    /* GPIO_EMC_34 */
    __RT1021_PIN(36, GPIO3, 3),    /* GPIO_EMC_35 */
    __RT1021_PIN(37, GPIO3, 4),    /* GPIO_EMC_36 */
    __RT1021_PIN(38, GPIO3, 5),    /* GPIO_EMC_37 */
    __RT1021_PIN(39, GPIO3, 6),    /* GPIO_EMC_38 */
    __RT1021_PIN(40, GPIO3, 7),    /* GPIO_EMC_39 */
    __RT1021_PIN(41, GPIO3, 8),    /* GPIO_EMC_40 */
    __RT1021_PIN(42, GPIO3, 9),    /* GPIO_EMC_41 */
	
    /* GPIO1 */
    __RT1021_PIN(43, GPIO1,  0),    /* GPIO_AD_B0_00 */ 
    __RT1021_PIN(44, GPIO1,  1),    /* GPIO_AD_B0_01 */
    __RT1021_PIN(45, GPIO1,  2),    /* GPIO_AD_B0_02 */
    __RT1021_PIN(46, GPIO1,  3),    /* GPIO_AD_B0_03 */
    __RT1021_PIN(47, GPIO1,  4),    /* GPIO_AD_B0_04 */
    __RT1021_PIN(48, GPIO1,  5),    /* GPIO_AD_B0_05 */
    __RT1021_PIN(49, GPIO1,  6),    /* GPIO_AD_B0_06 */
    __RT1021_PIN(50, GPIO1,  7),    /* GPIO_AD_B0_07 */
    __RT1021_PIN(51, GPIO1,  8),    /* GPIO_AD_B0_08 */
    __RT1021_PIN(52, GPIO1,  9),    /* GPIO_AD_B0_09 */
    __RT1021_PIN(53, GPIO1, 10),    /* GPIO_AD_B0_10 */
    __RT1021_PIN(54, GPIO1, 11),    /* GPIO_AD_B0_11 */
    __RT1021_PIN(55, GPIO1, 12),    /* GPIO_AD_B0_12 */
    __RT1021_PIN(56, GPIO1, 13),    /* GPIO_AD_B0_13 */
    __RT1021_PIN(57, GPIO1, 14),    /* GPIO_AD_B0_14 */
    __RT1021_PIN(58, GPIO1, 15),    /* GPIO_AD_B0_15 */
    __RT1021_PIN(59, GPIO1, 16),    /* GPIO_AD_B1_00 */
    __RT1021_PIN(60, GPIO1, 17),    /* GPIO_AD_B1_01 */
    __RT1021_PIN(61, GPIO1, 18),    /* GPIO_AD_B1_02 */
    __RT1021_PIN(62, GPIO1, 19),    /* GPIO_AD_B1_03 */
    __RT1021_PIN(63, GPIO1, 20),    /* GPIO_AD_B1_04 */
    __RT1021_PIN(64, GPIO1, 21),    /* GPIO_AD_B1_05 */
    __RT1021_PIN(65, GPIO1, 22),    /* GPIO_AD_B1_06 */
    __RT1021_PIN(66, GPIO1, 23),    /* GPIO_AD_B1_07 */
    __RT1021_PIN(67, GPIO1, 24),    /* GPIO_AD_B1_08 */
    __RT1021_PIN(68, GPIO1, 25),    /* GPIO_AD_B1_09 */
    __RT1021_PIN(69, GPIO1, 26),    /* GPIO_AD_B1_10 */
    __RT1021_PIN(70, GPIO1, 27),    /* GPIO_AD_B1_11 */
    __RT1021_PIN(71, GPIO1, 28),    /* GPIO_AD_B1_12 */
    __RT1021_PIN(72, GPIO1, 29),    /* GPIO_AD_B1_13 */
    __RT1021_PIN(73, GPIO1, 30),    /* GPIO_AD_B1_14 */
    __RT1021_PIN(74, GPIO1, 31),    /* GPIO_AD_B1_15 */
    
    /* GPIO3 */
    __RT1021_PIN(75, GPIO3, 13),    /* GPIO_SD_B0_00 */
    __RT1021_PIN(76, GPIO3, 14),    /* GPIO_SD_B0_01 */
    __RT1021_PIN(77, GPIO3, 15),    /* GPIO_SD_B0_02 */
    __RT1021_PIN(78, GPIO3, 16),    /* GPIO_SD_B0_03 */
    __RT1021_PIN(79, GPIO3, 17),    /* GPIO_SD_B0_04 */
    __RT1021_PIN(80, GPIO3, 18),    /* GPIO_SD_B0_05 */
    __RT1021_PIN(81, GPIO3, 19),    /* GPIO_SD_B0_06 */
    __RT1021_PIN(82, GPIO3, 20),    /* GPIO_SD_B1_00 */
    __RT1021_PIN(83, GPIO3, 21),    /* GPIO_SD_B1_01 */
    __RT1021_PIN(84, GPIO3, 22),    /* GPIO_SD_B1_02 */
    __RT1021_PIN(85, GPIO3, 23),    /* GPIO_SD_B1_03 */
    __RT1021_PIN(86, GPIO3, 24),    /* GPIO_SD_B1_04 */
    __RT1021_PIN(87, GPIO3, 25),    /* GPIO_SD_B1_05 */
    __RT1021_PIN(88, GPIO3, 26),    /* GPIO_SD_B1_06 */
    __RT1021_PIN(89, GPIO3, 27),    /* GPIO_SD_B1_07 */
    __RT1021_PIN(90, GPIO3, 28),    /* GPIO_SD_B1_08 */
    __RT1021_PIN(91, GPIO3, 29),    /* GPIO_SD_B1_09 */
    __RT1021_PIN(92, GPIO3, 30),    /* GPIO_SD_B1_10 */
		__RT1021_PIN(93, GPIO3, 31),    /* GPIO_SD_B1_11 */

    /* GPIO5 */
    __RT1021_PIN(94, GPIO5,  0),    /* WAKEUP */
    __RT1021_PIN(95, GPIO5,  1),    /* PMIC_ON_REQ */
    __RT1021_PIN(96, GPIO5,  2)     /* PMIC_STBY_REQ */
}; 

static struct rt1021_irq rt1021_irq_map[] = 
{
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} },
    {PIN_IRQ_DISABLE, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL} }
}; 

void gpio_isr(GPIO_Type* base, rt_uint32_t gpio_pin)
{
    if((GPIO_PortGetInterruptFlags(base) & (1 << gpio_pin)) != 0)
    {
        GPIO_PortClearInterruptFlags(base, gpio_pin); 
        
        if(rt1021_irq_map[gpio_pin].irq_info.hdr != RT_NULL)
        {
            rt1021_irq_map[gpio_pin].irq_info.hdr(rt1021_irq_map[gpio_pin].irq_info.args);
        }
    }
}

void GPIO1_Combined_0_15_IRQHandler(void)
{
    rt_uint8_t gpio_pin; 
    
    rt_interrupt_enter();

    for(gpio_pin = 0; gpio_pin <= 15; gpio_pin++)
    {
        gpio_isr(GPIO1, gpio_pin); 
    }
    
    rt_interrupt_leave();
}

void GPIO1_Combined_16_31_IRQHandler(void)
{
    rt_uint8_t gpio_pin; 
    
    rt_interrupt_enter();

    for(gpio_pin = 16; gpio_pin <= 31; gpio_pin++)
    {
        gpio_isr(GPIO1, gpio_pin); 
    }
    
    rt_interrupt_leave();
}

void GPIO2_Combined_0_15_IRQHandler(void)
{
    rt_uint8_t gpio_pin; 
    
    rt_interrupt_enter();

    for(gpio_pin = 0; gpio_pin <= 15; gpio_pin++)
    {
        gpio_isr(GPIO2, gpio_pin); 
    }
    
    rt_interrupt_leave();
}

void GPIO2_Combined_16_31_IRQHandler(void)
{
    rt_uint8_t gpio_pin; 
    
    rt_interrupt_enter();

    for(gpio_pin = 16; gpio_pin <= 31; gpio_pin++)
    {
        gpio_isr(GPIO2, gpio_pin); 
    }
    
    rt_interrupt_leave();
}

void GPIO3_Combined_0_15_IRQHandler(void) 
{
    rt_uint8_t gpio_pin; 
    
    rt_interrupt_enter();

    for(gpio_pin = 0; gpio_pin <= 15; gpio_pin++)
    {
        gpio_isr(GPIO3, gpio_pin); 
    }
    
    rt_interrupt_leave();
}

void GPIO3_Combined_16_31_IRQHandler(void)
{
    rt_uint8_t gpio_pin; 
    
    rt_interrupt_enter();

    for(gpio_pin = 16; gpio_pin <= 31; gpio_pin++)
    {
        gpio_isr(GPIO3, gpio_pin); 
    }
    
    rt_interrupt_leave();
}

void GPIO5_Combined_0_15_IRQHandler(void)
{ 
    rt_uint8_t gpio_pin; 
    
    rt_interrupt_enter();

    for(gpio_pin = 0; gpio_pin <= 2; gpio_pin++)
    {
        gpio_isr(GPIO5, gpio_pin); 
    }

    rt_interrupt_leave();
}

static IRQn_Type rt1021_get_irqnum(GPIO_Type *gpio, rt_uint32_t gpio_pin)
{
    IRQn_Type irq_num = -100;  /* Invalid interrupt number */
    
    if(gpio == GPIO1)
    {
        if(gpio_pin <= 15)
        {
            irq_num = GPIO1_Combined_0_15_IRQn; 
        }
        else
        {
            irq_num = GPIO1_Combined_16_31_IRQn; 
        }
    }
    else if(gpio == GPIO2)
    {
        if(gpio_pin <= 15)
        {
            irq_num = GPIO2_Combined_0_15_IRQn; 
        }
        else
        {
            irq_num = GPIO2_Combined_16_31_IRQn; 
        }
    }
    else if(gpio == GPIO3)
    {
        if(gpio_pin <= 15)
        {
            irq_num = GPIO3_Combined_0_15_IRQn; 
        }
        else
        {
            irq_num = GPIO3_Combined_16_31_IRQn; 
        }
    }
    else if(gpio == GPIO5)
    {
        if(gpio_pin <= 15)
        {
            irq_num = GPIO5_Combined_0_15_IRQn; 
        }
        else
        {
            irq_num = GPIO5_Combined_16_31_IRQn; 
        }
    }
    
    return irq_num; 
}

static void rt1021_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    gpio_pin_config_t gpio; 
    rt_uint32_t config_value = 0; 
    
    if((pin > __ARRAY_LEN(rt1021_pin_map)) || (pin == 0))
    {
        return; 
    }
    
    if(rt1021_pin_map[pin].gpio != GPIO5)
    {
        CLOCK_EnableClock(kCLOCK_Iomuxc); 
        IOMUXC_SetPinMux(0x401F8010U + pin*4, 0x5U, 0, 0, 0, 1); 
    }
    else
    {
        CLOCK_EnableClock(kCLOCK_IomuxcSnvs); 
        IOMUXC_SetPinMux(0x400A8000U + (pin-94)*4, 0x5U, 0, 0, 0, 1); 
    }
    
    gpio.outputLogic = 0; 
    gpio.interruptMode = kGPIO_NoIntmode; 
    
    switch(mode)
    {
        case PIN_MODE_OUTPUT:
        {
            config_value = 0x1030U;
            gpio.direction = kGPIO_DigitalOutput; 
        }
        break; 
        
        case PIN_MODE_INPUT:
        {
            config_value = 0x1030U;
            gpio.direction = kGPIO_DigitalInput; 
        }
        break;
        
        case PIN_MODE_INPUT_PULLDOWN:
        {
            config_value = 0x1030U;
            gpio.direction = kGPIO_DigitalInput; 
        }
        break; 
        
        case PIN_MODE_INPUT_PULLUP:
        {
            config_value = 0x5030U;
            gpio.direction = kGPIO_DigitalInput; 
        }
        break;
        
        case PIN_MODE_OUTPUT_OD:
        {
            config_value = 0x1830U;
            gpio.direction = kGPIO_DigitalOutput; 
        }
        break;
    }

    if(rt1021_pin_map[pin].gpio != GPIO5)
    {
        IOMUXC_SetPinConfig(0, 0, 0, 0, 0x401F8200U + pin*4, config_value); 
    }
    else
    {
        IOMUXC_SetPinConfig(0, 0, 0, 0, 0x400A8018U + (pin-94)*4, config_value); 
    }
    
    GPIO_PinInit(rt1021_pin_map[pin].gpio, rt1021_pin_map[pin].gpio_pin, &gpio); 
}

static int rt1021_pin_read(rt_device_t dev, rt_base_t pin)
{
    return GPIO_PinReadPadStatus(rt1021_pin_map[pin].gpio, rt1021_pin_map[pin].gpio_pin); 
}

static void rt1021_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    GPIO_PinWrite(rt1021_pin_map[pin].gpio, rt1021_pin_map[pin].gpio_pin, value);
}

static rt_err_t rt1021_pin_attach_irq(struct rt_device *device, rt_int32_t pin,
    rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
    struct rt1021_pin* pin_map = RT_NULL; 
    struct rt1021_irq* irq_map = RT_NULL; 
    
    pin_map = &rt1021_pin_map[pin]; 
    irq_map = &rt1021_irq_map[rt1021_pin_map[pin].gpio_pin]; 
    
    if(pin_map == RT_NULL || irq_map == RT_NULL) 
    {
        return RT_ENOSYS; 
    }
    
    if(irq_map->enable == PIN_IRQ_ENABLE)
    {
        return RT_EBUSY; 
    }
    
    irq_map->irq_info.pin  = pin; 
    irq_map->irq_info.hdr  = hdr; 
    irq_map->irq_info.mode = mode; 
    irq_map->irq_info.args = args; 
    
    return RT_EOK;
}

static rt_err_t rt1021_pin_detach_irq(struct rt_device *device, rt_int32_t pin)
{
    struct rt1021_pin* pin_map = RT_NULL; 
    struct rt1021_irq* irq_map = RT_NULL; 
    
    pin_map = &rt1021_pin_map[pin]; 
    irq_map = &rt1021_irq_map[rt1021_pin_map[pin].gpio_pin]; 
    
    if(pin_map == RT_NULL || irq_map == RT_NULL) 
    {
        return RT_ENOSYS; 
    }
    
    if(irq_map->enable == PIN_IRQ_DISABLE)
    {
        return RT_EOK; 
    }
    
    irq_map->irq_info.pin  = PIN_IRQ_PIN_NONE; 
    irq_map->irq_info.hdr  = RT_NULL; 
    irq_map->irq_info.mode = PIN_IRQ_MODE_RISING; 
    irq_map->irq_info.args = RT_NULL; 
    
    return RT_EOK;
}

static rt_err_t rt1021_pin_irq_enable(struct rt_device *device, rt_base_t pin, rt_uint32_t enabled)
{
    gpio_pin_config_t gpio; 
    IRQn_Type irq_num;
    rt_uint32_t config_value = 0x1b0a0; 
    
    struct rt1021_pin* pin_map = RT_NULL; 
    struct rt1021_irq* irq_map = RT_NULL; 
    
    pin_map = &rt1021_pin_map[pin]; 
    irq_map = &rt1021_irq_map[rt1021_pin_map[pin].gpio_pin]; 
    
    if(pin_map == RT_NULL || irq_map == RT_NULL) 
    {
        return RT_ENOSYS; 
    }
    
    if(enabled == PIN_IRQ_ENABLE) 
    {
        if(irq_map->enable == PIN_IRQ_ENABLE)
        {
            return RT_EBUSY; 
        }
        
        if(irq_map->irq_info.pin != pin)
        {
            return RT_EIO; 
        }
        
        irq_map->enable = PIN_IRQ_ENABLE; 
        
        if(rt1021_pin_map[pin].gpio != GPIO5)
        {
            CLOCK_EnableClock(kCLOCK_Iomuxc); 
            IOMUXC_SetPinMux(0x401F8010U + pin*4, 0x5U, 0, 0, 0, 0); 
        }
        else
        {
            CLOCK_EnableClock(kCLOCK_IomuxcSnvs); 
            IOMUXC_SetPinMux(0x400A8000U + (pin-94)*4, 0x5U, 0, 0, 0, 0); 
        }
        
        gpio.direction     = kGPIO_DigitalInput; 
        gpio.outputLogic   = 0; 
        
        switch(irq_map->irq_info.mode)
        {
            case PIN_IRQ_MODE_RISING:
            {
                gpio.interruptMode = kGPIO_IntRisingEdge; 
            }
            break;
            
            case PIN_IRQ_MODE_FALLING:
            {
                gpio.interruptMode = kGPIO_IntFallingEdge; 
            }
            break;
            
            case PIN_IRQ_MODE_RISING_FALLING:
            {
                gpio.interruptMode = kGPIO_IntRisingOrFallingEdge; 
            }
            break;
            
            case PIN_IRQ_MODE_HIGH_LEVEL:
            {
                gpio.interruptMode = kGPIO_IntHighLevel; 
            }
            break;
            
            case PIN_IRQ_MODE_LOW_LEVEL:
            {
                gpio.interruptMode = kGPIO_IntLowLevel; 
            }
            break;
        }
        
        if(rt1021_pin_map[pin].gpio != GPIO5)
        {
            IOMUXC_SetPinConfig(0, 0, 0, 0, 0x401F8200U + pin*4, config_value); 
        }
        else
        {
            IOMUXC_SetPinConfig(0, 0, 0, 0, 0x400A8018U + (pin-94)*4, config_value); 
        }
        
        irq_num = rt1021_get_irqnum(rt1021_pin_map[pin].gpio, rt1021_pin_map[pin].gpio_pin); 
        
        NVIC_SetPriority(irq_num, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
        EnableIRQ(irq_num);
        
        GPIO_PinInit(rt1021_pin_map[pin].gpio, rt1021_pin_map[pin].gpio_pin, &gpio); 
        GPIO_PortEnableInterrupts(rt1021_pin_map[pin].gpio, 1U << rt1021_pin_map[pin].gpio_pin); 
    }
    else if(enabled == PIN_IRQ_DISABLE)
    {
        if(irq_map->enable == PIN_IRQ_DISABLE)
        {
            return RT_EOK; 
        }
        
        irq_map->enable = PIN_IRQ_DISABLE;
        irq_num = rt1021_get_irqnum(rt1021_pin_map[pin].gpio, rt1021_pin_map[pin].gpio_pin); 
        
        NVIC_DisableIRQ(irq_num);
    }
    else
    {
        return RT_EINVAL; 
    }
    
    return RT_EOK;
}

int rt_hw_pin_init(void)
{
    int ret = RT_EOK;
    
    rt1021_pin_ops.pin_mode        = rt1021_pin_mode; 
    rt1021_pin_ops.pin_read        = rt1021_pin_read; 
    rt1021_pin_ops.pin_write       = rt1021_pin_write; 
    rt1021_pin_ops.pin_attach_irq  = rt1021_pin_attach_irq; 
    rt1021_pin_ops.pin_detach_irq  = rt1021_pin_detach_irq;
    rt1021_pin_ops.pin_irq_enable  = rt1021_pin_irq_enable; 
    
    ret = rt_device_pin_register("pin", &rt1021_pin_ops, RT_NULL);
    
    return ret;
}
INIT_BOARD_EXPORT(rt_hw_pin_init); 

#endif /*RT_USING_PIN */
