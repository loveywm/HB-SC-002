/*
********************************************************************************
*                            
********************************************************************************
*文件名     : device_storage.c
*作用       : 存储设备(参数存储和日志存储)
*原理       : STM32F103R8系列flash为64*1KByte,最少擦除单元为1KByte
*           : STM32F103RB系列flash为128*1KByte,最少擦除单元为1KByte
*           : storage参数存储采用追加式存储方式，存储形式为[Address][Data]，
*           : 即把数据的地址与数据作为一个整体存储进去，当数据有变化的时候，
*           : 把原数据地址写0，在后面追加存储即可。
*           : 日志存储直接存储写入数据即可，连续追加方式，注意，参数存储和
*           : 日志存储空间位置不同。
*           : 王修全(剑客)提出，Flash 1->0写入的次数是有限的，只有几次，
*           : 所以存储的时候，不允许多次写0,所以采用取反写入。
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/08/23       初始版本      
********************************************************************************
*/

#include "drive.h"
#include "system.h"

#define PageSize                1024 //Stm32F103R8T6 1Page=1024Byte

#define ParameterCellAddress    (FLASH_BASE + (63 * 1024)) 
#define ParameterCellSpace      PageSize / 4 / 2

#define LogAddress              (FLASH_BASE + (60 * 1024)) 
#define LogSpace                PageSize * 3

typedef struct
{
    uint Address;               //参数存储单元地址
    uint Data;                  //参数存储单元数据
}Cell;

static Cell * CellBlock = (Cell *)ParameterCellAddress;

static void Open(void)
{
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
}

static void Close(void)
{
    FLASH_Lock();
}

static void EraseParameter(void)
{
    FLASH_Status status;
    status = FLASH_ErasePage(ParameterCellAddress);
    Assert(status == FLASH_COMPLETE);
}

/*******************************************************************************
* 函数名	: ParameterRead
* 描述	    : 读取存储区数据
* 输入参数  : 参数的指针，直接对该指针对应的数据修改
* 返回参数  : 布尔变量，是否读取成功
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/10       初始版本
*******************************************************************************/
static bool ReadParameter(void * dataPointer)
{
    uint i;
    uint * pointer;
    pointer = (uint *)dataPointer;

    for (i = 0; i < ParameterCellSpace; i++)
    {
        if (CellBlock[i].Address == (uint)pointer)
        {
            *pointer = CellBlock[i].Data;
            return (true);
        }
        if (CellBlock[i].Address == 0xFFFFFFFF) return (false);
    }
    return (false);
}

/*******************************************************************************
* 函数名	: ParameterWrite
* 描述	    : 写入存储区参数
* 输入参数  : 参数的指针
* 返回参数  : 布尔变量，是否写入成功
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/10       初始版本
*******************************************************************************/
static bool WriteParameter(void * dataPointer)
{
    uint i;
    uint * pointer;
    FLASH_Status status;
    pointer = (uint *)dataPointer;
    
    

    if (dataPointer == (uint *)0) return (false);
    
    //查找存储中的记录，是否有相等的
    for (i = 0; i < ParameterCellSpace; i++)
    {
        if (CellBlock[i].Address == (uint)pointer)
        {
            if (CellBlock[i].Data == *pointer)
                return (true);      //若记录中存在了，直接返回True
            else
            {
                //相同地址不同数据清零,采用互补法清零会出现异常，无法清零。
                Open();
                status = FLASH_ProgramWord((uint)(&CellBlock[i].Address), 0x00000000);
                Close();
            }
        }
        if (CellBlock[i].Address == 0xFFFFFFFF) break;  
    }

    if (i == ParameterCellSpace) return (false);
    //烧写地址与数据
    Open();
    status = FLASH_ProgramWord((uint)(&CellBlock[i].Address), (uint)pointer);
    status = FLASH_ProgramWord((uint)(&CellBlock[i].Data), *pointer);
    Close();
    if (status == FLASH_COMPLETE) 
        return (true);
    else
        return (false);
}

/*******************************************************************************
* 函数名	: ReadParameterProfile
* 描述	    : 读取App.Data区域的参数，并返回存储区存储情况
* 输入参数  : 无
* 返回参数  : profile.Size 存储区占用大小，profile.Entries 参数个数
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/08       初始版本
*******************************************************************************/
static ParameterStruct ReadParameterProfile(void)
{
    ParameterStruct Parameter;   
    ushort i, j;
    i = j = 0;
    Parameter.Space = ParameterCellSpace;

    for (i = 0; i < ParameterCellSpace; i++)
    {
        if (CellBlock[i].Address == 0xFFFFFFFF)
        {
            Parameter.Size = i;
            Parameter.Entries = j;
            return (Parameter);
        }

        if (CellBlock[i].Address > 0x00000000)
        {
            GetUintFrom(CellBlock[i].Address) = CellBlock[i].Data;
            j++;
        }
    }

    Parameter.Size = i;  
    Parameter.Entries = j;
    return (Parameter);
}

