/****************************************************************************/
/*                           COPYRIGHT NOTICE                               */
/****************************************************************************/
/* A PL/I Compiler for the Win32 platform.                                  */
/* Copyright (C) 1997 - 2006 Hugh Gleaves.                                    */
/*                                                                          */
/* This program is free software; you can redistribute it and/or            */
/* modify it under the terms of the GNU General Public License              */
/* as published by the Free Software Foundation; either version 2           */
/*                                                                          */
/* This program is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program; if not, write to the Free Software              */
/* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 */
/* USA.                                                                     */
/****************************************************************************/

/****************************************************************************/
/*                         Modification History                             */
/****************************************************************************/
/*  Who    When                           What                              */
/* ------------------------------------------------------------------------ */
/* HWG    12-04-91       Initial Prototype.                                 */
/* HWG    20-05-91       Initial code for simple iterative loops.           */
/* HWG    21-05-91       Block entry and exit code now generated.           */
/* HWG    21-05-91       Static and automatic items now accessed.           */
/* HWG    23-05-91       Procedure call/return implemented.                 */
/* HWG    07-10-91       Conversion started for interface to code emitter.  */
/* HWG    13-10-91       Stack walking code created when accessing vars in  */
/*                       any outer blocks                                   */
/* HWG    17-10-91       Stack walker wasnt using segment overrides for _EBX  */
/* HWG    03-11-91       Stack accessing code is now created when a block   */
/*                       references a parameter.                            */
/* HWG    17-11-91       The fixup for the unconditional forward jump in a  */
/*                       iterative loop, was 1 byte too far, this caused a  */
/*                       vital POP instruction to be missed when a loop was */
/*                       ended, this caused a gradual stack destruction.    */
/* HWG    28-11-91       Call statements used to access the static and code */
/*                       of the target block using _EBX, this has been changed*/
/*                       to _DI because get_static_base uses _EBX itself.      */
/*                       Get_static_base has been inserted to allow calls   */
/*                       to external entries declared in outer blocks.      */   
/* HWG    17-12-91       When passing a parameter that was a static item    */
/*                       in an outer block, the data segment was passed as  */
/*                       DS this is the calling blocks static however, NOT  */
/*                       the declaring block. ES is now pushed in this case.*/    
/* HWG    10-01-92       A major alteration was made to make the compiler   */
/*                       pass parameters in the same way as Turbo C.        */
/*                       This involves reversing the order of Seg/Ofx when  */
/*                       pushing an arguments address, and also making the  */
/*                       caller pop any args rather than the callee.        */
/*                       It is now possible to write language-support code  */
/*                       in C, since stack-frame management is compatible.  */
/* HWG    15-01-92       Multiplication of bin(15) vals & constants has     */
/*                       been implemented.                                  */
/* HWG    16-01-92       Array references are now translated, the function  */
/*                       load_ofx() is used to acheive this.                */
/* HWG    22-01-92       The use of fixups for setting the offset of forward*/
/*                       jumps when generating loops, is discontinued.      */
/*                       by its nature, it limited the code in the loop to  */
/*                       about 900/1000 bytes. Backpatching is now used.    */
/* HWG    30-03-92       An initial form of the if-then-else statement has  */
/*                       been incorporated.                                 */
/*                                                                          */
/* HWG    24-06-93       tran_assignment Now checks to see if the result of */
/*                       an expression is in the NDP (CoProcessor) and uses */
/*                       it accordingly.                                    */ 
/*                                                                          */
/* HWG    02-07-93       The ops: >= and <= added for IF stmts.             */
/*																			*/
/* HWG01  15-01-96       'tran_call' did not check to see if there were any */
/*						 args before checking them, it refd NULL in this    */
/*						 case and NT slapped it !							*/ 
/*																		    */
/* HWG02  18-10-02       Added WORD_PTR to the instruction that addresses a */
/*                       numeric constant when addressing a reference,      */  
/****************************************************************************/

/****************************************************************************/
/*                         Functional Description                           */
/****************************************************************************/
/* This source file contains all those functions that are concerned with    */
/* code generation. The code phase, walks down the parse-tree (like pass2)  */
/* and generates code for each type of node encountered.                    */
/* The code generated in here is for the iAP-80286 CPU.                     */
/* Code is generated and written in a fairly 'high level' manner, the code  */
/* emitter is responsible for the physical creation of code records.        */
/****************************************************************************/

typedef unsigned char   chur;
typedef unsigned short  unt;

# include <setjmp.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <dos.h>
/* # include <dir.h> */
# include "c_types.h"
# include "tokens.h"
# include "nodes.h"
# include "symtab.h"
# include "pentium.h"
# include "fixup.h"
# include "options.h" 
# include "coff.h"
# include "globals.h"
# include "pli_machine.h"


# define _LINE_     ((short)__LINE__)

extern      FILE            *OB;
extern      Instruction     Inst;
extern      fixup           Fixup;
extern      char            line_no[10];
/*extern*/      Block_ptr       block_root; /* root of symtab */
extern      Procedure_ptr   parse_root;


Any_ptr        next_node          (void);
static short   tran_nodes         (Any_ptr);
short          nodetype           (Any_ptr);
void           report             (short,char *,short); 
static void    tran_procedure     (void);
static void    tran_if            (void);
static void    tran_stop          (void);
static void    tran_call          (void);
static void    tran_put           (void);
static void    tran_entry         (void);
static void    tran_return        (void);
static void    tran_loop          (void);
static void    tran_do            (void);
static void    tran_begin         (void);
static void    tran_allocate      (void);
static void    tran_free          (void);
static void    tran_goto          (void);
static void    tran_end           (void);
static void    tran_label         (void);
static void    tran_assignment    (void); 
static void    tran_add           (Any_ptr);
static void    tran_relation      (Any_ptr);
static void    tran_sub           (Any_ptr);
static void    tran_mul           (Any_ptr);
static void    tran_div           (Any_ptr);
static void    tran_expression    (Any_ptr);
static void    tran_reference     (Ref_ptr);
void    set_error_line     (Any_ptr);
short   next_node_type     (Any_ptr);
void    open_object        (char *);
void    enter_code         (void);
void    retreat            (void);
//void    write_modend       (void);

void    CpuSetSectPtr      (Section_ptr);

void    CpuGenerate        (void);
void    write_code         (void);
void    start_code         (Block_ptr);
void    patch_code         (long,Any_ptr,short); 
short   get_code_offset    (void);
short   get_ledata_offset  (void);
short   get_segment_offset (void);
long    get_file_offset    (void);
void    write_fixup        (void);
static void    gen_fixup   (void);
void    begin_fixup        (void); 
chur    get_auto_reg       (Symbol_ptr);
static chur    get_static_base    (Symbol_ptr);
static long    frame_offset       (Symbol_ptr);
static long    runtime_offset     (Symbol_ptr);
static void    address_reference  (Ref_ptr,short,chur);
long    unique             (void);
chur    load_ofx           (Ref_ptr,chur);
short   identical          (Any_ptr,Any_ptr);
  
jmp_buf    exit_code;
short      defbas_depth = 0;
short      block_ctr; 
char       recursive_name[33];
long       unique_ctr = 0;
long       patch_posn;
long       return_posn;
Any_ptr    curr_node_ptr     = NULL;
Block_ptr  curr_parent_block = NULL;
Block_ptr  curr_ES_block     = NULL; /* optimize access to outer statics    */
Block_ptr  curr_EBX_block     = NULL; /* ------------------------ automatics */   
Ref_ptr    curr_DI_ref       = NULL; /* optimize array index calcs          */ 
char       result_in_NDP     = 0;    /* If an expr's result is in NDP       */

/****************************************************************************/
/*         This is the main function entry for pass2 processing.            */
/****************************************************************************/

void enter_code (void)

     {

     Any_ptr  ptr;

     block_ctr = 0;

     begin_fixup();

     if (setjmp(exit_code) == 0)  /* Exit code, if we get a null node-ptr  */
        {                         /* This will only ocurr as a result of   */
        ptr = next_node();        /* a compiler error.                     */

        tran_nodes (ptr);           
		
     	CoffWriteObjFile (obj_root);
                                        
        // write_code(); /* flush any buffered code record */
        // write_fixup(); /* TEST FUNCTION ONLY !!! */
        // write_modend();
        }
     else
        return;

     }

/****************************************************************************/
/* This function will take a node-ptr and call a code function to tran */
/* that node !                                                              */
/****************************************************************************/

static short  

tran_nodes (Any_ptr node_ptr)


     {

     Procedure_ptr p_ptr;
     Block_ptr     here_ptr;
     Block_ptr     cb_ptr;
     Block_ptr     prev_parent_block;
	 short	       ntype;


     if (node_ptr == NULL)
        return (0);

     ntype = nodetype(node_ptr);

     switch (ntype) {

     case (PROCEDURE):
          p_ptr    = node_ptr;
          prev_parent_block = curr_parent_block;
          curr_parent_block = p_ptr->proc;
          here_ptr = curr_node_ptr;
          curr_node_ptr = p_ptr->proc->first_stmt;
          tran_procedure ();
          curr_node_ptr = here_ptr;
          curr_parent_block = prev_parent_block;
          break;
     case (IF):
          tran_if();
          break;
     case (STOP):
          tran_stop();
          break;
     case (CALL):
          tran_call();
          break;
     case (RETURN):
          tran_return();
          break;
     case (LOOP):
          tran_loop();
          break;
     case (DO):
          tran_do();
          break;
     case (BEGIN):
          tran_begin();
          break;
     case (ALLOCATE):
          tran_allocate();
          break;
     case (FREE):
          tran_free();
          break;
     case (GOTO):
          tran_goto();
          break;
     case (END):
          set_error_line(node_ptr);
          return(END);
     case (LABEL):
          tran_label();
          break;
     case (ASSIGNMENT):
          tran_assignment();
          break;
     case (PUT):
          tran_put();
          break;
     case (ENTRY):
          tran_entry();
          break; 
     default:
          {
          report (134,"",_LINE_);
          return(0);
          }
     }

     if (ntype == PROCEDURE)
        {
        /******************************************************************/
        /* OK We have processed a PL/1 block, but we must now process all */
        /* child blocks of this block.                                    */
        /******************************************************************/
        cb_ptr = p_ptr->proc->child;

        while (cb_ptr != NULL)
              {
              tran_nodes(cb_ptr->first_stmt);
              cb_ptr = cb_ptr->sister;
              }
        }


     return(0);

     }

/*************************************************************************/
/*           Translate a PL/1 procedure node.                            */
/*************************************************************************/ 

