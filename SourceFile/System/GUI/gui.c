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
*文件名     : gui.c
*作用       : 图形库文件
*原理       : 面向对象设计，把一个Menu分为多个Form,一个Form包含多个控件，如Chart、
*           : Label、TextBox等，建立一个最低优先级任务MenuTask去解析执行这些控件信息
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/

#include "system.h"

static char GuiBuffer[4][16];
static char DisplayBuffer[4][16];
static byte TextBoxFocusFlash;

static Form * FocusFormPointer;

/*******************************************************************************
* 函数名	: ParseBackText
* 描述	    : 背景文字扫描刷新
* 输入参数  : formPointer 当前窗体指针
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void ParseBackText(Form * formPointer)
{
    if(formPointer->BackTextPointer == (byte *)0)
        memset(GuiBuffer, ' ', 64);  
    else
        memcpy(GuiBuffer, formPointer->BackTextPointer, 64);
}


/*******************************************************************************
* 函数名	: ParseChartParse
* 描述	    : Chart图形表控件扫描刷新
* 输入参数  : formPointer 当前窗体指针
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void ParseChart(Form * formPointer)
{
    byte i, j;
    char character;
    byte column;

    if(formPointer->ChartPointer == (Chart *)0) return;
        
    character = formPointer->ChartPointer->Character;
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 16; j++)
        {
            column = formPointer->ChartPointer->Column[j];
            Assert(column < 5);
            
            if(column >= (4 - i))
                GuiBuffer[i][j] = character;
            else
                GuiBuffer[i][j] = ' ';
        }
    }
}


/*******************************************************************************
* 函数名	: ParseLabel
* 描述	    : Label控件扫描刷新，Label一般用于文本显示，但不能按键输入操作
* 输入参数  : formPointer 当前窗体指针
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void ParseLabel(Form * formPointer)
{
    byte i, x, y,remainder;
    uint data;
    RtcStruct * rtcPointer;
    bool plusFlag;
    string string;
    Label * labelPointer;
    
    labelPointer = formPointer->LabelPointer;
    x = labelPointer->X;
    y = labelPointer->Y;

    while(labelPointer != (Label *)0)
    {
        switch(labelPointer->Type)
        {
            case GuiDataTypeIntDec:
                data = *((uint *)(labelPointer->DataPointer));
                
                if(GetBit(data, 31) == 1)
                    plusFlag = false;
                else
                    plusFlag = true;

            case GuiDataTypeUintDec:
                Assert(labelPointer->DataPointer != null);
                data = *((uint *)(labelPointer->DataPointer));
                
                if(plusFlag == false) data = (~data) + 1;

                data = data + labelPointer->Offset;
                
    			for (i = 0; i < labelPointer->Shift; i++) 
                   data = data / 10;

                if(data == 0) GuiBuffer[y][x] = '0';
                
    			while (data) 
                {
    				remainder = data % 10;
    				data =  data / 10;
                    
    				if (data ||remainder) 
                        GuiBuffer[y][x] = HexToAscii(remainder);

                    if(x) x--;
                }
                
                if(plusFlag == false) GuiBuffer[y][x] = '-';

                plusFlag = true;
                break;

            case GuiDataTypeHex:
                Assert(labelPointer->DataPointer != null);
                data = *((uint *)(labelPointer->DataPointer));
                data = data >> (labelPointer->Shift);

                i = labelPointer->Flexible;
                if (i)
                {
                    while(i--)
                    {
                        GuiBuffer[y][x] = HexToAscii(data & 0x0000000F);
                        data = data >> 4;
                        if(x) x--;
                    }
                }
                else
                {
                    if (data == 0) GuiBuffer[y][x] = '0';
                    
                    while(data)
                    {
                        GuiBuffer[y][x] = HexToAscii(data & 0x0000000F);
                        data = data >> 4;
                        if(x) x--;
                    }
                }
                break;

            case GuiDataTypeString:
                Assert(labelPointer->DataPointer != null);
                data = *((uint *)(labelPointer->DataPointer));
                string = labelPointer->StringBlockPointer[data];
                i = strlen(string);
                if(labelPointer->Align == GuiDataAlignRight)
                {
                    Assert(x > i);
                    x = x - i + 1;
                }
                while(i--)
                {
                    GuiBuffer[y][x] = *string++;
                    if(x < 15) x++;
                }
                break;
                
            case GuiDataTypeDate:
                Assert(labelPointer->DataPointer != null);
                data = *((uint *)(labelPointer->DataPointer));
//日
                i = UintToByte0(data);
                Assert(i < 32);
                GuiBuffer[y][x] = HexToAscii(i % 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = HexToAscii(i / 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = '-';
                Assert(x > 0);
                x--;
//月
                i = UintToByte1(data);
                Assert(i < 13);
                GuiBuffer[y][x] = HexToAscii(i % 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = HexToAscii(i / 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = '-';
                Assert(x > 0);
                x--;
//年
                data = data >> 16;
                Assert(data < 2100);
                
                if(data == 0) GuiBuffer[y][x] = '0';
                    
    			while (data) 
                {
    				remainder = data % 10;
    				data =  data / 10;
    				if (data ||remainder)
    					GuiBuffer[y][x] = HexToAscii(remainder);

                    if(x) x--;
                }
                break;
                
            case GuiDataTypeTime:
                Assert(labelPointer->DataPointer != null);
                rtcPointer = (RtcStruct *)(labelPointer->DataPointer);
//秒
                i = rtcPointer->Second;
                Assert(i < 60);
                GuiBuffer[y][x] = HexToAscii(i % 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = HexToAscii(i / 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = ':';
                Assert(x > 0);
                x--;
//分
                i = rtcPointer->Minute;
                Assert(i < 60);
                GuiBuffer[y][x] = HexToAscii(i % 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = HexToAscii(i / 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = ':';
                Assert(x > 0);
                x--;
//时
                i = rtcPointer->Hour;
                Assert(i < 24);
                GuiBuffer[y][x] = HexToAscii(i % 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = HexToAscii(i / 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = ':';
                Assert(x > 0);
                x--;
//天
                data = rtcPointer->Day;   
                if(data == 0) GuiBuffer[y][x] = '0';

                while (data) 
                {
    				remainder = data % 10;
    				data =  data / 10;
    				if (data ||remainder)
    					GuiBuffer[y][x] = HexToAscii(remainder);

                    if(x) x--;
                }
                break;
        }
        labelPointer = labelPointer->NextLabelPointer;
        x = labelPointer->X;
        y = labelPointer->Y;
    }
}


/*******************************************************************************
* 函数名	: ParseTextBox
* 描述	    : TextBox控件扫描刷新，TextBox不仅用于文本显示，还能按键输入修改参数
* 输入参数  : formPointer 当前窗体指针
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void ParseTextBox(Form * formPointer)
{
    byte i, x, y,remainder;
    uint data;
    RtcStruct * rtcPointer;
    bool plusFlag;
    string string;
    TextBox * textBoxPointer;

    
    textBoxPointer = formPointer->TextBoxPointer;
    x = textBoxPointer->X;
    y = textBoxPointer->Y;

    while(textBoxPointer != (TextBox *)0)
    {
        if(formPointer->FocusTextBoxDataPointer == textBoxPointer->DataPointer)
        {
            if((TextBoxFocusFlash++ & 0x04) != false)
            {
                textBoxPointer = textBoxPointer->NextTextBoxPointer;
                x = textBoxPointer->X;
                y = textBoxPointer->Y;
                continue;
            }
        }
        switch(textBoxPointer->Type)
        {
            case GuiDataTypeIntDec:
                data = *((uint *)(textBoxPointer->DataPointer));
                
                if(GetBit(data, 31) == 1)
                    plusFlag = false;
                else
                    plusFlag = true;

            case GuiDataTypeUintDec:
                Assert(textBoxPointer->DataPointer != null);
                data = *((uint *)(textBoxPointer->DataPointer));
                
                if(plusFlag == false) data = (~data) + 1;
                
                data = data + textBoxPointer->Offset;
                
    			for (i = 0; i < textBoxPointer->Shift; i++) 
                   data = data / 10;
    				
                if(data == 0) GuiBuffer[y][x] = '0';

    			while (data) 
                {
    				remainder = data % 10;
    				data =  data / 10;
    				if (data ||remainder)
    					GuiBuffer[y][x] = HexToAscii(remainder);

                    if(x) x--;
                }
                
                if(plusFlag == false) GuiBuffer[y][x] = '-';

                plusFlag = true;
                break;

            case GuiDataTypeHex:
                Assert(textBoxPointer->DataPointer != null);
                data = *((uint *)(textBoxPointer->DataPointer));
                data = data >> (textBoxPointer->Shift);
                
                i = textBoxPointer->Flexible;
                if (i)
                {
                    while(i--)
                    {
                        GuiBuffer[y][x] = HexToAscii(data & 0x0000000F);
                        data = data >> 4;
                        if(x) x--;
                    }
                }
                else
                {
                    if (data == 0) GuiBuffer[y][x] = '0';
                    
                    while(data)
                    {
                        GuiBuffer[y][x] = HexToAscii(data & 0x0000000F);
                        data = data >> 4;
                        if(x) x--;
                    }
                }
                break;

            case GuiDataTypeString:
                Assert(textBoxPointer->DataPointer != null);
                data = *((uint *)(textBoxPointer->DataPointer));
                string = textBoxPointer->StringBlockPointer[data];
                i = strlen(string);
                if(textBoxPointer->Align == GuiDataAlignRight)
                {
                    Assert(x > i);
                    x = x - i + 1;
                }
                while(i--)
                {
                    GuiBuffer[y][x] = *string++;
                    if(x < 15) x++;
                }
                break;
            case GuiDataTypeDate:
                Assert(textBoxPointer->DataPointer != null);
                data = *((uint *)(textBoxPointer->DataPointer));
//日
                i = UintToByte0(data);
                Assert(i < 32);
                GuiBuffer[y][x] = HexToAscii(i % 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = HexToAscii(i / 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = '-';
                Assert(x > 0);
                x--;
//月
                i = UintToByte1(data);
                Assert(i < 13);
                GuiBuffer[y][x] = HexToAscii(i % 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = HexToAscii(i / 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = '-';
                Assert(x > 0);
                x--;
//年
                data = data >> 16;
                Assert(data < 2100);
                
                if(data == 0) GuiBuffer[y][x] = '0';

    			while (data) 
                {
    				remainder = data % 10;
    				data =  data / 10;
    				if (data ||remainder)
    					GuiBuffer[y][x] = HexToAscii(remainder);
                    
                    if(x) x--;
                }
                break;
            case GuiDataTypeTime:
                Assert(textBoxPointer->DataPointer != null);
                rtcPointer = (RtcStruct *)(textBoxPointer->DataPointer);
//秒
                i = rtcPointer->Second;
                Assert(i < 60);
                GuiBuffer[y][x] = HexToAscii(i % 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = HexToAscii(i / 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = ':';
                Assert(x > 0);
                x--;
//分
                i = rtcPointer->Minute;
                Assert(i < 60);
                GuiBuffer[y][x] = HexToAscii(i % 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = HexToAscii(i / 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = ':';
                Assert(x > 0);
                x--;
//时
                i = rtcPointer->Hour;
                Assert(i < 24);
                GuiBuffer[y][x] = HexToAscii(i % 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = HexToAscii(i / 10);
                Assert(x > 0);
                x--;
                GuiBuffer[y][x] = ':';
                Assert(x > 0);
                x--;
//天
                data = rtcPointer->Day;   

                if(data == 0) GuiBuffer[y][x] = '0';

                while (data) 
                {
    				remainder = data % 10;
    				data =  data / 10;
    				if (data ||remainder)
    					GuiBuffer[y][x] = HexToAscii(remainder);

                    if(x) x--;
                }
                break;
        }
        textBoxPointer = textBoxPointer->NextTextBoxPointer;
        x = textBoxPointer->X;
        y = textBoxPointer->Y;
    }
}


/*******************************************************************************
* 函数名	: Update
* 描述	    : LCD屏数据更新，发现显示数据有变化，按行更新LCD屏内容
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void Update(void)
{
    byte i, j;
    bool update,updateAll;
    byte * bufferPointer;

    updateAll = false;
    for (i = 0; i < 4; i++)
	{
        update = false;
        
        for (j = 0; j < 16; j++)
        {
            if(DisplayBuffer[i][j] != GuiBuffer[i][j])
            {
                DisplayBuffer[i][j] = GuiBuffer[i][j];
                update = true;
                updateAll = true;
            }
        }
        if (update)
        {
            System.Device.Lcd.DisplayString(i, DisplayBuffer[i]);
        }
    }
    if (updateAll == true)
    {
        System.Device.Usart1.WriteByte(0x02);
        for (i = 0; i < 64; i+=16)
        {
            bufferPointer = (byte *)DisplayBuffer + i;
            System.Device.Usart1.Write(bufferPointer, 16);
            System.Device.Usart1.WriteByte(0x0D);
            System.Device.Usart1.WriteByte(0x0A);
        }
        System.Device.Usart1.WriteByte(0x03);
    }
}


/*******************************************************************************
* 函数名	: AddLabel
* 描述	    : 窗体Form加载Label，加载时清空Label内容，加载后，需要对Label初始化内容
* 输入参数  : formPointer :窗体Form指针
*           : labelPointer: 控件Label指针
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void AddLabel(Form * formPointer, Label *labelPointer)
{
    labelPointer->DataPointer = null;
    labelPointer->Shift = 0;
    labelPointer->Offset = 0;
    labelPointer->Flexible = 0;
    labelPointer->StringBlockPointer = (const string *)0;
    
    labelPointer->NextLabelPointer = formPointer->LabelPointer;
    formPointer->LabelPointer = labelPointer;
}


/*******************************************************************************
* 函数名	: AddTextBox
* 描述	    : 窗体Form加载TextBox，加载时清空TextBox内容，加载后，需要对TextBox初始化内容
* 输入参数  : formPointer :窗体Form指针
*           : textBoxPointer: 控件TextBox指针
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void AddTextBox(Form * formPointer, TextBox *textBoxPointer)
{
    textBoxPointer->DataPointer = null;
    textBoxPointer->DataMax = 0;
    textBoxPointer->DataMin = 0;
    textBoxPointer->DataStep = 0;
    textBoxPointer->DataBigStep = 0;
    
    textBoxPointer->Shift = 0;
    textBoxPointer->Offset = 0;
    textBoxPointer->Flexible = 0;
    textBoxPointer->StringBlockPointer = (const string *)0;
    
    textBoxPointer->NextTextBoxPointer = formPointer->TextBoxPointer;
    formPointer->TextBoxPointer = textBoxPointer;
}

static void InitializeForm(Form *formPointer)
{
    formPointer->BackTextPointer = (const byte *)0;
    formPointer->ChartPointer = (Chart *)0;
    formPointer->LabelPointer = (Label *)0;
    formPointer->TextBoxPointer = (TextBox *)0;
    formPointer->FocusTextBoxDataPointer = (void *)0;
}

/*******************************************************************************
* 函数名	: SwitchTextBoxFocus
* 描述	    : 文本控件焦点切换处理程序，切换文本控件焦点
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void SwitchTextBoxFocus(void)
{
    void * lastDataPointer;
    void * endDataPointer;
    TextBox * textBoxPointer;

    textBoxPointer = FocusFormPointer->TextBoxPointer;
//没有TextBox跳出
    if (textBoxPointer == (TextBox *)0)
    {
        return;
    }
//没有焦点，设定焦点，因为数据插入是倒插顺序，所以要取到最后一个 
    if (FocusFormPointer->FocusTextBoxDataPointer == null)
    {
        while(textBoxPointer != (TextBox *)0)
        {
            lastDataPointer = textBoxPointer->DataPointer;
            textBoxPointer = textBoxPointer->NextTextBoxPointer;            
        }
        FocusFormPointer->FocusTextBoxDataPointer = lastDataPointer;
        return;
    }
//存在焦点
    else
    {
    //焦点在最后，要消逝
        endDataPointer = textBoxPointer->DataPointer;
        if(FocusFormPointer->FocusTextBoxDataPointer == endDataPointer)
        {
            FocusFormPointer->FocusTextBoxDataPointer = null;
            return;
        }
    //循环焦点
        while(textBoxPointer != (TextBox *)0)
        {
            if(FocusFormPointer->FocusTextBoxDataPointer != textBoxPointer->DataPointer)
            {
                lastDataPointer = textBoxPointer->DataPointer;
            }
            else
            {
                FocusFormPointer->FocusTextBoxDataPointer = lastDataPointer;
                return;
            }
            textBoxPointer = textBoxPointer->NextTextBoxPointer;            
        }
    }
}


/*******************************************************************************
* 函数名	: SetTextBoxData
* 描述	    : 文本控件数值修改处理修改程序
* 输入参数  : key: AddKey、AddLongKey、SubKey、SubLongKey
*           :      AssistUpKey、AssistUpLongKey、AssistDownKey、AssistDownLongKey
* 返回参数  : 
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void SetTextBoxData(KeyEnum key)
{
    TextBox * textBoxPointer;
    void * dataPointer;
    
    int intData;
    int intDataMax;
    int intDataMin;
    int intDataStep;
    int intDataBigStep;
    
    uint uintData;
    uint uintDataMax;
    uint uintDataMin;
    uint uintDataStep;
    uint uintDataBigStep;

    RtcStruct rtc;
    
    DataType dataType;

    textBoxPointer = FocusFormPointer->TextBoxPointer;
    dataPointer = FocusFormPointer->FocusTextBoxDataPointer;

    if (dataPointer == null)
    {
        return;
    }

    while(textBoxPointer != (TextBox *)0)
    {
        if(textBoxPointer->DataPointer == dataPointer)
        {
            break;
        }
        textBoxPointer = textBoxPointer->NextTextBoxPointer;   
    }
    
    Assert(textBoxPointer != (TextBox *)0);

    dataType = textBoxPointer->Type;

    switch(dataType)
    {
        case GuiDataTypeIntDec:
            intData = * (int *)(textBoxPointer->DataPointer);
            intDataMax = (int)(textBoxPointer->DataMax);
            intDataMin = (int)(textBoxPointer->DataMin);
            intDataStep = (int)(textBoxPointer->DataStep);        
            intDataBigStep = (int)(textBoxPointer->DataBigStep);
            
            switch(key)
            {
                case AddKey:
                    if(intData + intDataStep > intDataMax)
                        intData = intDataMax;
                    else
                        intData += intDataStep;
                    break;
                case AddLongKey:
                    if(intData + intDataBigStep > intDataMax)
                        intData = intDataMax;
                    else
                        intData += intDataBigStep;
                    break;
                case SubKey:
                    if(intData - intDataStep < intDataMin)
                        intData = intDataMin;
                    else
                        intData -= intDataStep;
                    break;
                case SubLongKey:
                    if(intData - intDataBigStep < intDataMin)
                        intData = intDataMin;
                    else
                        intData -= intDataBigStep;
                    break;
            }
            *(int *)dataPointer = intData;
            break;
        case GuiDataTypeUintDec:
        case GuiDataTypeHex:
        case GuiDataTypeString:
            uintData = * (uint *)(textBoxPointer->DataPointer);
            uintDataMax = (uint)(textBoxPointer->DataMax);
            uintDataMin = (uint)(textBoxPointer->DataMin);
            uintDataStep = (uint)(textBoxPointer->DataStep);        
            uintDataBigStep = (uint)(textBoxPointer->DataBigStep);
            
            switch(key)
            {
                case AddKey:
                    if((uintData + uintDataStep > uintDataMax) ||(uintData + uintDataStep < uintData)||(uintData + uintDataStep < uintDataStep))
                        uintData = uintDataMax;
                    else
                        uintData += uintDataStep;
                    
                    break;
                case AddLongKey:
                    if((uintData + uintDataBigStep > uintDataMax) ||(uintData + uintDataBigStep < uintData)||(uintData + uintDataBigStep < uintDataBigStep))
                        uintData = uintDataMax;
                    else
                        uintData += uintDataBigStep;
                    break;
                case SubKey:
                    if((uintData - uintDataStep < uintDataMin) || (uintData < uintDataStep))
                        uintData = uintDataMin;
                    else
                        uintData -= uintDataStep;
                    break;
                case SubLongKey:
                    if((uintData - uintDataBigStep < uintDataMin) || (uintData < uintDataBigStep))
                        uintData = uintDataMin;
                    else
                        uintData -= uintDataBigStep;
                    break;
            }  
            *(uint *)dataPointer = uintData;
            break;
        case GuiDataTypeDate:
            uintData = *(uint *)(textBoxPointer->DataPointer);
            switch(key)
            {
                case AddKey:
                    UintToByte0(uintData)++;
                    break;
                case AddLongKey:
                    UintToByte1(uintData)++;
                    break;
                case AssistUpKey:
                    UintToUshort1(uintData)++;
                    break;
                case SubKey:
                    UintToByte0(uintData)--;
                    break;
                case SubLongKey:
                    UintToByte1(uintData)--;
                    break;
                case AssistDownKey:
                    UintToUshort1(uintData)--;
                    break;                    
            }
            *(uint *)dataPointer = uintData;
            break;
        case GuiDataTypeTime:
            rtc = *(RtcStruct *)(textBoxPointer->DataPointer);
            switch(key)
            {
                case AddKey:
                    if(rtc.Second < 59)
                        rtc.Second++;
                    break;
                case AddLongKey:
                    if(rtc.Minute < 59)
                        rtc.Minute++;
                    break;
                case AssistUpKey:
                    if(rtc.Hour < 23)
                        rtc.Hour++;
                    break;
                case AssistUpLongKey:
                        rtc.Day++;
                    break;
                case SubKey:
                    if(rtc.Second > 0)
                        rtc.Second--;
                    break;
                case SubLongKey:
                    if(rtc.Minute > 0)
                        rtc.Minute--;
                    break;
                case AssistDownKey:
                    if(rtc.Hour > 0)
                        rtc.Hour--;
                    break; 
                case AssistDownLongKey:
                    if(rtc.Day > 0)
                        rtc.Day--;
                    break;
            }
            *(RtcStruct *)dataPointer = rtc;
            break;
        default:
            break;
    }

}



/*******************************************************************************
* 函数名	: Parse
* 描述	    : 解析页面
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void Parse(Form * formPointer)
{           
    FocusFormPointer = formPointer;
    
    ParseBackText(formPointer);         //背景文字控件扫描解析

    ParseChart(formPointer);            //表控件扫描解析

    ParseLabel(formPointer);            //标签控件扫描解析

    ParseTextBox(formPointer);          //文本控件扫描解析

    Update();                           //更新LCD显示内容
}

void InitializeGui(void)
{
    byte i;
    for (i = 0; i < 64; i++)
    {
        ((byte *)GuiBuffer)[i] = ' ';
        ((byte *)DisplayBuffer)[i] = ' ';
    }
    
    System.Gui.Parse = Parse;
    
    System.Gui.Form.AddLabel = AddLabel;

    System.Gui.Form.AddTextBox = AddTextBox;

    System.Gui.Form.Initialize = InitializeForm;    

    System.Gui.Form.SwitchTextBoxFocus = SwitchTextBoxFocus;

    System.Gui.Form.SetTextBoxData = SetTextBoxData;
}




