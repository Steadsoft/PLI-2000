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
/* HWG    19-04-91       Rewritten to work in a purely recursive manner     */
/* HWG    01-05-91       Structure and pointer qualification added          */
/* HWG    10-05-91       Bug to do with mismatch in arg counts fixed when   */
/*                       checking calls to external procedures.             */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                         Functional Description                           */
/****************************************************************************/
/* This source file contains all PL/1 semantic checking, and consists of a  */
/* set of mutually recursive functions that walk the parse-tree built by    */
/* pass1.                                                                   */
/* This pass will (for each type of tree node) verify that all sub-trees of */
/* a node are valid, and that all PL/1 references have the correct PL/1     */
/* attributes for the context of their use. Where appropriate, conversion   */
/* operators are inserted into the parse-tree.                              */    
/* result attributes are deduced, and propagated up each expression tree.   */
/****************************************************************************/

# include <setjmp.h>
# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include "c_types.h"
# include "tokens.h"
# include "nodes.h"
# include "symtab.h"

jmp_buf     exit_dt;
extern      Block_ptr block_root;
short       nodetype        (Any_ptr);
void report          (int,char *,int); 

static Any_ptr read_next_node   (void);
static int         dump_nodes   (Any_ptr);


static void dump_on         (void);
static void dump_select     (void);
static void dump_when       (void);
static void dump_entry      (void);
static void dump_procedure  (void);
static void dump_if         (void);
static void dump_stop       (void);
static void dump_call       (void);
static void dump_put        (void);
static void dump_return     (void);
static void dump_loop       (void);
static void dump_do         (void);
static void dump_begin      (void);
static void dump_allocate   (void);
static void dump_free       (void);
static void dump_goto       (void);
static void dump_leave      (void);
static void dump_end        (void);
static void dump_label      (void);
static void dump_assignment (void); 
static void dump_open       (void);
static void dump_read       (void);
static void dump_write      (void);
static void dump_delete     (void);
static void dump_rewrite    (void);
static void dump_close      (void);
static void dump_get        (void);   
static void dump_operator   (Any_ptr);
static void dump_expression (Any_ptr);
static void dump_reference  (Ref_ptr);
static int  dump_sublist    (Sub_ptr);
static void dump_intarg     (Intarg_ptr);
static void dump_outsrc     (Outsrc_ptr);
static void dump_format     (Format_ptr);
static void error_line_set   (Any_ptr);
static int  get_next_type   (Any_ptr);
static void dump_qualification (Block_ptr,Ref_ptr);
static void dump_struc_ref     (char *,Symbol_ptr,Ref_ptr);
/* long PTR (Any_ptr); */

#define PTR(x) (x) 

Any_ptr   current_ptr = NULL;

extern char   line_no[10];
long          match_count;
Block_ptr     current_block = NULL;

/****************************************************************************/
/*         This is the main function entry for dump  processing.            */
/****************************************************************************/

void dump_tree (void)

     {

     Any_ptr  ptr;

     printf("\nBEGINNING DUMP OF PL/I PARSE TREE\n\n");

     if (setjmp(exit_dt) == 0)    /* Exit the dump, if we get a null node-ptr */
        {                         /* this may happen if pass1 exited early    */
        ptr = read_next_node();   /* due to early EOF. In this case the       */
        dump_nodes (ptr);         /* parse-tree may be incomplete.            */
        printf("\nCOMPLETED DUMP OF PL/I PARSE TREE\n\n");
        }
     else
        {
        printf("\nAN UNEXPECTED ERROR OCURRED DURING DUMP OF PARSE TREE\n");
        printf("THE PARSE TREE DUMP HAS BEEN TERMINATED.\n");
        return;
        }

     }

/***************************************************************************/
/* This function will take a node-ptr and call a dump  function to process */
/* that node !                                                             */
/***************************************************************************/

static int  dump_nodes (Any_ptr node_ptr)

     {

     Begin_ptr      bg_ptr;
     Procedure_ptr  p_ptr;
     Block_ptr      b_saved;
     Block_ptr      cb_ptr;
     Any_ptr        here_ptr;
     short          nt;
     char           ntype[32];

     if (node_ptr == NULL)
        return (0);

     nt = nodetype(node_ptr);

     switch (nt) {

     case (PROCEDURE):
          b_saved = current_block;
          p_ptr   = node_ptr;
          current_block = p_ptr->proc;
          here_ptr = current_ptr;
          current_ptr = p_ptr->proc->first_stmt;
          dump_procedure ();
          current_ptr   = here_ptr;
          current_block = b_saved;
          break;
     case (ENTRY):
          dump_entry();
          break;
     case (SELECT):
          dump_select();
          break;
     case (WHEN):
          dump_when();
          break;
     case (IF):
          dump_if();
          break;
     case (STOP):
          dump_stop();
          break;
     case (CALL):
          dump_call();
          break;
     case (RETURN):
          dump_return();
          break;
     case (LOOP):
          dump_loop();
          break;
     case (DO):
          dump_do();
          break;
     case (BEGIN):
          b_saved = current_block;
          bg_ptr = node_ptr;
          current_block = bg_ptr->block;
          here_ptr = current_ptr;
          current_ptr = bg_ptr->block->first_stmt;
          dump_begin();
          current_ptr = here_ptr;
          current_block = b_saved;
          break;
     case (ALLOCATE):
          dump_allocate();
          break;
     case (FREE):
          dump_free();
          break;
     case (GOTO):
          dump_goto();
          break;
     case (LEAVE):
          dump_leave();
          break;
     case (END):
          dump_end();
          return(END); /* break; */
     case (LABEL):
          dump_label();
          break;
     case (ASSIGNMENT):
          dump_assignment();
          break;
     case (PUT):
          dump_put();
          break;
     case (OPEN):
          dump_open();
          break;
     case (READ):
          dump_read();
          break;
     case (WRITE):
          dump_write();
          break;
     case (DELETE):
          dump_delete();
          break;
     case (REWRITE):
          dump_rewrite();
          break;
     case (CLOSE):
          dump_close();
          break;
     case (GET):
          dump_get();
          break;
     case (ON):
          dump_on();
          break;
     default:
          {
          itoa(nt,ntype,10);
          report (62,ntype,__LINE__);
          }
     }

     if (nodetype(node_ptr) == PROCEDURE)
        {
        /******************************************************************/
        /* OK We have processed a PL/1 block, but we must now process all */
        /* child blocks of this block.                                    */
        /******************************************************************/
        cb_ptr = p_ptr->proc->child;

        while (cb_ptr != NULL)
              {
              dump_nodes(cb_ptr->first_stmt);
              cb_ptr = cb_ptr->sister;
              }
        }

     return(0);

     }

