/*
********************************************************************************
*                                嵌入式微系统
*                         
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
    
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//等待转换结束
    *RegisterPointerBlock[Counter] = ADC_GetConversionValue(ADC1);

    //if (++Counter > 3) Counter = 0;
    
    ADC_RegularChannelConfig(ADC1,0,1,ADC_SampleTime_239Cycles5);//设置规则序列通道以及采样周期

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能指定的 ADC1 的软件转换启动功能
}

void InitializeAdc(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        ADC_InitTypeDef ADC_InitStructure;

        //使用ADC1的通道0,对应的是PA0
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);//设置分频因子位 6，时钟为 72/6=12MHz
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        ADC_DeInit(ADC1);//ADC 时钟复位

        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC 的模式:独立模式
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;//设置是否开启扫描模式
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//设置是否开启连续转换模式
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//设置启动规则转换组转换的外部事件
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//设置 ADC 数据对齐方式是左对齐还是右对齐
        ADC_InitStructure.ADC_NbrOfChannel = 1;//顺序进行规则转换的 ADC 通道的数目 1
        ADC_Init(ADC1, &ADC_InitStructure);

        ADC_Cmd(ADC1, ENABLE);//使能指定的 ADC1

        ADC_ResetCalibration(ADC1);//执行复位校准

        while(ADC_GetResetCalibrationStatus(ADC1));//等待复位校准结束

        ADC_StartCalibration(ADC1);//开始指定 ADC1 的校准状态

        while(ADC_GetCalibrationStatus(ADC1));//等待校 AD 准结束

        ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能指定的 ADC1 的软件转换启动功能

        System.Device.Adc.Register = Register;
}

