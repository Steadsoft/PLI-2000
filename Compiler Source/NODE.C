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
/*  22-02-91   HWG       Initial Version.                                  */
/*  29-06-91   HWG       Real memory is only allocated when the external   */
/*                       variable 'lookahead_in_progress' is false.        */
/*  19-09-91   HWG       trace_heap flag examined to aid debugging.        */
/*                                                                         */
/*  04-01-92   HWG       Code introduced to control the freeing of nodes.  */
/*                                                                         */
/*  11-07-92   HWG5      When moving nodes around in pass2, an if stmt     */
/*                       with no else (ie stmt_ptr = else_ptr) was being   */
/*                       modified so that stmt_ptr pointed somewhere new   */
/*                       but else_ptr didnt ! This gave the codegen the    */
/*                       impression that the 'if' actually had an else.    */
/*                                                                         */  
/***************************************************************************/
/***************************************************************************/
/*                        I N C L U D E    F I L E S                       */
/***************************************************************************/

# include "stdio.h"
# include "string.h"
# include "stdlib.h"
# include "c_types.h"
# include "nodes.h"
# include "symtab.h"
# include "tokens.h"
# include "setjmp.h"            

# define _LINE_     ((short)__LINE__)


/***************************************************************************/
/*                E X T E R N A L     V A R I A B L E S                    */
/***************************************************************************/

extern char          line_no[10];
extern short         lookahead_in_progress;
extern short         unrecoverable_error;
extern jmp_buf       escape; /* see p1.c */
extern short         trace_heap; 

/***************************************************************************/
/*                   D E F I N E D     S Y M B O L S                       */
/***************************************************************************/


/***************************************************************************/
/*       I N T E R N A L L Y    D E F I N E D    P R O T O T Y P E S       */
/***************************************************************************/


/***************************************************************************/
/*            L O C A L     S T A T I C    V A R I A B L E S               */
/***************************************************************************/

char           generic_buffer[256]; /* dummy memory area */
unsigned long  node_heap_used  = 0;
unsigned long  nodes_allocated = 0;
Procedure_ptr  parse_root      = NULL; /* Root of parse tree */
short          priority    (Any_ptr);  /* see scanner.c */
Any_ptr        malloc_node (short);
short          nodetype    (Any_ptr);
void           insert_node (Any_ptr,Any_ptr);
Any_ptr        next_stmt   (Any_ptr);

void report    (short, char *, short);

/***************************************************************************/
/*        S T A R T   O F   E X E C U T A B L E    F U N C T I O N S       */
/***************************************************************************/

/***************************************************************************/
/* This function simply returns the type of the node pointed to by the ptr */
/* in_ptr. EVERY node structure begins with an integer field that denotes  */
/* the type. So despite the fact that each node  has a different layout    */
/* we can always correctly reference a node, by first determining its type */
/***************************************************************************/

short nodetype (Any_ptr in_ptr)

    {

    short          type;

    if (in_ptr == NULL)
       return (UNKNOWN);

    type = (* (short *) in_ptr);

    if ((type < 0) || (type > MAX_TOKEN)) /* Trap memory corruptions ! */
       {
       report (81,"",_LINE_);
       return(UNKNOWN);
       }

    return (type);
    
    }

/****************************************************************************/
/* Allocate and safely initialise a statement node of the required type.    */
/****************************************************************************/
 
