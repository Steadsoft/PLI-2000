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
/*           Replace definitions for the IA-32 register set.               */
/***************************************************************************/

typedef enum 
{
_AL=1,
_AX,
_EAX,
_CL,
_CX,
_ECX,
_DL,
_DX,
_EDX,
_BL,
_BX,
_EBX,
_AH,
_SP,
_ESP,
_CH,
_BP,
_EBP,
_DH,
_SI,
_ESI,
_BH,
_DI,
_EDI
} Reg;
  
// this was arbitrarily changed to 30 to stop a clash with the above
// HWG 07 Oct 02
# define NDP                    30 /* CoProcessor reg zero */ 

/* Segment control registers    */
/* segment override codes.      */

# define CS                     '\x2E'
# define DS                     '\x3E'
# define ES                     '\x26'
# define SS                     '\x36'

# define MAX_DISP8           +127    
# define MIN_DISP8           -128
# define MAX_DISP16          +32767  
# define MIN_DISP16          -32768

# define BYTE_PTR             1
# define WORD_PTR             2  
# define DWORD_PTR            3
# define QWORD_PTR            4

/***************************************************************************/
/*       This file defines the instruction set of the iAP-286 CPU.         */
/* All instruction nodes are defined in here.                              */
/***************************************************************************/

# define        UNSPECIFIED            0
# define        REGISTER               1
# define        IMMEDIATE              2
# define        SEGMENT                3
# define        MEMORY               255
# define        SOURCE                 0
# define        TARGET                 1

typedef enum 
{
_00,_01,_02,_03,_04,_05,_06,_07,_08,_09,_0A,_0B,_0C,_0D,_0E,_0F,
_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_1A,_1B,_1C,_1D,_1E,_1F,
_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_2A,_2B,_2C,_2D,_2E,_2F,
_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_3A,_3B,_3C,_3D,_3E,_3F,
_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_4A,_4B,_4C,_4D,_4E,_4F,
_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_5A,_5B,_5C,_5D,_5E,_5F,
_60,_61,_62,_63,_64,_65,_66,_67,_68,_69,_6A,_6B,_6C,_6D,_6E,_6F,
_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_7A,_7B,_7C,_7D,_7E,_7F,
_80,_81,_82,_83,_84,_85,_86,_87,_88,_89,_8A,_8B,_8C,_8D,_8E,_8F,
_90,_91,_92,_93,_94,_95,_96,_97,_98,_99,_9A,_9B,_9C,_9D,_9E,_9F,
_A0,_A1,_A2,_A3,_A4,_A5,_A6,_A7,_A8,_A9,_AA,_AB,_AC,_AD,_AE,_AF,
_B0,_B1,_B2,_B3,_B4,_B5,_B6,_B7,_B8,_B9,_BA,_BB,_BC,_BD,_BE,_BF,
_C0,_C1,_C2,_C3,_C4,_C5,_C6,_C7,_C8,_C9,_CA,_CB,_CC,_CD,_CE,_CF,
_D0,_D1,_D2,_D3,_D4,_D5,_D6,_D7,_D8,_D9,_DA,_DB,_DC,_DD,_DE,_DF,
_E0,_E1,_E2,_E3,_E4,_E5,_E6,_E7,_E8,_E9,_EA,_EB,_EC,_ED,_EE,_EF,
_F0,_F1,_F2,_F3,_F4,_F5,_F6,_F7,_F8,_F9,_FA,_FB,_FC,_FD,_FE,_FF,
};

typedef enum {
ADD=1,     
CAL,          
CMP,          
DEC,          
DIV,          
IN ,          
INC,          
INT,          
JA,   
JAE,        
JB, 
JBE,
JC,          
JE,           
JMP,          
JNZ,          
JZ,           
LOP,          
MOV,          
MUL,          
PUSH,          
RET,          
ROR,          
SUB,          
XCHG,         
POP,          
ADC,          
ENTER,        
EXIT,          /* 80286 Leave */
SBB,          
JLE,          
LEA,          
JL,             
JNE,          
JG,           
JGE,           
FILD,         
FIMUL,   
FIST,     
FISTP,        
JNG,			
JNL,			
JNGE,         
JNLE,         
FINAL,	
NOP=_90,	
WRITE_CODE=927, /* pseudo-op   */
START_CODE=928  /* pseudo-op   */
} Opcode;

typedef enum
{
A_INVALID,
A_NOT_PRESENT, /* The effective address isnt present, not invalid in many cases */
A_IMMEDIATE,
A_DISP,
A_REGISTER,
A_BASE,
A_INDEX,       /* Index without base can be true only when scale is > 1 */
A_BASE_DISP,
A_INDEX_DISP,  /* Index without base can be true only when scale is > 1 */
A_BASE_INDEX,
A_BASE_INDEX_DISP,
} AddrType;


