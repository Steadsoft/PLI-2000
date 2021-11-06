;/****************************************************************************/
;/*                           COPYRIGHT NOTICE                               */
;/****************************************************************************/
;/* Vulkan CONFIDENTIAL INFORMATION:                           Category:  1  */
;/* COPYRIGHT (c) 1990 Vulkan Technologies Ltd.                              */
;/*                                                                          */
;/* This data file contains confidential and proprietary information of      */
;/* Vulkan Technologies Ltd, and any reproduction, disclosure, or other use  */
;/* in whole or in part, is hereby expressly prohibited.                     */
;/*                                                                          */
;/* This restriction applies unless, prior written agreement or prior        */
;/* written permission has been given by Vulkan Technologies Ltd.            */
;/*                                                                          */
;/* Unlawful use of this material shall render the using party liable to     */
;/* prosecution.                                                             */
;/*                                                                          */
;/****************************************************************************/

;/***************************************************************************/
;/* This assembler file implements the PL/I compiler runtime library .      */
;/* All entries are written using the standard parameter accessing scheme   */
;/* and appear to the caller (compiler code) as if they were written in PL/I*/
;/* dos$ are calls that interface to DOS and BIOS interrupts.               */
;/* pli$ are language utilities and conversions etc.                        */
;/* sys$ are support routines used ONLY by runtime library.                 */
;/* If a new subroutine prefix is created in the future, be sure to update  */
;/* declare.c (check_reservations) to allow warning if user uses such a     */
;/* prefix for their own code.                                              */
;/***************************************************************************/      

;/***************************************************************************/
;/*                         Modification History                            */
;/***************************************************************************/
;/*  Who    *     When     *                  What                          */
;/***************************************************************************/          
;/*  HWG      10-09-91       Initial version created                        */
;/*  HWG      xx-10-91       Dos cursor management included                 */
;/*  HWG      xx-11-91       Graphics & disk stuff added                    */
;/*  HWG      xx-10-91       Binary to BCD conversion (NDP) added.          */
;/*  HWG      23-11-91       Dos version number function.                   */    
;/*  HWG      24-11-91       Horrible timing fault fixed, when running with */
;/*                          an NDP, lack of an fwait was causing a reg to  */
;/*                          be loaded with memory BEFORE the NDP had fully */
;/*                          stored its result, even though there were 10   */
;/*                          lines of assembler before the ref, it wasnt    */
;/*                          quite enough time.                             */          
;/*  HWG      26-11-91       sys$pack_bcd and sys$unpack_bcd implemented.   */
;/*  HWG      12-01-92       Major reorg to make parameter passing the same */
;/*                          as that of C (Turbo C).                        */        
;/*                                                                         */
;/***************************************************************************/
 


P286
P287

.MODEL large

EXTRN   pli$bootstrap_s: FAR      ; Both of these get created by compiler
EXTRN   pli$bootstrap_c: FAR      ; when it generates code for main proc

;/**************************************************************************/
;/*                   Data definitions start here                          */
;/**************************************************************************/ 

.DATA           

DEC_BUFF_SIZE EQU 64

PUBLIC  pli$stop_s  
PUBLIC  pli$skip_s
PUBLIC  dos$delay_s
PUBLIC  dos$set_cursor_s
PUBLIC  dos$terminate_s
PUBLIC  dos$set_current_disk_s
PUBLIC  dos$set_video_mode_s        
PUBLIC  dos$get_video_info_s 
PUBLIC  dos$get_dos_memory_s
PUBLIC  pli$reboot_s
PUBLIC  dos$get_cursor_s
PUBLIC  dos$get_current_disk_s
PUBLIC  pli$print_vcs_s
PUBLIC  dos$reset_mouse_s
PUBLIC  dos$show_mouse_s
PUBLIC  dos$hide_mouse_s
PUBLIC  dos$get_mouse_status_s
PUBLIC  dos$set_mouse_position_s
PUBLIC  dos$set_pixel_s
PUBLIC  dos$clear_screen_s
PUBLIC  dos$get_pixel_s
PUBLIC  dos$read_joystick_s
PUBLIC  dos$set_overscan_s
PUBLIC  dos$get_dos_info_s
PUBLIC  dos$open_file_s
PUBLIC  dos$set_repeat_rate_s
PUBLIC  dos$set_mouse_sensitivity_s
PUBLIC  dos$set_mouse_int_rate_s

