/*
********************************************************************************
*                                嵌入式微系统
*                                   msOS
*
*                            硬件平台:msPLC DEMO
*                          主芯片:STM32F103R8T6/RBT6
*                           深圳市雨滴科技有限公司
*
*                                作者:王绍伟
*                                网名:凤舞天
*                                标识:Wangsw
*
*                                QQ:26033613
*                               QQ群:291235815
*                        淘宝店:http://52edk.taobao.com
*                      论坛:http://gongkong.eefocus.com/bbs/
*                博客:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*文件名     : device_adc.c
*作用       : ADC设备
*原理       : Adc设备功能块，利用SystemTick节拍实现实时数据采样
*           : 利用注册函数，回馈采样数据给上层应用变量                  
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/01       初始版本
********************************************************************************
*/

#include "drive.h"
#include "system.h"



static ushort NullValue;

static ushort * RegisterPointerBlock[4] =
{
    &NullValue, &NullValue, &NullValue, &NullValue
};


/*******************************************************************************
* 函数名	: Register
* 描述	    : 把应用层变量指针注册到对应的通道数组中，实现底层数据向顶层传递
* 输入参数  : adcChannel: ADC通道号
*           : dataPoint: 应用层变量指针
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/08       初始版本
*******************************************************************************/
static void Register(AdcChannelEnum adcChannel, ushort * dataPointer)
{
    RegisterPointerBlock[adcChannel] = dataPointer;
}


/*******************************************************************************
* 函数名	: AdcSystick10000Routine
* 描述	    : 系统时钟10000/S，即0.1mS间隔调用一次运行，获取ADC值给App层变量
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/10       初始版本
*******************************************************************************/
void AdcSystick10000Routine(void)
{
    static byte Counter = 0;
    
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    *RegisterPointerBlock[Counter] = ADC_GetConversionValue(ADC1);

    if (++Counter > 3) Counter = 0;
    
    ADC_RegularChannelConfig(ADC1,Counter + 10, 1 , ADC_SampleTime_239Cycles5);

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void InitializeAdc(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        ADC_InitTypeDef ADC_InitStructure;
    
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        ADC_DeInit(ADC1);

        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfChannel = 1;
        ADC_Init(ADC1, &ADC_InitStructure);

        ADC_Cmd(ADC1, ENABLE);

        ADC_ResetCalibration(ADC1);

        while(ADC_GetResetCalibrationStatus(ADC1));

        ADC_StartCalibration(ADC1);

        while(ADC_GetCalibrationStatus(ADC1));

        ADC_SoftwareStartConvCmd(ADC1, ENABLE);

        System.Device.Adc.Register = Register;
}

