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
*文件名     : menu.c
*作用       : 界面设计
*原理       : 例子程序中包含五个窗体Form，每一个窗体可以加载BackText背景文字、
*           : Chart表、Label标签、TextBox文本四类控件，采用Add方法加载之后，
*           : 需要初始化值。
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/

#include "system.h"
#include "app.h"




//定义Chart页面自检表
static Chart CheckChart;

//定义数据label
static TextBox FrequencyTextBox;
static TextBox PowerPercentTextBox;
static Label PowerLabel;
static Label VoltageLabel;
static Label CurrentLabel;
static Label TemperatureLabel;
static Label StateLabel;

static TextBox MaxPowerTextBox;
static TextBox MaxTemperatureTextBox;
static TextBox MaxFrequencyTextBox;
static TextBox MaxFrequencyOffsetTextBox;
static TextBox MinStreamTextBox;
static Label TimeLabel;


static TextBox SerialNumberTextBox;
static TextBox ProductionDateTextBox;
static Label IdentifyNumber0Label;
static Label IdentifyNumber1Label;
static Label IdentifyNumber2Label;



const string StateString[5] = 
{
    "开机扫频",
    "正常待机",
    "定频工作",
    "锁相工作",
    "异常报警"
};

/*******************************************************************************
* 函数名	: InitializeMmi
* 描述	    : 初始化MMI,加载各类控件到各个窗体中
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
void InitializeMenu(void)
{
    byte i;

//页面焦点
    App.Menu.FocusFormPointer = &App.Menu.LogoForm;

//Logo Form
    System.Gui.Form.Initialize(&App.Menu.LogoForm);
    App.Menu.LogoForm.BackTextPointer = "    雨滴科技    "
                                        "      msOS      "
                                        "   Stm32-V0.09  "
                                        "   2014.05.05   ";
//Check Form
    System.Gui.Form.Initialize(&App.Menu.CheckForm);
    CheckChart.Character = '*';
    for(i = 0; i < 16; i++)
    {
        CheckChart.Column[i] = 0;
    }
    App.Menu.CheckForm.ChartPointer = &CheckChart;

//Work Form
    System.Gui.Form.Initialize(&App.Menu.WorkForm);
    App.Menu.WorkForm.BackTextPointer = "频率          Hz"
                                        "功率   %       W"
                                        "电压    电流    "
                                        "温度   C        ";

    System.Gui.Form.AddTextBox(&App.Menu.WorkForm, &FrequencyTextBox);
    FrequencyTextBox.DataPointer = (void *)(&App.Data.Frequency);
    FrequencyTextBox.DataMax = 980000;
    FrequencyTextBox.DataMin = 600000;
    FrequencyTextBox.DataStep = 1000;
    FrequencyTextBox.DataBigStep = 10000;
    FrequencyTextBox.Type = GuiDataTypeIntDec;
    FrequencyTextBox.X = 13;
    FrequencyTextBox.Y = 0;
    
    System.Gui.Form.AddTextBox(&App.Menu.WorkForm, &PowerPercentTextBox);
    PowerPercentTextBox.DataPointer = (void *)(&App.Data.PowerPercent);
    PowerPercentTextBox.Type = GuiDataTypeIntDec;
    PowerPercentTextBox.DataMax = 100;
    PowerPercentTextBox.DataMin = 0;
    PowerPercentTextBox.DataStep = 1;
    PowerPercentTextBox.DataBigStep = 10;
    PowerPercentTextBox.X = 6;
    PowerPercentTextBox.Y = 1;
    
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &PowerLabel);
    PowerLabel.DataPointer = (void *)(&App.Data.Power);
    PowerLabel.Type = GuiDataTypeUintDec;
    PowerLabel.X = 14;
    PowerLabel.Y = 1;
    
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &VoltageLabel);
    VoltageLabel.DataPointer = (void *)(&App.Data.Voltage);
    VoltageLabel.Type = GuiDataTypeIntDec;
    VoltageLabel.X = 7;
    VoltageLabel.Y = 2;
    
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &CurrentLabel);
    CurrentLabel.DataPointer = (void *)(&App.Data.Current);
    CurrentLabel.Type = GuiDataTypeIntDec;
    CurrentLabel.X = 15;
    CurrentLabel.Y = 2;
    
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &TemperatureLabel);
    TemperatureLabel.DataPointer = (void *)(&App.Data.Temperature);
    TemperatureLabel.Type = GuiDataTypeIntDec;
    TemperatureLabel.X = 6;
    TemperatureLabel.Y = 3;
    
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &StateLabel);
    StateLabel.Align = GuiDataAlignLeft;
    StateLabel.DataPointer = (void *)(&App.Data.State);
    StateLabel.StringBlockPointer = StateString;
    StateLabel.Type = GuiDataTypeString;
    StateLabel.X = 8;
    StateLabel.Y = 3;

//Setup Form
    System.Gui.Form.Initialize(&App.Menu.SetupForm);
    App.Menu.SetupForm.BackTextPointer = "功率    温度   C"
                                         "频率   K频偏   K"
                                         "水流            "
                                         "时间            ";
    System.Gui.Form.AddTextBox(&App.Menu.SetupForm, &MaxPowerTextBox);
    MaxPowerTextBox.DataPointer = (void *)(&App.Data.MaxPower);
    MaxPowerTextBox.DataMax = 7500;
    MaxPowerTextBox.DataMin = 4000;
    MaxPowerTextBox.DataStep = 500;
    MaxPowerTextBox.DataBigStep = 500;
    MaxPowerTextBox.Type = GuiDataTypeIntDec;
    MaxPowerTextBox.X = 7;
    MaxPowerTextBox.Y = 0;
    
    System.Gui.Form.AddTextBox(&App.Menu.SetupForm, &MaxTemperatureTextBox);
    MaxTemperatureTextBox.DataPointer = (void *)(&App.Data.MaxTemperature);
    MaxTemperatureTextBox.DataMax = 60;
    MaxTemperatureTextBox.DataMin = 40;
    MaxTemperatureTextBox.DataStep = 1;
    MaxTemperatureTextBox.DataBigStep = 1;
    MaxTemperatureTextBox.Type = GuiDataTypeIntDec;
    MaxTemperatureTextBox.X = 14;
    MaxTemperatureTextBox.Y = 0;
    
    System.Gui.Form.AddTextBox(&App.Menu.SetupForm, &MaxFrequencyTextBox);
    MaxFrequencyTextBox.DataPointer = (void *)(&App.Data.MaxFrequency);
    MaxFrequencyTextBox.DataMax = 980000;
    MaxFrequencyTextBox.DataMin = 500000;
    MaxFrequencyTextBox.DataStep = 100000;
    MaxFrequencyTextBox.DataBigStep = 100000;
    MaxFrequencyTextBox.Shift= 3;
    MaxFrequencyTextBox.Type = GuiDataTypeIntDec;
    MaxFrequencyTextBox.X = 6;
    MaxFrequencyTextBox.Y = 1;
    
    System.Gui.Form.AddTextBox(&App.Menu.SetupForm, &MaxFrequencyOffsetTextBox);
    MaxFrequencyOffsetTextBox.DataPointer = (void *)(&App.Data.MaxFrequencyOffset);
    MaxFrequencyOffsetTextBox.DataMax = 100000;
    MaxFrequencyOffsetTextBox.DataMin = 50000;
    MaxFrequencyOffsetTextBox.DataStep = 10000;
    MaxFrequencyOffsetTextBox.DataBigStep = 10000;
    MaxFrequencyOffsetTextBox.Shift = 3;
    MaxFrequencyOffsetTextBox.Type = GuiDataTypeIntDec;
    MaxFrequencyOffsetTextBox.X = 14;
    MaxFrequencyOffsetTextBox.Y = 1;

    System.Gui.Form.AddTextBox(&App.Menu.SetupForm, &MinStreamTextBox);
    MinStreamTextBox.DataPointer = (void *)(&App.Data.MinStream);
    MinStreamTextBox.DataMax = 100;
    MinStreamTextBox.DataMin = 50;
    MinStreamTextBox.DataStep = 1;
    MinStreamTextBox.DataBigStep = 1;
    MinStreamTextBox.Type = GuiDataTypeIntDec;
    MinStreamTextBox.X = 7;
    MinStreamTextBox.Y = 2;

    System.Gui.Form.AddLabel(&App.Menu.SetupForm, &TimeLabel);
    TimeLabel.DataPointer = (void *)(&(System.Device.Rtc));
    TimeLabel.Type = GuiDataTypeTime;
    TimeLabel.X = 15;
    TimeLabel.Y = 3;

//Service Form
    System.Gui.Form.Initialize(&App.Menu.ServiceForm);
    App.Menu.ServiceForm.BackTextPointer = "出厂编号        "
                                           "出厂日期        "
                                           "                "
                                           "                ";
    System.Gui.Form.AddTextBox(&App.Menu.ServiceForm, &SerialNumberTextBox);
    SerialNumberTextBox.DataPointer = (void *)(&App.Data.SerialNumber);
    SerialNumberTextBox.DataMax = 10000;
    SerialNumberTextBox.DataMin = 1;
    SerialNumberTextBox.DataStep = 1;
    SerialNumberTextBox.DataBigStep = 100;
    SerialNumberTextBox.Type = GuiDataTypeUintDec;
    SerialNumberTextBox.X = 15;
    SerialNumberTextBox.Y = 0;
    
    System.Gui.Form.AddTextBox(&App.Menu.ServiceForm, &ProductionDateTextBox);
    ProductionDateTextBox.DataPointer = (void *)(&App.Data.ProductionDate);
    ProductionDateTextBox.DataStep = 1;
    ProductionDateTextBox.DataBigStep = 256;
    ProductionDateTextBox.Type = GuiDataTypeDate;
    ProductionDateTextBox.X = 15;
    ProductionDateTextBox.Y = 1;
    
    System.Gui.Form.AddLabel(&App.Menu.ServiceForm, &IdentifyNumber0Label);
    IdentifyNumber0Label.DataPointer = (void *)(&App.Data.IdentifyNumber0);
    IdentifyNumber0Label.Type = GuiDataTypeHex;
    IdentifyNumber0Label.X = 7;
    IdentifyNumber0Label.Y = 2;
    IdentifyNumber0Label.Flexible = 8;
    
    System.Gui.Form.AddLabel(&App.Menu.ServiceForm, &IdentifyNumber1Label);
    IdentifyNumber1Label.DataPointer = (void *)(&App.Data.IdentifyNumber1);
    IdentifyNumber1Label.Type = GuiDataTypeHex;
    IdentifyNumber1Label.X = 15;
    IdentifyNumber1Label.Y = 2;
    IdentifyNumber1Label.Flexible = 8;
    
    System.Gui.Form.AddLabel(&App.Menu.ServiceForm, &IdentifyNumber2Label);
    IdentifyNumber2Label.DataPointer = (void *)(&App.Data.IdentifyNumber2);
    IdentifyNumber2Label.Type = GuiDataTypeHex;
    IdentifyNumber2Label.X = 7;
    IdentifyNumber2Label.Y = 3;
    IdentifyNumber2Label.Flexible = 8;
}

/*******************************************************************************
* 函数名	: MenuTask
* 描述	    : MenuTask任务，必须为最低优先级任务，解析窗体Form内的各个控件用于显示
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/12/16       修改版本
*******************************************************************************/
void MenuTask (void)
{
    while(true)             //大循环
    {
        DelayMs(100);       //延时100mS，降低显示速度
        
        System.Gui.Parse(App.Menu.FocusFormPointer);
    }
}