pli$stop_s               LABEL WORD   
pli$print_vcs_s          LABEL WORD
pli$skip_s               LABEL WORD
dos$delay_s              LABEL WORD       
dos$set_cursor_s         LABEL WORD
dos$terminate_s          LABEL WORD
dos$set_current_disk_s   LABEL WORD
dos$set_video_mode_s     LABEL WORD
dos$get_video_info_s     LABEL WORD
dos$get_dos_memory_s     LABEL WORD
pli$reboot_s             LABEL WORD
dos$get_cursor_s         LABEL WORD
dos$get_current_disk_s   LABEL WORD
dos$reset_mouse_s        LABEL WORD
dos$read_joystick_s      LABEL WORD
dos$show_mouse_s         LABEL WORD
dos$hide_mouse_s         LABEL WORD
dos$get_mouse_status_s   LABEL WORD
dos$set_mouse_position_s LABEL WORD
dos$set_pixel_s          LABEL WORD
dos$clear_screen_s       LABEL WORD
dos$get_pixel_s          LABEL WORD
dos$set_overscan_s       LABEL WORD
dos$get_dos_info_s       LABEL WORD
dos$open_file_s          LABEL WORD       
dos$set_repeat_rate_s    LABEL WORD
dos$set_mouse_sensitivity_s LABEL WORD
dos$set_mouse_int_rate_s    LABEL WORD

; Interrupt Handler messages

crlf       DB 0dh,0ah,'$'

bounds_msg DB 0dh,0ah,0dh,0ah,'One or more array subscripts has exceeded its bounds.','$'

error_msg1 DB 0dh,0ah,'Fatal error ocurred at address ','$'
error_msg2 DB ':','$'

dead_msg   DB 0dh,0ah,'Program Terminating.',,0dh,0ah,'$'

break_msg  DB 0dh,0ah,0dh,0ah,'The "BREAK" condition has been signalled.','$'

break_msg1 DB 0dh,0ah,'break condition ocurred in segment ','$'
break_msg2 DB ' at offset ','$'






declen DB 0,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,18
prec DB 0,2,2,2,3,3,3,4,4,4,5,5,5,5,6,6,6,7,7,7,8,8,8,8,9,9,9,10,10,10,11,11
scal DB 0,1,1,1,2,2,2,3,3,3,4,4,4,4,5,5,5,6,6,6,7,7,7,7,8,8,8, 9, 9, 9,10,10

; *************************************************************************
; * To build the BCD equivalent for a binary fraction, we add up elements *
; * of the array below (Using the NDP) each bit that is 'on' in the       *
; * fraction part of the binary, is used to index the array.              *
; * ie .1101101011 is the sum of elements 1 2 4 5 7 9 and 10.             *
; ************************************************************************* 

fracs     DT 500000000000000000 ; 2^-1
          DT 250000000000000000 ; 2^-2
          DT 125000000000000000 ; 2^-3
          DT 062500000000000000 ; 2^-4
          DT 031250000000000000 ; 2^-5
          DT 015625000000000000 ; 2^-6
          DT 007812500000000000 ; 2^-7
          DT 003906250000000000 ; 2^-8
          DT 001953125000000000 ; 2^-9
          DT 000976562500000000 ; 2^-10
          DT 000488281250000000 ; 2^-11
          DT 000244140625000000 ; 2^-12
          DT 000122070312500000 ; 2^-13
          DT 000061035156250000 ; 2^-14
          DT 000030517578125000 ; 2^-15
          DT 000015258789062500 ; 2^-16
          DT 000007629394531250 ; 2^-17
          DT 000003814697265625 ; 2^-18
          DT 000001907348632812 ; 2^-19
          DT 000000953674316406 ; 2^-20
          DT 000000476837158203 ; 2^-21
          DT 000000238418579101 ; 2^-22
          DT 000000119209289550 ; 2^-23
          DT 000000059604644775 ; 2^-24
          DT 000000029802322387 ; 2^-25
          DT 000000014901161193 ; 2^-26
          DT 000000007450580595 ; 2^-27
          DT 000000003725290298 ; 2^-28
          DT 000000001862645149 ; 2^-29
          DT 000000000931322574 ; 2^-30

