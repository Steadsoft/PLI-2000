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

/***************************************************************************/
/*                         Modification History                            */
/***************************************************************************/
/*                                                                         */
/*  When       Who                       What                              */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/*  04-10-91   HWG       Initial Version.                                  */
/*  18-10-91   HWG       emit_mov did not cater for move to/from segreg.   */
/*  18-10-91   HWG       Assembly listing didnt print: MOV DS,_CX.         */
/*  24-12-91   HWG       SEGDEF records were not having their correct      */
/*                       'segment length' value set. The compiler now      */
/*                       backs-up after emiting all code for a block       */
/*                       and patches the correct value into the SEGDEF.    */
/*                                                                         */
/*  24-12-91   HWG       'start_code' now sets 'offset_in_code_seg' to 0   */
/*                       so that the offset is zeroised every time a new   */
/*                       code segment generation begins.                   */
/*                                                                         */
/*  21-01-92   HWG       'start_code' and 'write_code' modified so that    */
/*                       code generation may produce multiple LEDATA recs. */
/*                                                                         */
/*  22-01-92   HWG       The flag 'fixup_pending' prevents a new LEDATA    */
/*                       rec from being started, when a Fixup is still     */
/*                       being built by codegen.                           */
/*                                                                         */
/*  22-01-92   HWG       patch_code is now a generic function that may be  */
/*                       called to 'patch' previously emited code.         */
/*                                                                         */
/*  14-03-92   HWG       When generating the 'mod' bits, the case of an    */
/*                       index reg WITHOUT a displacement was NOT being    */
/*                       specially catered for.                            */      
/*                                                                         */
/*  30-03-92   HWG       Added support for conditional jumps.              */
/*                                                                         */
/*  20-06-93   HWG       The variable 'machine_code' was too short, and    */
/*                       sometimes caused a compiler crash, when it was    */
/*                       assigned too long a string value. It is now 32.   */
/*                                                                         */
/*  21-06-93   HWG       memset used to replace slow loops !!              */
/*                                                                         */
/*  04-07-93   HWG       MUL and DIV merged into a single function, also   */
/*                       add_sub, adc_sbb, inc_dec etc...                  */
/*  13-10-02   HWG       Massive restructuring underway to support 32 bit  */
/*                       Intel Pentium class processors.                   */
/***************************************************************************/

/***************************************************************************/
/*                        Functional  Description                          */
/***************************************************************************/
/* This is the compiler code emiter, it accepts a data structure holding   */
/* details of the instruction to be generated into raw 80286 instructions. */
/* The production of an assembler-like listing is handled by this module   */
/* by performing a simple conversion of the instruction node to text.      */
/* Note that the algorithms used in here for instruction building, were    */
/* designed after analysis of the Intel instruction set reference.         */
/* Please refer to the: Turbo Assembler Quick Reference Guide, or the book */
/* 'Programming the 80286' by Sybex, in the event of any queries.          */
/*                                                                         */
/*                      V E R Y     I M P O R T A N T                      */
/* ----------------------------------------------------------------------- */
/* You MUST test the compiler with great thoroughness after making ANY     */
/* change to this module, no matter how trivial it may appear. This module */
/* is very carefully coded, and not documented as well as it could be.     */
/* There are many interdependencies between the various functions.         */
/***************************************************************************/

/***************************************************************************/
/*                   D E F I N E D     S Y M B O L S                       */
/***************************************************************************/

# define chur       unsigned char 
# define _LINE_     ((short)__LINE__)
# define NOT(x)     (!(x))

/***************************************************************************/
/*                     I N C L U D E    F I L E S                          */
/***************************************************************************/

# include "stdlib.h"
# include "setjmp.h"
# include "stdio.h"
# include "string.h"
# include "c_types.h"
# include "pentium.h"
# include "opcodes.h"
# include "objdefs.h"
//# include "acbp.h"
# include "fixup.h"
# include "nodes.h"
# include "coff.h"
# include "globals.h"

/***************************************************************************/
/*       E X T E R N A L L Y    D E F I N E D    P R O T O T Y P E S       */
/***************************************************************************/

void     report (short,char *,short);

/***************************************************************************/
/*      E X T E R N A L L Y    D E F I N E D    V A R I A B L E S          */
/***************************************************************************/ 

extern   char      line_no[10];
extern   short     listing_reqd;
extern   short     system_reqd;
extern   long      lines_printed;
extern   FILE      *LISTING;       /* see list.c     */ 
extern   jmp_buf   exit_code;      /* set in code.c  */
extern   short     current_seg;    /* set in allocate.c  */
extern   short     block_counter;
extern   short     trace_code;
extern   short     curr_index_num;
extern   unsigned  long bytes_read;
extern   Block_ptr curr_ES_block;
extern   Block_ptr curr_EBX_block;

Instruction Inst;
fixup		Fixup;

static char	Initialized = 0;

/***************************************************************************/
/*              L O C A L     S T A T I C    V A R I A B L E S             */
/***************************************************************************/


short                fixup_pending = 0;
Block_ptr            curr_code_block = NULL;  /* the block for which code is currently */
long                 operand_chars = 0;/* being generated */
char                 prev_line_no[10]="";
short                machine_bytes=0;
unsigned short       machine_code[32];
short                block_number = 0;
unsigned long        code_bytes = 0;
unsigned long        curr_offset = 0;
unsigned long        offset_in_code_seg;
chur                 code_rec[2048];
chur                 fix_rec[2048];
short                fix_len  = 0;
short                code_len = 0;  /* curr length of code rec */
short                first_time_called = 1;
long                 current_frame_size = 0;
long                 recs_written = 0; /* Number of objrecs written by */
                                       /* THIS phase.                  */

static Section_ptr	 curr_sect_ptr       = NULL; /* local static ptr to COFF code section */
static char			 emitter_initialized = 0;

/* These strucs are used to build the mod r/m byte and sib byte */

static Mrm  mod;
static Sib  sib;
static chur mod_reqd;
static chur sib_reqd;
	
static AddrType T;
static AddrType S;

chur OPER;




/***************************************************************************/
/*              L O C A L Y    D E F I N E D   F U N C T I O N S           */
/***************************************************************************/

#define emit(x)				  EmitByte((chur)(x))


static AddrType  GetAddrType               (Address_ptr);
static AddrClass GetAddrClass              (AddrType);
static AddrSize  GetAddrSize			   (Address_ptr);
static void      GenTwoOperandInstruction  (Instruction);
static void      GenOneOperandInstruction  (Instruction);
static void      GenZeroOperandInstruction (Instruction);
static void      InitMachine               (void);
static void      ResetInstruction          (void);
static short     GetSource                 (void);
static short     GetTarget                 (void);
static void      EmitExtra				   (void);
static void      EmitByte				   (chur);
static void      EmitData8				   (long);
static void      EmitData16				   (long);
static void      EmitData32				   (long);
static void      EmitDisp8				   (long);
static void      EmitDisp16				   (long);
static void      EmitDisp32				   (long);
static void      EmitDisp				   (long);
static short     ByteSize                  (Reg);
static short     WordSize                  (Reg);
static short     DwordSize                 (Reg);

//static short data_size         (long);

static short RegisterNumber    (Reg);
static void  set_mod_rm        (void);
static void  set_mod           (void);
static void  set_rm            (void);
static void  set_reg           (void);
static void  set_sib           (void);
void         write_code_rec	   (void);
void         clean_code_rec	   (void);
void         write_fixup	   (void);
void         begin_fixup	   (void);


static void  emit_fimul        (void);
static void  emit_int          (void);
static void  emit_exit         (void); /* ie leave */



static chur  short_idx         (short,short);
static short long_idx          (short,short);
static void  print_assembly    (void);
//static chur  C                 (mrm);   /* convert mod struc into a char */
void         check_print       (short);
static void  print_asm_heading (void);
static void  print_offset      (unsigned long);
static void  print_operation   (Opcode);
static void  print_register    (Reg);
static void  print_memory      (Address);
static void  print_imm         (long);
static void  print_code        (void);
static void  print_line_number (void);
static void  print_sundry      (void);
void         emit_fixup        (chur);
static void  emit_override     (void);
  
/*-----------------------------------------------------------------------------*/
/*                                    CpuGenerate                              */
/*-----------------------------------------------------------------------------*/
/* This function will generate the CPU instruction bytes based upon the values */
/* in the instruction structure. It handles all aspects of this activity and   */
/* creates the Mod R/M byte and SIB byte whenever it determines these are      */
/* required.																   */
/* The generated bytes are appended to the Raw data that is attached to the    */
/* current COFF section node.                                                  */
/*-----------------------------------------------------------------------------*/

void CpuGenerate (void)

