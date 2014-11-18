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
*文件名     : gui.h
*作用       : 
*原理       : 
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2014/06/21       初始版本
********************************************************************************
*/



#ifndef __GUI_H
#define __GUI_H




typedef enum 
{
    GuiDataTypeIntDec,
    GuiDataTypeUintDec,
    GuiDataTypeHex,
    GuiDataTypeString,
    GuiDataTypeDate,
    GuiDataTypeTime
}DataType;

typedef enum
{
	GuiDataAlignLeft,
	GuiDataAlignRight
}DataAlign;

typedef struct
{
    char Character;
    byte Column[16];
}Chart;

typedef struct LabelSelf
{
    byte X;
    byte Y;
    byte Shift;
    int Offset;
    int Flexible;
    DataType Type;
    DataAlign Align;
    void * DataPointer;
    const string * StringBlockPointer;
    struct LabelSelf * NextLabelPointer;
}Label;

typedef struct TextBoxSelf
{
    byte X;
    byte Y;
    byte Shift;
    int Offset;
    int Flexible;
    DataType Type;
    DataAlign Align;
    void * DataPointer;
    uint DataMax;
    uint DataMin;
    uint DataStep;
    uint DataBigStep;
    const string * StringBlockPointer;
    struct TextBoxSelf * NextTextBoxPointer;
}TextBox;

typedef struct
{
    const byte* BackTextPointer;
    Chart     * ChartPointer;
    Label     * LabelPointer;
    TextBox   * TextBoxPointer;
    void      * FocusTextBoxDataPointer;
}Form;


extern void InitializeGui(void);
    
#endif 