prect     DB 0
scalt     DB 0
precs     DB 0
scals     DB 0
          
dwhole    DT 0                     ; BCD of whole
dfract    DT 0                     ; BCD of fract
decimal   DT 0                     ; Fully converted temp BCD result

curpos    DB 0                     ; position of next free dec digit in tempd
start     DB 0
binlen    DB 0                     ; size (in bytes) of this binary  

swhole_d  LABEL DWORD
swhole    DB 0,0,0,0               ; holds whole part of the bin(p,q)
sfract    DB 0,0,0,0               ; holds fract part of the bin(p,q)
odd_fract DB 0                     ; flag value      
funpacked DB 19 DUP(0)             ; unpacked dec fraction part 
wunpacked DB 19 DUP(0)             ; unpacked dec whole part
gunpacked DB 19 DUP(0)             ; General purpose unpack buffer 

;/***************************************************************************/
;/*                           Code starts here.                             */
;/***************************************************************************/

.CODE

PUBLIC  pli$stop_c
PUBLIC  dos$delay_c
PUBLIC  dos$set_cursor_c
PUBLIC  dos$terminate_c     
PUBLIC  dos$set_current_disk_c   
PUBLIC  dos$set_video_mode_c
PUBLIC  dos$get_video_info_c
PUBLIC  dos$get_dos_memory_c   
PUBLIC  pli$reboot_c
PUBLIC  dos$get_cursor_c  
PUBLIC  dos$get_current_disk_c    
PUBLIC  pli$print_vcs_c
PUBLIC  pli$copy_vcs
PUBLIC  pli$print_char
PUBLIC  pli$set_cursor
PUBLIC  pli$skip_c
PUBLIC  dos$read_joystick_c
PUBLIC  pli$allocate
PUBLIC  dos$reset_mouse_c
PUBLIC  dos$show_mouse_c
PUBLIC  dos$hide_mouse_c
PUBLIC  dos$get_mouse_status_c
PUBLIC  dos$set_mouse_position_c
PUBLIC  dos$set_pixel_c
PUBLIC  dos$clear_screen_c 
PUBLIC  dos$get_pixel_c
PUBLIC  dos$set_overscan_c
PUBLIC  dos$get_dos_info_c
PUBLIC  dos$open_file_c
PUBLIC  dos$set_repeat_rate_c
PUBLIC  dos$set_mouse_sensitivity_c
PUBLIC  dos$set_mouse_int_rate_c

IDEAL

;****************************************************************************/
;* This is pli$main the initial entry point for PC-PL/1 programs, it calls  */
;* the user entry using the same prolog sequence as generated pl1/ code.    */
;* Thus the outer block can be compiled identically to any inner block.     */
;* pli$main will also store the default handler address for conditions.     */
;****************************************************************************/


PROC    pli$main

        ; Set up handler for Array Bounds Check

        mov     ah,25h  
        mov     al,5
        int     21h
        mov     dx, SEG    int$bounds_trap
        mov     ds,dx
        mov     dx, OFFSET int$bounds_trap
        int     21h

        ; Set up handler for Ctrl-Break key

        ; mov     ah,25h  
        ; mov     al,1Bh     ; Ctrl-Break
        ; int     21h
        ; mov     dx, SEG    int$break_trap
        ; mov     ds,dx
        ; mov     dx, OFFSET int$break_trap
        ; int     21h

        mov     cx, SEG pli$bootstrap_s ; Put address of users static in CX
        push    ds                      ; Save our static segment pointer
        call    pli$bootstrap_c         ; Directly call the users entry
        pop     ds                      ; Restore our static segment
        mov     ah,4Ch                  ; Terminate formally through DOS
        int     21h 

ENDP    pli$main

     
PROC    pli$stop_c
        
        ;/****************************************************************/
        ;/* This entry will terminate the calling program, and return to */
        ;/* the DOS command level, status is forced to zero.             */
        ;/****************************************************************/

                mov     al,0
                mov     ah,4Ch
                int     21h
                ret

ENDP    pli$stop_c


PROC    dos$get_dos_info_c

                pusha
                mov     bp,sp
                mov     ah,30h
                mov     al,00h
                int     21h
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     [si],al      ; Major number
                mov     ds,[bp+26]   ; set DS to seg of parm2
                mov     si,[bp+24]   ; set SI to offset of parm2
                mov     [si],ah      ; Minor number
                mov     ds,[bp+30]   ; set DS to seg of parm3
                mov     si,[bp+28]   ; set SI to offset of parm3
                mov     [si],bh      ; OEM Serial
                mov     sp,bp
                popa
                ret 