{

AddrType		TFirst,TSecond;

if (curr_sect_ptr == NULL)
   {
   report(198,"",_LINE_); 
   longjmp(exit_code,3);
	  }

if (NOT(emitter_initialized))
   InitMachine();

/*********************************************************************/
/* These statements are used to reset certain register optimization  */
/* variables whenever a reg is modified.                             */
/*********************************************************************/ 

if (Inst.target.reg == ES)
   curr_ES_block = NULL;   

if (Inst.target.reg == _EBX)
   curr_EBX_block = NULL;    

set_mod_rm(); /* set these flags here and now */

machine_bytes = 0;

/*-----------------------------------------------------------------*/
/* Get the offset at which the first byte of this instruction will */
/* be written. This is used to indicate the per-block offset in    */
/* the assembly language listing for each instruction.             */
/*-----------------------------------------------------------------*/

curr_offset = curr_sect_ptr->sect.SizeOfRawData; 

emit_override(); /* emit segment prefix (if any) right now */

/*-------------------------------------------*/
/* If there is a map defined then process it */
/*-------------------------------------------*/

if (map_table[Inst.opcode] != NULL)
   {
   TFirst  = GetAddrType (&Inst.target);
   TSecond = GetAddrType (&Inst.source);

   if ((TFirst != A_NOT_PRESENT) && (TSecond != A_NOT_PRESENT))
      GenTwoOperandInstruction(Inst);
   else
   if ((TFirst == A_NOT_PRESENT) && (TSecond == A_NOT_PRESENT))
      GenZeroOperandInstruction(Inst);
   else
      GenOneOperandInstruction(Inst);

   print_assembly();
   fflush(LISTING);
   ResetInstruction(); 
   return;
   }


switch (Inst.opcode) {

case(EXIT):
    emit_exit();
    break;
case(FIMUL):
    emit_fimul();
    break;
case(INT):
    emit_int();
    break;
default:
    {
    report(108,"",_LINE_); /* bad instruction */
    longjmp(exit_code,3);
    }
}

print_assembly();

fflush(LISTING);

ResetInstruction(); 

}

/*-----------------------------------------------------------------------------*/
/*                                   ResetInstruction                          */
/*-----------------------------------------------------------------------------*/


static void ResetInstruction (void)

{

Inst.target.reg   = UNSPECIFIED;
Inst.target.len   = 0;
Inst.target.dis   = 0;
Inst.target.idx   = UNSPECIFIED;
Inst.target.bas   = UNSPECIFIED;
Inst.target.seg   = UNSPECIFIED; 
Inst.target.imm   = 0;
Inst.target.scale = 0;

Inst.source.reg   = UNSPECIFIED;
Inst.source.len   = 0;
Inst.source.dis   = 0;
Inst.source.idx   = UNSPECIFIED;
Inst.source.bas   = UNSPECIFIED;
Inst.source.seg   = UNSPECIFIED;
Inst.source.imm   = 0;
Inst.source.scale = 0;

Inst.text[0] = 0;  /* null string */

}


/***************************************************************************/
/* This function re-initialises all the fields in the instruction struc    */
/* The opcode field is left as-is to simplify calling code slightly.       */
/***************************************************************************/

static void InitMachine (void)

   {

   /*----------------------------------------------*/
   /* Init the pointer array for the Opcode tables */
   /*----------------------------------------------*/

   map_table[ADD]   = &(add_map);
   map_table[ADC]   = &(adc_map);
   map_table[DEC]   = &(dec_map);
   map_table[INC]   = &(inc_map);
   map_table[SBB]   = &(sbb_map);
   map_table[SUB]   = &(sub_map);
   map_table[MUL]   = &(mul_map);
   map_table[DIV]   = &(div_map);
   map_table[MOV]   = &(mov_map);
   map_table[LEA]   = &(lea_map);
   map_table[CAL]   = &(call_map);
   map_table[RET]   = &(ret_map);
   map_table[ENTER] = &(enter_map);

   map_table[JMP]   = &(jmp_map);
   map_table[PUSH]  = &(push_map);
   map_table[POP]   = &(pop_map);
   map_table[CMP]   = &(cmp_map);
   map_table[FILD]  = &(fild_map);
   map_table[FIST]  = &(fist_map);
   map_table[FISTP] = &(fistp_map);

   map_table[JA]    = &(ja_map);
   map_table[JAE]   = &(jae_map);
   map_table[JB]    = &(jb_map);
   map_table[JBE]   = &(jbe_map);
   map_table[JC]    = &(jc_map);
   map_table[JE]    = &(je_map);
   map_table[JZ]    = &(jz_map);
   map_table[JG]    = &(jg_map);
   map_table[JGE]   = &(jge_map);
   map_table[JL]    = &(jl_map);

   map_table[JLE]   = &(jle_map);
   map_table[JNE]   = &(jne_map);
   map_table[JNG]   = &(jng_map);
   map_table[JNL]   = &(jnl_map);
   map_table[JNGE]  = &(jnge_map);
   map_table[JNLE]  = &(jnle_map);

   /*------------------------------------------------*/
   /* Init the pointer array for the Cmd code tables */
   /*------------------------------------------------*/

   cmd_table[ADD]   = &(add_ops);
   cmd_table[ADC]   = &(adc_ops);
   cmd_table[DEC]   = &(dec_ops);
   cmd_table[INC]   = &(inc_ops);
   cmd_table[SBB]   = &(sbb_ops);
   cmd_table[SUB]   = &(sub_ops);
   cmd_table[MUL]   = &(mul_ops);
   cmd_table[DIV]   = &(div_ops);
   cmd_table[MOV]   = &(mov_ops);
   cmd_table[LEA]   = &(lea_ops);
   cmd_table[CAL]   = &(call_ops);
   cmd_table[RET]   = &(ret_ops);
   cmd_table[ENTER] = &(enter_ops);

   cmd_table[JMP]   = &(jmp_ops);
   cmd_table[PUSH]  = &(push_ops);
   cmd_table[POP]   = &(pop_ops);
   cmd_table[CMP]   = &(cmp_ops);
   cmd_table[FILD]  = &(fild_ops);
   cmd_table[FIST]  = &(fist_ops);
   cmd_table[FISTP] = &(fistp_ops);

   cmd_table[JA]    = &(ja_ops);
   cmd_table[JAE]   = &(jae_ops);
   cmd_table[JB]    = &(jb_ops);
   cmd_table[JBE]   = &(jbe_ops);
   cmd_table[JC]    = &(jc_ops);
   cmd_table[JE]    = &(je_ops);
   cmd_table[JZ]    = &(jz_ops);
   cmd_table[JG]    = &(jg_ops);
   cmd_table[JGE]   = &(jge_ops);
   cmd_table[JL]    = &(jl_ops);

   cmd_table[JLE]   = &(jle_ops);
   cmd_table[JNE]   = &(jne_ops);
   cmd_table[JNG]   = &(jng_ops);
   cmd_table[JNL]   = &(jnl_ops);
   cmd_table[JNGE]  = &(jnge_ops);
   cmd_table[JNLE]  = &(jnle_ops);

   }


/***************************************************************************/
/*          This is the LEDATA header creation pseudo-op.                  */
/* This op will initialise the header details of a new LEDATA code record. */
/***************************************************************************/

void CpuSetSectPtr (Section_ptr sect_ptr)

{

curr_sect_ptr = sect_ptr;

}

/***************************************************************************/
/* The code generator can invoke this pseudo op to write out the current   */
/* code record as it stands.                                               */
/***************************************************************************/

void 

   write_code (void)

   {

   ;

   }

/***************************************************************************/
/*                Create an 80286 LEAVE instruction.                       */
/***************************************************************************/

static void 
   
   emit_exit (void)

   {

   emit (OPC9);

   }

/***************************************************************************/
/*                   Create an 80286 INT instruction.                      */
/***************************************************************************/

static void 

   emit_int (void)

   {

   emit(OPCD);
   EmitData8(Inst.source.imm);

   }


/****************************************************************************/
/*  This function emits the machine instructions for a FIMUL instruction.   */
/****************************************************************************/

static void 

   emit_fimul (void)

   {

   if ((GetTarget() == REGISTER) && (GetSource() == MEMORY))
      {
      emit(OPDE);
      EmitExtra();
      EmitDisp(Inst.source.dis);
      return;
      }

   report(136,"",_LINE_); /* bad NDP instruction */

   }


/****************************************************************************/
/* This function returns the type of the required target address reference. */
/****************************************************************************/

static short GetTarget (void)

{

AddrType A;
Address  R;

R = Inst.target;

A = GetAddrType (&R);

if (A == A_IMMEDIATE)
   return(IMMEDIATE);

if (A == A_REGISTER)
   return(REGISTER);

if ((A == A_NOT_PRESENT) || (A == A_INVALID))
   return(UNSPECIFIED);

return(MEMORY);

}

/****************************************************************************/
/* This function returns the type of the required source address reference. */
/****************************************************************************/

static short GetSource (void)

{

AddrType A;
Address  R;

R = Inst.source;

A = GetAddrType (&R);

if (A == A_IMMEDIATE)
   return(IMMEDIATE);

if (A == A_REGISTER)
   return(REGISTER);

if ((A == A_NOT_PRESENT) || (A == A_INVALID))
   return(UNSPECIFIED);

return(MEMORY);

}


/****************************************************************************/
/* This function returns true if a register number passed in is a byte reg  */
/****************************************************************************/

