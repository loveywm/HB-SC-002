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
*                                ��ʶ:wangsw
*
*                                QQ:26033613
*                               QQȺ:291235815
*                        �Ա���:http://52edk.taobao.com
*                      ��̳:http://gongkong.eefocus.com/bbs/
*                ����:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*�ļ���   : utils.em
*����     : SourceInsightע�ͼ�һЩ����������õ�֧������
*����ʱ�� : 2013.9.2
********************************************************************************
*/


/*******************************************************************************
* ������	: msOSInsertFileHeader
* ����	    : �����ļ�ע��
* �������  : ��
* ���ز���  : ��
*******************************************************************************/
macro msOSInsertFileHeader()
{
    // get aurthor name
    szMyName = getenv(MYNAME)
    if (strlen(szMyName) <= 0)
    {
        szMyName = "Wangsw"
    }

    // get company name
    szCompanyName = getenv(MYCOMPANY)
    if (strlen(szCompanyName) <= 0)
    {
        szCompanyName = szMyName
    }

    // get time
    szTime = GetSysTime(True)
    Day = szTime.Day
    Month = szTime.Month
    Year = szTime.Year
    if (Day < 10)
    {
        szDay = "0@Day@"
    }
    else
    {
        szDay = Day
    }
    if (Month < 10)
    {
        szMonth = "0@Month@"
    }
    else
    {
        szMonth = Month
    }

    // get file name
    hbuf = GetCurrentBuf()
    szpathName = GetBufName(hbuf)
    szfileName = GetFileName(szpathName)
    nlength = StrLen(szfileName)

    // assemble the string
    hbuf = GetCurrentBuf()

    InsBufLine(hbuf, 0,  "/*")
    InsBufLine(hbuf, 1,  "********************************************************************************")
    InsBufLine(hbuf, 2,  "*                                Ƕ��ʽ΢ϵͳ")
    InsBufLine(hbuf, 3,  "*                                   msOS")
    InsBufLine(hbuf, 4,  "*")
    InsBufLine(hbuf, 5,  "*                            Ӳ��ƽ̨:msPLC DEMO")
    InsBufLine(hbuf, 6,  "*                          ��оƬ:STM32F103R8T6/RBT6")
    InsBufLine(hbuf, 7,  "*                           ��������οƼ����޹�˾")
    InsBufLine(hbuf, 8,  "*")
    InsBufLine(hbuf, 9,  "*                                ����:����ΰ")
    InsBufLine(hbuf, 10, "*                                ����:������")
    InsBufLine(hbuf, 11, "*                                ��ʶ:Wangsw")
    InsBufLine(hbuf, 12, "*")
    InsBufLine(hbuf, 13, "*                                QQ:26033613")
    InsBufLine(hbuf, 14, "*                               QQȺ:291235815")
    InsBufLine(hbuf, 15, "*                        �Ա���:http://52edk.taobao.com")
    InsBufLine(hbuf, 16, "*                      ��̳:http://gongkong.eefocus.com/bbs/")
    InsBufLine(hbuf, 17, "*                ����:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM")
    InsBufLine(hbuf, 18, "********************************************************************************")
    InsBufLine(hbuf, 19, "*�ļ���     : @szfileName@")
    InsBufLine(hbuf, 20, "*����       : ")
    InsBufLine(hbuf, 21, "*ԭ��       : ")
    InsBufLine(hbuf, 22, "********************************************************************************")
    InsBufLine(hbuf, 23, "*�汾     ����            ����            ˵��")
    InsBufLine(hbuf, 24, "*V0.1    @szMyName@        @Year@/@szMonth@/@szDay@       ��ʼ�汾")
    InsBufLine(hbuf, 25, "********************************************************************************")
    InsBufLine(hbuf, 26, "*/")

    // put the insertion point
    SetBufIns(hbuf, 27, 0)
}