static void tran_procedure (void)

     {

	 Any_ptr		next;
     Procedure_ptr  ptr;
     short          code_size;
	 //long 			I,J,K;
	 Reg			regs[8];
	 Section_ptr	sec_ptr;
	 CoffReloc      reloc;

	 regs[0] = _EAX;
	 regs[1] = _ECX;
	 regs[2] = _EDX;
	 regs[3] = _EBX;
	 regs[4] = _ESP;
	 regs[5] = _EBP;
	 regs[6] = _ESI;
	 regs[7] = _EDI;

     /******************************************************************/
     /* If this block isnt called, then issue a warning and return.    */
     /******************************************************************/

     ptr = curr_node_ptr;

  /*   if (ptr->proc->function == 0)
        if (ptr->proc->called == 0)
           {
           report(-123,ptr->proc->block_name,_LINE_);
           return;
           }         */

     /******************************************************************/
     /* Start a new CODE segment and a new LEDATA header etc for this  */
     /* new PL/1 block.                                                */
     /******************************************************************/

     block_ctr++;

     CpuSetSectPtr (CoffGetSectionPtr(obj_root,ptr->proc->code_idx));   

     set_error_line (curr_node_ptr);

     next = next_node();

     /* Test code for generating arbitrary instructions */
	 /* This is for testing the machine generfator      */
/*
	 Inst.opcode	 =  ADD;
	 Inst.target.reg = _EAX;
	 Inst.source.reg = _EBX;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.reg = _EAX;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EBX;
	 Inst.source.reg = _EAX;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.reg = _EBX;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.bas = _EAX;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EBX;
	 Inst.source.bas = _EBX;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.bas = _EBX;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EBX;
	 Inst.source.bas = _EAX;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.bas = _EAX;
	 Inst.source.dis = 255;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.bas = _EAX;
	 Inst.source.dis = 15;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.bas = _EBX;
	 Inst.source.dis = 255;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.bas = _EBX;
	 Inst.source.idx = _EAX;
	 Inst.source.dis = 255;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.bas = _EBX;
	 Inst.source.idx = _ECX;
	 Inst.source.dis = 255;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.bas = _EAX;
	 Inst.source.idx = _EAX;
	 Inst.source.dis = 255;
	 CpuGenerate();  */


/*
	 Inst.opcode     =  MOV;
	 Inst.target.reg = _EAX;
	 Inst.source.bas = _ECX;
	 Inst.source.idx = _ESP;
	 Inst.source.dis = 255;
	 CpuGenerate();

	 Inst.opcode     =  MOV;
	 Inst.source.reg = _EAX;
	 Inst.target.bas = _ECX;
	 Inst.target.idx = _ESP;
	 Inst.target.dis = 255;
	 CpuGenerate();  */
/*
	 Inst.opcode       =  ADD;
	 Inst.target.reg   = _EBP;
	 Inst.source.idx   = _ESI;  // must be validated in some way, all this sort of stuff
	 Inst.source.scale = 8;
	 Inst.source.dis   = 0x1122;
	 CpuGenerate();

	 Inst.opcode       =  ADD;
	 Inst.target.reg   = _EBP;
     Inst.source.bas   = _EBP;
	 Inst.source.idx   = _ESI;  // must be validated in some way, all this sort of stuff
	 Inst.source.scale = 8;
	 Inst.source.dis   = 0x1122;
	 CpuGenerate();

	 Inst.opcode       =  SUB;
	 Inst.target.reg   = _EBP;
	 Inst.source.idx   = _ESI;  // must be validated in some way, all this sort of stuff
	 Inst.source.scale = 8;
	 Inst.source.dis   = 0x1122;
	 CpuGenerate();

	 Inst.opcode       =  SUB;
	 Inst.target.reg   = _EBP;
     Inst.source.bas   = _EBP;
	 Inst.source.idx   = _ESI;  // must be validated in some way, all this sort of stuff
	 Inst.source.scale = 8;
	 Inst.source.dis   = 0x1122;
	 CpuGenerate();



	 Inst.opcode       =  MOV;
	 Inst.target.reg   = _EBP;
	 Inst.source.idx   = _ESI;  // must be validated in some way, all this sort of stuff
	 Inst.source.scale = 8;
	 Inst.source.dis   = 0x1122;
	 CpuGenerate();

	 Inst.opcode       =  MOV;
	 Inst.target.reg   = _EBP;
     Inst.source.bas   = _EBP;
	 Inst.source.idx   = _ESI;  // must be validated in some way, all this sort of stuff
	 Inst.source.scale = 8;
	 Inst.source.dis   = 0x1122;
	 CpuGenerate();
*/
/*
     for (I=0; I<8; I++)
	     for (J=0; J<8; J++)
		     for (K=0; K<8; K++)
	             {

			     Inst.opcode     = JA;
				 Inst.target.imm = 23;
				 Inst.target.len = BYTE_PTR;
				 CpuGenerate();

			     Inst.opcode     = JA;
				 Inst.target.imm = 23;
				 Inst.target.len = DWORD_PTR;
				 CpuGenerate();

				 Inst.opcode     = FILD;
	             Inst.target.bas = regs[J];
				 if (K != 4)
				    Inst.target.idx = regs[K];
				 else
				    Inst.target.idx = 0;

				 Inst.target.scale = 4;
	             Inst.target.dis = I*J*K*J;
				 Inst.target.len = WORD_PTR;
				 CpuGenerate();

				 Inst.opcode     = FILD;
	             Inst.target.bas = regs[J];
				 if (K != 4)
				    Inst.target.idx = regs[K];
				 else
				    Inst.target.idx = 0;

				 Inst.target.scale = 4;
	             Inst.target.dis = I*J*K*J;
				 Inst.target.len = DWORD_PTR;
				 CpuGenerate();


				 Inst.opcode     = FILD;
	             Inst.target.bas = regs[J];
				 if (K != 4)
				    Inst.target.idx = regs[K];
				 else
				    Inst.target.idx = 0;

				 Inst.target.scale = 4;
	             Inst.target.dis = I*J*K*J;
				 Inst.target.len = QWORD_PTR;
			     CpuGenerate();
                 
  	             Inst.opcode     =  MUL;
	             Inst.source.reg = regs[I];
				 CpuGenerate();

				 Inst.opcode     = MUL;
	             Inst.target.bas = regs[J];
				 if (K != 4)
				    Inst.target.idx = regs[K];
				 else
				    Inst.target.idx = 0;

				 Inst.target.scale = 4;
	             Inst.target.dis = I*J*K*J;
                 
				 CpuGenerate();


  	             Inst.opcode     =  MOV;
	             Inst.source.reg = regs[I];
	             Inst.target.bas = regs[J];
				 if (K != 4)
				    Inst.target.idx = regs[K];
				 else
				    Inst.target.idx = 0;

				 Inst.target.scale = 4;
	             Inst.target.dis = I*J*K*J;
                 
				 CpuGenerate();

 	             Inst.opcode     =  CMP;
	             Inst.source.reg = regs[I];
	             Inst.target.bas = regs[J];
				 if (K != 4)
				    Inst.target.idx = regs[K];
				 else
				    Inst.target.idx = 0;

				 Inst.target.scale = 4;
	             Inst.target.dis = I*J*K*J;

	             CpuGenerate();


  	             Inst.opcode     =  CAL;
	             Inst.source.bas = regs[J];
				 if (K != 4)
				    Inst.source.idx = regs[K];
				 else
				    Inst.source.idx = 0;

				 Inst.source.scale = 4;
	             Inst.source.dis = I*J*K*J;
				                 
				 CpuGenerate();

			     Inst.opcode     = CAL;
				 Inst.target.imm = I*J*K;
				 Inst.target.len = DWORD_PTR;
				 CpuGenerate();

			     Inst.opcode     = CAL;
				 Inst.target.imm = I*J*K;
				 Inst.target.len = DWORD_PTR;
				 CpuGenerate();

  	             Inst.opcode     =  MOV;
	             Inst.target.reg = regs[I];
	             Inst.source.bas = regs[J];
				 if (K != 4)
				    Inst.source.idx = regs[K];
				 else
				    Inst.source.idx = 0;

				 Inst.source.scale = 4;
	             Inst.source.dis = I*J*K*J;
                 
				 CpuGenerate();

 	             Inst.opcode     =  CMP;
	             Inst.target.reg = regs[I];
	             Inst.source.bas = regs[J];
				 if (K != 4)
				    Inst.source.idx = regs[K];
				 else
				    Inst.source.idx = 0;

				 Inst.source.scale = 4;
	             Inst.source.dis = I*J*K*J;

	             CpuGenerate();

  	             Inst.opcode     =  ADD;
	             Inst.target.reg = regs[I];
	             Inst.source.bas = regs[J];
				 if (K != 4)
				    Inst.source.idx = regs[K];
				 else
				    Inst.source.idx = 0;
	             Inst.source.dis = I*J*K*I * (-1 * K%2);
	             CpuGenerate();

  	             Inst.opcode     =  SUB;
	             Inst.target.reg = regs[I];
	             Inst.source.bas = regs[J];
				 if (K != 4)
				    Inst.source.idx = regs[K];
				 else
				    Inst.source.idx = 0;
	             Inst.source.dis = I*J*K*K;
	             CpuGenerate();



  	             Inst.opcode     =  ADC;
	             Inst.target.reg = regs[I];
	             Inst.source.bas = regs[J];
				 if (K != 4)
				    Inst.source.idx = regs[K];
				 else
				    Inst.source.idx = 0;
	             Inst.source.dis = I*J*K*I * (-1 * K%2);
	             CpuGenerate();

  	             Inst.opcode     =  SBB;
	             Inst.target.reg = regs[I];
	             Inst.source.bas = regs[J];
				 if (K != 4)
				    Inst.source.idx = regs[K];
				 else
				    Inst.source.idx = 0;
	             Inst.source.dis = I*J*K*K;
	             CpuGenerate();

				 Inst.opcode     = INC;
				 Inst.target.reg = regs[I];
				 CpuGenerate();

				 Inst.opcode     = INC;
				 Inst.target.bas = regs[I];
				 if (K != 4)
				    Inst.target.idx = regs[K];
				 else
				    Inst.target.idx = 0;
                 CpuGenerate();


				 Inst.opcode	 = DEC;
				 Inst.target.reg = regs[I];
                 CpuGenerate();

				 Inst.opcode     = DEC;
				 Inst.target.bas = regs[I];
				 if (K != 4)
				    Inst.target.idx = regs[K];
				 else
				    Inst.target.idx = 0;
                 CpuGenerate();

				 Inst.opcode     = PUSH;
				 Inst.target.reg = regs[I];
				 CpuGenerate();

				 Inst.opcode     = POP;
				 Inst.target.reg = regs[I];
				 CpuGenerate();

				 Inst.opcode     = PUSH;
				 Inst.target.bas = regs[I];
				 if (K != 4)
				    Inst.target.idx = regs[K];
				 else
				    Inst.target.idx = 0;
                 CpuGenerate();

				 Inst.opcode     = POP;
				 Inst.target.bas = regs[J];
				 if (K != 4)
				    Inst.target.idx = regs[K];
				 else
				    Inst.target.idx = 0;
                 CpuGenerate();
				
                 }
*/
     /********************************************************************/
     /*               Generate the standard PL/1 entry sequence.         */
     /********************************************************************/

     Inst.opcode     = ENTER;
     Inst.target.imm = (short)ptr->proc->stack;
	 Inst.target.len = WORD_PTR;
     Inst.source.imm = 0;
	 Inst.source.len = BYTE_PTR;
     CpuGenerate();

     Inst.opcode     = MOV;
     Inst.source.reg = _ECX;
     Inst.target.bas = _EBP;
     Inst.target.len = DWORD_PTR;
     Inst.target.dis = -((short)(ptr->proc->stack)); /* save CX at offset 0 in frame */
     CpuGenerate();

	 
	 //Inst.target.reg = EDS; /* Load the procs DS seg reg */
     //Inst.source.reg = _ECX;
     //Inst.opcode     = MOV;
     //CpuGenerate();
 
     /*********************************************************************/
     /* Loop and process all statement nodes in this block. The loop will */
     /* terminate when the END for this block has been reached.           */
     /*********************************************************************/

     while (tran_nodes (next) == 0)
           {
           next = next_node();
           }

     /********************************************************************/
     /*      Return to invoking procedure or PLIMAIN (if main proc).     */
     /********************************************************************/

     Inst.opcode     = EXIT;  /* leave on MC68000 */
     CpuGenerate();

     /* caller now pops arg ptrs */

     Inst.opcode     = RET;
     Inst.source.imm = 0;  /* ptr->proc->num_args; */
     CpuGenerate();   

     reloc.RelUn.VirtualAddress = 8;
	 reloc.SymbolTableIndex     = 7;
	 reloc.Type                 = 20; //IMAGE_REL_I386_DIR32;

	 sec_ptr = CoffGetSectionPtr(obj_root,ptr->proc->code_idx);

     //CoffInsertReloc(sec_ptr,&reloc);

     /*******************************************************************/
     /* Flush any code for this block from the code emitters buffers.   */
     /*******************************************************************/

     write_code();

     code_size = get_segment_offset();

     patch_code(ptr->proc->seg_pos,&code_size,2); /* patch seglen etc */

     write_fixup();

     return; /* An END was read so were done with this procedure/block */

     
     }