static short 

   ByteSize (Reg value)

   {

   if ((value == _AL) || (value == _AH) ||
       (value == _BL) || (value == _BH) ||
       (value == _CL) || (value == _CH) ||
       (value == _DL) || (value == _DH))
       {
       return(1);
       }

   return(0);

   }

/***************************************************************************/
/* This function returns true if a register number passed in is a word reg */
/***************************************************************************/

static short 

   WordSize (Reg value)

   {

   if ((value == _AX) || (value == _BX) ||
       (value == _CX) || (value == _DX) ||
       (value == _SP) || (value == _BP) ||
       (value == _SI) || (value == _DI) ||
       (value == ES)  || (value == CS) ||
       (value == SS)  || (value == DS))
       {
       return(1);
       }

   return(0);

   } 

/***************************************************************************/
/* This function returns true if a register number passed in is a word reg */
/***************************************************************************/

static short 

   DwordSize (Reg value)

   {

   if ((value == _EAX) || (value == _EBX) ||
       (value == _ECX) || (value == _EDX) ||
       (value == _ESP) || (value == _EBP) ||
       (value == _ESI) || (value == _EDI))
       {
       return(1);
       }

   return(0);

   } 

/**************************************************************************/
/* This function returns the register number (80286 definition of) to the */
/* caller.                                                                */
/**************************************************************************/

static short 

   RegisterNumber (Reg inval)

   {

   switch(inval) {

   case(UNSPECIFIED):
       return(UNSPECIFIED);
   case(_AL):
   case(_AX):
   case(_EAX):
       return(0);
   case(_CL):
   case(_CX):
   case(_ECX):
       return(1);
   case(_DL):
   case(_DX):
   case(_EDX):
       return(2);
   case(_BL):
   case(_BX):
   case(_EBX):
       return(3);
   case(_AH):
   case(_SP):
   case(_ESP):
       return(4);
   case(_CH):
   case(_BP):
   case(_EBP):
       return(5);
   case(_DH):
   case(_SI):
   case(_ESI):
       return(6);
   case(_BH):
   case(_DI):
   case(_EDI):
       return(7);

   /* for more info on these settings see Sybex 80286 book, p273 */

   case(ES): 
      return(0);

   case(CS): 
      return(1);

   case(SS): 
      return(2);
   
   case(DS):
      return(3);
   default:
      {
      report (109,"",_LINE_); /* we should die here */
      longjmp(exit_code,3);
      }
   }

   return(0);

   }


/***************************************************************************/
/* This function puts the passed-in byte into the record buffer, to be     */
/* written out to the object file.                                         */
/***************************************************************************/

static void 

   EmitByte (chur byte)

   {

   Section_ptr	sect_ptr;
   
   if (system_reqd)
      {
      machine_code[machine_bytes] = byte;
      machine_bytes += 1;
      }

   sect_ptr = curr_sect_ptr;

   /*--------------------------------------------------------*/
   /* If this is the first byte to be output to this section */
   /* then allocate a 4k page.                               */
   /*--------------------------------------------------------*/

   if (sect_ptr->data_ptr == NULL)
      {
	  sect_ptr->allocated_space = 4096;
      sect_ptr->data_ptr = malloc(4096);
      sect_ptr->sect.SizeOfRawData = 0;
	  }

   /*-----------------------------------------------------------------------*/ 
   /* If all space allocated so far is already used up, we must expand      */
   /* that space. We reallocate the space but make it 4K larger than it was */
   /*-----------------------------------------------------------------------*/

   if (sect_ptr->sect.SizeOfRawData == sect_ptr->allocated_space)
      {
      sect_ptr->data_ptr        = realloc(sect_ptr->data_ptr,sect_ptr->sect.SizeOfRawData + 4096);   
 	  sect_ptr->allocated_space = sect_ptr->sect.SizeOfRawData + 4096;
	  }

   sect_ptr->data_ptr[sect_ptr->sect.SizeOfRawData] = byte;
   
   sect_ptr->sect.SizeOfRawData++;  

   code_len++;
   offset_in_code_seg++; /* Total bytes emited in current seg */
   code_bytes++;         /* Total bytes of code emited period! */

   }

/***************************************************************************/
/* This function returns the current offset within the LEDATA record that  */
/* is currently being built.                                               */
/***************************************************************************/

short 

   get_ledata_offset (void)

   {

   return (code_len-6); /* ie first 6 bytes in buffer are header etc */

   }

/***************************************************************************/
/* This function returns the current offset within the code segment  that  */
/* is currently being built.                                               */
/***************************************************************************/

short 

   get_segment_offset (void)

   {

   return ((short)offset_in_code_seg); 

   }


/***************************************************************************/
/* This function 't' (for translate) converts an mrm struc into a char.    */
/* this is cos 'emit' needs a char argument.                               */
/***************************************************************************/

static void EmitExtra(void) 

{

union temp 
{
Mrm      mbyte;
Sib	     sbyte;
chur     mask;
} uni;
   


if (mod_reqd)
   {
   uni.mbyte = mod;
   EmitByte(uni.mask);
   mod_reqd = 0;
  
   if (sib_reqd)
      {
      uni.sbyte = sib;
      EmitByte(uni.mask);
      sib_reqd = 0;
      }
   }


}  

/***************************************************************************/
/*        Set the Mod-R/M byte from the details of this instruction.       */
/***************************************************************************/

static void 

   set_mod_rm (void)

   {

   mod_reqd  = 0;
   sib_reqd  = 0;

   mod.mod   = 0;
   mod.regop = 0;
   mod.rm    = 0;

   sib.base  = 0;
   sib.index = 0;
   sib.scale = 0;

   /*--------------------------------------------------------------------*/
   /* OK we must decide whether we need a Mod/RM byte and/or an SIB byte */
   /*--------------------------------------------------------------------*/

   S = GetAddrType(&Inst.source);
   T = GetAddrType(&Inst.target);

   if (S == A_INVALID) 
      {
      report(182,"",_LINE_);
      //longjmp(exit_code,3);
      //return;
	  }

   if (T == A_INVALID) 
      {
      report(183,"",_LINE_);
      //longjmp(exit_code,3);
      //return;
	  }

   /*-----------------------------------------------------------*/
   /* This instruction specifies no operands so we dont require */
   /* a Mod or SIB byte.                                        */
   /*-----------------------------------------------------------*/

   if ((T == A_NOT_PRESENT) && (S == A_NOT_PRESENT))
      return;

   /*--------------------------------------------------------*/
   /* If only one operand present, then it must be in target */
   /* if caller has this wrong way, then its safe to sawp    */
   /* them.                                                  */
   /*--------------------------------------------------------*/

   if ((T == A_NOT_PRESENT) && (S != A_NOT_PRESENT))
      {
      Inst.target       = Inst.source;
      Inst.source.reg   = UNSPECIFIED;
      Inst.source.len   = 0;
      Inst.source.dis   = 0;
      Inst.source.idx   = UNSPECIFIED;
      Inst.source.bas   = UNSPECIFIED;
      Inst.source.seg   = UNSPECIFIED;
      Inst.source.imm   = 0;
      Inst.source.scale = 0;
	  /*-----------------------------------------------------------*/
	  /* Reset these target/source type variables as they are used */
	  /* below to drive other algorithms.                          */
	  /*-----------------------------------------------------------*/
      S = GetAddrType(&Inst.source);
      T = GetAddrType(&Inst.target);
      }      

   /*----------------------------------------------------------------*/
   /* Lets ensure that the combination of source and target is legal */
   /*----------------------------------------------------------------*/

   if ((T != A_NOT_PRESENT) && (S != A_NOT_PRESENT))
      if ((T != A_IMMEDIATE) && (S != A_IMMEDIATE))
         if ((T != A_REGISTER) && (S != A_REGISTER))
            {
            report(185,"",_LINE_);
            //longjmp(exit_code,3);
            //return;
	        }

   /* It seems that no instruction that takes an IMMEDIATE operand */
   /* can ever require a MODRM/SIB byte. However we wont assume this */
   /* in the code until its verified */

   //if (T == A_IMMEDIATE)
   //   return;

   set_mod();

   }

/***************************************************************************/
/* This function builds the mod bits in the Mod R/M byte from information  */
/* containd in the instruction.                                            */
/***************************************************************************/