Any_ptr allocate_node (short node_type)

      {

      Any_ptr       temp; 
      Dummy_ptr     dptr;
      short         I;
 
      /***************************************************************/
      /*     Allocate a node as indicated by the parameter 'type'.   */
      /***************************************************************/

      temp = NULL;

      if (trace_heap)
         printf("Request to ALLOCATE a node: %d\n",node_type);

      switch (node_type) {

        case(FORMAT):
            {
            temp = malloc_node (sizeof(Format));
            if (temp != NULL)
               {
               ((Format_ptr)temp)->type            = node_type;
               ((Format_ptr)temp)->fmat_itrn_expr   = NULL;
               ((Format_ptr)temp)->fmat_itrn_value  = 0;
               ((Format_ptr)temp)->nest_ptr         = NULL;
               ((Format_ptr)temp)->fixed_expr1      = NULL;
               ((Format_ptr)temp)->fixed_expr2      = NULL;
               ((Format_ptr)temp)->float_expr1      = NULL;
               ((Format_ptr)temp)->float_expr2      = NULL;
               ((Format_ptr)temp)->pic_str          = NULL;
               ((Format_ptr)temp)->char_expr        = NULL;
               ((Format_ptr)temp)->charf            = 0;
               ((Format_ptr)temp)->bit_radix        = 0;
               ((Format_ptr)temp)->bit_expr         = NULL;
               ((Format_ptr)temp)->l_format         = 0;
               ((Format_ptr)temp)->tab              = 0;
               ((Format_ptr)temp)->tab_expr         = NULL;
               ((Format_ptr)temp)->line_expr        = NULL;
               ((Format_ptr)temp)->space_expr       = NULL;
               ((Format_ptr)temp)->skip             = 0;
               ((Format_ptr)temp)->skip_expr        = NULL;
               ((Format_ptr)temp)->colm_expr        = NULL;
               ((Format_ptr)temp)->rmte_ptr         = NULL;
               ((Format_ptr)temp)->next_ptr         = NULL;
               }
            if (lookahead_in_progress == 0)
               node_heap_used += (sizeof(Format));
            break;
            }

        case(INTARG):
            {
            temp = malloc_node (sizeof(Intarg));
            if (temp != NULL)
               {
               ((Intarg_ptr)temp)->type            = node_type;
               ((Intarg_ptr)temp)->target_ref      = NULL;
               ((Intarg_ptr)temp)->intarg_ptr      = NULL;
               ((Intarg_ptr)temp)->do_ptr          = NULL;
               ((Intarg_ptr)temp)->next_ptr        = NULL;
               }
            if (lookahead_in_progress == 0)
               node_heap_used += (sizeof(Intarg));
            break;
            }

        case(OUTSRC):
            {
            temp = malloc_node (sizeof(Outsrc));
            if (temp != NULL)
               {
               ((Outsrc_ptr)temp)->type            = node_type;
               ((Outsrc_ptr)temp)->source_expr     = NULL;
               ((Outsrc_ptr)temp)->outsrc_ptr      = NULL;
               ((Outsrc_ptr)temp)->do_ptr          = NULL;
               ((Outsrc_ptr)temp)->next_ptr        = NULL;
               }
            if (lookahead_in_progress == 0)
               node_heap_used += (sizeof(Outsrc));
            break;
            }

        case(GETSTRING):
            {
            temp = malloc_node (sizeof(Getstring));
            if (temp != NULL)
               {
               ((Getstring_ptr)temp)->type            = node_type;
               ((Getstring_ptr)temp)->string_expr     = NULL;
               ((Getstring_ptr)temp)->targ_ptr        = NULL;
               ((Getstring_ptr)temp)->fmat_ptr        = NULL;
               }
            if (lookahead_in_progress == 0)
               node_heap_used += (sizeof(Getstring));
            break;
            }

        case(GETFILE):
            {
            temp = malloc_node (sizeof(Getfile));
            if (temp != NULL)
               {
               ((Getfile_ptr)temp)->type            = node_type;
               ((Getfile_ptr)temp)->file_ref        = NULL;
               ((Getfile_ptr)temp)->skip_expr       = NULL;
               ((Getfile_ptr)temp)->skip            = 0;
               ((Getfile_ptr)temp)->targ_ptr        = NULL;
               ((Getfile_ptr)temp)->fmat_ptr        = NULL; 
               }
            if (lookahead_in_progress == 0)
               node_heap_used += (sizeof(Getfile));
            break;
            }

        case(GET):
            {
            temp = malloc_node (sizeof(Get));
            if (temp != NULL)
               {
               ((Get_ptr)temp)->type            = node_type;
               ((Get_ptr)temp)->stmt_ptr        = NULL;
               ((Get_ptr)temp)->getf_ptr        = NULL;
               ((Get_ptr)temp)->gets_ptr        = NULL;
               }
            if (lookahead_in_progress == 0)
               node_heap_used += (sizeof(Get));
            break;
            }


        case(PUTSTRING):
            {
            temp = malloc_node (sizeof(Putstring));
            if (temp != NULL)
               {
               ((Putstring_ptr)temp)->type            = node_type;
               ((Putstring_ptr)temp)->string_ref      = NULL;
               ((Putstring_ptr)temp)->srce_ptr        = NULL;
               ((Putstring_ptr)temp)->fmat_ptr        = NULL;
               }
            if (lookahead_in_progress == 0)
               node_heap_used += (sizeof(Putstring));
            break;
            }

        case(PUTFILE):
            {
            temp = malloc_node (sizeof(Putfile));
            if (temp != NULL)
               {
               ((Putfile_ptr)temp)->type            = node_type;
               ((Putfile_ptr)temp)->file_ref        = NULL;
               ((Putfile_ptr)temp)->skip_expr       = NULL;
               ((Putfile_ptr)temp)->skip            = 0;
               ((Putfile_ptr)temp)->line_expr       = NULL;
               ((Putfile_ptr)temp)->page            = 0;
               ((Putfile_ptr)temp)->srce_ptr        = NULL;
               ((Putfile_ptr)temp)->fmat_ptr        = NULL; 
               }
            if (lookahead_in_progress == 0)
               node_heap_used += (sizeof(Putfile));
            break;
            }

        case(PUT):
            {
            temp = malloc_node (sizeof(Put));
            if (temp != NULL)
               {
               ((Put_ptr)temp)->type            = node_type;
               ((Put_ptr)temp)->stmt_ptr        = NULL;
               ((Put_ptr)temp)->putf_ptr        = NULL;
               ((Put_ptr)temp)->puts_ptr        = NULL;
               }
            if (lookahead_in_progress == 0)
               node_heap_used += (sizeof(Put));
            break;
            }


        case(OPEN):
            {
            temp = malloc_node (sizeof(Open));
            if (temp != NULL)
               {
               ((Open_ptr)temp)->type           = node_type;
               ((Open_ptr)temp)->stmt_ptr       = NULL;
               ((Open_ptr)temp)->file_ref       = NULL;                      
               ((Open_ptr)temp)->title_expr     = NULL;      
               ((Open_ptr)temp)->linesize_expr  = NULL;   
               ((Open_ptr)temp)->pagesize_expr  = NULL;
               ((Open_ptr)temp)->env_ptr        = NULL;             
               ((Open_ptr)temp)->stream         = 0;                 
               ((Open_ptr)temp)->record         = 0;  
               ((Open_ptr)temp)->input          = 0;  
               ((Open_ptr)temp)->output         = 0;  
               ((Open_ptr)temp)->update         = 0;  
               ((Open_ptr)temp)->sequential     = 0;  
               ((Open_ptr)temp)->direct         = 0;  
               ((Open_ptr)temp)->print          = 0;  
               ((Open_ptr)temp)->nonprint       = 0;  
               ((Open_ptr)temp)->keyed          = 0;
               ((Open_ptr)temp)->next_open      = NULL;  
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Open));
            break;
            }
       case(CLOSE):
            {
            temp = malloc_node (sizeof(Close));
            if (temp != NULL)
               {
               ((Close_ptr)temp)->type           = node_type;
               ((Close_ptr)temp)->stmt_ptr       = NULL;
               ((Close_ptr)temp)->file_ref       = NULL;                      
               ((Close_ptr)temp)->env_ptr        = NULL;   
               ((Close_ptr)temp)->next_close     = NULL;          
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Close));
            break;
            }
       case(DELETE):
            {
            temp = malloc_node (sizeof(Delete));
            if (temp != NULL)
               {
               ((Delete_ptr)temp)->type           = node_type;
               ((Delete_ptr)temp)->stmt_ptr       = NULL;
               ((Delete_ptr)temp)->file_ref       = NULL; 
               ((Delete_ptr)temp)->key_expr       = NULL;                     
               ((Delete_ptr)temp)->env_ptr        = NULL;   
                }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Delete));
            break;
            }
       case(READ):
            {
            temp = malloc_node (sizeof(Read));
            if (temp != NULL)
               {
               ((Read_ptr)temp)->type           = node_type;
               ((Read_ptr)temp)->stmt_ptr       = NULL;
               ((Read_ptr)temp)->file_ref       = NULL;
               ((Read_ptr)temp)->into_ref       = NULL;
               ((Read_ptr)temp)->key_expr       = NULL;
               ((Read_ptr)temp)->keyto_ref      = NULL;
               ((Read_ptr)temp)->sizeto_ref     = NULL; 
               ((Read_ptr)temp)->set_ref        = NULL;                       
               ((Read_ptr)temp)->env_ptr        = NULL;   
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Read));
            break;
            }
       case(REWRITE):
            {
            temp = malloc_node (sizeof(Rewrite));
            if (temp != NULL)
               {
               ((Rewrite_ptr)temp)->type           = node_type;
               ((Rewrite_ptr)temp)->stmt_ptr       = NULL;
               ((Rewrite_ptr)temp)->file_ref       = NULL;                      
               ((Rewrite_ptr)temp)->env_ptr        = NULL;   
               ((Rewrite_ptr)temp)->from_ref       = NULL;          
               ((Rewrite_ptr)temp)->key_expr       = NULL; 
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Rewrite));
            break;
            }
       case(WRITE):
            {
            temp = malloc_node (sizeof(Write));
            if (temp != NULL)
               {
               ((Write_ptr)temp)->type           = node_type;
               ((Write_ptr)temp)->stmt_ptr       = NULL;
               ((Write_ptr)temp)->file_ref       = NULL;
               ((Write_ptr)temp)->from_ref       = NULL;
               ((Write_ptr)temp)->keyfrom_expr   = NULL;                       
               ((Write_ptr)temp)->env_ptr        = NULL;   
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Write));
            break;
            }
      case(DATA):
            {
            temp = malloc_node (sizeof(Data));
            if (temp != NULL)
               {
               ((Data_ptr)temp)->type       = node_type;
               ((Data_ptr)temp)->data_type  = 0;
               ((Data_ptr)temp)->prec_1     = 0;
               ((Data_ptr)temp)->prec_2     = 0;
               ((Data_ptr)temp)->scale      = 0;
               ((Data_ptr)temp)->varying    = 0;
               ((Data_ptr)temp)->asterisk   = 0;
               ((Data_ptr)temp)->bad_dcl    = 0;
               ((Data_ptr)temp)->parent     = NULL;
               ((Data_ptr)temp)->next_ptr   = NULL;      
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Data));
            break;
            }
        case(LEAVE):
            {
            temp = malloc_node (sizeof(Leave));
            if (temp != NULL)
               {
               ((Leave_ptr)temp)->type     = node_type;
               ((Leave_ptr)temp)->stmt_ptr = NULL;
               ((Leave_ptr)temp)->ref      = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Leave));
            break;
            }
        case(ICON1):
            {
            temp = malloc_node (sizeof(Icon1));
            if (temp != NULL)
               {
               ((Icon1_ptr)temp)->type        = node_type;
			   ((Icon1_ptr)temp)->preop	      = 0;
               ((Icon1_ptr)temp)->str_con_ptr = NULL;
               ((Icon1_ptr)temp)->ic2_ptr     = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Icon1));
            break;
            }
        case(ICON2):
            {
            temp = malloc_node (sizeof(Icon2));
            if (temp != NULL)
               {
               ((Icon2_ptr)temp)->type        = node_type;
			   ((Icon2_ptr)temp)->preop	      = 0;
               ((Icon2_ptr)temp)->num_con_ptr = NULL;
               ((Icon2_ptr)temp)->ref_ptr     = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Icon2));
            break;
            }
        case(IELEM):
            {
            temp = malloc_node (sizeof(Ielem));
            if (temp != NULL)
               {
               ((Ielem_ptr)temp)->type        = node_type;
			   ((Ielem_ptr)temp)->asterisk    = 0;
               ((Ielem_ptr)temp)->expr_ptr    = NULL;
               ((Ielem_ptr)temp)->ifactor     = 0;
			   ((Ielem_ptr)temp)->ic2_ptr     = NULL;
			   ((Ielem_ptr)temp)->star		  = 0;
			   ((Ielem_ptr)temp)->next_ptr    = NULL;
			   ((Ielem_ptr)temp)->ie_ptr	  = NULL;
			   ((Ielem_ptr)temp)->ic1_ptr	  = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Ielem));
            break;
            }
         case(IARRAY):
            {
            temp = malloc_node (sizeof(Iarray));
            if (temp != NULL)
               {
               ((Iarray_ptr)temp)->type     = node_type;
               ((Iarray_ptr)temp)->ic1_ptr  = NULL;
               ((Iarray_ptr)temp)->ic2_ptr  = NULL;
			   ((Iarray_ptr)temp)->expr_ptr  = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Iarray));
            break;
            }
         case(SELECT):
            {
            temp = malloc_node (sizeof(Select));
            if (temp != NULL)
               {
               ((Select_ptr)temp)->type     = node_type;
               ((Select_ptr)temp)->stmt_ptr = NULL;
               ((Select_ptr)temp)->expr_ptr = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Select));
            break;
            }      
        case(WHEN):
            {
            temp = malloc_node (sizeof(When));
            if (temp != NULL)
               {
               ((When_ptr)temp)->type         = node_type;
               ((When_ptr)temp)->stmt_ptr     = NULL;
               ((When_ptr)temp)->unit_ptr     = NULL;
               ((When_ptr)temp)->any          = 0;
               ((When_ptr)temp)->all          = 0;
               ((When_ptr)temp)->setting_unit = 0;
               /**********************************************/
               /*           Set all 16 ptrs to NULL.         */
               /**********************************************/

               for (I = 0; I < 16; I++)
                   ((When_ptr)temp)->expr_ptr[I] = NULL;

               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(When));
            break;
            }      
        case(OTHER):
            {
            temp = malloc_node (sizeof(Other));
            if (temp != NULL)
               {
               ((Other_ptr)temp)->type     = node_type;
               ((Other_ptr)temp)->stmt_ptr = NULL;
               ((Other_ptr)temp)->unit_ptr = NULL;
               ((Other_ptr)temp)->setting_unit = 0;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Other));
            break;
            }

        case(INITIAL):
            {
            temp = malloc_node (sizeof(Init));
            if (temp != NULL)
               {
               ((Init_ptr)temp)->type      = node_type;
               ((Init_ptr)temp)->ia_ptr    = NULL;
			   ((Init_ptr)temp)->ie_ptr	   = NULL;
               }
            if (lookahead_in_progress == 0)
               node_heap_used += (sizeof(Init));
            break;
            }
                             
        case(LABEL):
            {
            temp = malloc_node (sizeof(Label));
            if (temp != NULL)
               {
               ((Label_ptr)temp)->type     = node_type;
               ((Label_ptr)temp)->stmt_ptr = NULL; 
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Label));
            break;
            }
      
        case(GOTO):
            {
            temp = malloc_node (sizeof(Goto));
            if (temp != NULL)
               {
               ((Goto_ptr)temp)->type     = node_type;
               ((Goto_ptr)temp)->target   = NULL;  
               ((Goto_ptr)temp)->stmt_ptr = NULL;  
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Goto));
            break;
            }
      
        case(LOOP):
            {
            temp = malloc_node (sizeof(Loop));
            if (temp != NULL)
               {
               ((Loop_ptr)temp)->type     = node_type;
               ((Loop_ptr)temp)->end      = NULL;
               ((Loop_ptr)temp)->counter  = NULL;  
               ((Loop_ptr)temp)->start    = NULL;
               ((Loop_ptr)temp)->finish   = NULL;
               ((Loop_ptr)temp)->step     = NULL;
               ((Loop_ptr)temp)->repeat   = NULL;
               ((Loop_ptr)temp)->stmt_ptr = NULL;
               ((Loop_ptr)temp)->loop     = 0;
               ((Loop_ptr)temp)->while_expr = NULL;
               ((Loop_ptr)temp)->until_expr = NULL;
                 
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Loop));
            break;
            }
      
        case(PROCEDURE):
            {
            temp = malloc_node (sizeof(Procedure));
            if (temp != NULL)
               {
               ((Procedure_ptr)temp)->type     = node_type;
               ((Procedure_ptr)temp)->end      = NULL;
               ((Procedure_ptr)temp)->proc     = NULL;  
               ((Procedure_ptr)temp)->argument = NULL;
               ((Procedure_ptr)temp)->stmt_ptr = NULL;
               ((Procedure_ptr)temp)->ret_ptr  = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Procedure));
            break;
            }

        case(ENTRY):
            {
            temp = malloc_node (sizeof(Entry));
            if (temp != NULL)
               {
               ((Entry_ptr)temp)->type     = node_type;
               ((Entry_ptr)temp)->end      = NULL;
               ((Entry_ptr)temp)->proc     = NULL;  
               ((Entry_ptr)temp)->argument = NULL;
               ((Entry_ptr)temp)->stmt_ptr = NULL;
               ((Entry_ptr)temp)->ret_ptr  = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Entry));
            break;
            }
     
        case(CALL):
            {
            temp = malloc_node (sizeof(Call));
            if (temp != NULL)
               {
               ((Call_ptr)temp)->type     = node_type;
               ((Call_ptr)temp)->entry    = NULL;  
               ((Call_ptr)temp)->stmt_ptr = NULL;  
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Call));
            break;
            }
      
        case(RETURN):
            {
            temp = malloc_node (sizeof(Return));
            if (temp != NULL)
               {
               ((Return_ptr)temp)->type     = node_type;
               ((Return_ptr)temp)->value    = NULL;  
               ((Return_ptr)temp)->stmt_ptr = NULL;  
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Return));
            break;
            }
      
        case(ALLOCATE):
            {
            temp = malloc_node (sizeof(Allocate));
            if (temp != NULL)
               {
               ((Allocate_ptr)temp)->type     = node_type;
               ((Allocate_ptr)temp)->area     = NULL;
               ((Allocate_ptr)temp)->target   = NULL;  
               ((Allocate_ptr)temp)->stmt_ptr = NULL;  
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Allocate));
            break;
            }
      
        case(FREE):
            {
            temp = malloc_node (sizeof(Free));
            if (temp != NULL)
               {
               ((Free_ptr)temp)->type     = node_type;
               ((Free_ptr)temp)->target   = NULL;  
               ((Free_ptr)temp)->stmt_ptr = NULL;  
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Free));
            break;
            }
      
        case(ASSIGNMENT):
            {
            temp = malloc_node (sizeof(Assignment));
            if (temp != NULL)
               {
               ((Assignment_ptr)temp)->type     = node_type;
               ((Assignment_ptr)temp)->target   = NULL;  
               ((Assignment_ptr)temp)->source   = NULL; 
               ((Assignment_ptr)temp)->stmt_ptr = NULL; 
               ((Assignment_ptr)temp)->by_name  = 0; 
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Assignment));
            break;
            }
      
        case(ARGUMENT):
            {
            temp = malloc_node (sizeof(Argument));
            if (temp != NULL)
               {
               ((Argument_ptr)temp)->type     = node_type;
               ((Argument_ptr)temp)->arg_ptr  = NULL;  
               ((Argument_ptr)temp)->next_arg = NULL; 
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Argument));
            break;
            }
      
        case(IF):
            {
            temp = malloc_node (sizeof(If));
            if (temp != NULL)
               {
               ((If_ptr)temp)->type       = node_type;
               ((If_ptr)temp)->action     = -1;
               ((If_ptr)temp)->expression = NULL;  
               ((If_ptr)temp)->then_ptr   = NULL;
               ((If_ptr)temp)->else_ptr   = NULL;
               ((If_ptr)temp)->stmt_ptr   = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(If));
            break;
            }
       
        case(DO):
            {
            temp = malloc_node (sizeof(Do));
            if (temp != NULL)
               {
               ((Do_ptr)temp)->type     = node_type;
               ((Do_ptr)temp)->end      = NULL;
               ((Do_ptr)temp)->stmt_ptr = NULL;  
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Do));
            break;
            }

        case(BEGIN):
            {
            temp = malloc_node (sizeof(Begin));
            if (temp != NULL)
               {
               ((Begin_ptr)temp)->type     = node_type;
               ((Begin_ptr)temp)->end      = NULL;
               ((Begin_ptr)temp)->stmt_ptr = NULL;  
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Begin));
            break;
            }

        case(END):
            {
            temp = malloc_node (sizeof(End));
            if (temp != NULL)
               {
               ((End_ptr)temp)->type     = node_type;
               ((End_ptr)temp)->partner  = NULL;
               ((End_ptr)temp)->stmt_ptr = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(End)); 
            break;
            }

        case(STOP):
            {
            temp = malloc_node (sizeof(Stop));
            if (temp != NULL)
               {
               ((Stop_ptr)temp)->type     = node_type;
               ((Stop_ptr)temp)->stmt_ptr = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Stop)); 
            break;
            }

        case(EXPRESSION):
            {
            temp = malloc_node (sizeof(Expr));
            if (temp != NULL)
               {
               ((Expr_ptr)temp)->type      = node_type;
               ((Expr_ptr)temp)->data_type = 0;
               ((Expr_ptr)temp)->child     = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Expr)); 
            break;
            }

        case(ARRAY):
            {
            temp = malloc_node (sizeof(Dim));
            if (temp != NULL)
               {
               ((Dim_ptr)temp)->type      = node_type;
               ((Dim_ptr)temp)->lower     = NULL;
               ((Dim_ptr)temp)->upper     = NULL;
               ((Dim_ptr)temp)->next_ptr  = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Dim)); 
            break;
            }

        case(SUBSCRIPT):
            {
            temp = malloc_node (sizeof(Sub));
            if (temp != NULL)
               {
               ((Sub_ptr)temp)->type       = node_type;
               ((Sub_ptr)temp)->pass_by    = REFERENCE; /* used for args */
               ((Sub_ptr)temp)->expression = NULL;
               ((Sub_ptr)temp)->next_ptr   = NULL;
               ((Sub_ptr)temp)->prev_ptr   = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Sub)); 
            break;
            }

        case(REFERENCE):
            {
            temp = malloc_node (sizeof(Ref));
            if (temp != NULL)
               {
               ((Ref_ptr)temp)->type       = node_type;
               ((Ref_ptr)temp)->num_subs   = 0;
               ((Ref_ptr)temp)->null_list  = 0;
               ((Ref_ptr)temp)->data_type  = 0;
               ((Ref_ptr)temp)->scale      = 0;
               ((Ref_ptr)temp)->resolved   = 0;
               ((Ref_ptr)temp)->spelling   = NULL;
               ((Ref_ptr)temp)->symbol     = NULL;
               ((Ref_ptr)temp)->attribs    = NULL;
               ((Ref_ptr)temp)->temp       = NULL;
               ((Ref_ptr)temp)->sublist    = NULL;
               ((Ref_ptr)temp)->dot_ptr    = NULL;
               ((Ref_ptr)temp)->ptr_ptr    = NULL;
               ((Ref_ptr)temp)->ofx_ptr    = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Ref)); 
            break;
            }
        case(OR):
        case(AND):
        case(GT):
        case(LT):
        case(GE):
        case(LE):
		case(NOTGT):
		case(NOTLT):
		case(NOTGE):
		case(NOTLE):
        case(NOTEQUAL):
        case(EQUALS):
        case(CONCAT):
        case(PLUS):
        case(MINUS):
        case(TIMES):
        case(DIVIDE):
            {
            temp = malloc_node (sizeof(Oper));
            if (temp != NULL)
               {
               ((Oper_ptr)temp)->type      = node_type;
               ((Oper_ptr)temp)->data_type = 0;   
               ((Oper_ptr)temp)->base      = 0;
               ((Oper_ptr)temp)->scale     = 0;
               ((Oper_ptr)temp)->prec_1    = 0;
               ((Oper_ptr)temp)->prec_2    = 0; 
               ((Oper_ptr)temp)->offset    = 0;   /* offset of temporary */
               ((Oper_ptr)temp)->bytes     = 0;   /* size of temporary   */   
               ((Oper_ptr)temp)->left_ptr  = NULL;
               ((Oper_ptr)temp)->rite_ptr  = NULL;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Oper)); 
            break;
            }

        case(ON):
            {
            temp = malloc_node (sizeof(On));
            if (temp != NULL)
               {
               ((On_ptr)temp)->type       = node_type;
               ((On_ptr)temp)->snap       = 0;
               ((On_ptr)temp)->system     = 0;
               ((On_ptr)temp)->stmt_ptr   = NULL;
               ((On_ptr)temp)->unit_ptr   = NULL;
               ((On_ptr)temp)->ref_ptr    = NULL;
               ((On_ptr)temp)->cond_type  = 0;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(On)); 
            break;
            }

        case(SIGNAL):
            {
            temp = malloc_node (sizeof(Signal));
            if (temp != NULL)
               {
               ((Signal_ptr)temp)->type       = node_type;
               ((Signal_ptr)temp)->stmt_ptr   = NULL;
               ((Signal_ptr)temp)->ref_ptr    = NULL;
               ((Signal_ptr)temp)->cond_type  = 0;
               }
            if (lookahead_in_progress == 0) 
               node_heap_used += (sizeof(Signal)); 
            break;
            }

        default:
            {
            report (54,"",_LINE_); /* we should abort compile here !!! */
            return(NULL);
            }
                   
        /*************************************************************/
        /* If we were unable to allocate a node, then no more memory */
        /*************************************************************/

        } /* The switch */

      if (temp == NULL)
         { 
         report (53,"",_LINE_);
         unrecoverable_error = 1;
         longjmp(escape,1);  /* Kill pass1 ! */
         }

      /*****************************************************************/
      /* NOTE: The line number is NOT recorded in individual expr/oper */
      /* nodes, the parent node will hold this.                        */
      /*****************************************************************/

      if ((priority(temp) == 0) &&
          (node_type != ARRAY)  &&
          (node_type != SUBSCRIPT) &&
          (node_type != REFERENCE) &&
          (node_type != EXPRESSION))
         {
         dptr = temp;
         strcpy (dptr->line_no,line_no);
         }

      if (lookahead_in_progress == 0)
         nodes_allocated++;

      return (temp);
 
   } /* The function */      


