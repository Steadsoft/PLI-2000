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
/*            PL/1 Compiler for the IBM Personal Computer.                 */
/***************************************************************************/
/* Copyright Vulkan Technologies Ltd 1990.                                 */
/***************************************************************************/
/*                                                                         */
/*                                                                         */
/* This module contains functions that are primarily concerned with the    */
/* symbol table management and variable context verification.              */
/* All  symbol table management functions are defined in here.             */
/***************************************************************************/

/***************************************************************************/
/*                         Modification History                            */
/***************************************************************************/
/*                                                                         */
/*  When       Who                       What                              */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/*  20-09-90   HWG       Initial Version.                                  */
/*  30-04-91   HWG       Number of dimensions field added to symtab.       */
/*  14-05-91   HWG       Builtin functions table and function added.       */
/*  18-07-91   HWG       Error reporting was erroneous for fixed/float     */
/*                       decimal, precision violations.                    */
/*  15-09-91   HWG       Referenced flag set to value of zero.             */
/*  18-10-91   HWG       All automatics now start at offset 2 in the frame */
/*                       offset 0 is reserved for holding DS at entry time.*/
/*                                                                         */
/*  03-02-92   HWG       Symbol insertion modified so that all names are   */
/*                       in ascending alphabetical order.                  */
/***************************************************************************/

/***************************************************************************/
/*                   I N C L U D E      F I L E S                          */
/***************************************************************************/

# include <malloc.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "c_types.h"
# include "nodes.h"
# include "symtab.h"
# include "tokens.h"
# include "lex.h"

# define _LINE_     ((short)__LINE__)


/***************************************************************************/
/*                 S T A T I C    D E C L A R A T I O N S                  */
/***************************************************************************/

extern   short       lookahead_in_progress;
extern   short       trace_heap;
unsigned long        symtab_heap_used  = 0;
Block_ptr            block_root = NULL;
Symbol               dummy_symbol;

/***************************************************************************/
/*       E X T E R N A L L Y    D E F I N E D    P R O T O T Y P E S       */
/***************************************************************************/

/***************************************************************************/
/*     I N T E R N A L L Y      D E F I N E D    P R O T O T Y P E S       */
/***************************************************************************/

/* Symbol_ptr resolve_reference (Block_ptr, char *);    */
/* Symbol_ptr resolve_name      (Symbol_ptr,char *);    */
short        qualified         (char *);  /* see pass1.c */
char      *lexeme_ptr;       /* Ptr to lexeme text */
char      *locate_lexeme     (char[]);
short       compare             (Symbol_ptr,Symbol_ptr);

void report (short, char *, short);

Lex_ptr item_root_ptr = NULL;

#define BUILTINS  72
#define EARLIER   1
#define LATER    -1
#define SAME      0
 
static char  builtins[BUILTINS][10] = {
      "abs",     /* arithmetic */
      "ceil",
      "divide",
      "exp",
      "floor",
      "log",
      "max",
      "min",
      "mod",
      "round",
      "sign",
      "sqrt",
      "trunc",
      "acos",    /* trig */
      "asin",
      "atan",
      "atand",
      "atanh",
      "cos",
      "cosd",
      "cosh",
      "sin",
      "sind",
      "sinh",
      "tan",
      "tand",
      "tanh",
      "bool",    /* string */
      "collate",
      "copy",
      "index",
      "length",
      "ltrim",
      "maxlength",
      "rtrim",
      "scaneq",
      "scanne",
      "search",
      "string",
      "substr",
      "translate",
      "valid",
      "verify",
      "binary",  /* conversion */
      "bit",
      "byte",
      "character",
      "decimal",
      "fixed",
      "float",
      "rank",
      "oncode",  /* conditions */
      "onfile",
      "onkey",
      "onloc",
      "addr",    /* pointer funcs */
      "addrel",
      "null",
      "pointer",
      "rel",
      "dimension", /* array funcs */
      "hbound",
      "lbound",
      "bytesize",  /* misc */
      "date",
      "lineno",
      "pageno",
      "size",
      "time",
      "unspec"};

/**************************************************************************/
/*    This function will add, a new entry to the symtab for 'procedure'   */
/**************************************************************************/