static void 

   set_mod (void)

   {

   AddrType		Atype,Rtype;
   Address_ptr  Aref, Rref;
   long			Disp;

   /* Validate register usage first */

   if (Inst.source.idx == _ESP)
      {
      report(196,"",_LINE_);
      longjmp(exit_code,3);
      return;
	  }

   if (Inst.target.idx == _ESP)
      {
      report(197,"",_LINE_);
      longjmp(exit_code,3);
      return;
	  }

   /*---------------------------------------------------------------------------------*/
   /*                                      Set the MOD field.                         */
   /*---------------------------------------------------------------------------------*/

   if ((T == A_REGISTER) && (S == A_REGISTER))
      {
	  /*--------------------------------------------------------------------------*/
	  /* For instructions with two register operands then we need to know which   */
	  /* register to use for setting R-M and which to use to set REG.             */
	  /* Instructions usually have two forms, one in which R/M is the target and  */
	  /* one in which REG is the target, the actual opcode differing in each case */
	  /* We will always assume in this case, that the instruction requires REG as */
	  /* target, eg  ADC   EAX,EBX will be treated as the ADC   REG/R-M form an   */
	  /* not the ADC   R-M/REG form. We will always generate the opcode based     */
	  /* upon this assumption.                                                    */
	  /* So in this example we will generate MOD/R-M and the Opcode for the       */
	  /* above example as:  ADC   EAX,EBX    -->  13 C3 and not 11 C3             */
	  /*--------------------------------------------------------------------------*/
      mod.mod   = 3;
      mod.rm    = RegisterNumber(Inst.source.reg);
	  mod.regop = RegisterNumber(Inst.target.reg);
	  return;
	  }
   else
      if ((T != A_NOT_PRESENT) && (S != A_NOT_PRESENT))
	     {
	     /*---------------------------*/
	     /* OK There are two operands */
		 /*---------------------------*/
         if (T == A_REGISTER)
            {
            Atype = S;
	        Aref  = &(Inst.source);
	        Disp  = Aref->dis;
	        Rtype = T;
	        Rref  = &(Inst.target);
	        }
         else
            {
            Atype = T;
	        Aref  = &(Inst.target);
	        Disp  = Aref->dis;
	        Rtype = S;
	        Rref  = &(Inst.source);
	        }
		 }
	  else
	     /*-------------------------------------------------------------------------------*/
         /* We have only a single operand, it could be a register or an effective address.*/
		 /* Single operand instructions must place operand in 'target' this is validated  */
		 /* before this function gets called.                                             */
		 /* If its a register, then things are simple.                                    */
		 /*-------------------------------------------------------------------------------*/
         {
         if (T == A_REGISTER)
		    {
			mod.mod = 3;
			mod.rm  = RegisterNumber(Inst.target.reg);
			return;
			}
		 else
		    {
            Atype = T;
	        Aref  = &(Inst.target);
	        Disp  = Aref->dis;
            }
		 }

   if (Atype == A_BASE)
      mod.mod = 0;

   if (Atype == A_DISP)
      mod.mod = 0;

   if (Atype == A_BASE_INDEX)
      mod.mod = 0;

   if (Atype == A_INDEX_DISP)
      mod.mod = 0;

   if ((Atype == A_BASE_DISP) || (Atype == A_BASE_INDEX_DISP))
      if ((Disp >= MIN_DISP8) && (Disp <= MAX_DISP8))
	     mod.mod = 1;
	  else
	     mod.mod = 2;

 
   /*---------------------------------------------------------------------------------*/
   /*                                      Set the R/M field.                         */
   /*---------------------------------------------------------------------------------*/

   if (Atype == A_REGISTER)
      mod.rm = RegisterNumber(Aref->reg);
   else
      if (Atype == A_DISP)
	     mod.rm = 5;
	  else
	     if ((Atype == A_INDEX)      || 
		     (Atype == A_INDEX_DISP) || 
			 (Atype == A_BASE_INDEX) ||
			 (Atype == A_BASE_INDEX_DISP))
			 mod.rm = 4;
		 else
		    mod.rm = RegisterNumber(Aref->bas);

   /*-----------------------------------------------------------------------------*/
   /*                            Now set the REG/OP field.                        */
   /*-----------------------------------------------------------------------------*/

   if ((T != A_NOT_PRESENT) && (S != A_NOT_PRESENT))
      mod.regop = RegisterNumber(Rref->reg);
   else
      /*----------------------------------------------*/
      /* this is a single operand instruction, reg is */
	  /* set to extension data in this case. We force */
	  /* it to zero here but it will require setting  */
	  /* by the opcode layer if it decides to emit mod*/
	  /*----------------------------------------------*/
      mod.regop = 0;

   if (mod.rm != 4)
      return; /* No SIB is required */
      
   sib_reqd = 1;

   /* OK Lets create the SIB byte */

   if (Aref->scale == 0)
      Aref->scale = 1;  /* If no scale was specified set it to 1 */

   switch (Aref->scale) {

   case(1):
       sib.scale = 0;
	   break;
   case(2):
       sib.scale = 1;
	   break;
   case(4):
       sib.scale = 2;
	   break;
   case(8):
       sib.scale = 3;
	   break;
   default:
       {
       ;
	   }

   }

   sib.index = RegisterNumber(Aref->idx);

   if (Atype == A_INDEX_DISP)
      sib.base = 5;
   else
      sib.base  = RegisterNumber(Aref->bas);

}


/***************************************************************************/
/* This function will accept a displacement value and if non-zero emit     */
/* either one byte or two depending on wether it will fit in one byte or   */
/* not. Also if the mod byte encoding implies a 16bit disp ONLY, we     */
/* will force a 16 bit displacament, irrespective of size.                 */
/***************************************************************************/

static void 

   EmitDisp (long value)

   {

 
   if ((mod.mod == 0) && (mod.rm == 5)) /* ie disp32 & no base reg */
      {
      EmitData32(value);
      return;
      }

   if ((value == 0) && (mod.mod == 0))
      return;         

   if ((value != 0) && (mod.mod == 0))
      {
	  report(187,"",_LINE_);
	  return;
	  }

   if ((value < MIN_DISP8) || (value > MAX_DISP8))
      if (mod.mod == 2)
         {
         EmitData32(value);
         return;
         }
	  else
	     {
  	     report(188,"",_LINE_);
	     return;
         }

   EmitData8(value);

   }

/***************************************************************************/
/* This function will accept a displacement value and emit two bytes .     */
/***************************************************************************/

static void 

   EmitData32 (long value)

   {

   unsigned char * temp;

   temp = (unsigned char *) (&value);

   emit(temp[0]);
   emit(temp[1]);
   emit(temp[2]);
   emit(temp[3]);

   }

static void 

   EmitData16 (long value)

   {

   unsigned char * temp;

   if ((value < MIN_DISP16) || (value > MAX_DISP16))
      {
      report(186,"",_LINE_);
      longjmp(exit_code,3);
      }

   temp = (unsigned char *) (&value);

   emit(temp[0]);
   emit(temp[1]);

   }

/***************************************************************************/
/* These functions will emit a single byte whose value is that of the arg  */
/***************************************************************************/

static void 

   EmitData8 (long value)

   {

   chur      temp;

   if ((value < MIN_DISP8) || (value > MAX_DISP8))
      {
      report(184,"",_LINE_);
      longjmp(exit_code,3);
      }

   temp = (chur)value;

   emit (temp);

   }

/**************************************************************************/
/* Calculate the checksum for this objrec, and set the objrec length.     */
/* To determine the checksum we add EVERY other byte in the record.       */
/**************************************************************************/

static void 

   set_check_sum (void)

   {

   chur         count;
   short          I;
   short          *len_ptr;

   if (code_len == 0)
      return;

   count = 0;

   code_len++; /* we must set length field to reflect the extra */
               /* check sum byte.                               */
   len_ptr = (short *) &code_rec[REC_LENGTH];
   *len_ptr = code_len - 3;

   for (I=0; I < code_len; I++)
       count += code_rec[I];

   count = (-count);
   code_rec[code_len-1] = count; 

   }



/***************************************************************************/
/* Return a value indicating the magnitude of a numeric value as either    */
/* byte sized or word sized.                                               */
/***************************************************************************/

static short 

   data_size (long value)

   {

   if ((value < MIN_DISP16) || (value > MAX_DISP16))
      return (DWORD_PTR);

   if ((value < MIN_DISP8) || (value > MAX_DISP8))
      return (WORD_PTR);

   return(BYTE_PTR);

   }



/**************************************************************************/
/* This function will print an assembly language-like version of the      */
/* instruction.                                                           */
/**************************************************************************/

static void 

   print_assembly (void)

   {

   short      comma_needed = 0;
   short      op;

   operand_chars = 0; /* num of chars used when printing operand(s) */

   if (system_reqd == 0)
      return; 

   if (first_time_called)
      {
      first_time_called = 0;
      print_asm_heading();
      }

   print_offset(curr_offset);  /* curr_offset */

   print_code();

   print_operation (Inst.opcode);

   /*******************************************/
   /* Print this instructions target details. */
   /*******************************************/

   if ((GetTarget() == REGISTER) || (GetTarget() == SEGMENT))
      {
      print_register (Inst.target.reg);
      comma_needed = 1;
      }
   else
   if (GetTarget() == MEMORY)
      {
      comma_needed = 1;
      print_memory (Inst.target);
      }

   if (GetTarget() == IMMEDIATE)
      {
      if (comma_needed)
         operand_chars += fprintf(LISTING,",");
      print_imm(Inst.target.imm); 
      }

   op = Inst.opcode;

   if ((op == CAL)||(op == DEC)||(op == INC)||(op == POP)||(op == EXIT))
      {
      print_sundry();
      return;
      }

   if (Inst.target.reg == NDP)
      comma_needed = 0;

   if ((op == RET) && (Inst.source.imm == 0))
      {
      print_sundry();
      return;
      }

   /*******************************************/
   /* Print this instructions source details  */
   /*******************************************/

   if (GetAddrType(&Inst.source) == A_NOT_PRESENT)
      {
	  print_sundry();
	  return;
	  }

   if ((GetSource() == REGISTER) || (GetSource() == SEGMENT))
      {
      if (comma_needed)
         operand_chars += fprintf(LISTING,","); 
      print_register (Inst.source.reg);
      }
   else
   if (GetSource() == MEMORY)
      {
      if (comma_needed)
         operand_chars += fprintf(LISTING,",");
      print_memory (Inst.source);
      }
   else
   if (GetSource() == IMMEDIATE)
      {
      if (comma_needed)
         operand_chars += fprintf(LISTING,",");
      print_imm(Inst.source.imm); 
      }

   print_sundry();
              
   }