/***************************************************************************/
/*     This function is called to tran a PL/1 expression tree.             */
/***************************************************************************/

static void tran_expression (Any_ptr node_ptr)

     {

     switch (nodetype(node_ptr)) {

     case (PLUS):
          tran_add (node_ptr);
          break;
     case (MINUS):
          tran_sub (node_ptr);
          break;
     case (TIMES):
          tran_mul (node_ptr);
          break;
     case (DIVIDE):
          tran_div (node_ptr);
          break;     
     case (EQUALS):
     case (NOTEQUAL):
     case (LT):
     case (GT):
     case (GE):
     case (LE):
	 case (NOTGT):
	 case (NOTLT):
	 case (NOTGE):
	 case (NOTLE):
          tran_relation (node_ptr);
          break; 
     case (REFERENCE):
          tran_reference (node_ptr); 
          break;
     default:
          {
          report(134,"",_LINE_);
          }
     }

     }

/*************************************************************************/
/* Analyse and process a PL/1 + operator node, and any subnodes.         */
/*************************************************************************/ 

static void tran_add (Any_ptr g_ptr)

     {

     Oper_ptr   ptr;
     Ref_ptr    rl_ptr;
     Ref_ptr    rr_ptr;
     short        lt,rt;

     ptr = g_ptr;

     lt = nodetype(ptr->left_ptr);
     rt = nodetype(ptr->rite_ptr); 

     /***************************************************************/
     /* If both operands of this plus operator are refs then ...... */
     /***************************************************************/ 

     if ((lt == REFERENCE) &&
         (rt == REFERENCE))
         {
         rl_ptr = (Ref_ptr)ptr->left_ptr;
         rr_ptr = (Ref_ptr)ptr->rite_ptr;
         address_reference(rl_ptr,SOURCE,DWORD_PTR);
         Inst.opcode     = MOV;
         Inst.target.reg = _EAX;
         CpuGenerate();

         /***************************************************************/
         /* If we are adding a constant of '0' then emit no instruction */
         /***************************************************************/

         if (rr_ptr->attribs->class == CONSTANT)
            if (rr_ptr->attribs->type == NUMERIC)
               if (strcmp(rr_ptr->spelling,"0") == 0)
                  return;

         /***************************************************************/
         /* If we are adding a constant of '1' then use inc instruction */
         /***************************************************************/

         if (rr_ptr->attribs->class == CONSTANT)
            if (rr_ptr->attribs->type == NUMERIC)
               if (strcmp(rr_ptr->spelling,"1") == 0)
                  {
                  Inst.opcode     = INC;
                  Inst.target.reg = _EAX;
                  CpuGenerate();
                  return;
                  }

         address_reference(rr_ptr,SOURCE,DWORD_PTR);
         Inst.opcode     = ADD;
         Inst.target.reg = _EAX;
         CpuGenerate();
         return;
         }

     /*****************************************************************/
     /* If the left operand is a ref and the right is an expression...*/
     /*****************************************************************/ 

     if ((lt == REFERENCE) &&
         (rt != REFERENCE))
        {
        rl_ptr = (Ref_ptr)ptr->left_ptr;
        tran_expression (ptr->rite_ptr);
        Inst.opcode     = PUSH;
        Inst.source.reg = _EAX;
        CpuGenerate();
        address_reference(rl_ptr,SOURCE,DWORD_PTR);
        Inst.opcode     = MOV;
        Inst.target.reg = _EAX;
        CpuGenerate();  
        Inst.opcode     = POP;
        Inst.target.reg = _EBX;
        CpuGenerate();
        Inst.opcode     = ADD;
        Inst.target.reg = _EAX;
        Inst.source.reg = _EBX;
        CpuGenerate();
        return;
        }

     /*****************************************************************/
     /* If the left operand is an expression and the rite is a ref... */
     /*****************************************************************/

     if ((lt != REFERENCE) &&
         (rt == REFERENCE))
        {
        rr_ptr = (Ref_ptr)ptr->rite_ptr;
        tran_expression (ptr->left_ptr);

        /***************************************************************/
        /* If we are adding a constant of '0' then emit no instruction */
        /***************************************************************/

        if (rr_ptr->attribs->class == CONSTANT)
           if (rr_ptr->attribs->type == NUMERIC)
              if (strcmp(rr_ptr->spelling,"0") == 0)
                 return;

        /***************************************************************/
        /* If we are adding a constant of '1' then use inc instruction */
        /***************************************************************/

        if (rr_ptr->attribs->class == CONSTANT)
           if (rr_ptr->attribs->type == NUMERIC)
              if (strcmp(rr_ptr->spelling,"1") == 0)
                 {
                 Inst.opcode     = INC;
                 Inst.target.reg = _EAX;
                 CpuGenerate();
                 return;
                 }

        address_reference(rr_ptr,SOURCE,DWORD_PTR);
        Inst.opcode     = ADD;
        Inst.target.reg = _EAX;
        CpuGenerate(); 
        return;
        }

     /*****************************************************************/
     /* If both sides of the operator are expressions then . . . . .  */
     /*****************************************************************/

     if ((lt != REFERENCE) &&
         (rt != REFERENCE))
        {
        tran_expression (ptr->left_ptr);
        Inst.opcode     = PUSH;
        Inst.source.reg = _EAX;
        CpuGenerate(); 
        tran_expression (ptr->rite_ptr);
        Inst.opcode     = POP;
        Inst.target.reg = _EBX;
        CpuGenerate(); 
        Inst.opcode     = ADD;
        Inst.target.reg = _EAX;
        Inst.source.reg = _EBX;
        CpuGenerate();
        return;
        } 

     }

/*************************************************************************/
/* Analyse and process a PL/1 * operator node, and any subnodes.         */
/* We first see if NDP is required and if it is we create NDP code else  */
/* we create 'normal' code.                                              */
/*************************************************************************/ 