Symbol_ptr add_symbol (Block_ptr proc_ptr,
                       char * spelling,
                       short token,
                       short keyword,
                       short length,
                       short scale,
                       short class,
                       short type,
		               short scope,
                       short declared,
                       short level,
                       Symbol_ptr prnt_ptr,
                       char line[10])
/*
     Block_ptr      proc_ptr;
     char          *spelling;
     short          token;
     short          keyword;
     short          length;
     short          scale;
     short          class;
     short          type;
     short          scope;
     short          declared;
     short          level;
     Symbol_ptr     prnt_ptr;
     char           line[10];
*/
     {



     /*****************************************************************/
     /*              L O C A L     D E C L A R A T I O N S            */
     /*****************************************************************/ 

     Symbol_ptr      symbol_ptr;
     Symbol_ptr      temp_ptr;
     Symbol_ptr      last_ptr;
 
     short             rel;

     /*****************************************************************/
     /*        S T A R T    O F   E X E C U T A B L E    C O D E      */
     /*****************************************************************/

     if (proc_ptr == NULL)
         proc_ptr = block_root;  /* Builtin refs are added in this way */

     if (lookahead_in_progress)
        return(&dummy_symbol);


     if (trace_heap)
        printf("Request to allocate a Symbol node\n");


     symbol_ptr = (Symbol_ptr) malloc (sizeof(Symbol));

     if (symbol_ptr == NULL)
        {
        report(53,"",_LINE_);
        return (NULL);  /* Heap exhausted */
        }

     symtab_heap_used += sizeof(Symbol);

     /**************************************************************/
     /* OK We have created a new PL/1 symbol entry, so build it    */
     /**************************************************************/

     symbol_ptr->spelling = spelling;  /* copy the ptr */

     /* strcpy(symbol_ptr -> spelling,spelling); */

     strcpy(symbol_ptr -> line,line);

     symbol_ptr -> node_type  = SYMBOL;
     symbol_ptr -> bad_dcl    = 0;
     symbol_ptr -> token      = token;
     symbol_ptr -> keyword    = keyword;
     symbol_ptr -> bytes      = length;
     symbol_ptr -> prec_1     = 0;
/*     symbol_ptr -> value_reg  = 0;
       symbol_ptr -> locn_reg   = 0;   */
     symbol_ptr -> prec_2     = 0; 
     symbol_ptr -> class      = class;
     symbol_ptr -> type       = type;
     symbol_ptr -> scope      = scope;
     symbol_ptr -> scale      = scale;
     symbol_ptr -> num_dims   = 0;
     symbol_ptr -> aligned    = 1;         /* default */
     symbol_ptr -> asterisk   = 0;
     symbol_ptr -> declarator = proc_ptr;
     symbol_ptr -> declared   = declared;
     symbol_ptr -> temporary  = 0;
     symbol_ptr -> prev_ptr   = NULL;
     symbol_ptr -> next_ptr   = NULL;
     symbol_ptr -> offset     = 0;
     symbol_ptr -> ext_code_idx = 0;
     symbol_ptr -> ext_static_idx = 0;
	 symbol_ptr -> coff_symtab_idx = 0;
	 symbol_ptr -> coff_strtab_idx = 0;
	 symbol_ptr -> cons_idx   = 0;
     symbol_ptr -> known_size = 0;
     symbol_ptr -> known_locn = 0;
     symbol_ptr -> qualified  = 0;
     symbol_ptr -> varying    = 0;
	 symbol_ptr -> initial    = 0;
     symbol_ptr -> variable   = 0;
     symbol_ptr -> vola_tile  = 0;
     symbol_ptr -> computational = 0;
     symbol_ptr -> file       = 0;
     symbol_ptr -> level      = level;
     symbol_ptr -> parent     = prnt_ptr;
     symbol_ptr -> sister     = NULL;
     symbol_ptr -> child      = NULL;
     symbol_ptr -> array_ptr  = NULL;
     symbol_ptr -> proc_ptr   = NULL;
     symbol_ptr -> pic_text   = NULL;
     symbol_ptr -> defbas_ptr = NULL;
     symbol_ptr -> referenced = 0;
     symbol_ptr -> init_ptr   = NULL;
     symbol_ptr -> ret_ptr    = NULL;

 
     /*****************************************************************/
     /* If the token passed in here, is NUMBER or STRING, then we set */
     /* the type attribute to CONSTANT.                               */
     /*****************************************************************/ 

     if ((token == STRING) || (token == NUMERIC) || (token == BIT_STRING))
        {
        symbol_ptr->class    = CONSTANT;
        symbol_ptr->type     = token;
        symbol_ptr->scope    = INTERNAL;
        symbol_ptr->level    = 0;
        symbol_ptr->declared = 1; /* declared by appearance ! */
        }

     if (level == 0)
        symbol_ptr->structure = 0;
     else
        symbol_ptr->structure = 1; 

     /****************************************************************/
     /* We must now insert the new symbol entry into the procs chain */
     /* NOTE that only level '1' structures are linked into the      */
     /* symbol list in a given block, structure members all point to */
     /* this level '1' name.                                         */
     /****************************************************************/

     if ((symbol_ptr->level) < 2) /* ie scalars and level 1 names */
        {
        if (proc_ptr -> first_symbol == NULL)
           {
           /***********************************************/
           /*   The first symbol entry for this block !   */
           /***********************************************/
           proc_ptr -> first_symbol = symbol_ptr;
           }
        else
           {
           /**************************************************************/
           /* Insert the symbol into its correct lexical position, the   */
           /* list is effectively arranged in ascending alphabetic order */
           /**************************************************************/

           temp_ptr = proc_ptr->first_symbol;
           last_ptr = proc_ptr->first_symbol;

           while (temp_ptr != NULL)
                 {
                 rel = compare(symbol_ptr,temp_ptr);
               
                 if (rel == EARLIER)
                    {
                    if (temp_ptr->prev_ptr == NULL) /* ie 1st */
                       {
                       symbol_ptr->next_ptr   = temp_ptr;
                       proc_ptr->first_symbol = symbol_ptr;
                       temp_ptr->prev_ptr     = symbol_ptr;
                       goto inserted;       
                       }
                    else
                       {
                       symbol_ptr->next_ptr = temp_ptr;
                       symbol_ptr->prev_ptr = temp_ptr->prev_ptr;
                       temp_ptr->prev_ptr   = symbol_ptr;
                       symbol_ptr->prev_ptr->next_ptr = symbol_ptr; 
                       goto inserted; 
                       } 
                    }

                 last_ptr = temp_ptr;
                 temp_ptr = temp_ptr->next_ptr;
                 }    

           /***********************************************************/
           /* OK The new symbol must go at the 'end' of the list.     */
           /***********************************************************/

           if (last_ptr->prev_ptr == NULL) /* ie 1st */
              {
              symbol_ptr->next_ptr   = last_ptr;
              proc_ptr->first_symbol = symbol_ptr;
              last_ptr->prev_ptr     = symbol_ptr;
              goto inserted;       
              }
           else
              {
              symbol_ptr->prev_ptr = last_ptr;
              symbol_ptr->next_ptr = NULL;
              last_ptr->next_ptr   = symbol_ptr;
              goto inserted;
              } 
           }
        }
     else
        {
        /*****************************************************************/
        /* If this symbol is a structure member, then link it into that  */
        /* structures symbol tree.                                       */
        /*****************************************************************/

        if ((prnt_ptr -> child) == NULL)
           {
           /*********************************************************/
           /* This is the first member at this structure level      */
           /*********************************************************/
           prnt_ptr -> child = symbol_ptr;
           prnt_ptr -> bytes += symbol_ptr -> bytes;
           }
        else
           {
           /********************************************/
           /* Add this symbol to the tail of the chain */
           /********************************************/
           prnt_ptr -> bytes += symbol_ptr -> bytes;
           temp_ptr = prnt_ptr->child;

           while (temp_ptr->sister != NULL)
                 {
                 temp_ptr = temp_ptr->sister;
                 }
           temp_ptr->sister = symbol_ptr;
     /*       symbol_ptr -> sister = prnt_ptr -> child;
           prnt_ptr -> child = symbol_ptr;   */
           }
        }

inserted:     

     (proc_ptr -> sym_count)++;

     return(symbol_ptr);

     }

 