/**************************************************************************/
/* This function will print the headings for the assembler listing.       */
/**************************************************************************/

static void 

   print_asm_heading (void)

   {

   check_print(60); /* force new page */
   fprintf(LISTING,"\nASSEMBLY LISTING OF CODE GENERATED FOR Pentium Processor.\n");
   check_print(1);
    
   }


/**************************************************************************/
/*       Print any additional text that this instruction may have.        */
/**************************************************************************/

static void 
   
   print_sundry (void)

   {

   char     blanks[32];
   long     bytes;

   if (strlen(Inst.text) == 0)
      return;

   bytes = 25 - operand_chars;

   if (bytes <= 0)
      {
      return;
      }

   strnset(blanks,' ',bytes);  /* align Inst.text */

   blanks[(bytes)] = '\x00';
       
   fprintf (LISTING,"%s%s",blanks,Inst.text);

   }     

/**************************************************************************/
/* Print the code offset (within code segment) of the current instruction */
/**************************************************************************/

static void 

   print_offset (unsigned long offset)

   {

   unsigned long heof;

   heof = offset;

   if (strcmp(prev_line_no,line_no) == 0)
      {
      check_print(1);
      fprintf (LISTING,"\n    %08X  ",heof);
      }
   else
      {
      check_print(4);
      print_line_number();
      fprintf (LISTING,"\n\n    %08X  ",heof);
      }

   }

/*************************************************************************/
/* This function will print the machine code bytes that were generated   */
/* by the emiter for the current instruction.                           */
/*************************************************************************/

static void 

   print_code (void)

   {

   short         I,L;
   short         distance = 16;
   
   L = machine_bytes;

   for (I=0; I<L; I++)
       {               
       fprintf(LISTING,"%02X",machine_code[I]);
       /*************************************************************/
       /* If weve just printed the 1st byte, and a seg override was */
       /* specified, then print a colon.                            */
       /*************************************************************/
       if ((I == 0) && ((Inst.source.seg != 0) || (Inst.target.seg != 0)))
          {
          fprintf(LISTING,":");
          distance -= 1;
          }
       /***************************************************************/
       /* If weve just printed the 1st byte an no seg override was    */
       /* specified, then this is an opcode so print a space after it */
       /***************************************************************/
       if ((I == 0) && (Inst.source.seg == 0) && (Inst.target.seg == 0))
          {
          fprintf(LISTING," ");
          distance -= 1;
          }
       /***************************************************************/
       /* If weve just printed the 2nd byte, and a seg override was   */
       /* specified then this must be the opcode so print a space.    */
       /***************************************************************/ 
       if ((I == 1) && ((Inst.source.seg != 0) || (Inst.target.seg != 0)))
          {
          fprintf(LISTING," ");
          distance -= 1;
          }
       }

   L = distance - (2 * L); /* L now = space left on line */

   /********************************************/
   /* Fill in the remaining space with spaces. */
   /********************************************/

   for (I=0; I<L; I++)
       fprintf(LISTING," ");

   }     
   
/*************************************************************************/
/*        Print a memonic for the supplied opcode number.                */
/*************************************************************************/

static void 

   print_operation (Opcode op)

   {


    switch (op) {

    case(ENTER):
        fprintf(LISTING,"  ENTER    ");
        break;
    case(EXIT):
        fprintf(LISTING,"  LEAVE    ");
        break;
    case(ADC):
        fprintf(LISTING,"  ADC      ");
        break;
    case(ADD):
        fprintf(LISTING,"  ADD      ");
        break; 
    case(MUL):
        fprintf(LISTING,"  MUL      ");
        break;  
    case(DIV):
        fprintf(LISTING,"  DIV      ");
        break;
    case(SBB):
        fprintf(LISTING,"  SBB      ");
        break;
    case(SUB):
        fprintf(LISTING,"  SUB      ");
        break; 
    case(FILD):
        fprintf(LISTING,"# FILD     ");
        break; 
    case(FISTP):
        fprintf(LISTING,"# FISTP    ");
        break; 
    case(FIMUL):
        fprintf(LISTING,"# FIMUL    ");
        break; 
    case(MOV):
        fprintf(LISTING,"  MOV      ");
        break;
    case(PUSH):
        fprintf(LISTING,"  PUSH     ");
        break;
    case(POP):
        fprintf(LISTING,"  POP      ");
        break;
    case(JMP):
        fprintf(LISTING,"  JMP      ");
        break; 
    case(JLE):
        fprintf(LISTING,"  JLE      ");
        break;
    case(JL):
        fprintf(LISTING,"  JL       ");
        break;
    case(JA):
        fprintf(LISTING,"  JA       ");
        break;
    case(JE):
        fprintf(LISTING,"  JE       ");
        break;
    case(JG):
        fprintf(LISTING,"  JG       ");
        break;
    case(JNE):
        fprintf(LISTING,"  JNE      ");
        break;
    case(JGE):
        fprintf(LISTING,"  JGE      ");
        break;
	case(JNG):
	    fprintf(LISTING,"  JNG      ");
		break;
	case(JNL):
	    fprintf(LISTING,"  JNL      ");
		break;
	case(JNGE):
	    fprintf(LISTING,"  JNGE     ");
		break;
	case(JNLE):
	    fprintf(LISTING,"  JNLE     ");
		break;
    case(CAL):
        fprintf(LISTING,"  CALL     ");
        break;
    case(RET):
        fprintf(LISTING,"  RET      ");
        break;
    case(INT):
        fprintf(LISTING,"  INT      ");
        break;
    case(INC):
        fprintf(LISTING,"  INC      ");
        break;
    case(DEC):
        fprintf(LISTING,"  DEC      ");
        break;
    case(CMP):
        fprintf(LISTING,"  CMP      ");
        break;
    case(LEA):
        fprintf(LISTING,"  LEA      ");
        break;  
    default:
        {
        report(108,"",_LINE_); /* bad instruction */
        longjmp(exit_code,3);
        }

    }

   }

/**************************************************************************/
/* This function will print the name of a register to the listing.        */
/**************************************************************************/

static void 

   print_register (Reg reg)

   {

   operand_chars += 2;

   switch (reg) {

   case(_EAX):
      fprintf(LISTING,"EAX");
      break;
   case(_AX):
      fprintf(LISTING,"AX");
      break;
   case(_AL):
      fprintf(LISTING,"AL");
      break;
   case(_AH):
      fprintf(LISTING,"AH");
      break;

   case(_EBX):
      fprintf(LISTING,"EBX");
      break;
   case(_BX):
      fprintf(LISTING,"BX");
      break;
   case(_BL):
      fprintf(LISTING,"BL");
      break;
   case(_BH):
      fprintf(LISTING,"BH");
      break;

   case(_ECX):
      fprintf(LISTING,"ECX");
      break;
   case(_CX):
      fprintf(LISTING,"CX");
      break;
   case(_CL):
      fprintf(LISTING,"CL");
      break;
   case(_CH):
      fprintf(LISTING,"CH");
      break;

   case(_EDX):
      fprintf(LISTING,"EDX");
      break;
   case(_DX):
      fprintf(LISTING,"DX");
      break;
   case(_DL):
      fprintf(LISTING,"DL");
      break;
   case(_DH):
      fprintf(LISTING,"DH");
      break;

   case(_ESP):
      fprintf(LISTING,"ESP");
      break;
   case(_SP):             
      fprintf(LISTING,"SP");
      break;

   case(_EBP):
      fprintf(LISTING,"EBP");
      break;
   case(_BP):
      fprintf(LISTING,"BP");
      break;

   case(_ESI):
      fprintf(LISTING,"ESI");
      break;
   case(_SI):
      fprintf(LISTING,"SI");
      break;

   case(_EDI):
      fprintf(LISTING,"EDI");
      break;
   case(_DI):
      fprintf(LISTING,"DI");
      break;

   case(ES):
      fprintf(LISTING,"ES");
      break;
   case(DS):
      fprintf(LISTING,"DS");
      break;
   case(SS):
      fprintf(LISTING,"SS");
      break;
   case(CS):   
      fprintf(LISTING,"CS");
      break;
   case(NDP):
      break;
   default:
      {
      report(109,"",_LINE_);     /* bad reg */
      }

   }

  }