static void tran_mul (Any_ptr g_ptr)

     {

     Oper_ptr   ptr;
     Ref_ptr    rl_ptr;
     Ref_ptr    rr_ptr;
     short        lt,rt;

     ptr = g_ptr;

     lt = nodetype(ptr->left_ptr);
     rt = nodetype(ptr->rite_ptr); 

     if (ndp_reqd)
        /**************************************************************/
        /* User wants 80287 NDP instructions so get on with it....    */
        /**************************************************************/ 
        {
        if ((lt == REFERENCE) &&
            (rt == REFERENCE))
            {
            rl_ptr = (Ref_ptr)ptr->left_ptr; 
            rr_ptr = (Ref_ptr)ptr->rite_ptr;
            address_reference(rl_ptr,SOURCE,DWORD_PTR);
            Inst.opcode     = FILD;
            Inst.target.reg = NDP;
            CpuGenerate();
            if (rr_ptr->attribs->class != CONSTANT)
               {
               address_reference(rr_ptr,SOURCE,DWORD_PTR);
               Inst.opcode     = FIMUL;
               Inst.target.reg = NDP;
               CpuGenerate();
               }
             else  /* ndp code on immediates nyi */
               {
               address_reference(rr_ptr,SOURCE,DWORD_PTR);
               Inst.opcode     = MOV;
               Inst.target.reg = _EBX;
               CpuGenerate();
               Inst.opcode     = MUL;
               Inst.source.reg = _EBX;
               CpuGenerate();
               }
            result_in_NDP = 1;
            return;
            }
   
        if ((lt == REFERENCE) &&
            (rt != REFERENCE))
           {
           rl_ptr = (Ref_ptr)ptr->left_ptr;
           tran_expression (ptr->rite_ptr);
           Inst.opcode     = PUSH;
           Inst.source.reg = _EAX;
           CpuGenerate();
           address_reference(rl_ptr,SOURCE,DWORD_PTR);
           Inst.opcode     = MOV;
           Inst.target.reg = _EAX;
           CpuGenerate();  
           Inst.opcode     = POP;
           Inst.target.reg = _EBX;
           CpuGenerate();
           Inst.opcode     = MUL;
           Inst.source.reg = _EBX;
           CpuGenerate();
           return;
           }
   
        if ((lt != REFERENCE) &&
            (rt == REFERENCE))
           {
           rr_ptr = (Ref_ptr)ptr->rite_ptr;
           tran_expression (ptr->left_ptr);
           /* CpuGenerate(); */
           if (rr_ptr->attribs->class != CONSTANT)
              {
              address_reference(rr_ptr,SOURCE,DWORD_PTR);
              Inst.opcode     = MUL;
              CpuGenerate();
              }
           else
              {
              address_reference(rr_ptr,SOURCE,DWORD_PTR);
              Inst.opcode     = MOV;
              Inst.target.reg = _EBX;
              CpuGenerate();
              Inst.opcode     = MUL;
              Inst.source.reg = _EBX;
              CpuGenerate();
              }
           return;
           }
   
        if ((lt != REFERENCE) &&
            (rt != REFERENCE))
           {
           tran_expression (ptr->left_ptr);
           Inst.opcode     = PUSH;
           Inst.source.reg = _EAX;
           CpuGenerate(); 
           tran_expression (ptr->rite_ptr);
           Inst.opcode     = POP;
           Inst.target.reg = _EBX;
           CpuGenerate(); 
           Inst.opcode     = MUL;
           Inst.source.reg = _EBX;
           CpuGenerate();
           return;
           } 
    
        }
     else
        /***************************************************************/
        /*     We are not using an NDP so produce normal 80286 code.   */
        /***************************************************************/ 
        {
        if ((lt == REFERENCE) &&
            (rt == REFERENCE))
           {
           rl_ptr = (Ref_ptr)ptr->left_ptr; 
           rr_ptr = (Ref_ptr)ptr->rite_ptr;
           address_reference(rl_ptr,SOURCE,DWORD_PTR);
           Inst.opcode     = MOV;
           Inst.target.reg = _EAX;
           CpuGenerate();
           if (rr_ptr->attribs->class != CONSTANT)
              {
              address_reference(rr_ptr,SOURCE,DWORD_PTR);
              Inst.opcode     = MUL;
              CpuGenerate();
              }
            else
              {
              address_reference(rr_ptr,SOURCE,DWORD_PTR);
              Inst.opcode     = MOV;
              Inst.target.reg = _EBX;
              CpuGenerate();
              Inst.opcode     = MUL;
              Inst.source.reg = _EBX;
              CpuGenerate();
              }
           return;
           }
  
        if ((lt == REFERENCE) &&
            (rt != REFERENCE))
           {
           rl_ptr = (Ref_ptr)ptr->left_ptr;
           tran_expression (ptr->rite_ptr);
           Inst.opcode     = PUSH;
           Inst.source.reg = _EAX;
           CpuGenerate();
           address_reference(rl_ptr,SOURCE,DWORD_PTR);
           Inst.opcode     = MOV;
           Inst.target.reg = _EAX;
           CpuGenerate();  
           Inst.opcode     = POP;
           Inst.target.reg = _EBX;
           CpuGenerate();
           Inst.opcode     = MUL;
           Inst.source.reg = _EBX;
           CpuGenerate();
           return;
           }
   
        if ((lt != REFERENCE) &&
            (rt == REFERENCE))
           {
           rr_ptr = (Ref_ptr)ptr->rite_ptr;
           tran_expression (ptr->left_ptr);
           /* CpuGenerate(); */
           if (rr_ptr->attribs->class != CONSTANT)
              {
              address_reference(rr_ptr,SOURCE,DWORD_PTR);
              Inst.opcode     = MUL;
              CpuGenerate();
              }
           else
              {
              address_reference(rr_ptr,SOURCE,DWORD_PTR);
              Inst.opcode     = MOV;
              Inst.target.reg = _EBX;
              CpuGenerate();
              Inst.opcode     = MUL;
              Inst.source.reg = _EBX;
              CpuGenerate();
              }
           return;
           }
   
        if ((lt != REFERENCE) &&
            (rt != REFERENCE))
           {
           tran_expression (ptr->left_ptr);
           Inst.opcode     = PUSH;
           Inst.source.reg = _EAX;
           CpuGenerate(); 
           tran_expression (ptr->rite_ptr);
           Inst.opcode     = POP;
           Inst.target.reg = _EBX;
           CpuGenerate(); 
           Inst.opcode     = MUL;
           Inst.source.reg = _EBX;
           CpuGenerate();
           return;
           } 
    
        } /* else */

     }


/*************************************************************************/
/* Analyse and process a PL/1 - operator node, and any subnodes.         */
/*************************************************************************/ 

static void tran_sub (Any_ptr g_ptr)

   {

   Oper_ptr   ptr;
   Ref_ptr    rl_ptr;
   Ref_ptr    rr_ptr;
   short        lt,rt;

   ptr = g_ptr;

   lt = nodetype(ptr->left_ptr);
   rt = nodetype(ptr->rite_ptr); 

   if ((lt == REFERENCE) &&
       (rt == REFERENCE))
       {
       rl_ptr = (Ref_ptr)ptr->left_ptr;
       rr_ptr = (Ref_ptr)ptr->rite_ptr;
       address_reference(rl_ptr,SOURCE,DWORD_PTR);
       Inst.opcode     = MOV;
       Inst.target.reg = _EAX;
       CpuGenerate();

       /***************************************************************/
       /* If we are adding a constant of '0' then emit no instruction */
       /***************************************************************/

       if (rr_ptr->attribs->class == CONSTANT)
          if (rr_ptr->attribs->type == NUMERIC)
             if (strcmp(rr_ptr->spelling,"0") == 0)
                return;

       /********************************************************************/
       /* If we are subtracting a constant of '1' then use dec instruction */
       /********************************************************************/

       if (rr_ptr->attribs->class == CONSTANT)
          if (rr_ptr->attribs->type == NUMERIC)
             if (strcmp(rr_ptr->spelling,"1") == 0)
                {
                Inst.opcode     = DEC;
                Inst.target.reg = _EAX;
                CpuGenerate();
                return;
                }

       address_reference(rr_ptr,SOURCE,DWORD_PTR);
       Inst.opcode     = SUB;
       Inst.target.reg = _EAX;
       CpuGenerate();  
       return;
       }

   if ((lt == REFERENCE) &&
       (rt != REFERENCE))
      {
      rl_ptr = (Ref_ptr)ptr->left_ptr;
      tran_expression (ptr->rite_ptr);
      Inst.opcode      = PUSH;
      Inst.source.reg  = _EAX;
      CpuGenerate();
      address_reference(rl_ptr,TARGET,DWORD_PTR);
      Inst.opcode      = MOV;
      Inst.source.reg  = _EAX;
      CpuGenerate();
      Inst.opcode      = POP;
      Inst.target.reg  = _EBX;
      CpuGenerate();
      Inst.opcode      = SUB;
      Inst.target.reg  = _EAX;
      Inst.source.reg  = _EBX;
      CpuGenerate(); 
      return;
      }

   if ((lt != REFERENCE) &&
       (rt == REFERENCE))
      {
      rr_ptr = (Ref_ptr)ptr->rite_ptr;
      tran_expression (ptr->left_ptr);
      address_reference(rr_ptr,SOURCE,DWORD_PTR);
      Inst.opcode      = SUB;
      Inst.target.reg  = _EAX;
      CpuGenerate();
      return;
      }

   if ((lt != REFERENCE) &&
       (rt != REFERENCE))
      {
      tran_expression (ptr->rite_ptr);
      Inst.opcode      = PUSH;
      Inst.source.reg  = _EAX;
      CpuGenerate();  
      tran_expression (ptr->left_ptr);
      Inst.opcode      = POP;
      Inst.target.reg  = _EBX;
      CpuGenerate(); 

      Inst.opcode      = SUB;
      Inst.target.reg  = _EAX;
      Inst.source.reg  = _EBX; 
      CpuGenerate();
      return;
      } 
   

   }

/*************************************************************************/
/* Analyse and process a PL/1 / operator node, and any subnodes.         */
/*************************************************************************/ 

static void tran_div (Any_ptr g_ptr)

     {

     Oper_ptr   ptr;
     Ref_ptr    rl_ptr;
     Ref_ptr    rr_ptr;
     short        lt,rt;

     ptr = g_ptr;

     lt = nodetype(ptr->left_ptr);
     rt = nodetype(ptr->rite_ptr); 

     /***************************************************************/
     /* In the case of 16 bit divide, the result of _DX-_EAX/value  */
     /* can be too large to fit in _EAX, an will gen div-by-zero.   */
     /* _DX must be zero anyway for bin(15).                        */
     /***************************************************************/  

     Inst.opcode     = MOV;
     Inst.target.reg = _DX;
     Inst.source.imm = 0; 
	 Inst.source.len = WORD_PTR;
     CpuGenerate();

     if ((lt == REFERENCE) &&
         (rt == REFERENCE))
        {
        rl_ptr = (Ref_ptr)ptr->left_ptr; 
        rr_ptr = (Ref_ptr)ptr->rite_ptr;
        address_reference(rl_ptr,SOURCE,DWORD_PTR);
        Inst.opcode     = MOV;
        Inst.target.reg = _EAX;
        CpuGenerate();
        if (rr_ptr->attribs->class != CONSTANT)
           {
           address_reference(rr_ptr,SOURCE,DWORD_PTR);
           Inst.opcode     = DIV;
           CpuGenerate();
           }
         else
           {
           address_reference(rr_ptr,SOURCE,DWORD_PTR);
           Inst.opcode     = MOV;
           Inst.target.reg = _EBX;
           CpuGenerate();
           Inst.opcode     = DIV;
           Inst.source.reg = _EBX;
           CpuGenerate();
           }
        return;
        }
  
     if ((lt == REFERENCE) &&
         (rt != REFERENCE))
        {
        rl_ptr = (Ref_ptr)ptr->left_ptr;
        tran_expression (ptr->rite_ptr);
        Inst.opcode     = PUSH;
        Inst.source.reg = _EAX;
        CpuGenerate();
        address_reference(rl_ptr,SOURCE,DWORD_PTR);
        Inst.opcode     = MOV;
        Inst.target.reg = _EAX;
        CpuGenerate();  
        Inst.opcode     = POP;
        Inst.target.reg = _EBX;
        CpuGenerate();
        Inst.opcode     = DIV;
        Inst.source.reg = _EBX;
        CpuGenerate();
        return;
        }

     if ((lt != REFERENCE) &&
         (rt == REFERENCE))
        {
        rr_ptr = (Ref_ptr)ptr->rite_ptr;
        tran_expression (ptr->left_ptr);
        /* CpuGenerate(); */
        if (rr_ptr->attribs->class != CONSTANT)
           {
           address_reference(rr_ptr,SOURCE,DWORD_PTR);
           Inst.opcode     = DIV;
           CpuGenerate();
           }
        else
           {
           address_reference(rr_ptr,SOURCE,DWORD_PTR);
           Inst.opcode     = MOV;
           Inst.target.reg = _EBX;
           CpuGenerate();
           Inst.opcode     = DIV;
           Inst.source.reg = _EBX;
           CpuGenerate();
           }
        return;
        }

     if ((lt != REFERENCE) &&
         (rt != REFERENCE))
        {
        tran_expression (ptr->left_ptr);
        Inst.opcode     = PUSH;
        Inst.source.reg = _EAX;
        CpuGenerate(); 
        tran_expression (ptr->rite_ptr);
        Inst.opcode     = POP;
        Inst.target.reg = _EBX;
        CpuGenerate(); 
        Inst.opcode     = DIV;
        Inst.source.reg = _EBX;
        CpuGenerate();
        return;
        } 
 
     }

