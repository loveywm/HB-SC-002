/*
********************************************************************************
*                            
********************************************************************************
*�ļ���     : device_storage.c
*����       : �洢�豸(�����洢����־�洢)
*ԭ��       : STM32F103R8ϵ��flashΪ64*1KByte,���ٲ�����ԪΪ1KByte
*           : STM32F103RBϵ��flashΪ128*1KByte,���ٲ�����ԪΪ1KByte
*           : storage�����洢����׷��ʽ�洢��ʽ���洢��ʽΪ[Address][Data]��
*           : �������ݵĵ�ַ��������Ϊһ������洢��ȥ���������б仯��ʱ��
*           : ��ԭ���ݵ�ַд0���ں���׷�Ӵ洢���ɡ�
*           : ��־�洢ֱ�Ӵ洢д�����ݼ��ɣ�����׷�ӷ�ʽ��ע�⣬�����洢��
*           : ��־�洢�ռ�λ�ò�ͬ��
*           : ����ȫ(����)�����Flash 1->0д��Ĵ��������޵ģ�ֻ�м��Σ�
*           : ���Դ洢��ʱ�򣬲�������д0,���Բ���ȡ��д�롣
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/08/23       ��ʼ�汾      
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
    uint Address;               //�����洢��Ԫ��ַ
    uint Data;                  //�����洢��Ԫ����
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
* ������	: ParameterRead
* ����	    : ��ȡ�洢������
* �������  : ������ָ�룬ֱ�ӶԸ�ָ���Ӧ�������޸�
* ���ز���  : �����������Ƿ��ȡ�ɹ�
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/10       ��ʼ�汾
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
* ������	: ParameterWrite
* ����	    : д��洢������
* �������  : ������ָ��
* ���ز���  : �����������Ƿ�д��ɹ�
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/10       ��ʼ�汾
*******************************************************************************/
static bool WriteParameter(void * dataPointer)
{
    uint i;
    uint * pointer;
    FLASH_Status status;
    pointer = (uint *)dataPointer;
    
    

    if (dataPointer == (uint *)0) return (false);
    
    //���Ҵ洢�еļ�¼���Ƿ�����ȵ�
    for (i = 0; i < ParameterCellSpace; i++)
    {
        if (CellBlock[i].Address == (uint)pointer)
        {
            if (CellBlock[i].Data == *pointer)
                return (true);      //����¼�д����ˣ�ֱ�ӷ���True
            else
            {
                //��ͬ��ַ��ͬ��������,���û��������������쳣���޷����㡣
                Open();
                status = FLASH_ProgramWord((uint)(&CellBlock[i].Address), 0x00000000);
                Close();
            }
        }
        if (CellBlock[i].Address == 0xFFFFFFFF) break;  
    }

    if (i == ParameterCellSpace) return (false);
    //��д��ַ������
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
* ������	: ReadParameterProfile
* ����	    : ��ȡApp.Data����Ĳ����������ش洢���洢���
* �������  : ��
* ���ز���  : profile.Size �洢��ռ�ô�С��profile.Entries ��������
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/08       ��ʼ�汾
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
* ������	: ArrangeParameter
* ����	    : ����洢�����ݣ���Flashռ����Դ����
* �������  : profile.Size �洢��ռ�ô�С��profile.Entries ��������
* ���ز���  : ��
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
* ������	: ReadLogProfile
* ����	    : ��ȡ��־�洢��Ϣ����̬������
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/14       ��ʼ�汾
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
* ������	: EraseLog
* ����	    : ������־�洢��
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/14       ��ʼ�汾
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
* ������	: ReadLog
* ����	    : ��ȡ��־�洢�������ַ�����ȡ
* �������  : sequence: -1: ���ַ���һ��һ����ȡ��ѭ����
*           :            0: ��ʼ����ȡ��ַΪ�洢����ͷ�����ؿ�ͷ��ַ����ȡ��һ��
*           :           >0: ��ȡ��N����־
* ���ز���  : �ַ���ָ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/14       ��ʼ�汾
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
* ������	: WriteLog
* ����	    : д��־�����ַ���˳��д�룬׷�Ӵ洢��ʽ�������÷�����printf����
* �������  : �ַ������߱���������printf�����÷�
* ���ز���  : ���������������Ƿ�ɹ�
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/14       ��ʼ�汾
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