typedef enum
{
IMM,
REG,
MEM
} AddrClass;

typedef enum
{
IMM08,
IMM16,
IMM32,
REG08,
REG16,
REG32,
MEM08,
MEM16,
MEM32,
MEM64,
MEMNUM
} AddrSize;

typedef enum
{
SR0,  /* Set reg field to zero  */
SR1,  /* Set reg field to one.. */
SR2,
SR3,
SR4,
SR5,
SR6,
SR7,
SRT,  /* Set mod.reg = target register number */
SRS,  /* Set mod.reg = source register number */
MOD,  /* Mod R/M byte required                */
ATR,  /* Add target register number to opcode */
ASR   /* Add source reguster nuber to opcode  */
} OpCmd;

/*                 FIRST   SECOND */

typedef unsigned short  opcode_map[MEMNUM][MEMNUM]; /* some opcodes require two instruction bytes */
typedef chur            cmcode_map[MEMNUM][MEMNUM];
typedef opcode_map *    opcode_map_ptr;
typedef cmcode_map *    cmcode_map_ptr;

                     static opcode_map mov_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { _B0,  NOP,  NOP,  _8A,  NOP,  NOP,  _8A,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  _B8,  NOP,  NOP,  _8B,  NOP,  NOP,  _8B,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  _B8,  NOP,  NOP,  _8B,  NOP,  NOP,  _8B,  NOP },