/**************************************************************************/
/* This function prints the correct memory reference for an instruction   */
/**************************************************************************/

static void 

   print_memory (Address addr)


   { 

   char cdis;

   if (addr.len == BYTE_PTR)
      operand_chars += fprintf(LISTING,"BYTE_PTR  ");
   if (addr.len == WORD_PTR)
      operand_chars += fprintf(LISTING,"WORD_PTR  ");
   if (addr.len == DWORD_PTR)
      operand_chars += fprintf(LISTING,"DWORD_PTR ");
   if (addr.len == QWORD_PTR)
      operand_chars += fprintf(LISTING,"QWORD_PTR ");


   if (addr.seg != 0)
      {
      if (addr.seg == SS)
         operand_chars += fprintf(LISTING,"SS: ");
      if (addr.seg == ES)
         operand_chars += fprintf(LISTING,"ES: ");
      }

   if ((addr.bas != 0) || (addr.idx != 0))
      {
      operand_chars += fprintf(LISTING,"[");
      /************************************************************/
      /* If both a base an index reg are present, print em both.  */
      /************************************************************/
      if ((addr.bas != 0) && (addr.idx != 0))  
         {
         print_register(addr.bas);
         operand_chars += fprintf(LISTING,"+");
         print_register(addr.idx);

		 if (addr.scale == 1)
		    operand_chars += fprintf(LISTING,"*%d",1);
		 if (addr.scale == 2)
		    operand_chars += fprintf(LISTING,"*%d",2);
		 if (addr.scale == 4)
		    operand_chars += fprintf(LISTING,"*%d",4);
		 if (addr.scale == 8)
		    operand_chars += fprintf(LISTING,"*%d",8);
	
         if (addr.dis > 0) 
		    if (addr.dis <= MAX_DISP8)
               operand_chars += fprintf(LISTING,"+%d",addr.dis);
			else
               operand_chars += fprintf(LISTING,"+%d",addr.dis);

         if (addr.dis < 0)
		    if (addr.dis >= MIN_DISP8)
			   {
			   cdis = (char)addr.dis;
               operand_chars += fprintf(LISTING,"%d",cdis);
			   }
			else
               operand_chars += fprintf(LISTING,"%d",addr.dis);


         operand_chars += fprintf(LISTING,"]");
         return;
         } 

      /************************************************************/
      /* If only one present then just print it.                  */
      /************************************************************/

      if (addr.bas != 0)
         {
         print_register(addr.bas);
         if (addr.dis > 0) 
		    if (addr.dis <= MAX_DISP8)
               operand_chars += fprintf(LISTING,"+%d",addr.dis);
			else
               operand_chars += fprintf(LISTING,"+%d",addr.dis);

         if (addr.dis < 0)
		    if (addr.dis >= MIN_DISP8)
               operand_chars += fprintf(LISTING,"%d",addr.dis);
			else
               operand_chars += fprintf(LISTING,"%d",addr.dis);


         operand_chars += fprintf(LISTING,"]");
         return;
         }

      if (addr.idx != 0)
         {
         print_register(addr.idx);

		 if (addr.scale == 1)
		    operand_chars += fprintf(LISTING,"*%d",1);
		 if (addr.scale == 2)
		    operand_chars += fprintf(LISTING,"*%d",2);
		 if (addr.scale == 4)
		    operand_chars += fprintf(LISTING,"*%d",4);
		 if (addr.scale == 8)
		    operand_chars += fprintf(LISTING,"*%d",8);

        if (addr.dis > 0) 
		    if (addr.dis < MAX_DISP8)
               operand_chars += fprintf(LISTING,"+%d",addr.dis);
			else
               operand_chars += fprintf(LISTING,"+%d",addr.dis);

         if (addr.dis < 0)
		    if (addr.dis > MIN_DISP8)
               operand_chars += fprintf(LISTING,"%d",addr.dis);
			else
               operand_chars += fprintf(LISTING,"%d",addr.dis);


         operand_chars += fprintf(LISTING,"]");
         return;
         }
            
      }

   if ((addr.bas != 0) || (addr.idx != 0))
      {
         if (addr.dis > 0) 
		    if (addr.dis <= MAX_DISP8)
               operand_chars += fprintf(LISTING,"+%d",addr.dis);
			else
               operand_chars += fprintf(LISTING,"+%d",addr.dis);

         if (addr.dis < 0)
		    if (addr.dis >= MIN_DISP8)
               operand_chars += fprintf(LISTING,"%d",addr.dis);
			else
               operand_chars += fprintf(LISTING,"%d",addr.dis);


       }

   /*********************************************************************/
   /* If base and index are zero then this memory reference MUST be via */
   /* a displacement only, possibly of zero.                            */
   /* Put brackets around the displacement to underline the fact that   */
   /* it means 'contents of' so as not to confuse with immediate.       */
   /*********************************************************************/

   if ((addr.bas == 0) && (addr.idx == 0))
      {
         if (addr.dis > 0) 
		    if (addr.dis <= MAX_DISP8)
               operand_chars += fprintf(LISTING,"+%02X",addr.dis);
			else
               operand_chars += fprintf(LISTING,"+%08X",addr.dis);

         if (addr.dis < 0)
		    if (addr.dis >= MIN_DISP8)
               operand_chars += fprintf(LISTING,"-%02X",addr.dis);
			else
               operand_chars += fprintf(LISTING,"-%08X",addr.dis);


       }
 
   }

/***************************************************************************/
/*         This function simply prints out an immediate value.             */
/***************************************************************************/

static void 

   print_imm (long data)

   {

   operand_chars += fprintf(LISTING,"%d",data);

   }

/***************************************************************************/
/* This function prints the line number for the line being translated.     */
/***************************************************************************/

static void 

   print_line_number (void)

   {

   if (strcmp(prev_line_no,line_no) == 0)
      return;

   strcpy(prev_line_no,line_no);

   if (Inst.opcode == ENTER)
      fprintf(LISTING,"\n\n    LINE %s              /* Prolog Sequence */",prev_line_no);
   else
   if (Inst.opcode == EXIT) // LEAVE
      fprintf(LISTING,"\n\n    LINE %s              /* Epilog Sequence */",prev_line_no);
   else
      fprintf(LISTING,"\n\n    LINE %s",prev_line_no);

   }

/***************************************************************************/
/*        This function writes a fixup record to the object file.          */
/***************************************************************************/

void 

   write_fixup (void)

   {

   chur            count;
   short           I;
   short          *len_ptr;

   if (fix_len <= 3) /* ie if no data in buffer yet */
      return;

   count = 0;

   fix_len++; /* we must set length field to reflect the extra */
               /* check sum byte.                               */
   len_ptr = (short *) &fix_rec[REC_LENGTH];
   *len_ptr = fix_len - 3;

   for (I=0; I < fix_len; I++)
       count += fix_rec[I];

   count = (-count);
   fix_rec[fix_len-1] = count; 

   //fwrite (fix_rec,fix_len,1,OB);

   bytes_read += fix_len;

   memset (fix_rec,0x00,2048);

   begin_fixup();

   }

/***************************************************************************/
/* This function simply initialises the header etc of a new fixup record.  */
/***************************************************************************/

void 

   begin_fixup (void)

   {

   fix_rec[0] = FIXUPP;

   fix_len = 3;

   }

/****************************************************************************/
/*                  Emit a segment override prefix opcode.                  */
/****************************************************************************/

static void 

   emit_override (void)

   {

   if (Inst.target.seg != 0)
      {
      emit(Inst.target.seg);
      return;
      }

   if (Inst.source.seg != 0)
      {
      emit(Inst.source.seg);
      return;
      }

   } 

/****************************************************************************/
/* This function will reposition into the object record and set the seglen  */
/* field to its correct value in the blocks SEGDEF record.                  */
/****************************************************************************/

void

   patch_code (long position,void * patch_ptr,short patch_len)

   {


   }


/****************************************************************************/
/*     Return the current physical offset within the object file itself.    */
/****************************************************************************/

long

   get_file_offset (void)

   {

   long           ofx;

   ofx = 0; //ftell(OB) + get_ledata_offset() + 6;

   return (ofx);

   }

/*--------------------------------------------------------------------------*/
/*                                GetAddrType                               */
/*--------------------------------------------------------------------------*/
/* This function returns a constant that indicates the type of address that */
/* instruction operand has. eg Immediate, Register, Indexex, Indexed with   */
/* displacement etc.                                                        */
/*--------------------------------------------------------------------------*/

static AddrType GetAddrType (Address * addr)