/***************************************************************************/
/*     This function is called to check/process an expression tree.     */
/***************************************************************************/

static void dump_expression (Any_ptr node_ptr)

     {

     short               tp;

     if (node_ptr == NULL) /* a NULL ptr simply means there is no expression */
        return; 

     tp = nodetype(node_ptr);
 
     switch (tp) {
     case(OR):
     case(AND):
     case(GT):
     case(LT):
     case(GE):
     case(LE):
     case(NOTEQUAL):
     case(EQUALS):
     case(CONCAT):
     case(PLUS):
     case(MINUS):
     case(TIMES):
     case(DIVIDE):
          dump_operator (node_ptr);
          break; 
     case (REFERENCE):
          dump_reference (node_ptr);
          break;    
     case (SYMBOL):
          return;  /* dump_symbol not implemented yet */
     default:
          {
          report(93,"",__LINE__);
          }
     }

     }

/*************************************************************************/
/* Analyse and process a PL/1 operator node, and any subnodes.           */
/*************************************************************************/ 

static void dump_operator (Any_ptr g_ptr)

     {

     Oper_ptr  ptr;
  
     ptr = g_ptr;

     switch (ptr->type) {
     case(PLUS):     printf ("Operator Node '+' At: %08lX\n", PTR(ptr)); break;
     case(MINUS):    printf ("Operator Node '-' At: %08lX\n", PTR(ptr)); break;
     case(TIMES):    printf ("Operator Node '*' At: %08lX\n", PTR(ptr)); break;
     case(DIVIDE):   printf ("Operator Node '/' At: %08lX\n", PTR(ptr)); break;
     case(CONCAT):   printf ("Operator Node '||' At: %08lX\n",PTR(ptr));break;
     case(OR):       printf ("Operator Node '|' At: %08lX\n", PTR(ptr));break;
     case(AND):      printf ("Operator Node '&' At: %08lX\n", PTR(ptr));break;
     case(GT):       printf ("Operator Node '>' At: %08lX\n", PTR(ptr));break;
     case(LT):       printf ("Operator Node '<' At: %08lX\n", PTR(ptr));break;
     case(GE):       printf ("Operator Node '>=' At: %08lX\n",PTR(ptr));break;
     case(LE):       printf ("Operator Node '<=' At: %08lX\n",PTR(ptr));break;
     case(NOTEQUAL): printf ("Operator Node 'ª=' At: %08lX\n",PTR(ptr));break;
     case(EQUALS):   printf ("Operator Node '=' At: %08lX\n", PTR(ptr));break;
     default:        printf ("Operator Node UNKNOWN At: %08lX\n",PTR(ptr));break;

     }


     if (ptr->base == BINARY)
        printf ("Base BINARY\n");
     if (ptr->base == DECIMAL)
        printf ("Base DECIMAL\n");

     if (ptr->scale == FIXED)
        printf("Scale FIXED\n");
     if (ptr->scale == FLOAT)
        printf("Scale FLOAT\n");

     printf ("Precision:            %d,%d\n",ptr->prec_1,ptr->prec_2);
 
     printf ("Left Child At:        %08lX\n",PTR(ptr->left_ptr));
     printf ("Rite Child At:        %08lX\n",PTR(ptr->rite_ptr));
     printf ("\n");

     if (ptr->left_ptr == NULL)
        printf("Warning the above operators Left Ptr is NULL !\n\n");

     if (ptr->rite_ptr == NULL)
        printf("Warning the above operators Rite Ptr is NULL !\n\n");

     dump_expression (ptr->left_ptr);
     dump_expression (ptr->rite_ptr);

     return;

     }

/*************************************************************************/
/*               Analyse and process a PROCEDURE node.                   */
/*************************************************************************/ 

static void dump_procedure (void)

     {

     Any_ptr   next;
     Procedure_ptr ptr;

     ptr  = current_ptr;
     next = read_next_node();

     printf ("Procedure Node At:    %08lX\n",PTR(ptr));
     printf ("Block Name:           %s\n",ptr->proc->block_name);

     if (ptr->argument != NULL)
        {
        printf ("Parameter :           %08lX\n",PTR(ptr->argument));
        }

     printf ("\n");

     while (dump_nodes (next) == 0)
           {
           next = read_next_node();
           }

     return; /* An END was read so were done ! */

     
     }