/***************************************************************************/
/*      This function will insert a parse node into the parse tree.        */
/***************************************************************************/

void insert_node (Any_ptr prev_ptr,Any_ptr curr_ptr)

           
     /* prev_ptr: We dont know what type of node this is yet */
     /* curr_ptr  - - - - - - - - - - - - - - - - - - - - -  */

     {

     short          prev_type;
     short          curr_type;

     curr_type = nodetype(curr_ptr);
     prev_type = nodetype(prev_ptr);

     /********************************************************************/
     /* Some nodes are linked back to their previous node, so that pass2 */
     /* can replace them.                                                */
     /********************************************************************/
 
     switch (prev_type) {

     case(PROCEDURE):
         ((Procedure_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(LEAVE):
         ((Leave_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break; 
     case(ON):
         ((On_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break; 
     case(SIGNAL):
         ((Signal_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break; 
     case(OPEN):
         ((Open_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(GET):
         ((Get_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(CLOSE):
         ((Close_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(READ):
         ((Read_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(WRITE):
         ((Write_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(DELETE):
         ((Delete_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(REWRITE):
         ((Rewrite_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(ENTRY):
         ((Entry_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(ASSIGNMENT):
         ((Assignment_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(DO):
         ((Do_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(BEGIN):
         ((Begin_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(END):
         ((End_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(CALL):
         ((Call_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(IF):
         /***************************************************************/
         /* The field 'action' is used to inform this function, about   */
         /* which part of the 'if' the new node should be chained on to */
         /***************************************************************/
         if (((If_ptr)prev_ptr)->action == THEN)
            ((If_ptr)prev_ptr)->then_ptr = curr_ptr; 
         else
         if (((If_ptr)prev_ptr)->action == ELSE)
            ((If_ptr)prev_ptr)->else_ptr = curr_ptr; 
         else 
         if (((If_ptr)prev_ptr)->action == STMT)
            {    
            if (((If_ptr)prev_ptr)->stmt_ptr == ((If_ptr)prev_ptr)->else_ptr)
               {
               /********************************************************/
               /* if the else_ptr and stmt_ptr are the same, then make */
               /* sure we modify them both !                           */
               /********************************************************/
               ((If_ptr)prev_ptr)->stmt_ptr = curr_ptr; /* HWG5 */
               if (((If_ptr)prev_ptr)->else_ptr != NULL)
                  ((If_ptr)prev_ptr)->else_ptr = curr_ptr; 
               }
            else
               ((If_ptr)prev_ptr)->stmt_ptr = curr_ptr;
            /***************************************************************/
            /* When multiple if's are nested (immediately) then only the   */
            /* outermost IF gets its stmt_ptr set. We must therefore check */
            /* to see if either our then or else ptr's point to an IF, and */
            /* should they, we must set THEIR stmt_ptr to our own.         */
            /***************************************************************/

            if (nodetype (((If_ptr)prev_ptr)->then_ptr) == IF)
               insert_node (((If_ptr)prev_ptr)->then_ptr,curr_ptr);

            if (nodetype(((If_ptr)prev_ptr)->else_ptr) == IF)
               insert_node (((If_ptr)prev_ptr)->else_ptr,curr_ptr);
  
            /************************************************************/
            /* If there was no else clause for this IF then set the     */
            /* else pointer to point to the same place as next stmt ptr */
            /************************************************************/
  
             if (((If_ptr)prev_ptr)->else_ptr == NULL)
                  ((If_ptr)prev_ptr)->else_ptr = curr_ptr; 
            }
         else
            report (59,"",_LINE_);
         break;
     case(LOOP):
         ((Loop_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(RETURN):
         ((Return_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(ALLOCATE):
         ((Allocate_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(FREE):
         ((Free_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(LABEL):
         ((Label_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(GOTO):
         ((Goto_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(STOP):
         ((Stop_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(PUT):
         ((Put_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(SELECT):
         ((Select_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(WHEN):
         if (((When_ptr)prev_ptr)->setting_unit)
            ((When_ptr)prev_ptr)->unit_ptr = curr_ptr;
         else
            ((When_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     case(OTHER):
         if (((Other_ptr)prev_ptr)->setting_unit)
            ((Other_ptr)prev_ptr)->unit_ptr = curr_ptr;
         else
            ((Other_ptr)prev_ptr)->stmt_ptr = curr_ptr;
         break;
     default:
         report(58,"",_LINE_); 

     }
     }
/***************************************************************************/
/* This function will return a ptr to the last node (if any) that follows  */
/* the supplied node pointer.                                              */
/***************************************************************************/

Any_ptr next_stmt (Any_ptr node_ptr)

           
     /* node_ptr;  We dont know what type of node this is yet */

     {

     short          type;

     if (node_ptr == NULL)
        {
        report (60,"",_LINE_);
        return (NULL);
        }
    

     type = nodetype(node_ptr);

     switch (type) {     

     case(PROCEDURE):
         if (((Procedure_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt (((Procedure_ptr)node_ptr)->stmt_ptr));
     case(LEAVE):
         if (((Leave_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt (((Leave_ptr)node_ptr)->stmt_ptr)); 
     case(ON):
         if (((On_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt (((On_ptr)node_ptr)->stmt_ptr)); 
     case(SIGNAL):
         if (((Signal_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt (((Signal_ptr)node_ptr)->stmt_ptr)); 
     case(ENTRY):
         if (((Entry_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt (((Entry_ptr)node_ptr)->stmt_ptr));
     case(ASSIGNMENT):
         if (((Assignment_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt (((Assignment_ptr)node_ptr)->stmt_ptr));
     case(DO):
         if (((Do_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt (((Do_ptr)node_ptr)->stmt_ptr));
     case(BEGIN):
         if (((Begin_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt (((Begin_ptr)node_ptr)->stmt_ptr));
     case(END):
         if (((End_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((End_ptr)node_ptr)->stmt_ptr));
     case(CALL):
         if (((Call_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Call_ptr)node_ptr)->stmt_ptr));
     case(IF):
         if (((If_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((If_ptr)node_ptr)->stmt_ptr));
     case(LOOP):
         if (((Loop_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Loop_ptr)node_ptr)->stmt_ptr));
     case(RETURN):
         if (((Return_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Return_ptr)node_ptr)->stmt_ptr));
     case(ALLOCATE):
         if (((Allocate_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Allocate_ptr)node_ptr)->stmt_ptr));
     case(FREE):
         if (((Free_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Free_ptr)node_ptr)->stmt_ptr));
     case(LABEL):
         if (((Label_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Label_ptr)node_ptr)->stmt_ptr));
     case(GOTO):
         if (((Goto_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Goto_ptr)node_ptr)->stmt_ptr));
     case(STOP):
         if (((Stop_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Stop_ptr)node_ptr)->stmt_ptr));
     case(PUT):
         if (((Put_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Put_ptr)node_ptr)->stmt_ptr));
     case(OPEN):
         if (((Open_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Open_ptr)node_ptr)->stmt_ptr));
     case(CLOSE):
         if (((Close_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Close_ptr)node_ptr)->stmt_ptr));
     case(READ):
         if (((Read_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Read_ptr)node_ptr)->stmt_ptr));
     case(WRITE):
         if (((Write_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Write_ptr)node_ptr)->stmt_ptr));
     case(DELETE):
         if (((Delete_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Delete_ptr)node_ptr)->stmt_ptr));
     case(REWRITE):
         if (((Rewrite_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Rewrite_ptr)node_ptr)->stmt_ptr));
     case(GET):
         if (((Get_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Get_ptr)node_ptr)->stmt_ptr));
     case(SELECT):
         if (((Select_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Select_ptr)node_ptr)->stmt_ptr));
     case(WHEN):
         if (((When_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((When_ptr)node_ptr)->stmt_ptr));
     case(OTHER):
         if (((Other_ptr)node_ptr)->stmt_ptr == NULL)
            return (node_ptr);
         return (next_stmt(((Other_ptr)node_ptr)->stmt_ptr));
     default:
         report(61,"",_LINE_); 
         return(NULL);

     }
     }

/***************************************************************************/
/* This function actually allocates memory for the required node, we only  */
/* allocate in the true sense, if lookahead_in_progress is false.          */
/* Else we simply return the address of a fixed buffer area in memory.     */
/* Lookahead means that the syntax analyser is trying to determine what    */
/* type of statement to parse, and does not want to build a real tree      */
/* whilst doing this.                                                      */
/***************************************************************************/

Any_ptr malloc_node (short size)

   {

   Any_ptr		  p;
   static char *  step_ptr = NULL;
   static long    step_left = 0;

   if (lookahead_in_progress)
      p = &generic_buffer;
   else
      {
      if (step_left < size)
	     {
	     step_ptr = malloc(1024);
         step_left = 1024;
	     }

	  /**********************************************************************/
	  /* 'alloc' from the area pointed to by 'setp_ptr'					    */
	  /**********************************************************************/
          
      p = step_ptr;
	  step_ptr += size;
	  step_left -= size;
	  }


   return (p);

   }
 
/****************************************************************************/
/* This function will release the storage used by a no-longer required node */
/****************************************************************************/

Any_ptr

   free_node (Any_ptr ptr)


   {

   short            type;

   type = nodetype(ptr);

   if (trace_heap)
      printf("Request to FREE     a node type: %d\n",type);
 /*
   switch (type) {

   case(PUT): */
       /* free (ptr); */
       nodes_allocated--;
       node_heap_used -= (sizeof(Put)); 
       return(NULL);  /*
   case(ICON2):
       free (ptr);
	   nodes_allocated--;
	   node_heap_used -= (sizeof(Icon2));
	   return(NULL);
   default:
       report(130,"",_LINE_);	  
   } */									   

   }