/*******************************************************************************
* ������	: msOSInsertFunctionHeader
* ����	    : ���뺯��ע��
* �������  : ��
* ���ز���  : ��
*******************************************************************************/
macro msOSInsertFunctionHeader()
{
    // get function name
    hbuf = GetCurrentBuf()
    szFunc = GetCurSymbol()
    ln = GetSymbolLine(szFunc)


    // get aurthor name
    szMyName = getenv(MYNAME)
    if (strlen(szMyName) <= 0)
    {
        szMyName = "Wangsw"
    }

    // get time
    szTime = GetSysTime(True)
    Day = szTime.Day
    Month = szTime.Month
    Year = szTime.Year
    if (Day < 10)
    {
        szDay = "0@Day@"
    }
    else
    {
        szDay = Day
    }
    if (Month < 10)
    {
        szMonth = "0@Month@"
    }
    else
    {
        szMonth = Month
    }

    // assemble the string
    hbuf = GetCurrentBuf()
    InsBufLine(hbuf, ln, "")
    InsBufLine(hbuf, ln+1, "/*******************************************************************************")
    InsBufLine(hbuf, ln+2, "* ������	: @szFunc@")
    InsBufLine(hbuf, ln+3, "* ����	    : ")
    InsBufLine(hbuf, ln+4, "* �������  : ")
    InsBufLine(hbuf, ln+5, "* ���ز���  : ")
    InsBufLine(hbuf, ln+6, "********************************************************************************")
    InsBufLine(hbuf, ln+7, "*�汾     ����            ����            ˵��")
    InsBufLine(hbuf, ln+8, "*V0.1    @szMyName@        @Year@/@szMonth@/@szDay@       ��ʼ�汾")
    InsBufLine(hbuf, ln+9, "*******************************************************************************/")

    // put the insertion point
    SetBufIns(hbuf, ln+10, 0)
}


/*******************************************************************************
* ������	: msOSInsertHFileBanner
* ����	    : ����*.hͷ�ļ����ؼ��������궨��
* �������  : ��
* ���ز���  : ��
*******************************************************************************/
macro msOSInsertHFileBanner()
{
    // get file name
    hbuf = GetCurrentBuf()
    szpathName = GetBufName(hbuf)
    szfileName = GetFileName(szpathName)
    szfileName = toupper(szfileName)

    // create banner
    banner = "__"
    nlength = strlen(szfileName)
	
    i=0
    while (i < nlength)
    {
        if (szfileName[i] == ".")
        {
            banner = cat(banner, "_")
        }
        else
        {
            banner = cat(banner, szfileName[i])
        }

        i = i+1
    }

    //banner = cat(banner, "_")

    // print banner
    hwnd = GetCurrentWnd()
    lnFirst = GetWndSelLnFirst(hwnd)
    lnLast = GetWndSelLnLast(hwnd)
	 
    hbuf = GetCurrentBuf()
    InsBufLine(hbuf, lnFirst, "")
    InsBufLine(hbuf, lnFirst+1, "")
    InsBufLine(hbuf, lnFirst+2, "#ifndef @banner@")
    InsBufLine(hbuf, lnFirst+3, "#define @banner@")
    InsBufLine(hbuf, lnFirst+4, "")
    InsBufLine(hbuf, lnFirst+5, "")
    InsBufLine(hbuf, lnFirst+6, "")
    InsBufLine(hbuf, lnFirst+7, "")
    InsBufLine(hbuf, lnFirst+8, "")
    InsBufLine(hbuf, lnFirst+9, "#endif /*@banner@*/")
    InsBufLine(hbuf, lnFirst+10, "")

    SetBufIns(hbuf, lnFirst+11, 0)
}

/*==================================================================
* Function	: GetFileName
* Description	: get file name from path
*			  
* Input Para	: pathName	: path string
			  
* Output Para	: None
			  
* Return Value: name		: file name
==================================================================*/
macro GetFileName(pathName)
{
    nlength = strlen(pathName)
    i = nlength - 1
    name = ""
    while (i + 1)
    {
        ch = pathName[i]
        if ("\\" == "@ch@")
            break
        i = i - 1
    }
    i = i + 1
    while (i < nlength)
    {
        name = cat(name, pathName[i])
        i = i + 1
    }

    return name
}