/*************************************************************************/
/*               Analyse and process an IF node                          */
/*************************************************************************/ 

static void dump_if (void)

     {

     If_ptr       ptr;
     Any_ptr  next;

     ptr = current_ptr;

     next = current_ptr;  /* save current location in tree */

     error_line_set (ptr);


     /* if (ptr->visited == 0)
        return;
     ptr->visited = 0;  */

     printf ("If Node At:           %08lX\n",PTR(ptr));

     if (ptr->then_ptr == NULL)
        printf ("Then pointer is       NULL\n");
     else
        printf ("Then pointer:         %08lX\n",PTR(ptr->then_ptr));
     if (ptr->else_ptr == NULL)
        printf ("Else pointer is       NULL\n");
     else
        printf ("Else pointer:         %08lX\n",PTR(ptr->else_ptr));
     if (ptr->stmt_ptr == NULL)
        printf ("Stmt pointer is       NULL\n");
     else
        printf ("Stmt pointer:         %08lX\n",PTR(ptr->stmt_ptr));
     if (ptr->expression == NULL) 
        printf ("Expr pointer is       NULL\n");
     else
        printf ("Expr pointer:         %08lX\n",PTR(ptr->expression));
 
     printf ("\n");

     dump_expression (ptr->expression);

     current_ptr = ptr->then_ptr;

     dump_nodes (current_ptr);

     if (ptr->else_ptr != ptr->stmt_ptr)
        {
        current_ptr = ptr->else_ptr;
        dump_nodes (current_ptr);
        }

     current_ptr = next;

     return;
 
     }

/*************************************************************************/
/*               Analyse and process a WHEN node                         */
/*************************************************************************/ 

static void dump_when (void)

     {

     When_ptr     ptr;
     Any_ptr  next;

     ptr = current_ptr;

     next = current_ptr;  /* save current location in tree */

     error_line_set (ptr);

     printf ("When Node At:         %08lX\n",PTR(ptr));

     if (ptr->unit_ptr == NULL)
        printf ("Unit pointer is       NULL\n");
     else
        printf ("Unit pointer:         %08lX\n",PTR(ptr->unit_ptr));

     if (ptr->stmt_ptr == NULL)
        printf ("Stmt pointer is       NULL\n");
     else
        printf ("Stmt pointer:         %08lX\n",PTR(ptr->stmt_ptr));

     if (ptr->expr_ptr[0] == NULL) 
        printf ("Expr pointer is       NULL\n");
     else
        printf ("Expr pointer:         %08lX\n",PTR(ptr->expr_ptr[0]));
 
     printf ("\n");

     dump_expression (ptr->expr_ptr[0]);

     current_ptr = ptr->unit_ptr;

     dump_nodes (current_ptr);

     current_ptr = ptr->stmt_ptr;

     dump_nodes (current_ptr);

     current_ptr = next;

     return;
 
     }

/*************************************************************************/
/*               Analyse and process a STOP node.                        */
/*************************************************************************/ 

static void dump_stop (void)

     {

     Stop_ptr    ptr;

     ptr = current_ptr;

     printf ("Stop Node At:         %08lX\n",PTR(ptr));
     printf ("\n");

     return;

     }


/*************************************************************************/
/*               Analyse and process a CALL node.                        */
/*************************************************************************/ 

static void dump_call (void)

     {

     Call_ptr   ptr;
 
     ptr = current_ptr;

     error_line_set (ptr);

     printf ("Call Node At:         %08lX\n",PTR(ptr));
    
     if (ptr->entry == NULL)
        printf ("Entry Pointer is      NULL\n");
     else
        dump_reference(ptr->entry);

     return;

     }

/*************************************************************************/
/*               Analyse and process a RETURN node                       */
/*************************************************************************/ 

static void dump_return (void)

     {

     Return_ptr ptr;

     ptr = current_ptr;

     error_line_set (ptr);

     printf ("Return Node At:       %08lX\n",PTR(ptr));
     if (ptr->value != NULL)
        printf ("Value At:             %08lX\n",PTR(ptr->value)); 
     printf ("\n");

     if (ptr->value != NULL)
        dump_expression (ptr->value);

     return;

     }

/*************************************************************************/
/*               Analyse and process a OPEN node                         */
/*************************************************************************/ 

static void dump_open (void)

     {

     Open_ptr ptr;

     ptr = current_ptr;

     error_line_set (ptr);

     printf ("Open Node At:       %08lX\n",PTR(ptr));

     if (ptr->file_ref != NULL)
        {
        printf ("File Ref:             %08lX\n",PTR(ptr->file_ref)); 
        printf ("\n");
        dump_reference (ptr->file_ref);
        }

     if (ptr->title_expr != NULL)
        {
        printf ("Title Expr:             %08lX\n",PTR(ptr->title_expr)); 
        printf ("\n");
        dump_expression (ptr->title_expr);
        }

     if (ptr->linesize_expr != NULL)
        {
        printf ("Linesize Expr:             %08lX\n",PTR(ptr->linesize_expr)); 
        printf ("\n");
        dump_expression (ptr->linesize_expr);
        }

     if (ptr->pagesize_expr != NULL)
        {
        printf ("Pagesize Expr:             %08lX\n",PTR(ptr->pagesize_expr)); 
        printf ("\n");
        dump_expression (ptr->pagesize_expr);
        }

     if (ptr->stream)
        printf("STREAM\n");
     if (ptr->record)
        printf("RECORD\n");
     if (ptr->input)
        printf("INPUT\n");
     if (ptr->output)
        printf("OUTPUT\n");
     if (ptr->update)
        printf("UPDATE\n");
     if (ptr->sequential)
        printf("SEQUENTIAL\n");
     if (ptr->direct)
        printf("DIRECT\n");
     if (ptr->print)
        printf("PRINT\n");
     if (ptr->nonprint)
        printf("NONPRINT\n");
     if (ptr->keyed)
        printf("KEYED\n");
     
 /*    if (ptr->next_open != NULL)
        dump_open(    */
                      

     return;

     }