ENDP    dos$get_dos_info_c
                 

PROC    pli$reboot_c

                jmp       far 0F000h:0FFF0H

                ; ret                ; Not working as expected
                ; int     19h
                ; ret

ENDP    pli$reboot_c

PROC    dos$set_cursor_c

        ;/****************************************************************/
        ;/*                Set the dos cursor position                   */
        ;/****************************************************************/

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     bh,[si]      ; Page number for set cursor
                mov     ds,[bp+26]   ; set DS to seg of parm2
                mov     si,[bp+24]   ; set SI to offset of parm2
                mov     dh,[si]      ; Row (y coord)
                mov     ds,[bp+30]   ; set DS to seg of parm3
                mov     si,[bp+28]   ; set SI to offset of parm3
                mov     dl,[si]      ; Col (x coord)
                mov     ah,2
                int     10h          ; Trap into DOS
                mov     sp,bp
                popa
                ret     

ENDP    dos$set_cursor_c   

PROC    dos$set_mouse_sensitivity_c

        ;/****************************************************************/
        ;/*                Set the dos cursor position                   */
        ;/****************************************************************/

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     bx,[si]      ; Horizontal Mickeys
                mov     ds,[bp+26]   ; set DS to seg of parm2
                mov     si,[bp+24]   ; set SI to offset of parm2
                mov     cx,[si]      ; Vertical Mickeys
                mov     ds,[bp+30]   ; set DS to seg of parm3
                mov     si,[bp+28]   ; set SI to offset of parm3
                mov     dx,[si]      ; Doubling threshold
                mov     ax,001Ah
                int     33h          ; Trap into DOS
                mov     sp,bp
                popa
                ret     

ENDP    dos$set_mouse_sensitivity_c   


PROC    dos$set_repeat_rate_c

        ;/****************************************************************/
        ;/*                Set the Keyboard speed                        */
        ;/****************************************************************/

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     bh,[si]      ; Repeat Delay
                mov     ds,[bp+26]   ; set DS to seg of parm2
                mov     si,[bp+24]   ; set SI to offset of parm2
                mov     bl,[si]      ; Repeat Rate
                mov     al,5
                mov     ah,3
                int     16h          ; Trap into DOS
                mov     sp,bp
                popa
                ret     

ENDP    dos$set_repeat_rate_c   




PROC    dos$get_video_info_c

                pusha
                mov     bp,sp
                mov     ah,0Fh
                int     10h
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     [si],ah      ; screen columns
                mov     ds,[bp+26]   ; set DS to seg of parm2
                mov     si,[bp+24]   ; set SI to offset of parm2
                mov     [si],al      ; video mode
                mov     ds,[bp+30]   ; set DS to seg of parm3
                mov     si,[bp+28]   ; set SI to offset of parm3
                mov     [si],bh      ; active page number
                mov     sp,bp
                popa
                ret  

ENDP    dos$get_video_info_c

PROC    dos$get_cursor_c

                pusha
                mov     bp,sp
                mov     ah,03h       ; get cursor pos'n
                int     10h
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     [si],dh      ; current row
                mov     ds,[bp+26]   ; set DS to seg of parm3
                mov     si,[bp+24]   ; set SI to offset of parm3
                mov     [si],dl      ; current column
                mov     sp,bp
                popa
                ret 
 
ENDP    dos$get_cursor_c
                

        ;/*****************************************************************/
        ;/* Terminate the calling program, and set the return code.       */
        ;/*****************************************************************/

PROC    dos$terminate_c

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     al,[si]      ; Set AL to return code
                mov     ah,4ch       ; terminate caller
                int     21h          ; Trap into DOS
                mov     sp,bp
                popa
                ret   
           

ENDP    dos$terminate_c

        ;/*****************************************************************/
        ;/* Terminate the calling program, and set the return code.       */
        ;/*****************************************************************/

PROC    dos$set_mouse_int_rate_c

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     bx,[si]      ; Set AL to rate flags
                mov     ax,001Ch     ; terminate caller
                int     33h          ; Trap into DOS
                mov     sp,bp
                popa
                ret   
           

