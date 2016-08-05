;/*****************************************************************************
; * @file:    startup_MPS_CM0.s
; * @purpose: CMSIS Cortex-M0 Core Device Startup File 
; *           for the ARM 'Microcontroller Prototyping System' 
; * @version: V1.0
; * @date:    19. Aug. 2009
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; * Copyright (C) 2008-2009 ARM Limited. All rights reserved.
; * ARM Limited (ARM) is supplying this software for use with Cortex-M0 
; * processor based microcontrollers.  This file can be freely distributed 
; * within development tools that are supporting such ARM based processors. 
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; *****************************************************************************/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000800

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                ;DCD    0                         ; 0:  Watchdog Timer
                ;DCD    0                         ; 1:  Real Time Clock
				;DCD 	0						  ; 2:	Timer0 / Timer1
				;DCD 	0						  ; 3:	Timer2 / Timer3
				;DCD 	0						  ; 4:	MCIa
				;DCD	0						  ; 5:	MCIb
				;DCD	0						  ; 6:  UART0 - DUT FPGA
				;DCD	0						  ; 7:  UART1 - DUT	FPGA
				;DCD	0						  ; 8:  UART2 - DUT	FPGA
                DCD     WriteEP0_Handler          ; 0:	Write EP0 finish
                DCD     ReadPkt_Handler           ; 1:	Read a packet finish
				DCD 	StallEP0_Handler		  ; 2:	Stall EP0 finish
				DCD		Suspend_Handler			  ; 3:  Enter suspend status
				DCD		USBReset_Handler		  ; 4:  USB Reset
				DCD		USBSpeedInt_Handler		  ; 5:  USB Speed Int
				DCD		0						  ; 6:	UART0 - DUT FPGA
				DCD		0						  ; 7:	UART1 - DUT FPGA
				DCD 	GPIOInt_Handler 	      ; 8:	GPIO Int
                DCD     0                         ; 9:  UART4 - not connected
                DCD     0                         ; 10: AACI / AC97
                DCD     0                         ; 11: CLCD Combined Interrupt
                DCD     0                         ; 12: Ethernet
                DCD     0                         ; 13: USB Device
                DCD     0                         ; 14: USB Host Controller
                DCD     0                         ; 15: Character LCD
                DCD     0                         ; 16: Flexray
                DCD     0                         ; 17: CAN
                DCD     0                         ; 18: LIN
                DCD     0                         ; 19: I2C ADC/DAC
                DCD     0                         ; 20: Reserved
                DCD     0                         ; 21: Reserved
                DCD     0                         ; 22: Reserved
                DCD     0                         ; 23: Reserved
                DCD     0                         ; 24: Reserved
                DCD     0                         ; 25: Reserved
                DCD     0                         ; 26: Reserved
                DCD     0                         ; 27: Reserved
                DCD     0                         ; 28: Reserved - CPU FPGA CLCD
                DCD     0                         ; 29: Reserved - CPU FPGA
                DCD     0                         ; 30: UART3    - CPU FPGA
                DCD     0                         ; 31: SPI Touchscreen - CPU FPGA


                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  __main
                  LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP									    
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

WriteEP0_Handler PROC
                EXPORT  WriteEP0_Handler           [WEAK]
                B       .
                ENDP
ReadPkt_Handler PROC
                EXPORT  ReadPkt_Handler           [WEAK]
                B       .
                ENDP
StallEP0_Handler PROC
				EXPORT	StallEP0_Handler		   [WEAK]
				B		.
				ENDP
Suspend_Handler PROC
				EXPORT	Suspend_Handler		  		[WEAK]
				B		.
				ENDP
USBReset_Handler PROC
				EXPORT	USBReset_Handler	 	   [WEAK]
				B		.
				ENDP
USBSpeedInt_Handler PROC
				EXPORT	USBSpeedInt_Handler		   [WEAK]
				B		.
				ENDP
GPIOInt_Handler PROC
				EXPORT	GPIOInt_Handler 	   [WEAK]
				B		.
				ENDP


Default_Handler PROC

                EXPORT  WDT_IRQHandler            [WEAK]
                EXPORT  RTC_IRQHandler            [WEAK]
                EXPORT  TIM0_IRQHandler           [WEAK]
                EXPORT  TIM2_IRQHandler           [WEAK]
                EXPORT  MCIA_IRQHandler           [WEAK]
                EXPORT  MCIB_IRQHandler           [WEAK]
                EXPORT  UART0_IRQHandler          [WEAK]
                EXPORT  UART1_IRQHandler          [WEAK]
                EXPORT  UART2_IRQHandler          [WEAK]
                EXPORT  UART3_IRQHandler          [WEAK]
                EXPORT  UART4_IRQHandler          [WEAK]
                EXPORT  AACI_IRQHandler           [WEAK]
                EXPORT  CLCD_IRQHandler           [WEAK]
                EXPORT  ENET_IRQHandler           [WEAK]
                EXPORT  USBDC_IRQHandler          [WEAK]
                EXPORT  USBHC_IRQHandler          [WEAK]
                EXPORT  CHLCD_IRQHandler          [WEAK]
                EXPORT  FLEXRAY_IRQHandler        [WEAK]
                EXPORT  CAN_IRQHandler            [WEAK]
                EXPORT  LIN_IRQHandler            [WEAK]
                EXPORT  I2C_IRQHandler            [WEAK]
				EXPORT  CPU_CLCD_IRQHandler       [WEAK]
                EXPORT  SPI_IRQHandler            [WEAK]

WDT_IRQHandler
RTC_IRQHandler
TIM0_IRQHandler
TIM2_IRQHandler
MCIA_IRQHandler
MCIB_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
UART3_IRQHandler
UART4_IRQHandler
AACI_IRQHandler
CLCD_IRQHandler
ENET_IRQHandler
USBDC_IRQHandler
USBHC_IRQHandler
CHLCD_IRQHandler
FLEXRAY_IRQHandler
CAN_IRQHandler
LIN_IRQHandler
I2C_IRQHandler
CPU_CLCD_IRQHandler
SPI_IRQHandler
                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB
                
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit
                
                ELSE
                
                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
