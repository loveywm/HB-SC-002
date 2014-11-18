/*
********************************************************************************
*                                Ƕ��ʽ΢ϵͳ
*                                   msOS
*
*                            Ӳ��ƽ̨:msPLC DEMO
*                          ��оƬ:STM32F103R8T6/RBT6
*                           ��������οƼ����޹�˾
*
*                                ����:����ΰ
*                                ����:������
*                                ��ʶ:Wangsw
*
*                                QQ:26033613
*                               QQȺ:291235815
*                        �Ա���:http://52edk.taobao.com
*                      ��̳:http://gongkong.eefocus.com/bbs/
*                ����:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*�ļ���     : menu.c
*����       : �������
*ԭ��       : ���ӳ����а����������Form��ÿһ��������Լ���BackText�������֡�
*           : Chart��Label��ǩ��TextBox�ı�����ؼ�������Add��������֮��
*           : ��Ҫ��ʼ��ֵ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
********************************************************************************
*/

#include "system.h"
#include "app.h"




//����Chartҳ���Լ��
static Chart CheckChart;

//��������label
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
    "����ɨƵ",
    "��������",
    "��Ƶ����",
    "���๤��",
    "�쳣����"
};

/*******************************************************************************
* ������	: InitializeMmi
* ����	    : ��ʼ��MMI,���ظ���ؼ�������������
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
*******************************************************************************/
void InitializeMenu(void)
{
    byte i;

//ҳ�潹��
    App.Menu.FocusFormPointer = &App.Menu.LogoForm;

//Logo Form
    System.Gui.Form.Initialize(&App.Menu.LogoForm);
    App.Menu.LogoForm.BackTextPointer = "    ��οƼ�    "
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
    App.Menu.WorkForm.BackTextPointer = "Ƶ��          Hz"
                                        "����   %       W"
                                        "��ѹ    ����    "
                                        "�¶�   C        ";

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
    App.Menu.SetupForm.BackTextPointer = "����    �¶�   C"
                                         "Ƶ��   KƵƫ   K"
                                         "ˮ��            "
                                         "ʱ��            ";
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
    App.Menu.ServiceForm.BackTextPointer = "�������        "
                                           "��������        "
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
* ������	: MenuTask
* ����	    : MenuTask���񣬱���Ϊ������ȼ����񣬽�������Form�ڵĸ����ؼ�������ʾ
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/12/16       �޸İ汾
*******************************************************************************/
void MenuTask (void)
{
    while(true)             //��ѭ��
    {
        DelayMs(100);       //��ʱ100mS��������ʾ�ٶ�
        
        System.Gui.Parse(App.Menu.FocusFormPointer);
    }
}








