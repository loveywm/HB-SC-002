;/*********************** (C) COPYRIGHT 2010 Libraworks *************************
;* File Name	: os_cpu_a.asm 
;* Author		: Librae 
;* Version		: V1.0
;* Date			: 06/10/2010
;* Description	: ��COS-II asm port
;*******************************************************************************/

        IMPORT  OsRunning               ; External references
        IMPORT  OsCurrentPriority
        IMPORT  OsHighReadyPriority
        IMPORT  OsCurrentTaskPointer
        IMPORT  OsHighReadyTaskPointer
                     
        EXPORT  OsStartHighReadyTask               
        EXPORT  OsSwitchContext     
            
        EXPORT  PendSV_Handler
        	
     
NVIC_INT_CTRL   	EQU     0xE000ED04  ; �жϿ��ƼĴ���
NVIC_PENDSVSET  	EQU     0x10000000  ; ��������жϵ�ֵ.


		PRESERVE8 
		
		AREA    |.text|, CODE, READONLY
        THUMB 
    
          


;/**************************************************************************************
;* ��������: OsStartHighReadyTask
;*
;* ��������: ʹ�õ��������е�һ������
;* 
;* ��    ��: None
;*
;* �� �� ֵ: None
;**************************************************************************************/  

OsStartHighReadyTask
        MOV     R4, #0                 ; set the PSP to 0 for initial context switch call
        MSR     PSP, R4

        LDR     R4, =OsRunning         ; OSRunning = TRUE
        MOV     R5, #1
        STRB    R5, [R4]

                                       ;�л���������ȼ�������
        LDR     R4, =NVIC_INT_CTRL     ;rigger the PendSV exception (causes context switch)
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]

        CPSIE   I                      ;enable interrupts at processor level
StartHang
        B       StartHang            ;should never get here

;/**************************************************************************************
;* ��������: OsSwitchContext
;*
;* ��������: �����������л�         
;*
;* ��    ��: None
;*
;* �� �� ֵ: None
;***************************************************************************************/
  
OsSwitchContext
		PUSH    {R4, R5}
        LDR     R4, =NVIC_INT_CTRL  	;����PendSV�쳣 (causes context switch)
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]
		POP     {R4, R5}
        BX      LR

;/**************************************************************************************
;* ��������: PendSV
;*
;* ��������: PendSV is used to cause a context switch.
;*
;* ��    ��: None
;*
;* �� �� ֵ: None
;***************************************************************************************/

PendSV_Handler
    CPSID   I                                                   ; Prevent interruption during context switch
    MRS     R0, PSP                                             ; PSP is process stack pointer �������PSP��ջ,����Ժ��Ա���Ĵ���,�ο�CM3Ȩ���е�˫��ջ-�ײ�ע
    CBZ     R0, PendSV_Handler_Nosave		                    ; Skip register save the first time

    SUBS    R0, R0, #0x20                                       ; Save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}

    LDR     R1, =OsCurrentTaskPointer                                       ; OSTCBCur->OSTCBStkPtr = SP;
    LDR     R1, [R1]
    STR     R0, [R1]                                            ; R0 is SP of process being switched out

                                                                ; At this point, entire context of process has been saved
PendSV_Handler_Nosave
    LDR     R0, =OsCurrentPriority                                      ; OSPrioCur = OSPrioHighRdy;
    LDR     R1, =OsHighReadyPriority
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, =OsCurrentTaskPointer                                       ; OSTCBCur  = OSTCBHighRdy;
    LDR     R1, =OsHighReadyTaskPointer
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                                            ; R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;
    LDM     R0, {R4-R11}                                        ; Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                                             ; Load PSP with new process SP
    ORR     LR, LR, #0x04                                       ; Ensure exception return uses process stack
    CPSIE   I
    BX      LR                                                  ; Exception return will restore remaining context




    	ALIGN
					
        END
        
;/*********************** (C) COPYRIGHT 2010 Libraworks ************************/