static void dump_read (void)

{
  Read_ptr ptr;

  ptr = current_ptr;

  error_line_set (ptr);

  printf ("Read Node At:       %08lX\n",PTR(ptr));

  if (ptr->file_ref != NULL) 
     {
     printf ("File Ref:             %08lX\n",PTR(ptr->file_ref)); 
     printf ("\n");
     dump_reference (ptr->file_ref);
     }

  if (ptr->into_ref != NULL) 
     {
     printf ("Into Ref:             %08lX\n",PTR(ptr->into_ref)); 
     printf ("\n");
     dump_reference (ptr->into_ref);
     }

  if (ptr->key_expr != NULL) 
     {
     printf ("Key Expr:             %08lX\n",PTR(ptr->key_expr)); 
     printf ("\n");
     dump_expression (ptr->key_expr);
     }

  if (ptr->keyto_ref != NULL) 
     {
     printf ("Keyto Ref:             %08lX\n",PTR(ptr->keyto_ref)); 
     printf ("\n");
     dump_reference (ptr->keyto_ref);
     }

  if (ptr->set_ref != NULL) 
     {
     printf ("Set Ref:             %08lX\n",PTR(ptr->set_ref)); 
     printf ("\n");
     dump_reference (ptr->set_ref);
     }

  if (ptr->sizeto_ref != NULL) 
     {
     printf ("Sizeto Ref:             %08lX\n",PTR(ptr->sizeto_ref)); 
     printf ("\n");
     dump_reference (ptr->sizeto_ref);
     }
 

}

static void dump_write (void)

{
  Write_ptr ptr;

  ptr = current_ptr;

  error_line_set (ptr);

  printf ("Write Node At:       %08lX\n",PTR(ptr));

  if (ptr->file_ref != NULL) 
     {
     printf ("File Ref:             %08lX\n",PTR(ptr->file_ref)); 
     printf ("\n");
     dump_reference (ptr->file_ref);
     }

  if (ptr->from_ref != NULL) 
     {
     printf ("From Ref:             %08lX\n",PTR(ptr->from_ref)); 
     printf ("\n");
     dump_reference (ptr->from_ref);
     }

  if (ptr->keyfrom_expr != NULL) 
     {
     printf ("Keyfrom Expr:             %08lX\n",PTR(ptr->keyfrom_expr)); 
     printf ("\n");
     dump_reference (ptr->keyfrom_expr);
     }


}

static void dump_delete (void)

{
  Delete_ptr ptr;

  ptr = current_ptr;

  error_line_set (ptr);

  printf ("Delete Node At:       %08lX\n",PTR(ptr));

  if (ptr->file_ref != NULL) 
     {
     printf ("File Ref:             %08lX\n",PTR(ptr->file_ref)); 
     printf ("\n");
     dump_reference (ptr->file_ref);
     }

  if (ptr->key_expr != NULL) 
     {
     printf ("Key Expr:             %08lX\n",PTR(ptr->key_expr)); 
     printf ("\n");
     dump_reference (ptr->key_expr);
     }

  
}

static void dump_rewrite (void)

{

  Rewrite_ptr ptr;

  ptr = current_ptr;

  error_line_set (ptr);

  printf ("Rewrite Node At:       %08lX\n",PTR(ptr));

  if (ptr->file_ref != NULL) 
     {
     printf ("File Ref:             %08lX\n",PTR(ptr->file_ref)); 
     printf ("\n");
     dump_reference (ptr->file_ref);
     }

  if (ptr->from_ref != NULL) 
     {
     printf ("From Ref:             %08lX\n",PTR(ptr->from_ref)); 
     printf ("\n");
     dump_reference (ptr->from_ref);
     }

  if (ptr->key_expr != NULL) 
     {
     printf ("Key Expr:             %08lX\n",PTR(ptr->key_expr)); 
     printf ("\n");
     dump_reference (ptr->key_expr);
     }




}

static void dump_close (void)

{

 Rewrite_ptr ptr;

 ptr = current_ptr;

 error_line_set (ptr);

 printf ("Close Node At:       %08lX\n",PTR(ptr));

 if (ptr->file_ref != NULL) 
    {
    printf ("File Ref:             %08lX\n",PTR(ptr->file_ref)); 
    printf ("\n");
    dump_reference (ptr->file_ref);
    }

}

static void dump_get (void)