{

/*---------------------------------------------------------------------*/
/* Perform some basic validation thats independent of any other fields */
/*---------------------------------------------------------------------*/

if (NOT(addr->bas) &&
    NOT(addr->dis) &&
	NOT(addr->idx) &&
	NOT(addr->imm) &&
	NOT(addr->len) &&
	NOT(addr->reg) &&
	NOT(addr->scale) &&
	NOT(addr->seg))
	return(A_NOT_PRESENT);

if (NOT(addr->bas) &&
    NOT(addr->dis) &&
	NOT(addr->idx) &&
	NOT(addr->imm) &&
	   (addr->len) &&
	NOT(addr->reg) &&
	NOT(addr->scale) &&
	NOT(addr->seg))
	return(A_IMMEDIATE);

if ((addr->scale != 0) && 
    (addr->scale != 1) &&
    (addr->scale != 2) &&
    (addr->scale != 4) &&
    (addr->scale != 8))
    return(A_INVALID);

if ((addr->len != BYTE_PTR)  &&
    (addr->len != WORD_PTR)  &&
    (addr->len != DWORD_PTR) &&
	(addr->len != QWORD_PTR))
    addr->len = DWORD_PTR;  /* assume this as default */

/*---------------------------------------------------------------------------*/
/* If an effective address is IMMEDIATE then it may have no other components */
/*---------------------------------------------------------------------------*/

if (addr->imm)
   if (NOT(addr->bas) && 
       NOT(addr->dis) &&
	   NOT(addr->idx) &&
	   NOT(addr->reg) &&
	   NOT(addr->seg) &&
	   NOT(addr->scale))
   	   return(A_IMMEDIATE);
   else
       return(A_INVALID);

/*--------------------------------------------------------------------------*/
/* If an effective address is REGISTER then it may have no other components */
/*--------------------------------------------------------------------------*/

if (addr->reg)
   if (NOT(addr->bas) && 
       NOT(addr->dis) &&
	   NOT(addr->idx) &&
	   NOT(addr->imm) &&
	   NOT(addr->seg) &&
	   NOT(addr->scale))
   	   return(A_REGISTER);
   else
       return(A_INVALID);

/*------------------------------------------------------------------------*/
/* If an effective address is pure DISPLACEMENT then it cant have a scale */
/*------------------------------------------------------------------------*/

if (addr->dis)
   if (NOT(addr->bas) &&
       NOT(addr->idx))
	   if (addr->scale)
	      return(A_INVALID);
	   else
	      return(A_DISP);
   else
      if (addr->bas)
	     if (addr->idx)
		    return(A_BASE_INDEX_DISP);
		 else
		    return(A_BASE_DISP);
	  else
	     if (addr->scale <= 1)
		    {
			/*-----------------------------------------------------------------------*/
			/* In the case of an INDEX and no BASE then if scale <= 1 it is the same */
			/* as having a BASE and no INDEX.                                        */
			/*-----------------------------------------------------------------------*/
			addr->scale = 0;
			addr->bas = addr->idx;
			addr->idx = 0;
			return(A_BASE_DISP);
			}
		 else
		    return(A_INDEX_DISP);

if NOT(addr->dis)
   if (NOT(addr->bas) &&
       NOT(addr->idx))
	   return(A_INVALID);
   else
      if (addr->bas)
	     if (addr->idx)
		    return(A_BASE_INDEX);
		 else
		    return(A_BASE);
	  else
	     if (addr->scale <= 1)
		    {
			/*-----------------------------------------------------------------------*/
			/* In the case of an INDEX and no BASE then if scale <= 1 it is the same */
			/* as having a BASE and no INDEX.                                        */
			/*-----------------------------------------------------------------------*/
			addr->scale = 0;
			addr->bas = addr->idx;
			addr->idx = 0;
			return(A_BASE);
			}
		 else
		    return(A_INDEX);


return(A_INVALID);

}



/*----------------------------------------------------------------------------*/
/*                                GetAddrClass                                */
/*----------------------------------------------------------------------------*/
/* This function returns a constant that indicates the 'class' of an operands */
/* address. These are IMMediate, REGister or MEMory.                          */
/*----------------------------------------------------------------------------*/


static AddrClass GetAddrClass (AddrType atype)

{

if (atype == A_IMMEDIATE)
   return(IMM);

if (atype == A_REGISTER)
   return(REG);

return(MEM);

}

/*----------------------------------------------------------------------------*/
/*                                GetAddrClass                                */
/*----------------------------------------------------------------------------*/
/* This function returns the size of an opernads address, BYTE, WORD etc.     */
/*----------------------------------------------------------------------------*/

static AddrSize GetAddrSize (Address * addr)

{

AddrClass   CAddr;

CAddr = GetAddrClass(GetAddrType(addr));

if (CAddr == IMM)
   {
   if (addr->len == BYTE_PTR)
      return(IMM08);
   if (addr->len == WORD_PTR)
      return(IMM16);
   else
      return(IMM32);
   }

if (CAddr == REG)
   {
   if (ByteSize(addr->reg))
      return(REG08);
   if (WordSize(addr->reg))
      return(REG16);
   else
      return(REG32);
   }

if (addr->len == BYTE_PTR)
   return(MEM08);

if (addr->len == WORD_PTR)
   return(MEM16);

if (addr->len == DWORD_PTR)
   return(MEM32);

return(MEM64); /* ie QWORD PTR used by some FPU instructions */

}



/*----------------------------------------------------------------------------*/
/*                            GenTwoOperandInstruction                        */
/*----------------------------------------------------------------------------*/
/* This function takes and instruction and generates the bytes for it. It is  */
/* responsible for handling any two operand instruction.                      */
/*----------------------------------------------------------------------------*/

static void GenTwoOperandInstruction (Instruction inst)

{

AddrType		TFirst;
AddrType		TSecond;
AddrSize		SFirst;
AddrSize		SSecond;
unsigned short  oper;
chur			code;
chur            code1;
chur            code2;
chur            cmd;
chur            size_override;
opcode_map_ptr  opcode_ptr;
cmcode_map_ptr  cmcode_ptr;
AddrClass       CFirst;
AddrClass       CSecond;

size_override = 0;

TFirst  = GetAddrType (&inst.target);
TSecond = GetAddrType (&inst.source);

CFirst  = GetAddrClass(TFirst);
CSecond = GetAddrClass(TSecond);

if (TFirst == A_INVALID)
   {
   report(190,"",_LINE_); 
   longjmp(exit_code,3);
   }

if (TFirst == A_NOT_PRESENT)
   {
   report(191,"",_LINE_); 
   longjmp(exit_code,3);
   }

/*----------------------------------------------------*/
/* Classify the address type as mem, reg or immediate */
/*----------------------------------------------------*/

SFirst  = GetAddrSize(&inst.target);
SSecond = GetAddrSize(&inst.source);

if (TFirst == A_REGISTER) 
   {
   if (DwordSize(inst.target.reg) && (inst.target.len != DWORD_PTR))
      {
      report(199,"",_LINE_); 
      longjmp(exit_code,3);
	  }

   if (ByteSize(inst.target.reg) && (inst.target.len != BYTE_PTR))
      {
      report(200,"",_LINE_); 
      longjmp(exit_code,3);
	  }

   if (WordSize(inst.target.reg) && (inst.target.len != WORD_PTR))
      {
      report(201,"",_LINE_); 
      longjmp(exit_code,3);
	  }

   if (WordSize(inst.target.reg))
      size_override = 1;
   }

if (TSecond == A_REGISTER) 
   {
   if (DwordSize(inst.source.reg) && (inst.source.len != DWORD_PTR))
      {
      report(199,"",_LINE_); 
      longjmp(exit_code,3);
	  }

   if (ByteSize(inst.source.reg) && (inst.source.len != BYTE_PTR))
      {
      report(200,"",_LINE_); 
      longjmp(exit_code,3);
	  }

   if (WordSize(inst.source.reg) && (inst.source.len != WORD_PTR))
      {
      report(201,"",_LINE_); 
      longjmp(exit_code,3);
	  }

   if (WordSize(inst.source.reg))
      size_override = 1;
   }



/*--------------------------------------------------------------------------*/
/* Lookup this instructions raw opcode based upon its addressing attributes */
/*--------------------------------------------------------------------------*/

opcode_ptr = map_table[inst.opcode];

if (opcode_ptr == NULL)
   {
   report(194,"",_LINE_); 
   longjmp(exit_code,3);
   }
   
oper = (*opcode_ptr)[SFirst][SSecond];

if (oper <= 0xFF)
   {
   code1 = (chur)oper;
   code  = code1;
   }
else
   {
   code1 = (chur)(oper >> 8);
   code2 = (chur)(oper && 0x00FF);
   code  = code2;
   }

/*------------------------------------------------------------------------*/
/* Lookup the additional command code which tells us what special further */
/* actions need to be taken to finalise the instruction.                  */
/* This table is optional since many instructions do not require this.    */
/*------------------------------------------------------------------------*/

cmcode_ptr = cmd_table[inst.opcode];

if (cmcode_ptr != NULL)
   {
   cmd  = (*cmcode_ptr)[SFirst][SSecond];

   switch (cmd) {

   case(SR0):
   case(SR1):  
   case(SR2):
   case(SR3):
   case(SR4):
   case(SR5):
   case(SR6):
   case(SR7):
       {
	   mod.regop = cmd; /* ie a register number */
	   mod_reqd = 1;
	   break;
	   }
   case(ATR):
       {
	   code += RegisterNumber(inst.target.reg);  
	   break;
	   }
   case(ASR):
       {
	   code += RegisterNumber(inst.source.reg);
	   break;
       }
   case(SRS):
       {
	   mod.regop = RegisterNumber(inst.source.reg);  
	   mod_reqd = 1;
	   break;
	   }
   case(SRT):
       {
	   mod.regop = RegisterNumber(inst.target.reg);
	   mod_reqd = 1;
	   break;
       }
   case(MOD):
       {
	   mod_reqd = 1;
	   break;
	   }
   case(NOP):  
       break;
   default:
       {
       report(195,"",_LINE_); 
       longjmp(exit_code,3);
       return;
	   }

     }
   }


/* OK we can now emit the instruction itself */

if (size_override)
   emit(_66);

if (oper <= 0xFF)
   {
   emit(code);
   }
else
   {
   emit(code1);
   emit(code);
   }

EmitExtra();

if (TFirst == A_IMMEDIATE)
   {
   switch(SFirst) {
   case(IMM08):
       EmitData8(inst.target.imm);
       break;
   case(IMM16):
       EmitData16(inst.target.imm);
       break;
   case(IMM32):
       EmitData32(inst.target.imm);
       break;
       }
   }

if (TSecond == A_IMMEDIATE)
   {
   switch(SSecond) {
   case(IMM08):
       EmitData8(inst.source.imm);
       break;
   case(IMM16):
       EmitData16(inst.source.imm);
       break;
   case(IMM32):
       EmitData32(inst.source.imm);
       break;
       }
   }

if (CFirst == MEM)
   {
   if ((TFirst == A_DISP) || (TFirst == A_BASE_DISP) || (TFirst == A_BASE_INDEX_DISP))
      {
      if (mod.mod == 1)
          EmitData8(inst.target.dis);
      if ((mod.mod == 0) || (mod.mod == 2))
         EmitData32(inst.target.dis);
      }
   else
      if (TFirst == A_INDEX_DISP)
         EmitData32(inst.target.dis);
	     
   }

if (CSecond == MEM)
   {
   if ((TSecond == A_DISP) || (TSecond == A_BASE_DISP) || (TSecond == A_BASE_INDEX_DISP))
      {
      if (mod.mod == 1)
          EmitData8(inst.source.dis);
      if (mod.mod == 2)
         EmitData32(inst.source.dis);
      }
   else
      if (TSecond == A_INDEX_DISP)
         EmitData32(inst.source.dis);
	     
   }

}