/*======================================================================
1��BackSpace���˼�
======================================================================*/
macro SuperBackspace()
{
    hwnd = GetCurrentWnd();
    hbuf = GetCurrentBuf();
    if (hbuf == 0)
        stop; // empty buffer
    // get current cursor postion
    ipos = GetWndSelIchFirst(hwnd);
    // get current line number
    ln = GetBufLnCur(hbuf);
    if ((GetBufSelText(hbuf) != "") || (GetWndSelLnFirst(hwnd) != GetWndSelLnLast(hwnd))) {
        // sth. was selected, del selection
        SetBufSelText(hbuf, " "); // stupid & buggy sourceinsight
        // del the " "
        SuperBackspace(1);
        stop;
    }
    // copy current line
    text = GetBufLine(hbuf, ln);
    // get string length
    len = strlen(text);
    // if the cursor is at the start of line, combine with prev line
    if (ipos == 0 || len == 0) {
        if (ln <= 0)
            stop; // top of file
        ln = ln - 1; // do not use "ln--" for compatibility with older versions
        prevline = GetBufLine(hbuf, ln);
        prevlen = strlen(prevline);
        // combine two lines
        text = cat(prevline, text);
        // del two lines
        DelBufLine(hbuf, ln);
        DelBufLine(hbuf, ln);
        // insert the combined one
        InsBufLine(hbuf, ln, text);
        // set the cursor position
        SetBufIns(hbuf, ln, prevlen);
        stop;
    }
    num = 1; // del one char
    if (ipos >= 1) {
        // process Chinese character
        i = ipos;
        count = 0;
        while (AsciiFromChar(text[i - 1]) >= 160) {
            i = i - 1;
            count = count + 1;
            if (i == 0)
                break;
        }
        if (count > 0) {
            // I think it might be a two-byte character
            num = 2;
            // This idiot does not support mod and bitwise operators
            if ((count / 2 * 2 != count) && (ipos < len))
                ipos = ipos + 1; // adjust cursor position
        }
    }
    // keeping safe
    if (ipos - num < 0)
        num = ipos;
    // del char(s)
    text = cat(strmid(text, 0, ipos - num), strmid(text, ipos, len));
    DelBufLine(hbuf, ln);
    InsBufLine(hbuf, ln, text);
    SetBufIns(hbuf, ln, ipos - num);
    stop;
}
/*======================================================================
2��ɾ��������SuperDelete.em
======================================================================*/
macro SuperDelete()
{
    hwnd = GetCurrentWnd();
    hbuf = GetCurrentBuf();
    if (hbuf == 0)
        stop; // empty buffer
    // get current cursor postion
    ipos = GetWndSelIchFirst(hwnd);
    // get current line number
    ln = GetBufLnCur(hbuf);
    if ((GetBufSelText(hbuf) != "") || (GetWndSelLnFirst(hwnd) != GetWndSelLnLast(hwnd))) {
        // sth. was selected, del selection
        SetBufSelText(hbuf, " "); // stupid & buggy sourceinsight
        // del the " "
        SuperDelete(1);
        stop;
    }
    // copy current line
    text = GetBufLine(hbuf, ln);
    // get string length
    len = strlen(text);

    if (ipos == len || len == 0) {
totalLn = GetBufLineCount (hbuf);
lastText = GetBufLine(hBuf, totalLn-1);
lastLen = strlen(lastText);
        if (ipos == lastLen)// end of file
   stop;
        ln = ln + 1; // do not use "ln--" for compatibility with older versions
        nextline = GetBufLine(hbuf, ln);
        nextlen = strlen(nextline);
        // combine two lines
        text = cat(text, nextline);
        // del two lines
        DelBufLine(hbuf, ln-1);
        DelBufLine(hbuf, ln-1);
        // insert the combined one
        InsBufLine(hbuf, ln-1, text);
        // set the cursor position
        SetBufIns(hbuf, ln-1, len);
        stop;
    }
    num = 1; // del one char
    if (ipos > 0) {
        // process Chinese character
        i = ipos;
        count = 0;
      while (AsciiFromChar(text[i-1]) >= 160) {
            i = i - 1;
            count = count + 1;
            if (i == 0)
                break;
        }
        if (count > 0) {
            // I think it might be a two-byte character
            num = 2;
            // This idiot does not support mod and bitwise operators
            if (((count / 2 * 2 != count) || count == 0) && (ipos < len-1))
                ipos = ipos + 1; // adjust cursor position
        }
// keeping safe
if (ipos - num < 0)
            num = ipos;
    }
    else {
i = ipos;
count = 0;
while(AsciiFromChar(text) >= 160) {
     i = i + 1;
     count = count + 1;
     if(i == len-1)
   break;
}
if(count > 0) {
     num = 2;
}
    }

    text = cat(strmid(text, 0, ipos), strmid(text, ipos+num, len));
    DelBufLine(hbuf, ln);
    InsBufLine(hbuf, ln, text);
    SetBufIns(hbuf, ln, ipos);
    stop;
}
/*======================================================================
3�����Ƽ�����SuperCursorLeft.em
======================================================================*/
macro IsComplexCharacter()
{
hwnd = GetCurrentWnd();
hbuf = GetCurrentBuf();
if (hbuf == 0)
   return 0;
//��ǰλ��
pos = GetWndSelIchFirst(hwnd);
//��ǰ����
ln = GetBufLnCur(hbuf);
//�õ���ǰ��
text = GetBufLine(hbuf, ln);
//�õ���ǰ�г���
len = strlen(text);
//��ͷ���㺺���ַ��ĸ���
if(pos > 0)
{
   i=pos;
   count=0;
   while(AsciiFromChar(text[i-1]) >= 160)
   {
    i = i - 1;
    count = count+1;
    if(i == 0)
     break;
   }
   if((count/2)*2==count|| count==0)
    return 0;
   else
    return 1;
}
return 0;
}
macro moveleft()
{
hwnd = GetCurrentWnd();
hbuf = GetCurrentBuf();
if (hbuf == 0)
        stop; // empty buffer

ln = GetBufLnCur(hbuf);
ipos = GetWndSelIchFirst(hwnd);
if(GetBufSelText(hbuf) != "" || (ipos == 0 && ln == 0)) // ��0�л�����ѡ������,���ƶ�
{
   SetBufIns(hbuf, ln, ipos);
   stop;
}
if(ipos == 0)
{
   preLine = GetBufLine(hbuf, ln-1);
   SetBufIns(hBuf, ln-1, strlen(preLine)-1);
}
else
{
   SetBufIns(hBuf, ln, ipos-1);
}
}
macro SuperCursorLeft()
{
moveleft();
if(IsComplexCharacter())
   moveleft();
}
/*======================================================================
4�����Ƽ�����SuperCursorRight.em
======================================================================*/
macro moveRight()
{
hwnd = GetCurrentWnd();
hbuf = GetCurrentBuf();
if (hbuf == 0)
        stop; // empty buffer
ln = GetBufLnCur(hbuf);
ipos = GetWndSelIchFirst(hwnd);
totalLn = GetBufLineCount(hbuf);
text = GetBufLine(hbuf, ln);
if(GetBufSelText(hbuf) != "") //ѡ������
{
   ipos = GetWndSelIchLim(hwnd);
   ln = GetWndSelLnLast(hwnd);
   SetBufIns(hbuf, ln, ipos);
   stop;
}
if(ipos == strlen(text)-1 && ln == totalLn-1) // ĩ��
   stop;
if(ipos == strlen(text))
{
   SetBufIns(hBuf, ln+1, 0);
}
else
{
   SetBufIns(hBuf, ln, ipos+1);
}
}
macro SuperCursorRight()
{
moveRight();
if(IsComplexCharacter()) // defined in SuperCursorLeft.em
   moveRight();
}
/*======================================================================
5��shift+���Ƽ�����ShiftCursorRight.em
======================================================================*/
macro IsShiftRightComplexCharacter()
{
hwnd = GetCurrentWnd();
hbuf = GetCurrentBuf();
if (hbuf == 0)
   return 0;
selRec = GetWndSel(hwnd);
pos = selRec.ichLim;
ln = selRec.lnLast;
text = GetBufLine(hbuf, ln);
len = strlen(text);
if(len == 0 || len < pos)
return 1;
//Msg("@len@;@pos@;");
if(pos > 0)
{
   i=pos;
   count=0;
   while(AsciiFromChar(text[i-1]) >= 160)
   {
    i = i - 1;
    count = count+1;
    if(i == 0)
     break;
   }
   if((count/2)*2==count|| count==0)
    return 0;
   else
    return 1;
}
return 0;
}
macro shiftMoveRight()
{
hwnd = GetCurrentWnd();
hbuf = GetCurrentBuf();
if (hbuf == 0)
        stop; 

ln = GetBufLnCur(hbuf);
ipos = GetWndSelIchFirst(hwnd);
totalLn = GetBufLineCount(hbuf);
text = GetBufLine(hbuf, ln);
selRec = GetWndSel(hwnd);
curLen = GetBufLineLength(hbuf, selRec.lnLast);
if(selRec.ichLim == curLen+1 || curLen == 0)
{
   if(selRec.lnLast == totalLn -1)
    stop;
   selRec.lnLast = selRec.lnLast + 1;
   selRec.ichLim = 1;
   SetWndSel(hwnd, selRec);
   if(IsShiftRightComplexCharacter())
    shiftMoveRight();
   stop;
}
selRec.ichLim = selRec.ichLim+1;
SetWndSel(hwnd, selRec);
}
macro SuperShiftCursorRight()
{
if(IsComplexCharacter())
   SuperCursorRight();
shiftMoveRight();
if(IsShiftRightComplexCharacter())
   shiftMoveRight();
}
/*======================================================================
6��shift+���Ƽ�����ShiftCursorLeft.em
======================================================================*/
macro IsShiftLeftComplexCharacter()
{
hwnd = GetCurrentWnd();
hbuf = GetCurrentBuf();
if (hbuf == 0)
   return 0;
selRec = GetWndSel(hwnd);
pos = selRec.ichFirst;
ln = selRec.lnFirst;
text = GetBufLine(hbuf, ln);
len = strlen(text);
if(len == 0 || len < pos)
   return 1;
//Msg("@len@;@pos@;");
if(pos > 0)
{
   i=pos;
   count=0;
   while(AsciiFromChar(text[i-1]) >= 160)
   {
    i = i - 1;
    count = count+1;
    if(i == 0)
     break;
   }
   if((count/2)*2==count|| count==0)
    return 0;
   else
    return 1;
}
return 0;
}
macro shiftMoveLeft()
{
hwnd = GetCurrentWnd();
hbuf = GetCurrentBuf();
if (hbuf == 0)
        stop; 

ln = GetBufLnCur(hbuf);
ipos = GetWndSelIchFirst(hwnd);
totalLn = GetBufLineCount(hbuf);
text = GetBufLine(hbuf, ln);
selRec = GetWndSel(hwnd);
//curLen = GetBufLineLength(hbuf, selRec.lnFirst);
//Msg("@curLen@;@selRec@");
if(selRec.ichFirst == 0)
{
   if(selRec.lnFirst == 0)
    stop;
   selRec.lnFirst = selRec.lnFirst - 1;
   selRec.ichFirst = GetBufLineLength(hbuf, selRec.lnFirst)-1;
   SetWndSel(hwnd, selRec);
   if(IsShiftLeftComplexCharacter())
    shiftMoveLeft();
   stop;
}
selRec.ichFirst = selRec.ichFirst-1;
SetWndSel(hwnd, selRec);
}
macro SuperShiftCursorLeft()
{
if(IsComplexCharacter())
   SuperCursorLeft();
shiftMoveLeft();
if(IsShiftLeftComplexCharacter())
   shiftMoveLeft();
}
/*---END---*/