{

    Get_ptr       ptr;
    Getfile_ptr   fptr = NULL;
    Getstring_ptr sptr = NULL;

    ptr = current_ptr;

    error_line_set (ptr);

    printf ("\nGet Node At:          %08lX\n",PTR(ptr));

    if (ptr->getf_ptr != NULL)
       fptr = ptr->getf_ptr;

    if (ptr->gets_ptr != NULL)
       sptr = ptr->gets_ptr;

    if (fptr != NULL)
       {
       printf("Get FILE info:\n");
       dump_reference(fptr->file_ref);
       if (fptr->skip)
          printf("SKIP specified.\n");
       if (fptr->skip_expr != NULL)
          {
          printf("SKIP(expression) specified:\n");
          dump_expression(fptr->skip_expr);
          }
       dump_intarg(fptr->targ_ptr);
       dump_format(fptr->fmat_ptr);
       }       
       
    if (sptr != NULL)
       {
       printf("Get STRING info:\n");
       dump_expression(sptr->string_expr);
       dump_intarg(sptr->targ_ptr);
       dump_format(sptr->fmat_ptr);
       }
}

void dump_intarg (Intarg_ptr iptr)

{

Any_ptr           temp;

while (iptr != NULL)
      {
      dump_reference (iptr->target_ref);

      if (iptr->intarg_ptr != NULL)
         {
         printf("Nested I/P target\n");
         dump_intarg (iptr->intarg_ptr);
         }

      if (iptr->do_ptr != NULL)
         {
         printf("LIST <do-spec>\n");
         temp = current_ptr;
         current_ptr = iptr->do_ptr;
         dump_loop();
         current_ptr = temp;
         }
      iptr = iptr->next_ptr;

      if (iptr != NULL)
         {
         printf("Next I/P target in commalist\n");
         }
    
      }

}

static void dump_put (void)

{

    Put_ptr       ptr;
    Putfile_ptr   fptr = NULL;
    Putstring_ptr sptr = NULL;

    ptr = current_ptr;

    error_line_set (ptr);

    printf ("\nPut Node At:          %08lX\n",PTR(ptr));

    if (ptr->putf_ptr != NULL)
       fptr = ptr->putf_ptr;

    if (ptr->puts_ptr != NULL)
       sptr = ptr->puts_ptr;

    if (fptr != NULL)
       {
       printf("Put FILE info:\n");
       dump_reference(fptr->file_ref);
       if (fptr->skip)
          printf("SKIP specified.\n");
       if (fptr->skip_expr != NULL)
          {
          printf("SKIP(expression) specified:\n");
          dump_expression(fptr->skip_expr);
          }
       if (fptr->line_expr != NULL)
          {
          printf("LINE(expression) specified:\n");
          dump_expression(fptr->line_expr);
          }
       dump_outsrc(fptr->srce_ptr);
       dump_format(fptr->fmat_ptr);
       }       
       
    if (sptr != NULL)
       {
       printf("Put STRING info:\n");
       dump_reference(sptr->string_ref);
       dump_outsrc(sptr->srce_ptr);
       dump_format(sptr->fmat_ptr);
       }
}

static void dump_outsrc (Outsrc_ptr optr)

{

Any_ptr           temp;

while (optr != NULL)
      {
      dump_expression (optr->source_expr);

      if (optr->outsrc_ptr != NULL)
         {
         printf("Nested O/P source\n");
         dump_outsrc (optr->outsrc_ptr);
         }

      if (optr->do_ptr != NULL)
         {
         printf("LIST <do-spec>\n");
         temp = current_ptr;
         current_ptr = optr->do_ptr;
         dump_loop();
         current_ptr = temp;
         }
      optr = optr->next_ptr;

      if (optr != NULL)
         {
         printf("Next O/P source in commalist\n");
         }
    
      }

}




void dump_format (Format_ptr fptr)

{

short             counter = 0;

if (fptr != NULL)
   printf("\nBeginning dump of Format List.\n\n");

while (fptr != NULL)
      {
      counter++;
      printf("\nFormat entry number %d.\n\n",counter);

      if (fptr->fmat_itrn_expr != NULL)
         {
         printf("Format iteration expression.\n");
         dump_expression(fptr->fmat_itrn_expr);
         }

      if (fptr->fmat_itrn_value != 0)
         printf("Format iteration integer: %d\n",fptr->fmat_itrn_value);

      if (fptr->nest_ptr != NULL)
         {
         printf("Nested format list found, dump follows...\n\n");
         dump_format(fptr->nest_ptr);
         }

      if (fptr->fixed_expr1 != NULL)
         {
         printf("Format F found.\n");
         dump_expression(fptr->fixed_expr1);
         dump_expression(fptr->fixed_expr2);
         }

      if (fptr->float_expr1 != NULL)
         {
         printf("Format E found.\n");
         dump_expression(fptr->float_expr1);
         dump_expression(fptr->float_expr2);
         }
       
      if (fptr->pic_str != NULL)
         {
         printf("Format P found.\n");
         printf("picture: %s\n",fptr->pic_str);
         }

      if (fptr->charf)
         {
         printf("Format A found.\n");
         if (fptr->char_expr != NULL)
            {
            printf("A expression.\n");
            dump_expression(fptr->char_expr);
            }
         }

      if (fptr->bit_radix > 0)
         {
         printf("Format B found.\n");
         printf("Radix: %d\n",fptr->bit_radix);
         if (fptr->bit_expr)
            {
            printf("Bit expression:\n");
            dump_expression(fptr->bit_expr);
            }
         }

      if (fptr->l_format != 0)
         printf("Format L found.\n");

      if (fptr->tab)
         {
         printf("Format TAB found.\n");
         if (fptr->tab_expr != NULL)
            {
            printf("TAB expresion:\n");
            dump_expression(fptr->tab_expr);
            }
         }

      if (fptr->line_expr != NULL)
         {
         printf("Format LINE found.\n");
         printf("LINE expression:\n");
         dump_expression(fptr->line_expr);
         }

      if (fptr->space_expr != NULL)
         {
         printf("Format X found.\n");
         printf("X expression:\n");
         dump_expression(fptr->space_expr);
         }

      if (fptr->skip)
         {
         printf("Format SKIP found.\n");
         if (fptr->skip_expr != NULL)
            {
            printf("SKIP expresion:\n");
            dump_expression(fptr->skip_expr);
            }
         }
         
      if (fptr->colm_expr != NULL)
         {
         printf("Format COLUMN found.\n");
         printf("COLUMN expression:\n");
         dump_expression(fptr->colm_expr);
         }

      if (fptr->rmte_ptr != NULL)
         {
         printf("Format R found.\n");
         printf("R reference:\n");
         dump_reference(fptr->rmte_ptr);
         }
    
      fptr = fptr->next_ptr;

      }

}