/*************************************************************************/
/* Analyse and process a relation operator node, and any subnodes.       */
/*************************************************************************/ 

static void tran_relation (Any_ptr g_ptr)

     {

     Oper_ptr   ptr;
     Ref_ptr    rl_ptr;
     Ref_ptr    rr_ptr;
     short        lt,rt;

     ptr = g_ptr;

     lt = nodetype(ptr->left_ptr);
     rt = nodetype(ptr->rite_ptr); 


     /***************************************************************/
     /* If both operands of this = operator are refs then ......    */
     /***************************************************************/ 

     if ((lt == REFERENCE) &&
         (rt == REFERENCE))
         {
         rl_ptr = (Ref_ptr)ptr->left_ptr;
         rr_ptr = (Ref_ptr)ptr->rite_ptr;
         address_reference(rl_ptr,SOURCE,DWORD_PTR);
         Inst.opcode     = MOV;
         Inst.target.reg = _EAX;
         CpuGenerate();

         address_reference(rr_ptr,SOURCE,DWORD_PTR);
         Inst.opcode     = CMP;
         Inst.target.reg = _EAX;
         CpuGenerate();
         return;
         }

     /*****************************************************************/
     /* If the left operand is a ref and the right is an expression...*/
     /*****************************************************************/ 

     if ((lt == REFERENCE) &&
         (rt != REFERENCE))
        {
        rl_ptr = (Ref_ptr)ptr->left_ptr;
        tran_expression (ptr->rite_ptr);
        Inst.opcode     = PUSH;
        Inst.source.reg = _EAX;
        CpuGenerate();
        address_reference(rl_ptr,SOURCE,DWORD_PTR);
        Inst.opcode     = MOV;
        Inst.target.reg = _EAX;
        CpuGenerate();  
        Inst.opcode     = POP;
        Inst.target.reg = _EBX;
        CpuGenerate();
        Inst.opcode     = CMP;
        Inst.target.reg = _EAX;
        Inst.source.reg = _EBX;
        CpuGenerate();
        return;
        }

     /*****************************************************************/
     /* If the left operand is an expression and the rite is a ref... */
     /*****************************************************************/

     if ((lt != REFERENCE) &&
         (rt == REFERENCE))
        {
        rr_ptr = (Ref_ptr)ptr->rite_ptr;
        tran_expression (ptr->left_ptr);

        address_reference(rr_ptr,SOURCE,DWORD_PTR);
        Inst.opcode     = CMP;
        Inst.target.reg = _EAX;
        CpuGenerate(); 
        return;
        }

     /*****************************************************************/
     /* If both sides of the operator are expressions then . . . . .  */
     /*****************************************************************/

     if ((lt != REFERENCE) &&
         (rt != REFERENCE))
        {
        tran_expression (ptr->left_ptr);
        Inst.opcode     = PUSH;
        Inst.source.reg = _EAX;
        CpuGenerate(); 
        tran_expression (ptr->rite_ptr);
        Inst.opcode     = POP;
        Inst.target.reg = _EBX;
        CpuGenerate(); 
        Inst.opcode     = CMP;
        Inst.target.reg = _EAX;
        Inst.source.reg = _EBX;
        CpuGenerate();
        return;
        } 

     }

/*************************************************************************/
/*               Translate an IF type node.                              */
/*************************************************************************/ 

static void tran_if (void)

     {

     If_ptr       ptr;
     Any_ptr      next;
     long         target_offset_t;  /* then part */
     short        jump_disp_t;
     short        jump_posn_t;
     short        targ_posn_t;
     char         else_present;
     long         target_offset_e;  /* else part */
     short        jump_disp_e;
     short        jump_posn_e;
     short        targ_posn_e;

     set_error_line(curr_node_ptr);

     ptr = curr_node_ptr;

     next = curr_node_ptr;

     if (ptr->else_ptr != ptr->stmt_ptr)
        else_present = 1;
     else
        else_present = 0;

     tran_expression (ptr->expression);

     if (nodetype(ptr->expression) == EQUALS) 
        {
        Inst.opcode     = JE;
        Inst.source.imm = 3;  /* to be fixed up */
        CpuGenerate();
        }

     if (nodetype(ptr->expression) == NOTEQUAL) 
        {
        Inst.opcode     = JNE;
        Inst.source.imm = 3;  /* to be fixed up */
        CpuGenerate();
        }

     if (nodetype(ptr->expression) == GT) 
        {
        Inst.opcode     = JG;
        Inst.source.imm = 3;  /* to be fixed up */
        CpuGenerate();
        }

     if (nodetype(ptr->expression) == NOTGT) 
        {
        Inst.opcode     = JNG;
        Inst.source.imm = 3;  /* to be fixed up */
        CpuGenerate();
        }

     if (nodetype(ptr->expression) == LT) 
        {
        Inst.opcode     = JL;
        Inst.source.imm = 3;  /* to be fixed up */
        CpuGenerate();
        }

     if (nodetype(ptr->expression) == NOTLT) 
        {
        Inst.opcode     = JNL;
        Inst.source.imm = 3;  /* to be fixed up */
        CpuGenerate();
        }

     if (nodetype(ptr->expression) == GE) 
        {
        Inst.opcode     = JGE;
        Inst.source.imm = 3;  /* to be fixed up */
        CpuGenerate();
        }

     if (nodetype(ptr->expression) == NOTGE) 
        {
        Inst.opcode     = JNGE;
        Inst.source.imm = 3;  /* to be fixed up */
        CpuGenerate();
        }

     if (nodetype(ptr->expression) == LE) 
        {
        Inst.opcode     = JLE;
        Inst.source.imm = 3;  /* to be fixed up */
        CpuGenerate();
        }

     if (nodetype(ptr->expression) == NOTLE) 
        {
        Inst.opcode     = JNLE;
        Inst.source.imm = 3;  /* to be fixed up */
        CpuGenerate();
        }


     //target_offset_t = get_file_offset() + 1; /* offset into .obj file */

     Inst.opcode     = JMP;
     Inst.source.imm = 0;  /* to be fixed up */
     CpuGenerate();

     jump_posn_t     = get_segment_offset();
 
     /*******************************************************************/
     /*             Translate the THEN clause of the statement.         */
     /*******************************************************************/
   
     curr_node_ptr = ptr->then_ptr;

     tran_nodes (curr_node_ptr);

     if (else_present)
        {
        target_offset_e = get_file_offset() + 1; /* offset into .obj file */
        Inst.opcode     = JMP;
        Inst.source.imm = 0;  /* to be fixed up */
        CpuGenerate();
        jump_posn_e     = get_segment_offset();
        }

     targ_posn_t = get_segment_offset();

     jump_disp_t = targ_posn_t - jump_posn_t;

     //patch_code (target_offset_t,&jump_disp_t,2);

     if (else_present)
        {
        curr_node_ptr = ptr->else_ptr;
        tran_nodes (curr_node_ptr);
        targ_posn_e = get_segment_offset();
        jump_disp_e = targ_posn_e - jump_posn_e;
        //patch_code (target_offset_e,&jump_disp_e,2);
        }

     curr_node_ptr = next;

     return;
 
     }


/*************************************************************************/
/*               Translate a STOP node.                                  */
/*************************************************************************/ 

static void tran_stop (void)

     {

     set_error_line (curr_node_ptr);

     /*emit_old(OB,"    MOV   _AH,4Ch\n");
     emit_old(OB,"    INT   21h\n");     */

     Inst.opcode     = MOV;
     Inst.target.reg = _AH;
     Inst.source.imm = 76;
     CpuGenerate();

     Inst.opcode     = INT;
     Inst.source.imm = 33;
     CpuGenerate();
 
     return;

     }


/*************************************************************************/
/*               Translate a PL/1 CALL statement.                        */
/*************************************************************************/ 