/* MEM08  */ { _C6,  NOP,  NOP,  _88,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  _C7,  NOP,  NOP,  _89,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  _C7,  NOP,  NOP,  _89,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map mov_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { ATR,  NOP,  NOP,  SRT,  NOP,  NOP,  MOD,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  ATR,  NOP,  NOP,  SRT,  NOP,  NOP,  MOD,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  ATR,  NOP,  NOP,  SRT,  NOP,  NOP,  MOD,  NOP },
/* MEM08  */ { SR0,  NOP,  NOP,  MOD,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  SR0,  NOP,  NOP,  MOD,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  SR0,  NOP,  NOP,  MOD,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static opcode_map add_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  _02,  NOP,  NOP,  _02,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  _03,  NOP,  NOP,  _03,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  _03,  NOP,  NOP,  _03,  NOP },
/* MEM08  */ { _80,  NOP,  NOP,  _00,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _83,  _81,  NOP,  NOP,  _01,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _83,  NOP,  _81,  NOP,  NOP,  _01,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map add_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP },
/* MEM08  */ { SR0,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR0,  SR0,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR0,  NOP,  SR0,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};



                     static opcode_map div_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { _F6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { _F7,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { _F7,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { _F6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _F7,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _F7,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map div_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { SR6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { SR6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { SR6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { SR6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};




                     static opcode_map mul_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { _F6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { _F7,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { _F7,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { _F6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _F7,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _F7,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map mul_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { SR4,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { SR4,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { SR4,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { SR4,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR4,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR4,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};







                     static opcode_map adc_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  _12,  NOP,  NOP,  _12,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  _13,  NOP,  NOP,  _13,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  _13,  NOP,  NOP,  _13,  NOP },
/* MEM08  */ { _80,  NOP,  NOP,  _10,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _83,  _81,  NOP,  NOP,  _11,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _83,  NOP,  _81,  NOP,  NOP,  _11,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map adc_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP },
/* MEM08  */ { SR2,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR2,  SR2,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR2,  NOP,  SR2,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};


                     static opcode_map sub_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  _2A,  NOP,  NOP,  _2A,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  _2B,  NOP,  NOP,  _2B,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  _2B,  NOP,  NOP,  _2B,  NOP },
/* MEM08  */ { _80,  NOP,  NOP,  _28,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _83,  _81,  NOP,  NOP,  _29,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _83,  NOP,  _81,  NOP,  NOP,  _29,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map sub_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP },
/* MEM08  */ { SR5,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR5,  SR5,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR5,  NOP,  SR5,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static opcode_map sbb_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  _1A,  NOP,  NOP,  _1A,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  _1B,  NOP,  NOP,  _1B,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  _1B,  NOP,  NOP,  _1B,  NOP },
/* MEM08  */ { _80,  NOP,  NOP,  _18,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _83,  _81,  NOP,  NOP,  _19,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _83,  NOP,  _81,  NOP,  NOP,  _19,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map sbb_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP },
/* MEM08  */ { SR3,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR3,  SR3,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR3,  NOP,  SR3,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};



                     static opcode_map lea_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  _8D,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map lea_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  MOD,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static opcode_map dec_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { _48,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { _48,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { _FE,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map dec_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { ATR,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { ATR,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { SR1,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR1,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR1,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};



                     static opcode_map inc_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { _40,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { _40,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { _FE,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map inc_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { ATR,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { ATR,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { SR0,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR0,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR0,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};



                     static opcode_map  push_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _6A,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { _68,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { _68,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { _50,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { _50,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map push_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { ATR,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { ATR,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR6,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                    static opcode_map  fild_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _DF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _DB,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { _DF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
};

                     static cmcode_map fild_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR0,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR0,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { SR5,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};


                    static opcode_map  fist_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _DF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _DB,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
};

                     static cmcode_map fist_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR2,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR2,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                    static opcode_map  fistp_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _DF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _DB,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { _DF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
};

                     static cmcode_map fistp_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR3,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR3,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { SR7,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};



                    static opcode_map  pop_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { _58,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { _58,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _8F,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _8F,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map pop_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { ATR,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { ATR,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR0,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR0,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};


                    static opcode_map  call_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { _E8,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { _E8,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map call_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { SR2,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { SR2,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR2,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR2,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                    static opcode_map  jmp_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _EB,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { _E9,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { _E9,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _FF,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jmp_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { SR4,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { SR4,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR4,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR4,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                    static opcode_map  ja_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _77,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF87, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF87, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map ja_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jae_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _73,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF83, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF83, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jae_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jb_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _72,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF82, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF82, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jb_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jbe_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _76,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF86, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF86, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jbe_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};


                   static opcode_map  jc_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _72,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF82, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF82, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jc_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  je_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _74,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF84, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF84, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map je_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jz_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _74,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF84, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF84, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jz_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jg_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _7F,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF8F, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF8F, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jg_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jge_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _7D,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF8D, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF8D, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jge_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jl_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _7C,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF8C, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF8C, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jl_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jle_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _7E,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF8E, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF8E, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jle_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jne_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _75,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF85, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF85, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jne_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jng_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _7E,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF8E, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF8E, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jng_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jnl_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _7D,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF8D, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF8D, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jnl_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jnge_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _7C,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF8C, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF8C, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jnge_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  jnle_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _7F,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ {0xF8F, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ {0xF8F, NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map jnle_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                   static opcode_map  ret_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { _C3,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { _C2,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map ret_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};


                   static opcode_map  enter_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { _C8,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map enter_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                    static opcode_map  cmp_map = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  _3A,  NOP,  NOP,  _3A,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  _3B,  NOP,  NOP,  _3B,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  _3B,  NOP,  NOP,  _3B,  NOP },
/* MEM08  */ { _80,  NOP,  NOP,  _38,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { _83,  _81,  NOP,  NOP,  _39,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { _83,  NOP,  _81,  NOP,  NOP,  _39,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

                     static cmcode_map cmp_ops = { 
/*--------------------------------------------------------------------------*/
/*             IMM08 IMM16 IMM32 REG08 REG16 REG32 MEM08 MEM16 MEM32 MEM64  */
/*--------------------------------------------------------------------------*/ 
/* IMM08  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM16  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* IMM32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* REG08  */ { NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP,  NOP,  NOP },
/* REG16  */ { NOP,  NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP,  NOP },
/* REG32  */ { NOP,  NOP,  NOP,  NOP,  NOP,  SRT,  NOP,  NOP,  SRT,  NOP },
/* MEM08  */ { SR7,  NOP,  NOP,  SRS,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM16  */ { SR7,  SR7,  NOP,  NOP,  SRS,  NOP,  NOP,  NOP,  NOP,  NOP },
/* MEM32  */ { SR7,  NOP,  SR7,  NOP,  NOP,  SRS,  NOP,  NOP,  NOP,  NOP },
/* MEM64  */ { NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP,  NOP }, 
};

static opcode_map_ptr map_table[FINAL];
static cmcode_map_ptr cmd_table[FINAL];

typedef struct 
{
unsigned  rm:     3;   /* bits 0 1 2 */
unsigned  regop:  3;   /* bits 3 4 5 */
unsigned  mod:    2;   /* bits 6 7   */
} Mrm;


typedef struct 
{
unsigned  base:   3;   /* bits 0 1 2 */
unsigned  index:  3;   /* bits 3 4 5 */
unsigned  scale:  2;   /* bits 6 7   */
} Sib;


typedef struct {
        long   imm; /* used for JMP                   */
        chur   len; /* analogous to WORD PTR/BYTE PTR */
	chur   scale;
        Reg    reg;
        long   dis;
        Reg    idx;
        Reg    bas;
        Reg    seg; /* Used if segment overrides reqd..*/
} Address, * Address_ptr;


typedef struct {
Opcode		opcode;
Address		source;
Address		target;
char		text[256];
} Instruction;


/****************************** end of include *****************************/