ENDP    dos$set_mouse_int_rate_c

        ;/******************************************************/
        ;/* Delay caller.                                      */
        ;/******************************************************/ 

PROC    dos$delay_c

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     cx,[si]      ; Set CX to interval
                mov     dx,0 
                mov     ah,086h     ; Sleep
                int     15h          ; Trap into DOS
                mov     sp,bp
                popa
                ret   
           

ENDP    dos$delay_c



        ;/*****************************************************************/
        ;/* Reset mouse driver and get status code.                       */
        ;/*****************************************************************/

PROC    dos$reset_mouse_c

                pusha
                mov     bp,sp
                mov     ds,[bp+22]              ; set DS to seg of parm1
                mov     si,[bp+20]              ; set SI to offset of parm1
                mov     [word ptr si],0000h     ; Set status to zero for now
                mov     ax,0000h                ; Code for mouse reset
                int     33h                     ; Trap into DOS and reset driver
                cmp     ax,0000h                ; If AX=0000 then NO mouse !
                jne     mouse_ok
                mov     [word ptr si],0001h     ; set status to a non-zero value
mouse_ok: 
                mov     sp,bp
                popa
                ret   
           

ENDP    dos$reset_mouse_c

PROC    dos$show_mouse_c
        
                pusha
                mov     bp,sp
                mov     ax,0001h
                int     33h
                mov     sp,bp
                popa
                ret   

ENDP    dos$show_mouse_c                

PROC    dos$hide_mouse_c
        
                pusha
                mov     bp,sp
                mov     ax,0002h
                int     33h
                mov     sp,bp
                popa
                ret   

ENDP    dos$hide_mouse_c       


PROC    dos$get_mouse_status_c

                pusha
                mov     bp,sp
                mov     ax,0003h
                int     33h
                mov     ds,[bp+22]              
                mov     si,[bp+20]              
                mov     [word ptr si],bx      
                mov     ds,[bp+26]   
                mov     si,[bp+24]   
                mov     [word ptr si],cx      
                mov     ds,[bp+30]   
                mov     si,[bp+28]   
                mov     [word ptr si],dx      
                mov     sp,bp
                popa
                ret   

ENDP    dos$get_mouse_status_c         

PROC    dos$set_mouse_position_c

        ;/****************************************************************/
        ;/*                Set the mouse pointer position                */
        ;/****************************************************************/

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     cx,[word ptr si]      ; X coord 
                mov     ds,[bp+26]   ; set DS to seg of parm2
                mov     si,[bp+24]   ; set SI to offset of parm2
                mov     dx,[word ptr si]      ; Y coord
                mov     ax,0004h
                int     33h          ; Trap into DOS mouse-driver
                mov     sp,bp
                popa
                ret   

ENDP    dos$set_mouse_position_c   


PROC    dos$set_pixel_c

                pusha
                mov     bp,sp
                mov     ds,[bp+22]
                mov     si,[bp+20]
                mov     al,[si]      ; pixel value
                mov     ds,[bp+26]   
                mov     si,[bp+24]   
                mov     bh,[si]      ; Page number 
                mov     ds,[bp+30]   
                mov     si,[bp+28]   
                mov     cx,[word ptr si]      ; Col (x coord)
                mov     ds,[bp+34]   
                mov     si,[bp+32]   
                mov     dx,[word ptr si]      ; Row (y coord)
                mov     ah,0Ch
                int     10h          ; Trap into BIOS
                mov     sp,bp
                popa
                ret   

ENDP    dos$set_pixel_c 

PROC    dos$get_pixel_c

                pusha
                mov     bp,sp
                mov     ah,0Dh
                mov     ds,[bp+26]   
                mov     si,[bp+24]   
                mov     bh,[si]              ; Page number 
                mov     ds,[bp+30]   
                mov     si,[bp+28]   
                mov     cx,[word ptr si]     ; Col (x coord)
                mov     ds,[bp+34]   
                mov     si,[bp+32]   
                mov     dx,[word ptr si]     ; Row (y coord)
                int     10h                  ; Trap to BIOS  
                mov     ds,[bp+22]
                mov     si,[bp+20]
                mov     [si],al              ; pixel value
                mov     sp,bp
                popa
                ret  

ENDP    dos$get_pixel_c 