static void tran_call (void)

     {

     Call_ptr     c_ptr;
     Symbol_ptr   s_ptr;
     Sub_ptr      a_ptr;
     Ref_ptr      r_ptr; 
     short          num_args;
     short          I;

     set_error_line (curr_node_ptr);

     c_ptr    = curr_node_ptr;
     s_ptr    = c_ptr->entry->attribs;
     num_args = c_ptr->entry->num_subs;
     a_ptr    = c_ptr->entry->sublist;

  
     //strcpy(Inst.text,"/* save static segment */");
     //Inst.opcode     = PUSH; /* Save our static segment ptr */
     //Inst.source.reg = _ESI;
     //CpuGenerate();

     /***************************************************************/
     /* Push far pointers to any arguments here. We must push the   */
     /* arg ptrs in reverse order for C compatability, so start off */
     /* last element on the list.                                   */
     /***************************************************************/

	 if (num_args > 0)
	    { 
        while (a_ptr->next_ptr != NULL) /* locate last element on list */
              {
              a_ptr = a_ptr->next_ptr;
              }
		} /* HWG01 */

     /***************************************************************/
     /* Now process each arg, and move on to the previous arg next. */
     /***************************************************************/

     for (I=1; I <= num_args; I++)
         {
         if (a_ptr->pass_by == REFERENCE)
            {
            r_ptr = a_ptr->expression;
            address_reference(r_ptr,SOURCE,DWORD_PTR);
            Inst.opcode     = LEA;
            Inst.target.reg = _EAX;
            CpuGenerate();

			/* This is not reqd. We dont use segment regs any more and */
			/* dont need to pass this as part of args addr.            */

            //if (r_ptr->symbol->class == AUTOMATIC)
              // {
              // Inst.opcode     = PUSH;
              // Inst.source.reg = SS;
              // CpuGenerate();
              // }
            //if ((r_ptr->symbol->class == STATIC) ||
              //  (r_ptr->symbol->class == CONSTANT))
              // {
              // Inst.opcode     = PUSH;
               /*******************************************************/
               /* If the parameter being passed is in the callers     */
               /* static then push DS, but if it is in an outer block */
               /* we must push ES (ES will already be set because of  */
               /* the call to 'address_reference' above.                 */
               /*******************************************************/
               //if (r_ptr->attribs->declarator == curr_parent_block) 
               //   Inst.source.reg = _ESI;
               //else
               //   Inst.source.reg = ES;  
               //CpuGenerate();
              // }
            Inst.opcode     = PUSH;
            Inst.source.reg = _EAX;
            CpuGenerate();
            }

         a_ptr = a_ptr->prev_ptr; /* get previous element */

         }

     /****************************************************************/
     /*            Emit the standard PL/1 calling sequence.          */
     /****************************************************************/

     /*-----------------------------*/
     /* Save our own static pointer */
	 /*-----------------------------*/

	 Inst.opcode     = PUSH;
	 Inst.target.reg = _ESI;
	 CpuGenerate();

     /* Load ECX with code address of callee */

	 Inst.opcode     = MOV;
     Inst.source.bas = (chur)get_static_base(s_ptr);
     Inst.source.dis = (short)s_ptr->offset + CODE_PTR_OFFSET; 
     Inst.source.len = DWORD_PTR; 
     Inst.target.reg = _ECX;      
     CpuGenerate(); 

     /* Load ESI with static address of callee */

	 Inst.opcode     = MOV;
     Inst.source.bas = (chur)get_static_base(s_ptr);
     Inst.source.dis = (short)s_ptr->offset + DATA_PTR_OFFSET; 
     Inst.source.len = DWORD_PTR; 
     Inst.target.reg = _ESI;      
     CpuGenerate(); 

     /***************************************************************/
     /*           OK Now call the specified entry point.            */
     /***************************************************************/

     Inst.target.reg = _ECX;
     Inst.target.len = DWORD_PTR;
     Inst.opcode     = CAL;
     strncpy(Inst.text,s_ptr->spelling,32);
     Inst.text[32] = 0x0;
     CpuGenerate();

     /***************************************************************/
     /* OK now emit the code to restore the callers static segment  */
     /***************************************************************/

     strcpy(Inst.text,"/* restore callers static pointer */");
     Inst.opcode     = POP;  
     Inst.target.reg = _ESI;
     CpuGenerate();

     /***************************************************************/
     /*        OK Pop the pushed arg pointers, immediately !        */
     /***************************************************************/

     if (num_args > 0)
        { 
        Inst.opcode     = ADD;
        Inst.target.reg = _ESP;
        Inst.source.imm = (num_args * 4);
        Inst.source.len = DWORD_PTR;
        CpuGenerate();
        } 

     return;

     }

/*************************************************************************/
/*               Translate a PL/1 RETURN node                            */
/*************************************************************************/ 

static void tran_return (void)

     {

     Return_ptr ptr;

     ptr = curr_node_ptr;

     if (ptr->value != NULL)
        tran_expression (ptr->value);

/*     emit_old(OB,"    LEAVE\n");
     emit_old(OB,"    RET\n");  */

     return;

     }
/*************************************************************************/
/*               Translate a PL/1 PUT node                               */
/*************************************************************************/ 

static void tran_put (void)

     {

     Put_ptr ptr;

/* Phased out completely, when PUT/GET were reimplemented from scratch */

     ptr = curr_node_ptr;
/*
     if (ptr->skip != NULL)
        tran_expression (ptr->skip);

     if (ptr->list != NULL)
        tran_expression (ptr->list);
*/
     /***************************************************************/
     /* At this point we will have a partially built instruction    */
     /* the source part of which holds the address of our string.   */
     /***************************************************************/

     return;

     }

/*************************************************************************/
/*                   Translate a PL/1 ENTRY node                         */
/*************************************************************************/ 

static void tran_entry (void)

     {

    
/*     ptr = curr_node_ptr;  */

     return;

     }


/***************************************************************************/
/* Translate a PL/1 iterative loop statement.                              */ 
/***************************************************************************/ 

static void tran_loop (void)

     {

     Loop_ptr      ptr;
     Any_ptr       next;
     short         saved_offset;
     short         current_offset;
     long          target_offset;
     short         jump_disp;  
     short         jump_posn;
     short         targ_posn;

     set_error_line(curr_node_ptr);

     ptr = curr_node_ptr;

     next = next_node();

     if ((ptr->counter->attribs->class != STATIC) &&
         (ptr->counter->attribs->class != AUTOMATIC) &&
         (ptr->counter->attribs->class != PARAMETER)) 
        {
        report (90,ptr->line_no,_LINE_);
        retreat();
        } 
 
      if (ptr->counter->attribs->type != BINARY)
        {
        report (90,ptr->line_no,_LINE_);     /* NYI */
        retreat();
        }

     /* If its an array, each element must be bin(15) */
  
     if (ptr->counter->attribs->array_ptr == NULL)
        if (ptr->counter->attribs->bytes != 2)
           {
           report (90,ptr->line_no,_LINE_);     /* NYI */
           retreat();
           }
     else
        if (ptr->counter->attribs->size != 2)
           {
           report (90,ptr->line_no,_LINE_);     /* NYI */
           retreat();
           }
   
   
         
     /****************************************/
     /* Load start expressions value into _EAX */
     /****************************************/ 

     tran_expression (ptr->start); /* _EAX = start expr */

     Inst.opcode     = MOV;
     Inst.target.reg = _ECX;
     Inst.source.reg = _EAX;
     CpuGenerate(); 

     /***************************************/
     /* Load finish expression into _EAX      */
     /***************************************/ 

     tran_expression (ptr->finish); /* _EAX = finish expr */

     Inst.opcode     = PUSH;
     Inst.source.reg = _EAX;
     CpuGenerate();

     address_reference(ptr->counter,TARGET,DWORD_PTR);
     Inst.source.reg = _ECX;
     Inst.opcode     = MOV;
     CpuGenerate(); 

     saved_offset = get_segment_offset(); /* this is a jump target */

     /* move counter value into _EAX */

     address_reference(ptr->counter,SOURCE,DWORD_PTR);
     Inst.target.reg = _EAX; 
     Inst.opcode     = MOV;
     CpuGenerate();
 
     Inst.opcode     = POP;
     Inst.target.reg = _EBX;
     CpuGenerate();

     Inst.opcode     = PUSH;
     Inst.source.reg = _EBX;
     CpuGenerate();

     Inst.opcode     = CMP;
     Inst.target.reg = _EAX;
     Inst.source.reg = _EBX;
     CpuGenerate();

     Inst.opcode     = JLE;   /* Jump over the following 16 bit uncond   */
     Inst.source.imm = 3;     /* jump instruction.                       */
     CpuGenerate();

     target_offset = get_file_offset() + 1; /* offset into .obj file */

     Inst.opcode     = JMP;
     Inst.source.imm = 0;  /* to be fixed up */
     CpuGenerate();

     jump_posn     = get_segment_offset();
 
     /*******************************************************************/
     /*      Now tran all code inside this iterative loop.              */
     /*******************************************************************/

     while (tran_nodes(next) == 0)
           {
           next = next_node();
           }
 
     /* OK An END was read, so thats the END of THIS LOOP ! */

     if (ptr->step == NULL)
        { 
        address_reference(ptr->counter,TARGET,DWORD_PTR);
        Inst.source.imm = 1;  
		Inst.source.len = DWORD_PTR;
        Inst.opcode     = ADD;
        CpuGenerate();
        }
     else
        {
        tran_expression (ptr->step);
        address_reference(ptr->counter,TARGET,DWORD_PTR);
        Inst.source.reg = _EAX;
        Inst.opcode     = ADD;
        CpuGenerate();
        }   

     current_offset = get_segment_offset();
     strcpy (Inst.text,"/* End of loop */");
     Inst.opcode     = JMP;
     Inst.source.imm = (saved_offset - current_offset) - 1;
     CpuGenerate();

     targ_posn = get_segment_offset();

     /******************************************************************/
     /* generate a fixup to patch the unconditional jump at loop start */
     /******************************************************************/
/*
     Fixup.M              = 0;
     Fixup.S              = 0;
     Fixup.loc            = LOCN_TYPE_1;
     Fixup.offset         = target.posn + 1; 
     Fixup.fix_frame      = FRAME_METHOD_0;
     Fixup.fix_target     = TARGT_METHOD_0; 
     Fixup.frame          = curr_parent_block->code_idx;
     Fixup.target         = curr_parent_block->code_idx;
     Fixup.disp           = frame_posn ; 
     Fixup.disp_present   = 1;
     Fixup.frame_present  = 1;
     Fixup.target.present = 1; 
     gen_fixup();   */
  
     Inst.opcode     = POP;
     Inst.target.reg = _EBX;
     CpuGenerate();

     jump_disp = targ_posn - jump_posn;

     patch_code (target_offset,&jump_disp,2);

     }

/*************************************************************************/
/*               Translate a DO statement                                */
/*************************************************************************/ 

static void tran_do (void)

     {

     Any_ptr  next;

     set_error_line(curr_node_ptr);

     next = next_node();

     while (tran_nodes(next) == 0)
           {
           next = next_node();
           }
     
     /* An end must have been read, so its the end of this loop */

     return;

     }

/*************************************************************************/
/*               Translate a BEGIN statement.                            */
/*************************************************************************/ 

static void tran_begin (void)

     {

     Any_ptr     next;

     next = next_node();

     while (tran_nodes(next) == 0)
           {
           next = next_node();
           }
     
     /* An end must have been read, so its the end of this loop */

     return;

     }



/*************************************************************************/
/*               Translate a PL/1 ALLOCATE statement                     */
/*************************************************************************/ 

static void tran_allocate (void)

     {

   
/*     ptr = curr_node_ptr;  */

     return;
     

     }


/*************************************************************************/
/*               Translate a PL/1 FREE statement.                        */
/*************************************************************************/ 

static void tran_free (void)

     {

   
/*     ptr = curr_node_ptr; */
    
     return;
 

     }


/*************************************************************************/
/*               Translate a PL/1 GOTO statement.                        */
/*************************************************************************/ 

static void tran_goto (void)

     {
  
/*     ptr = curr_node_ptr; */

/*     emit_old(OB,"    JMP   LAB_%s\n",ptr->target->spelling); */

     return;
     

     }


/*************************************************************************/
/*               Translate a PL1 END statement.                          */
/*************************************************************************/ 

static void tran_end (void)

     {

   
     set_error_line (curr_node_ptr);

     /* Dont read next node, caller will (?) */
     
     return;

     }

/*************************************************************************/
/*               Translate a PL/1 LABEL statement.                       */
/*************************************************************************/ 

static void tran_label (void)

     {

  
/*     ptr = curr_node_ptr; */

/*     emit_old(OB,"LAB_%s:\n",ptr->identity->spelling); */

     return;

     }