/*************************************************************************/
/*               Analyse and process a SELECT node                       */
/*************************************************************************/ 

static void dump_select (void)

     {

     Select_ptr ptr;

     ptr = current_ptr;

     error_line_set (ptr);

     printf ("Select Node At:       %08lX\n",PTR(ptr));
     if (ptr->expr_ptr != NULL)
        printf ("Expression At:        %08lX\n",PTR(ptr->expr_ptr));
     printf ("\n");

     if (ptr->expr_ptr != NULL)
        dump_expression (ptr->expr_ptr);

     return;

     }

/*************************************************************************/
/*               Analyse and process an ENTRY node                       */
/*************************************************************************/ 

static void dump_entry (void)

     {

     Entry_ptr ptr;

     ptr = current_ptr;

     error_line_set (ptr);

     return;

     }



/***************************************************************************/
/* Verify that all aspects of this required do-loop are valid.             */
/***************************************************************************/ 

static void dump_loop (void)

     {

     Loop_ptr    ptr;
     Any_ptr next;

     ptr = current_ptr;

     error_line_set(ptr);

     printf ("Loop Node At:         %08lX\n",PTR(ptr));

     if (ptr->counter != NULL)
     printf ("Counter  Ref:         %08lX\n",PTR(ptr->counter));

     if (ptr->start != NULL)
     printf ("Start    Expr:        %08lX\n",PTR(ptr->start));

     if (ptr->finish != NULL)
     printf ("'TO'     Expr:        %08lX\n",PTR(ptr->finish));

     if (ptr->repeat != NULL)
     printf ("'REPEAT' Expr:        %08lX\n",PTR(ptr->repeat));

     if (ptr->step != NULL)
     printf ("'BY'     Expr:        %08lX\n",PTR(ptr->step));

     if (ptr->while_expr != NULL)
     printf ("'WHILE'  Expr:        %08lX\n",PTR(ptr->while_expr));

     if (ptr->until_expr != NULL)
     printf ("'UNTIL'  Expr:        %08lX\n",PTR(ptr->until_expr)); 
   
     printf ("\n");

     dump_reference  (ptr->counter);
     dump_expression (ptr->start);
     dump_expression (ptr->finish);
     dump_expression (ptr->repeat);
     dump_expression (ptr->step);
     dump_expression (ptr->while_expr);
     dump_expression (ptr->until_expr);

     if (ptr->stmt_ptr == NULL) /* This is probably NOT a real LOOP, but a <do-spec> in a PUT/GET LIST */
        return;

     next = read_next_node();

     while (dump_nodes(next) == 0)
           {
           next = read_next_node();
           }

 
     /* OK An END was read, so thats the END of THIS LOOP ! */
     
     return;

     }

/*************************************************************************/
/*               Analyse and process a do   node.                        */
/*************************************************************************/ 

static void dump_do (void)

     {

     Do_ptr       ptr;
     Any_ptr  next;

     ptr  = current_ptr;
     next = read_next_node();
 
     printf ("Do Node At:           %08lX\n",PTR(ptr));
     printf ("\n");

     while (dump_nodes(next) == 0)
           {
           next = read_next_node();
           }
     
     /* An end must have been read, so its the end of this loop */

     return;

     }

/*************************************************************************/
/*               Analyse and process a BEGIN node                        */
/*************************************************************************/ 

static void dump_begin (void)

     {

     Begin_ptr       ptr;
     Any_ptr     next;

     next = read_next_node();
     ptr  = current_ptr;

     printf ("Begin Node At:        %08lX\n",PTR(ptr));
     printf ("end pointer:          %08lX\n",PTR(ptr->end)); 
     printf ("\n");

     while (dump_nodes(next) == 0)
           {
           next = read_next_node();
           }
     
     /* An end must have been read, so its the end of this loop */

     return;

     }



/*************************************************************************/
/*               Analyse and process an ALLOCATE node                    */
/*************************************************************************/ 

static void dump_allocate (void)

     {

     Allocate_ptr ptr;

     ptr = current_ptr;

     error_line_set (ptr);

     printf ("Allocate Node At:     %08lX\n",PTR(ptr));
     printf ("Target:               %s\n",ptr->target->spelling);
     printf ("Memory:               %s\n",ptr->area->spelling);
     printf ("\n");

     return;

     }

/*************************************************************************/
/*               Analyse and process a FREE node.                        */
/*************************************************************************/ 