PROC    dos$read_joystick_c

                pusha
                mov     bp,sp
                mov     ah,84h
                mov     dx,00h
                int     15h

                

                mov     ds,[bp+22]
                mov     si,[bp+20]
                mov     [si],al              ; Switch values
 
                mov     ah,84h
                mov     dx,01h
                int     15h

                mov     ds,[bp+26]   
                mov     si,[bp+24]   
                mov     [word ptr si],ax     ; A x 
                mov     ds,[bp+30]   
                mov     si,[bp+28]   
                mov     [word ptr si],bx     ; A y
                mov     ds,[bp+34]   
                mov     si,[bp+32]   
                mov     [word ptr si],cx     ; B x
                mov     ds,[bp+38]   
                mov     si,[bp+36]   
                mov     [word ptr si],dx     ; B x

                mov     sp,bp
                popa
                ret  

ENDP    dos$read_joystick_c 


PROC    dos$set_overscan_c

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     bh,[si]      ; Set BH to border color
                mov     ah,10h       ; terminate caller
                mov     al,01h
                int     10h          ; Trap into BIOS
                mov     sp,bp
                popa
                ret              

ENDP    dos$set_overscan_c

                 

PROC    dos$clear_screen_c

                pusha
                mov     bp,sp
                mov     ah,0Fh
                int     10h
                mov     ah,0h                
                int     10h
                mov     sp,bp
                popa
                ret

ENDP    dos$clear_screen_c

PROC    dos$set_video_mode_c

                pusha
                mov     bp,sp
                mov     ds,[bp+22]
                mov     si,[bp+20]
                mov     al,[si]
                mov     ah,00h
                int     10h
                mov     sp,bp
                popa
                ret     

ENDP    dos$set_video_mode_c        
 
PROC    dos$set_current_disk_c

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     dl,[si]      ; Set DL to drive number   
                mov     ah,0Eh
                int     21h
                mov     sp,bp
                popa
                ret   

ENDP    dos$set_current_disk_c

PROC    dos$get_dos_memory_c

                pusha
                mov     bp,sp
                int     12h        
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     [si],ax      ; memory in Kb   
                mov     sp,bp
                popa
                ret   

ENDP    dos$get_dos_memory_c                   

PROC    dos$get_current_disk_c

                pusha
                mov     bp,sp
                mov     ah,19h
                int     21h        
                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     si,[bp+20]   ; set SI to offset of parm1
                mov     ah,0         ; set msb byte off
                mov     [si],ax      ; curr disk num   
                mov     sp,bp
                popa
                ret    
ENDP    dos$get_current_disk_c

PROC    dos$open_file_c

                pusha
                mov     bp,sp

                ; Set DS:DX to point to the string (pathname)

                mov     ds,[bp+22]   ; set DS to seg of parm1
                mov     dx,[bp+20]   ; set DX to offset of parm1

                mov     es,[bp+26]   ; set DS to seg of parm2
                mov     si,[bp+24]   ; set SI to offset of parm2
                mov     al,[es:si]   ; Access mode

                mov     ah,3Dh
                int     21h

                jc      open_error   ; If carry, then error ocurred !

                mov     ds,[bp+30]   ; set DS to seg of parm3
                mov     si,[bp+28]   ; set SI to offset of parm3
                mov     [si],ax      ; Store handle value
                mov     ds,[bp+34]
                mov     si,[bp+32]
                mov     [word ptr si],0h      ; Set error code to zero

                jmp     exit_open
open_error:
                mov     ds,[bp+30]   ; set DS to seg of parm3
                mov     si,[bp+28]   ; set SI to offset of parm3
                mov     [word ptr si],0h      ; Store handle value of Zero
                mov     ds,[bp+34]
                mov     si,[bp+32]
                mov     [si],ax      ; Set error code to AX
exit_open:
                popa
                ret     
      
ENDP    dos$open_file_c

PROC    pli$print_vcs_c

        ;/**************************************************************/
        ;/* This entry will print a varying string, to the DOS default */
        ;/* output channel.                                            */
        ;/* It expects a single argument, a pointer to the string.     */
        ;/**************************************************************/ 

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; Get segment part of far ptr
                mov     bx,[bp+20]   ; Get address of arg 1 from stack
                mov     cx,[bx]      ; Get length of varying string (2 bytes)
                inc     bx