/*----------------------------------------------------------------------------*/
/*                            GenOneOperandInstruction                        */
/*----------------------------------------------------------------------------*/
/* This function takes and instruction and generates the bytes for it. It is  */
/* responsible for handling any single operand instruction.                   */
/*----------------------------------------------------------------------------*/

static void GenOneOperandInstruction (Instruction inst)

{

AddrType		TFirst;
AddrType		TSecond;
AddrSize		SFirst;
chur			code;
chur            code1;
chur            code2;
unsigned short  oper;
chur            cmd;
char			size_override;
opcode_map_ptr  opcode_ptr;
cmcode_map_ptr  cmcode_ptr;
AddrClass       CFirst;
AddrClass       CSecond;

size_override = 0;

TFirst  = GetAddrType (&inst.target);
TSecond = GetAddrType (&inst.source);

CFirst  = GetAddrClass(TFirst);
CSecond = GetAddrClass(TSecond);

if (TFirst == A_INVALID)
   {
   report(190,"",_LINE_); 
   longjmp(exit_code,3);
   }

if (TFirst == A_NOT_PRESENT)
   {
   report(191,"",_LINE_); 
   longjmp(exit_code,3);
   }

/* There must not be a second operand */

if (TSecond != A_NOT_PRESENT)
   {
   report(192,"",_LINE_); 
   longjmp(exit_code,3);
   }

/*----------------------------------------------------*/
/* Classify the address type as mem, reg or immediate */
/*----------------------------------------------------*/

SFirst  = GetAddrSize(&inst.target);

if (TFirst == A_REGISTER) 
   {
   if (DwordSize(inst.target.reg) && (inst.target.len != DWORD_PTR))
      {
      report(199,"",_LINE_); 
      longjmp(exit_code,3);
	  }

   if (ByteSize(inst.target.reg) && (inst.target.len != BYTE_PTR))
      {
      report(200,"",_LINE_); 
      longjmp(exit_code,3);
	  }

   if (WordSize(inst.target.reg) && (inst.target.len != WORD_PTR))
      {
      report(201,"",_LINE_); 
      longjmp(exit_code,3);
	  }

   if (WordSize(inst.target.reg))
      size_override = 1;
   }



/*--------------------------------------------------------------------------*/
/* Lookup this instructions raw opcode based upon its addressing attributes */
/*--------------------------------------------------------------------------*/

opcode_ptr = map_table[inst.opcode];

if (opcode_ptr == NULL)
   {
   report(194,"",_LINE_); 
   longjmp(exit_code,3);
   }

oper = (*opcode_ptr)[SFirst][0];

if (oper <= 0xFF)
   {
   code1 = (chur)oper;
   code  = code1;
   }
else
   {
   code1 = (chur)(oper / 256);
   code2 = (chur)(oper % 256);
   code  = code2;
   }

/*------------------------------------------------------------------------*/
/* Lookup the additional command code which tells us what special further */
/* actions need to be taken to finalise the instruction.                  */
/* This table is optional since many instructions do not require this.    */
/*------------------------------------------------------------------------*/

cmcode_ptr = cmd_table[inst.opcode];

if (cmcode_ptr != NULL)
   {
   cmd  = (*cmcode_ptr)[SFirst][0];

   switch (cmd) {

   case(SR0):
   case(SR1):  
   case(SR2):
   case(SR3):
   case(SR4):
   case(SR5):
   case(SR6):
   case(SR7):
       {
	   mod.regop = cmd; /* ie a register number */
	   mod_reqd = 1;
	   break;
	   }
   case(ATR):
       {
	   code += RegisterNumber(inst.target.reg);  
	   break;
	   }
   case(ASR):
       {
       /* We shouldnt refer to source operand in this single operand instruction table */
       report(193,"",_LINE_); 
       longjmp(exit_code,3);
	   return;
       }
   case(MOD):
       {
	   mod_reqd = 1;
	   break;
	   }
   case(NOP):  
       break;
   default:
       {
       report(195,"",_LINE_); 
       longjmp(exit_code,3);
       return;
	   }

     }
   }

/* OK we can now emit the instruction itself */

if (size_override)
   emit(_66);

if (oper <= 0xFF)
   {
   emit(code);
   }
else
   {
   emit(code1);
   emit(code);
   }

EmitExtra();

if (TFirst == A_IMMEDIATE)
   {
   switch(SFirst) {
   case(IMM08):
       EmitData8(inst.target.imm);
       break;
   case(IMM16):
       EmitData16(inst.target.imm);
       break;
   case(IMM32):
       EmitData32(inst.target.imm);
       break;
       }
   }
else
if (CFirst == MEM)
   {
   if ((TFirst == A_DISP) || (TFirst == A_BASE_DISP) || (TFirst == A_BASE_INDEX_DISP))
      {
      if (mod.mod == 1)
          EmitData8(inst.target.dis);
      if ((mod.mod == 0) || (mod.mod == 2))
         EmitData32(inst.target.dis);
      }
   else
      if (TFirst == A_INDEX_DISP)
         EmitData32(inst.target.dis);
	     
   }

}

/*----------------------------------------------------------------------------*/
/*                            GenZeroOperandInstruction                       */
/*----------------------------------------------------------------------------*/
/* This function takes and instruction and generates the bytes for it. It is  */
/* responsible for handling any zero (ie none) operand instruction.           */
/*----------------------------------------------------------------------------*/

static void GenZeroOperandInstruction (Instruction inst)

{

AddrType		TFirst;
AddrType		TSecond;
chur			code;
chur            code1;
chur            code2;
unsigned short  oper;
opcode_map_ptr  opcode_ptr;

TFirst  = GetAddrType (&inst.target);
TSecond = GetAddrType (&inst.source);

if (TFirst != A_NOT_PRESENT)
   {
   report(191,"",_LINE_); 
   longjmp(exit_code,3);
   }

/* There must not be a second operand */

if (TSecond != A_NOT_PRESENT)
   {
   report(192,"",_LINE_); 
   longjmp(exit_code,3);
   }


/*--------------------------------------------------------------------------*/
/* Lookup this instructions raw opcode based upon its addressing attributes */
/*--------------------------------------------------------------------------*/

opcode_ptr = map_table[inst.opcode];

if (opcode_ptr == NULL)
   {
   report(194,"",_LINE_); 
   longjmp(exit_code,3);
   }

oper = (*opcode_ptr)[0][0]; /* Instructions with no operands, simply use this slot */

if (oper <= 0xFF)
   {
   code = (chur)oper;
   emit(code);
   }
else
   {
   code1 = (chur)(oper / 256);
   code2 = (chur)(oper % 256);
   emit(code1);
   emit(code2);
   }


}