static void dump_free (void)

     {

     Free_ptr     ptr;

     ptr = current_ptr;

     error_line_set (ptr);

     printf ("Free Node At:         %08lX\n",PTR(ptr));
     printf ("Target:               %s\n",ptr->target->spelling);
     printf ("\n");


     return;

     }

/*************************************************************************/
/*               Analyse and process a GOTO node.                        */
/*************************************************************************/ 

static void dump_goto (void)

     {

     Goto_ptr ptr;

     ptr = current_ptr;

     error_line_set(ptr);
     printf ("Goto Node At:         %08lX\n",PTR(ptr));
     printf ("Target:               %s\n",ptr->target->spelling);
     printf ("\n");

     return;

     }

/*************************************************************************/
/*               Analyse and process an ON node.                         */
/*************************************************************************/ 

static void dump_on (void)

     {

     On_ptr ptr;

     ptr = current_ptr;

     error_line_set(ptr);
     printf ("On Node At:           %08lX\n",PTR(ptr));

     switch (ptr->cond_type) {
     case(ERROR):
         printf ("Condition: error\n");
         break;
     case(IO_CONDITION):
         printf ("Condition: I/O\n");
         dump_reference(ptr->ref_ptr);
         break;
     case(SIZE):
         printf ("Condition: size\n");
         break;
     case(FINISH):
         printf ("Condition: finish\n");
         break;
     case(USER_CONDITION):
         printf ("Condition: user_defined\n");
         dump_reference(ptr->ref_ptr);
         break;
     }

     printf ("Unit At:              %08lX\n",PTR(ptr->unit_ptr));
     if (ptr->snap)
        printf ("Snap Enabled\n");
     if (ptr->system)
        printf ("System Enabled\n");
     printf ("\n");

     return;

     }
/*************************************************************************/
/*               Analyse and process a LEAVE node.                       */
/*************************************************************************/ 

static void dump_leave (void)

     {

     Leave_ptr ptr;

     ptr = current_ptr;

     error_line_set(ptr);
     printf ("Leave Node At:        %08lX\n",PTR(ptr));
     printf ("Target:               %s\n",ptr->ref->spelling);
     printf ("\n");

     return;

     }

/*************************************************************************/
/*               Analyse and process an END node.                        */
/*************************************************************************/ 

static void dump_end (void)

     {

     End_ptr ptr;

     /* Dont read next node, caller will (?) */

     ptr = current_ptr;

     printf ("End Node At:          %08lX\n",PTR(ptr));
     printf ("Partner  At:          %08lX\n",PTR(ptr->partner));
     printf ("\n");
     
     return;

     }

/*************************************************************************/
/*               Analyse and process a LABEL node                        */
/*************************************************************************/ 

static void dump_label (void)

     {

     Label_ptr ptr;

     ptr = current_ptr;

     printf ("Label Node At:        %08lX\n",PTR(ptr));
     printf ("Name:                 %s\n",ptr->identity->spelling);
     printf ("\n");
   
     return;

     }


/*************************************************************************/
/*               Analyse and process an ASSIGNMENT node                  */
/*************************************************************************/ 

static void dump_assignment (void)

     {

     Assignment_ptr ptr;

     ptr = current_ptr;

     error_line_set (ptr);

     printf ("Assignment Node At:   %08lX\n",PTR(ptr));
     if (ptr->target == NULL)
        printf ("Target Pointer is:    NULL\n");
     else
        printf ("Target At:            %08lX\n",PTR(ptr->target));

     if (ptr->source == NULL)
        printf ("Source Pointer is:    NULL\n");
     else
        printf ("Source At:            %08lX\n",PTR(ptr->source));
     printf ("\n");

     dump_reference (ptr->target);

     dump_expression (ptr->source);
     
     return;

     }


/**************************************************************************/
/* This function sets the external variable 'line_no' to that held in the */
/* program node currently being processesed. The variable is used to      */
/* report error line numbers in report.c                                  */
/**************************************************************************/    

static void error_line_set (Any_ptr g_ptr)

     {

     Dummy_ptr       d_ptr;

     d_ptr = g_ptr;

     strcpy (line_no,d_ptr->line_no);

     }

/**************************************************************************/
/* Determine the arithmetic SCALE of the result of this operator node     */
/* This is dependend on the SCALE of the two children of this node.       */
/**************************************************************************/ 

/***************************************************************************/
/* This function accepts a ptr to a Ref node, and verifes all semantic     */
/* aspects of the reference.                                               */
/***************************************************************************/

static void dump_reference (Ref_ptr ptr)

     {
   
     if (ptr == NULL)
        return;

     /* s_ptr = ptr->symbol; */

     printf ("Reference Node At:    %08lX\n",PTR(ptr)); 
     printf ("Symbol Name:          %s\n",ptr->symbol->spelling);
     if (ptr->dot_ptr != NULL)
        printf ("Structure ref At:     %08lX\n",PTR(ptr->dot_ptr));
     if (ptr->ptr_ptr != NULL)
        printf ("Pointer ref At:       %08lX\n",PTR(ptr->ptr_ptr));
     if (ptr->sublist != NULL)
        printf ("Subscript ref At:     %08lX\n",PTR(ptr->sublist));
     if (ptr->ofx_ptr != NULL)
        printf ("Offset Expression:    %08lX\n",PTR(ptr->ofx_ptr));
 
     printf ("\n");
     
     /* sub_cnt = */ 

     dump_sublist (ptr->sublist);

     dump_reference (ptr->dot_ptr);

     dump_reference (ptr->ptr_ptr);

     dump_expression (ptr->ofx_ptr);

     }