@@_next_char:   cmp     cx,0         ; Is Length zero ?
                je      @@_end_of_str
                inc     bx
                dec     cx
                mov     dl,[bx]
                mov     ah,2         ; Print one character
                int     21h          ; .  .  .  .  .  .  .  .      
                jmp     @@_next_char
                mov     sp,bp
@@_end_of_str:  popa                 ; Restore all registers
                ret   

ENDP    pli$print_vcs_c


PROC    pli$copy_vcs

        ;/***********************************************************/
        ;/* This entry will copy a varying string from srce address */
        ;/* to dest address.                                        */     
        ;/***********************************************************/

                pusha
                mov     bp,sp
                mov     ds,[bp+26]      ;2 Destination segment
                mov     bx,[bp+24]      ;2 Destination offset
                mov     es,[bp+22]      ;1 Source      segment
                mov     di,[bp+20]      ;1 Source      offset
                mov     cx,[es:di]      ;CX = length word of srce string
                mov     [bx],cx         ;Set length word of dest
                add     bx,2            ;point to 1st char after length
                add     di,2            ;. . . . . . . . . . . . . . . .
@@_next_char:   cmp     cx,0
                je      @@_end_of_str
                mov     al,[es:di]
                mov     [bx],al
                inc     bx
                inc     di
                dec     cx
                jmp     @@_next_char
@@_end_of_str:  popa
                ret 
        
ENDP    pli$copy_vcs          



PROC    pli$print_char

                push    bp        
                mov     bp,sp
                mov     dx,[bp+6]
                mov     ah,9
                int     21h
                pop     bp
                ret   

ENDP    pli$print_char                

PROC    pli$skip_c

                pusha
                mov     bp,sp
                mov     ds,[bp+22]   ; Get segment part of far ptr
                mov     bx,[bp+20]   ; Get offset  part of far ptr
                mov     cx,[bx]      ; put skip value into CX
next_skip:
                cmp     cx,0
                je      skip_done
                mov     dl,0dh
                mov     ah,2
                int     21h
                mov     dl,0ah
                mov     ah,2
                int     21h
                dec     cx
                jmp     next_skip
skip_done:
                popa
                ret     
ENDP    pli$skip_c                


PROC    pli$set_cursor

                pusha
                mov     bp,sp
                add     bp,20
                mov     bx,[bp]         ;Col
                mov     di,[bp+2]       ;Row
                mov     dh,[bx]
                mov     dl,[di]
                mov     bh,0
                mov     ah,2
                int     10h
                popa
                ret   

ENDP    pli$set_cursor



PROC    pli$allocate

;/**************************************************************************/
;/*  Return a pointer to a block of memory allocated from the DOS heap.    */
;/**************************************************************************/

                pusha
                mov     bp,sp

                mov     ds,[bp+26]      ;DS = segment of size required     
                mov     di,[bp+24]      ;DI = offset of size required
                mov     es,[bp+22]      ;ES = segment of result ptr
                mov     si,[bp+20]      ;SI = offset of result ptr

                mov     ah,48h
                mov     bx,[di]
                int     21h
                jnc     @@_allocate_ok
                mov     ax,0
                mov     [es:[di]],ax
                mov     [es:[si+2]],ax      
                jmp     @@_finished
@@_allocate_ok: mov     [es:[si]],ax
                mov     ax,0
                mov     [es:[si+2]],ax
@@_finished:    mov     sp,bp
                popa
                ret                     

ENDP     

;/****************************************************************************/
;/* Take a 10 Byte BCD value and unpack it into a 20 Byte result. The digits */
;/* are in the same order as before.                                         */
;/****************************************************************************/

        
PROC     sys$unpack_bcd

         add    bx,9                  ; Point to sign (tail) of the BCD
         add    si,18                 ; Point to tail of result
         mov    cx,10                 ; counter = 10
         mov    al,[byte ptr bx]      ; Get sign byte
         mov    [byte ptr si],al      ; store it

unext_digit:

         dec    cx
         dec    bx
         dec    si

         mov    al,[byte ptr bx]
         shr    al,4
         mov    [byte ptr si],al
         mov    al,[byte ptr bx]
         and    al,0Fh
         dec    si
         mov    [byte ptr si],al
        
         cmp    cx,1
         jne    unext_digit
         retf

