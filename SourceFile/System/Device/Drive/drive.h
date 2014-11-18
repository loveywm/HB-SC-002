/*
******************************************************************************
* 
******************************************************************************
*文件名   : drive.h
*作用     : 驱动库头文件
*创建时间 : 2013.7.21
******************************************************************************
*/

#ifndef __DRIVE_H
#define __DRIVE_H
#include "stm32f10x.h"

//IO口地址映射
#define GPIOA_IDR_ADDR    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_ADDR    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_ADDR    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_ADDR    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_ADDR    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_ADDR    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_ADDR    (GPIOG_BASE+8) //0x40011E08 

#define GPIOA_ODR_ADDR    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_ADDR    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_ADDR    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_ADDR    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_ADDR    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_ADDR    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_ADDR    (GPIOG_BASE+12) //0x40011E0C    

//IO口操作,只对单一的IO口!确保n的值小于16!
#define BIT_BAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BIT_BAND(addr, bitnum)) 

#define PaIn(n)    BIT_ADDR(GPIOA_IDR_ADDR,n)
#define PaOut(n)   BIT_ADDR(GPIOA_ODR_ADDR,n)

#define PbIn(n)    BIT_ADDR(GPIOB_IDR_ADDR,n)
#define PbOut(n)   BIT_ADDR(GPIOB_ODR_ADDR,n)

#define PcIn(n)    BIT_ADDR(GPIOC_IDR_ADDR,n)
#define PcOut(n)   BIT_ADDR(GPIOC_ODR_ADDR,n)

#define PdIn(n)    BIT_ADDR(GPIOD_IDR_ADDR,n)
#define PdOut(n)   BIT_ADDR(GPIOD_ODR_ADDR,n)

#define PeIn(n)    BIT_ADDR(GPIOE_IDR_ADDR,n)
#define PeOut(n)   BIT_ADDR(GPIOE_ODR_ADDR,n)

#define PfIn(n)    BIT_ADDR(GPIOF_IDR_ADDR,n)
#define PfOut(n)   BIT_ADDR(GPIOF_ODR_ADDR,n)

#define PgIn(n)    BIT_ADDR(GPIOG_IDR_ADDR,n)
#define PgOut(n)   BIT_ADDR(GPIOG_ODR_ADDR,n)





#endif