/**************************************************************************/
/*        Validate the expressions used in each subscript reference.      */
/**************************************************************************/

static int dump_sublist (Sub_ptr ptr)

    {

    short         count;

    if (ptr == NULL)
       return(0);

    count = 0;

    printf ("Subscript Node At:    %08lX\n",PTR(ptr)); 
 
    while (ptr != NULL)
          {
          count++;
          dump_expression (ptr->expression);
          ptr = ptr->next_ptr;
          }

    return (count);

    
    } 

/***************************************************************************/
/* Validate a structure qualified reference, for ambiguity, and existence  */
/***************************************************************************/
     
static void dump_qualification (Block_ptr b_ptr,Ref_ptr r_ptr)

     {

     Symbol_ptr    s_ptr;   
     char          name[128] = "";

     if (b_ptr == NULL)
        return;

     if (r_ptr->dot_ptr == NULL) /* Not a qualified name */
        {
        r_ptr->data_type = r_ptr->symbol->type;
        r_ptr->scale     = r_ptr->symbol->scale;
        return;
        }

     match_count = 0;

     s_ptr = b_ptr->first_symbol;

     while (s_ptr  != NULL)
           {
           if (s_ptr->structure)
              {
              if (strcmp(s_ptr->spelling,r_ptr->symbol->spelling) == 0)
                 dump_struc_ref (name,s_ptr->child,r_ptr->dot_ptr);
              else
                 dump_struc_ref (name,s_ptr->child,r_ptr);
              }
           /**********************************************/
           /* OK Get the next symbol in this PL/1 block  */
           /**********************************************/


           s_ptr = s_ptr->next_ptr;
           }

     if (match_count == 0)
        dump_qualification (b_ptr->parent,r_ptr);

     /**************************************************************/
     /* If we had NO match_count then, the qualified reference is crap */
     /* If we had more than one, then its ambiguous !              */
     /**************************************************************/ 

     if (match_count == 0)      /* no known strucs can satisfy reference */
        report (79,name,__LINE__);
     else
     if (match_count > 1)
        report (87,name,__LINE__);   /* ambiguous struc ref */
     else
        {
        /*************************************************/
        /* the type and scale of this ref node, must be  */
        /* copied from its partner.                      */
        /*************************************************/
        r_ptr->data_type = r_ptr->dot_ptr->data_type;
        r_ptr->scale     = r_ptr->dot_ptr->scale;
        } 
     }

/***************************************************************************/
/* Recursively, walk-down the reference tree trying to match the qualified */
/* reference with the structure parent passed in (s_ptr).                  */
/***************************************************************************/
  
static void dump_struc_ref (char * name,Symbol_ptr s_ptr,Ref_ptr r_ptr)
                               
     {

     if (r_ptr == NULL)
        {
        match_count++;
        return;
        }

     if (r_ptr->dot_ptr == NULL) /* ie last component of the name */
        {
        name[0] = '\x00';
        strcat (name,r_ptr->symbol->spelling);
        } 

     while (s_ptr != NULL)
           {
           if (strcmp(s_ptr->spelling,r_ptr->symbol->spelling) == 0)
              {
              /********************************************************/
              /* set the attributes of this ref, from the simple name */
              /********************************************************/
              r_ptr->data_type = s_ptr->type;
              r_ptr->scale     = s_ptr->scale;
              dump_struc_ref (name,s_ptr->child,r_ptr->dot_ptr);
              }
           else
              dump_struc_ref (name,s_ptr->child,r_ptr);
           
           /**********************************************/
           /* OK Get the next symbol in this PL/1 block  */
           /**********************************************/

           s_ptr = s_ptr->sister;
           }

     /*****************************************************************/
     /* Before we return, we check to see if 'match_count' is set to one  */
     /* if it is, then we know that the ref-node following this one   */
     /* MUST have had its attributes set, so we set ours now !        */
     /*****************************************************************/

     if ((match_count == 1) && (r_ptr->dot_ptr != NULL))
        { 
        r_ptr->data_type = r_ptr->dot_ptr->data_type;
        r_ptr->scale     = r_ptr->dot_ptr->scale;
        }
     }


static int  get_next_type (Any_ptr g_ptr)

     {

     Dummy_ptr       d_ptr;

     if (g_ptr == NULL)
        return (UNKNOWN);

     d_ptr = g_ptr;

     return (nodetype(d_ptr->next_ptr));

     }   

static Any_ptr read_next_node (void)

     {

     Dummy_ptr             d_ptr;


     if  (current_ptr == NULL)
         current_ptr = block_root->first_stmt;
     else
        {  
        d_ptr    = current_ptr;
        current_ptr = d_ptr->next_ptr;
        }

     if (current_ptr == NULL)
        longjmp (exit_dt,1);

     return (current_ptr);

     }
       /*
long

   PTR(arg)

   Any_ptr              arg;

   {
  
   long *			long_temp;
   char				bytes[4];
   char				copy[4];

   memcpy(copy,&(arg),4);

   bytes[0] = copy[0];
   bytes[1] = copy[1];
   bytes[2] = copy[2];
   bytes[3] = copy[3];

   long_temp = (long *)bytes;

   return(*long_temp);

   }
*/