/**************************************************************************/
/*    This function creates and inserts a block node in the symtab.       */
/**************************************************************************/

Block_ptr insert_block (Block_ptr current_ptr,
                        char block_name[32],
                        short func_switch,
                        short recur_switch,
                        short arg_count,
                        short sym_count)
     {

     Block_ptr    block_ptr;

     /******************************************************************/
     /*        Firstly see if we CAN allocate a new block node !       */
     /******************************************************************/

     if (trace_heap)
         printf("Request to allocate a Block node\n");

     
     block_ptr = (Block_ptr) malloc (sizeof(Block)); 

     if (block_ptr == NULL)
        {
        report(53,"",_LINE_);
        return(NULL); /* Heap exhausted */
        }
     
     symtab_heap_used += sizeof(Symbol);

     /******************************************************************/
     /* If the current_ptr is NULL, then this is the very first block  */
     /******************************************************************/

     if (current_ptr == NULL)
        {
        block_root = block_ptr;
        block_ptr -> depth = 0;
        }
     else
        block_ptr -> depth += current_ptr -> depth;
                
     strcpy (block_ptr->block_name,block_name);

     block_ptr -> parent       = current_ptr;
     block_ptr -> first_symbol = NULL; /* New block, no dcls (yet) */
     block_ptr -> first_stmt   = NULL;
     block_ptr -> function     = func_switch;
     block_ptr -> recursive    = recur_switch;
     block_ptr -> begin        = 0;
     block_ptr -> main         = 0;
     block_ptr -> called       = 0;
     block_ptr -> num_args     = arg_count;
     block_ptr -> sym_count    = sym_count;
     block_ptr -> sister       = NULL; /* none yet */
     block_ptr -> child        = NULL; /* none yet */
     block_ptr -> stack        = 4; /* all vars start here, prev 4 bytes */
                                    /* used to hold DS at entry to block */
                                    /* and on-unit ptr.                  */
     block_ptr -> stattic      = 2; /* Linker need static to exist */   
     block_ptr -> seg_pos      = 0;
     block_ptr -> params       = 0; 
     block_ptr -> ret_ptr      = NULL;
     block_ptr -> num_rets     = 0;
     block_ptr -> stack_idx    = 0;
     block_ptr -> data_idx     = 0;
     block_ptr -> code_idx     = 0;

     if (current_ptr == NULL)
        return(block_ptr);

     /********************************************************************/
     /* Well this isnt the root-block, so we must insert in parents list */
     /********************************************************************/

     if ((current_ptr -> child) == NULL)
        {
        /*********************************************************/
        /* This is the first child block for this current block  */
        /*********************************************************/
        current_ptr -> child = block_ptr;
        }
     else
        {
        block_ptr -> sister = current_ptr -> child;
        current_ptr -> child = block_ptr;
        }
      
     return (block_ptr);
     
     }  