/*******************************************************************************
* 函数名	: ArrangeParameter
* 描述	    : 清理存储区数据，让Flash占用资源最少
* 输入参数  : profile.Size 存储区占用大小，profile.Entries 参数个数
* 返回参数  : 无
*******************************************************************************/
static void CleanParameter(ParameterStruct parameter)
{
    ushort i, j;
    uint cellAddressBlock[ParameterCellSpace];
    i = 0; j = 0;
    
    for (i = 0; i < parameter.Size; i++)
    {
        if (CellBlock[i].Address > 0x00000000)
        {
            cellAddressBlock[j] = CellBlock[i].Address;
            j++;
        }
    }
    Open();
    EraseParameter();
    for (i = 0; i < parameter.Entries; i++)
    {
        WriteParameter((uint *)(cellAddressBlock[i]));
    }
    Close();
}

static LogStruct Log;
/*******************************************************************************
* 函数名	: ReadLogProfile
* 描述	    : 读取日志存储信息到静态变量中
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/14       初始版本
*******************************************************************************/
static LogStruct ReadLogProfile(void)
{
    uint i;
    char * readPointer;
    Log.Space = LogSpace;
    Log.ReadPointer = (char *)LogAddress;
    readPointer = (char *)LogAddress;
    
    for (i = 0; i < LogSpace; i++)
    {
        if (*readPointer == 0xFF) 
        {
            Log.WritePointer = readPointer;
            Log.Size = i;        
            return(Log);
        }
        readPointer++;
    }
    Log.WritePointer = Log.ReadPointer;
    Log.Size = 0;
    return(Log);
}


/*******************************************************************************
* 函数名	: EraseLog
* 描述	    : 擦出日志存储区
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/14       初始版本
*******************************************************************************/
static void EraseLog(void)
{
    uint i;
    FLASH_Status status;
    
    for (i = 0; i < (LogSpace / PageSize); i++)
    {
        status = FLASH_ErasePage(LogAddress + (i * PageSize));
        Assert(status == FLASH_COMPLETE);
    }
}
    

/*******************************************************************************
* 函数名	: ReadLog
* 描述	    : 读取日志存储区，按字符串读取
* 输入参数  : sequence: -1: 按字符串一条一条读取，循环读
*           :            0: 初始化读取地址为存储区开头，返回开头地址，读取第一条
*           :           >0: 读取第N条日志
* 返回参数  : 字符串指针
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/14       初始版本
*******************************************************************************/
static char * ReadLog(int sequence)
{
    char * pointer;
    char * returnPointer;
    
    pointer = (char *)LogAddress;
    if (*pointer == 0xFF) return ("Null\n");
    
    if (sequence == 0)
    {
        Log.ReadPointer = (char *)LogAddress;
        return((char *)LogAddress);
    }
    else if (sequence == -1)
    {
        returnPointer = Log.ReadPointer;
        pointer = Log.ReadPointer;

        while(*pointer++ > 0x00);
            
        if (*pointer == 0x00) pointer++;
        
        if ((*pointer == 0xFF) || ((uint)pointer > (LogAddress + LogSpace)))
            Log.ReadPointer = (char *)LogAddress;
        else
            Log.ReadPointer = pointer;
        
        return (returnPointer);
    }
    else if (sequence > 0)
    {
        while(sequence--)
        {
            if(*pointer == 0xFF) return ("Null\n");
            while(*pointer++ > 0x00);
            pointer--;
            while(*pointer++ == 0x00);
            pointer--;
        }
        if ((*pointer == 0xFF) || ((uint)pointer > (LogAddress + LogSpace)))
            return ("Null\n");
        else
            return (pointer);
    }
    return ("ArgumentError\n");
}


/*******************************************************************************
* 函数名	: WriteLog
* 描述	    : 写日志，按字符串顺序写入，追加存储方式，函数用法类似printf函数
* 输入参数  : 字符串或者变量，类似printf函数用法
* 返回参数  : 布尔变量，返回是否成功
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/14       初始版本
*******************************************************************************/
static bool WriteLog(char *fmt, ...)
{
    int i, j, len;
    ushort * pointer;
    va_list list;
    char buffer[100];
    
    pointer = (ushort *)buffer;
    for (i = 0; i < 50; i++) *pointer++ = 0x0000;
    
    va_start(list, fmt);
    len = vsprintf(buffer, fmt, list);
    va_end(list);

    if (len < 1) return (false);

    len++;

    if (len & 0x01) len++;
    
    if (Log.Size + len >= LogSpace)
    {
        Assert(Log.Size + len < LogSpace);
        return (false);
    }
    
    j = len >> 1;
    
    pointer = (ushort *)buffer;
    Open();
    for (i = 0; i < j; i++)
    {
        FLASH_ProgramHalfWord((uint)(Log.WritePointer), *pointer);
        Log.WritePointer += 2;
        pointer++;
    }
    Close();

    Log.Size += len; 
    return (true);
}

void InitializeStorage(void)
{
    System.Device.Storage.Parameter.Profile = ReadParameterProfile;
    System.Device.Storage.Parameter.Erase = EraseParameter;
    System.Device.Storage.Parameter.Read = ReadParameter;
    System.Device.Storage.Parameter.Write = WriteParameter;
    System.Device.Storage.Parameter.Clean = CleanParameter;

    System.Device.Storage.Log.Profile = ReadLogProfile;
    System.Device.Storage.Log.Erase = EraseLog;
    System.Device.Storage.Log.Read = ReadLog;
    System.Device.Storage.Log.Write = WriteLog;
}