ENDP     

;/****************************************************************************/
;/* Take a 20 Byte unpacked value & unpack it into a 10 Byte BCD. The digits */
;/* are in the same order as before.                                         */
;/****************************************************************************/

        
PROC     sys$pack_bcd

         add    bx,9                  ; Point to sign (tail) of the BCD
         add    si,18                 ; Point to tail of result
         mov    cx,10                 ; counter = 10
         mov    al,[byte ptr si]      ; Get sign byte
         mov    [byte ptr bx],al      ; store it

pnext_digit:

         dec    cx
         dec    bx
         dec    si

         mov    al,[byte ptr si]
         shl    al,4
         add    al,[byte ptr si-1]
         mov    [byte ptr bx],al
         dec    si

         cmp    cx,1
         jne    pnext_digit
         retf

ENDP    

;/**************************************************************************/
;/* This is the trap handler for Array subscript violations, the compiled  */
;/* code test the subscript with known limits, and traps here if the index */
;/* to the array is to large.                                              */
;/* Although the handler is always installed, it will only ever get invoked*/
;/* (Normally !) via an INT 5 instruction.                                 */
;/**************************************************************************/ 


PROC    int$bounds_trap  ; Invoked when BOUND check fails

        sti
        pusha
    
        ; set up static seg for the handler

        mov     ax,@data
        mov     ds,ax
        mov     bp,sp

        mov     dx,offset bounds_msg
        mov     ah,9
        int     21h

        ; Get Interrupt retn addr

        mov     dx,offset error_msg1
        mov     ah,9
        int     21h

        mov     dl,[byte ptr bp+13h]
        call    far sys$write_hex_byte
        mov     dl,[byte ptr bp+12h]
        call    far sys$write_hex_byte

        mov     dx,offset error_msg2
        mov     ah,9
        int     21h

        mov     dl,[byte ptr bp+11h]
        call    far sys$write_hex_byte
        mov     dl,[byte ptr bp+10h]
        call    far sys$write_hex_byte

        mov     dx,offset dead_msg
        mov     ah,9
        int     21h

        mov     dx,offset crlf
        mov     ah,9
        int     21h
    
        mov     ah,4ch     ; Kill the application DEAD.
        int     21h
        
        popa
        retf
        
ENDP    int$bounds_trap

PROC    int$break_trap  ; Invoked when BOUND check fails

        pusha
    
        ; set up static seg for the handler

        mov     ax,@data
        mov     ds,ax
        mov     bp,sp

        mov     dx,offset break_msg
        mov     ah,9
        int     21h

        ; Get Interrupt retn addr

        mov     dx,offset break_msg1
        mov     ah,9
        int     21h

        mov     dl,[byte ptr bp+13h]
        call    far sys$write_hex_byte
        mov     dl,[byte ptr bp+12h]
        call    far sys$write_hex_byte

        mov     dx,offset break_msg2
        mov     ah,9
        int     21h

        mov     dl,[byte ptr bp+11h]
        call    far sys$write_hex_byte
        mov     dl,[byte ptr bp+10h]
        call    far sys$write_hex_byte

        mov     dx,offset dead_msg
        mov     ah,9
        int     21h

        mov     dx,offset crlf
        mov     ah,9
        int     21h

        mov     ah,4ch     ; Kill the application DEAD.
        int     21h
        
        sti
        popa
        retf
        
ENDP    int$break_trap


PROC    sys$write_hex_byte ; Print the byte in DL in hex at curr posn

        push    cx
        push    dx
        mov     dh,dl
        mov     cx,4
        shr     dl,cl
        call    far sys$write_hex_digit
        mov     dl,dh
        and     dl,0Fh
        call    far sys$write_hex_digit
        pop     dx
        pop     cx
        retf

ENDP    sys$write_hex_byte

PROC    sys$write_hex_digit

        push    dx
        cmp     dl,10   
        jae     hex_letter
        add     dl,"0"
        jmp     write_digit

hex_letter:

        add     dl,"A"-10

write_digit:                                

        call    far sys$write_char
        pop     dx
        ret

ENDP    sys$write_hex_digit

PROC    sys$write_char

        push    ax
        mov     ah,2
        int     21h
        pop     ax
        ret

ENDP    sys$write_char

        end  pli$main ; Primary entry point for final user program 


                
        