/**************************************************************************/
/* This function searches the symbol list, for the required name, in  the */
/* specified block.                                                       */
/**************************************************************************/

Symbol_ptr get_symbol (Block_ptr current_block,
                       char * symbol_name)

      {

      Symbol_ptr  temp_ptr;

      if (current_block == NULL)
         return (NULL);

      temp_ptr = current_block -> first_symbol;

      
      while (temp_ptr != NULL)
            {
            if (strcmp(symbol_name,(temp_ptr->spelling)) == 0)
               return(temp_ptr);
            temp_ptr = temp_ptr -> next_ptr;
            }

      /* we didint find it so. . . .  */

      return (NULL);

   }
    
       
/***************************************************************************/
/*            Return true, if 'name' represents a builtin function.        */
/***************************************************************************/
 
short builtin (char * name)

    {

    short     I;

    for (I=0; I <= BUILTINS;I++)
        {
        if (*name == builtins[I][0])
           {
           if (strcmp(name,builtins[I]) == 0)
              return(1);
           }
        }

   return(0);

   }
         

/**************************************************************************/
/* Compare the textual spelling of two symbols, and return a value that   */
/* indicate their relative lexical position.                              */
/**************************************************************************/ 

short

   compare (Symbol_ptr s1,Symbol_ptr s2)

   {

   short         val;

   val = stricmp (s1->spelling,s2->spelling);

   if (val > 0)
      return(LATER);

   if (val < 0)
      return(EARLIER);

   return(SAME);

   }

