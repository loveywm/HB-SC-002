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
*�ļ���     : gui.h
*����       : 
*ԭ��       : 
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2014/06/21       ��ʼ�汾
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