/*************************************************************************/
/*               Translate a PL/1  ASESIGNMENT statement                  */
/*************************************************************************/ 

static void tran_assignment (void)

     {

     Assignment_ptr ptr;
     Symbol_ptr     t_ptr;

     set_error_line (curr_node_ptr);

     ptr = curr_node_ptr;

     /******************************************************************/
     /* Allow bin(15) to bin(15), (static/auto) or CONSTANT to bin(15) */
     /******************************************************************/ 

     if ((ptr->target->attribs->type != BINARY) &&
         (ptr->target->attribs->type != CHARACTER))
        {
        report(90,ptr->line_no,_LINE_);
        retreat();
        }

     if ((ptr->target->attribs->class != STATIC) &&
         (ptr->target->attribs->class != AUTOMATIC) &&
         (ptr->target->attribs->class != DEFINED) &&
         (ptr->target->attribs->class != PARAMETER))
        {
        report(90,ptr->line_no,_LINE_);
        retreat();
        }

     t_ptr = ptr->target->attribs;

     /*****************************************************************/
     /*           OK We can tran this simple assignment.         */
     /*****************************************************************/

     result_in_NDP = 0;
                                    /****************************************/
     tran_expression (ptr->source); /* if bin(15) then result will be in _EAX */
                                    /* or else in the NDP.                  */
                                    /****************************************/

     if (result_in_NDP)
        {
        if (t_ptr->type == BINARY)
           {
           address_reference(ptr->target,TARGET,DWORD_PTR);
           Inst.opcode     = FISTP;
           Inst.source.reg = NDP; 
           CpuGenerate();
           }
        }
     else  /* use normal code */
        {
        /*************************************************************/
        /* If the ref has NO offset expression, then _EAX will not get */
        /* changed when generating addressing code.                  */
        /*************************************************************/

        if (ptr->target->ofx_ptr != NULL)
           {
           Inst.opcode     = MOV;  /* save result in _ECX cos _EAX may get used */
           Inst.target.reg = _ECX;   /* when addressing target ref.           */
           Inst.source.reg = _EAX;
           CpuGenerate();
      
           if (t_ptr->type == BINARY)
              {
              address_reference(ptr->target,TARGET,DWORD_PTR);
              Inst.opcode     = MOV;
              Inst.source.reg = _ECX; 
              CpuGenerate();
              }
           }
        else
           /******************************************************/
           /* Addressing the ref WILL NOT alter _EAX, so lets just */
           /* copy _EAX to the addressed ref.                      */
           /******************************************************/
           if (t_ptr->type == BINARY)
              {
              address_reference(ptr->target,TARGET,DWORD_PTR);
              Inst.opcode     = MOV;
              Inst.source.reg = _EAX; 
              CpuGenerate();
              }
 

        }

     return;

     }


/***************************************************************************/
/* This function accepts a ptr to a Ref node, and creates the required     */
/* code to load the referenced item.                                       */
/***************************************************************************/

static void tran_reference (Ref_ptr r_ptr)

     {

     Symbol_ptr          s_ptr;
    
     if (r_ptr == NULL)
        return;

     s_ptr = r_ptr->attribs; 
   
     /*******************************************************************/
     /* In the case of a binary variable, load it into the _EAX register  */
     /*******************************************************************/ 

     if ((s_ptr->type == BINARY) ||
         (s_ptr->type == NUMERIC))
        {
        address_reference(r_ptr,SOURCE,DWORD_PTR);
        Inst.opcode     = MOV;
        Inst.target.reg = _EAX;
        CpuGenerate();
        return;
        }

     /*******************************************************************/
     /* In the case of a string variable simply build the source part   */
     /* of the 80286 instruction, caller will do the rest.              */
     /*******************************************************************/   

     if (s_ptr->type == STRING)
        {
        address_reference(r_ptr,SOURCE,DWORD_PTR);
        return;
        }  

     }

/***************************************************************************/
/* Move thru the parse tree to the NEXT tree node, from the current node.  */
/***************************************************************************/

Any_ptr next_node (void)

     {

     Dummy_ptr             d_ptr;


     if (curr_node_ptr == NULL)
        curr_node_ptr = block_root->first_stmt;
     else
        {  
        d_ptr    = curr_node_ptr;
        curr_node_ptr = d_ptr->next_ptr;
        }

     if (curr_node_ptr == NULL)
        longjmp (exit_code,1);

     return (curr_node_ptr);

     }
     

/***************************************************************************/
/*               This function will open the object file.                  */
/***************************************************************************/

/****************************************************************************/
/*       Genearte a unique integer value for use in label building.         */
/****************************************************************************/

long unique (void)

     {

     unique_ctr++;

     return (unique_ctr);

     }

/****************************************************************************/
/* Exit the code generator right now, something really bad has happened !   */
/****************************************************************************/

void retreat (void)

     {

     longjmp(exit_code,2);

     }

/****************************************************************************/
/* This function walks up the symtab, and emits code for each block level.  */
/* It returns the register ID, (_EBP or _EBX) of the register to use to access  */
/* an automatic variable in the current or an outer block.                  */
/* Any required stack accessing code is emitted in here.                    */
/****************************************************************************/

chur get_auto_reg (Symbol_ptr s_ptr)

    {

    Block_ptr     b_ptr;

    
    if (s_ptr->declarator == curr_parent_block)
       return(_EBP);


/*   This ALMOST works, but has a bug, only largetst.pl1 reveals it */
/*   the codegen is patching the last loop in largetst incorrectly  */
/*   and at the wrong place in the .OBJ                             */
/*
    if (curr_EBX_block == s_ptr->declarator)
       return(_EBX);    */

    b_ptr = curr_parent_block;

    /* gen:    MOV   _EBX,[_EBP]  */

    Inst.opcode     = MOV;
    Inst.target.reg = _EBX;
    Inst.source.bas = _EBP;
    Inst.source.len = DWORD_PTR;
    CpuGenerate();

    b_ptr = b_ptr->parent;
    
    while (b_ptr != s_ptr->declarator)
          {
          /* gen:   MOV   _EBX,[_EBX]  */
          Inst.opcode     = MOV;
          Inst.target.reg = _EBX;
          Inst.source.bas = _EBX;
          Inst.source.seg = SS; /* seg override */
          Inst.source.len = DWORD_PTR;
          CpuGenerate();
          b_ptr = b_ptr->parent;
          }

    curr_EBX_block = s_ptr->declarator;

    return(_EBX);

    }

/****************************************************************************/
/* This function will return either 'DS' or else generate code to load 'ES' */
/* with the segment of the static that contains the supplied symbol 's_ptr' */
/* Remember to call this prior to setting any other instruction fields, cos */
/* this function may destroy any existing settings in the instruction !     */
/****************************************************************************/

static chur get_static_base (Symbol_ptr s_ptr)

    {

    Block_ptr     b_ptr;
    
    if (s_ptr->declarator == curr_parent_block)
       return(_ESI); /* we dont need anything doing, its in our static */

    /*********************************************************************/
    /* OK We must now generate code to load ES with the static segment   */
    /* location for the block containing 's_ptr'.                        */
    /* We check to see that ES doesnt already point there though !       */
    /*********************************************************************/

    //if (curr_ES_block == s_ptr->declarator)
    //   return(ES);

    b_ptr = curr_parent_block;

    /* gen MOV  _EBX,[_EBP] */

    Inst.opcode     = MOV;
    Inst.target.reg = _EBX;
    Inst.source.bas = _EBP;
	Inst.source.dis = FRAME_CALLERS_FRAME;
    Inst.source.len = DWORD_PTR;
    CpuGenerate();

    b_ptr = b_ptr->parent;

    while (b_ptr != s_ptr->declarator)
          {
          /* gen:   MOV   _EBX,[_EBX]  */
          Inst.opcode     = MOV;
          Inst.target.reg = _EBX;
          Inst.source.bas = _EBX;
          Inst.source.dis = FRAME_CALLERS_FRAME;
          Inst.source.len = DWORD_PTR;
          CpuGenerate();
          b_ptr = b_ptr->parent;
          }

    /********************************************************************/
    /* OK, EBX now points to the stack frame of the declaring block, so */
    /* lets load the saved static ptr into EDI.                         */
    /********************************************************************/

    Inst.opcode      = MOV;
    Inst.target.reg  = _EDI;
    Inst.source.bas  = _EBX;
    Inst.source.dis  = FRAME_CALLERS_STATIC;
    Inst.source.len  = DWORD_PTR;
    CpuGenerate();  

    curr_ES_block = s_ptr->declarator; /* remember that ES points here */

    return(_EDI);  /* caller must use ES to access static variable */

    }

   
/***************************************************************************/
/* This function takes a symbol and returns its adjusted offset within the */
/* stack frame of its declaring block.                                     */
/***************************************************************************/

static long frame_offset (Symbol_ptr s_ptr)
    
    {

    Block_ptr     b_ptr;

    b_ptr = s_ptr->declarator;

    return ((runtime_offset(s_ptr) - b_ptr->stack));

    }

/****************************************************************************/
/* This function takes a symbol ptr and builds the relevant parts of the    */
/* instruction so that the symbol can be processed.                         */
/* Note that the 'len' part of the instruction must be set to zero when we  */
/* are using immediate data !                                               */
/* It is via the presence of a 'len' value that the emitter distinguishes   */
/* between immedate and a memory reference with just a displacement.        */
/****************************************************************************/

static void address_reference (Ref_ptr r_ptr,short direction,chur size)


     {

     Symbol_ptr        s_ptr;
     chur              index_reg;

     defbas_depth++; /* trap recursive based/defined declarations */
     
     if (defbas_depth == 1)
        strcpy(recursive_name,r_ptr->spelling);

     if (defbas_depth > 50)
        {
        report (133,recursive_name,_LINE_);
        retreat(); 
        }

     s_ptr = r_ptr->attribs; 

     if (direction == SOURCE)
        {
        if (s_ptr->class == AUTOMATIC)
           {
           index_reg       = load_ofx (r_ptr,_EDI); 
           Inst.source.bas = get_auto_reg(s_ptr);
           Inst.source.dis = frame_offset(s_ptr);
           Inst.source.len = size;
           Inst.source.idx = index_reg;
           /*******************************************************/
           /* If we are referencing an item in a block other than */
           /* our own, then we MUST specify a segment override    */
           /* cos 'get_auto_reg' will already have created stack  */
           /* access code, for the outer block using _EBX as frame  */
           /* pointer.                                            */
           /*******************************************************/ 
           if (s_ptr->declarator != curr_parent_block)
              Inst.source.seg = SS;
           strncpy (Inst.text,s_ptr->spelling,32);
           Inst.text[32] = 0x0;
           defbas_depth--;
           return;
           }
        if (s_ptr->class == STATIC)
           {
           index_reg       = load_ofx(r_ptr,_EDI);
           Inst.source.bas = get_static_base(s_ptr);
           Inst.source.dis = runtime_offset(s_ptr);
           Inst.source.len = size;
           Inst.source.idx = index_reg;
           strncpy (Inst.text,s_ptr->spelling,32);
           Inst.text[32] = 0x0;
           defbas_depth--;
           return;
           }
        if (s_ptr->class == PARAMETER)
           {
           Inst.opcode     = MOV;
           Inst.target.reg = _ESI;
           Inst.source.bas = _EBP;
           Inst.source.dis = (short)(12 + s_ptr->offset /* (s_ptr->declarator->params - s_ptr->offset) */ );
           Inst.source.len = DWORD_PTR;
           CpuGenerate();
           /* now address the parameter */
           Inst.source.idx = _ESI;
           Inst.source.len = size;  
           strncpy (Inst.text,s_ptr->spelling,32);
           Inst.text[32] = 0x0;
           defbas_depth--; 
           return;
           }
        if (s_ptr->class == CONSTANT)
           {
           if (s_ptr->type == STRING)
              {
              Inst.source.bas = get_static_base(s_ptr);
              Inst.source.dis = runtime_offset(s_ptr);
              Inst.source.len = size;
              strcpy (Inst.text,"'");
              strncat (Inst.text,s_ptr->spelling,30);
              strcat (Inst.text,"'");
              defbas_depth--;
              return;
              }   
		   /* This is a temp/weak bit of code to address a numeric constant */
		   /* We are paying no attention to its attributes !!               */
           Inst.source.imm = atoi(s_ptr->spelling);
           Inst.source.len = DWORD_PTR; /* HWG02 */
           defbas_depth--;
           return;
           } 
        if (s_ptr->class == DEFINED)
           {
           address_reference (s_ptr->defbas_ptr,direction,size);
           defbas_depth--; 
           return;
           }
        }

     if (direction == TARGET)
        {
        if (s_ptr->class == AUTOMATIC)
           {
           index_reg = load_ofx(r_ptr,_EDI);  
           Inst.target.bas = get_auto_reg(s_ptr);
           Inst.target.dis = frame_offset(s_ptr);
           Inst.target.len = size;
           Inst.target.idx = index_reg;
           if (s_ptr->declarator != curr_parent_block)
              Inst.target.seg = SS;
           strncpy (Inst.text,s_ptr->spelling,32); 
           Inst.text[32] = 0x0; 
           defbas_depth--;
           return;
           }
        if (s_ptr->class == STATIC)
           {
           index_reg = load_ofx(r_ptr,_EDI);
           Inst.target.bas = get_static_base(s_ptr);
           Inst.target.dis = runtime_offset(s_ptr);
           Inst.target.len = size;
           Inst.target.idx = index_reg;
           strncpy (Inst.text,s_ptr->spelling,32);
           Inst.text[32] = 0x0; 
           defbas_depth--;
           return;
           }
        if (s_ptr->class == PARAMETER)
           {
           Inst.opcode     = MOV;
           Inst.target.reg = _ESI;
           Inst.source.bas = _EBP;
           Inst.source.dis = (short)(12 + s_ptr->offset) ; //(s_ptr->declarator->params - s_ptr->offset));
           Inst.source.len = DWORD_PTR;
           CpuGenerate();
           /* now address the parameter */
           Inst.target.idx = _ESI;
           Inst.target.len = size;  
           strncpy (Inst.text,s_ptr->spelling,32);
           Inst.text[32] = 0x0;
           defbas_depth--; 
           return;
           }
        if (s_ptr->class == CONSTANT)
           {
           Inst.target.imm = atoi(s_ptr->spelling);
           Inst.target.len = 0;
           defbas_depth--;
           return;
           } 
        if (s_ptr->class == DEFINED)
           {
           address_reference (s_ptr->defbas_ptr,direction,size);
           defbas_depth--;
           return;
           }
         }

     }

/***************************************************************************/
/* This function determines the runtime offset of a particular symbol.     */
/***************************************************************************/

static long runtime_offset (Symbol_ptr s_ptr)

    {

    Symbol_ptr      level_1_ptr;

    if (s_ptr->structure == 0) /* if not a struc or member */
       return(s_ptr->offset);
    
    if (s_ptr->parent == NULL) /* if already a level 1 name */
       return(s_ptr->offset);

    level_1_ptr = s_ptr->parent;

    while (level_1_ptr->parent != NULL)
          level_1_ptr = level_1_ptr->parent;

    /****************************************************************/
    /*         OK Weve got the level 1 name, so add the offsets.    */
    /****************************************************************/

    return(s_ptr->offset + level_1_ptr->offset);

    }

/****************************************************************************/
/* This function will load the result of a calculated offset expresion into */
/* The specified Index register.                                            */
/****************************************************************************/    

chur

   load_ofx (Ref_ptr r_ptr,chur reg)

   {

   if (r_ptr->ofx_ptr == NULL)
      return(0);

   if (curr_DI_ref == r_ptr)
      return(reg); /* _DI has not changed, since this ref was */
                   /* addressed (We hope !!)                 */ 

   curr_DI_ref = r_ptr;

   /*****************************************************************/
   /* Convert the offset expression, into runtime code to calculate */
   /* the array element offset.                                     */
   /*****************************************************************/

   tran_expression (r_ptr->ofx_ptr); 

   /*****************************************************************/
   /* Load the calculated offset into the specified 80x86 index reg */
   /*****************************************************************/

   Inst.opcode     = MOV;
   Inst.source.reg = _EAX;
   Inst.target.reg = reg;
   CpuGenerate();

   /*****************************************************************/
   /*            Check array index for bounds violations.           */
   /*****************************************************************/

   if (bounds_reqd)
      {
      Inst.opcode     = CMP;
      Inst.source.imm = r_ptr->symbol->bytes;
      Inst.target.reg = reg;
      CpuGenerate();

      Inst.opcode     = JL;   /* Jump over the following INT   */
      Inst.source.imm = 2;     
      CpuGenerate();

      Inst.opcode     = INT;
      Inst.source.imm = 5;
      CpuGenerate();
      } 

   return (reg);
                            /* the 'next' instruction emmited will have */
                            /* and index reg specified on its behalf.   */
   }

/***************************************************************************/
/* This function returns TRUE if the two trees passed in, are the same     */
/* that is, if they would result in the same code being generated.         */
/***************************************************************************/

short

   identical (Any_ptr t1_ptr, Any_ptr t2_ptr)

   {

   short             t1,t2;
   Ref_ptr           r1_ptr,r2_ptr;
   Symbol_ptr        s1_ptr,s2_ptr;
   Sub_ptr           b1_ptr,b2_ptr;
  
   if (!optimize_reqd)
      return(0); /* Even if the MAY be the same, we dont have the time */

   /**********************************************************************/
   /* Two trees are the 'same' if :                                      */   
   /* 1) Their root nodes contain the same data.                         */
   /* 2) All subtress of the root node are also the same.                */
   /* Naturally this function is recursive !                             */
   /**********************************************************************/

   if ((t1_ptr == NULL) && (t2_ptr != NULL))
      return(0);

   if ((t1_ptr != NULL) && (t2_ptr == NULL))
      return(0);

   if (t1_ptr == t2_ptr)
      return(1); /* They MUST be the same ! */

   t1 = nodetype (t1_ptr);
   t2 = nodetype (t2_ptr);

   if (t1 != t2)
      return(0);

   switch (t1) { 

   case (REFERENCE):
        {
        r1_ptr = t1_ptr;
        r2_ptr = t2_ptr;

        if (r1_ptr->num_subs != r2_ptr->num_subs)      return(0);
        if (r1_ptr->data_type != r2_ptr->data_type)    return(0);
        if (r1_ptr->scale != r2_ptr->scale)            return(0);
        if (r1_ptr->null_list != r2_ptr->null_list)    return(0);
        if (strcmp(r1_ptr->spelling,r2_ptr->spelling)) return(0);

        /**************************************************************/
        /* OK the data parts of these two nodes are the same, but are */
        /* their child nodes ?                                        */
        /**************************************************************/

        if (!identical(r1_ptr->symbol,r2_ptr->symbol))   return(0);
        if (!identical(r1_ptr->attribs,r2_ptr->attribs)) return(0);
        if (!identical(r1_ptr->sublist,r2_ptr->sublist)) return(0);
        if (!identical(r1_ptr->dot_ptr,r2_ptr->dot_ptr)) return(0);
        if (!identical(r1_ptr->ptr_ptr,r2_ptr->ptr_ptr)) return(0);
        if (!identical(r1_ptr->ofx_ptr,r2_ptr->ofx_ptr)) return(0);

        /*************************************************************/
        /* Having found no differences so-far, we may conclude that  */
        /* they are the same.                                        */
        /*************************************************************/
        return(1);
        }

   case (SYMBOL):
        {
        s1_ptr = t1_ptr;
        s2_ptr = t2_ptr;

        if (strcmp(s1_ptr->spelling,s2_ptr->spelling)) return(0);
        if (s1_ptr->varying != s2_ptr->varying)        return(0);
        if (s1_ptr->structure != s2_ptr->structure)    return(0);
        if (s1_ptr->declared  != s2_ptr->declared)     return(0);
        if (s1_ptr->level != s2_ptr->level)            return(0);
        if (s1_ptr->bytes != s2_ptr->bytes)            return(0);
        if (s1_ptr->size != s2_ptr->size)              return(0);
        if (s1_ptr->prec_1 != s2_ptr->prec_1)          return(0);
        if (s1_ptr->prec_2 != s2_ptr->prec_2)          return(0);
        if (s1_ptr->class  != s2_ptr->class)           return(0);
        if (s1_ptr->type != s2_ptr->type)              return(0);
        if (s1_ptr->scope != s2_ptr->scope)            return(0);
        if (s1_ptr->num_dims != s2_ptr->num_dims)      return(0);
        if (s1_ptr->declarator != s2_ptr->declarator)  return(0);     
      
        /**************************************************************/
        /* OK the data parts of these two nodes are the same, but are */
        /* their child nodes ?                                        */
        /**************************************************************/
      
        if (!identical(s1_ptr->parent,s2_ptr->parent)) return(0);
        
        return(1);
        }

   case (SUBSCRIPT):
        {
        b1_ptr = t1_ptr;
        b2_ptr = t2_ptr;

        if (!identical(b1_ptr->expression,b2_ptr->expression)) return(0);

        return(1);
        }
   default:
        return(0);   

   }



}
