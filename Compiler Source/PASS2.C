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
/*                         PL/1 Compiler For DOS.                           */
/****************************************************************************/

/****************************************************************************/
/*                         Modification History                             */
/****************************************************************************/
/*  Who    When                           What                              */
/* ------------------------------------------------------------------------ */
/* HWG1   12-04-91       Initial Prototype.                                 */
/* HWG2   19-04-91       Rewritten to work in a purely recursive manner     */
/* HWG3   01-05-91       Structure and pointer qualification added          */
/* HWG4   10-05-91       Bug to do with mismatch in arg counts fixed when   */
/*                       checking calls to external procedures.             */
/* HWG5   18-07-91       Functions cant be called, procs MUST be called.    */
/* HWG6   22-07-91       Returns cant have result in a proc, but must have  */
/*                       a result in a function.                            */
/* HWG7   22-07-91       Funcs must now have at-least one return stmt.      */
/* HWG8   02-08-91       References are now resolved totally in pass2.      */
/* HWG9   12-08-91       Ambigous refs not reported/found when the ref was  */
/*                       just a simple unqualified ref, ie:   a = b(3);     */
/*                       and 'b' is ambiguous.                              */                                                       
/* HWG10  08-09-91       Calls to procs were being mistaken for functions   */
/*                       in the function 'validate_reference'.              */
/* HWG11  08-09-91       Specific errors now exist for too many/few args    */
/* HWG12  10-09-91       Ext entries with wrong num args, were reported as  */
/*                       array mis-subscripting !                           */      
/* HWG13  13-09-91       Offset expression trees are built for every        */
/*                       reference processeed. This will be used by code    */
/*                       generator to create accessing code.                */ 
/* HWG14  16-09-91       allocate_node was NOT declared in here but the it  */
/*                       still compiled and linked OK, but got a run error. */ 
/* HWG15  16-09-91       Erroneous errors were reported, for variables that */
/*                       had not actually been resolved. This was due to    */
/*                       the fact that 'validate_reference' was not doing   */
/*                       a test to see if the ref had been resolved and so  */
/*                       was simply using a NULL symbol ptr blindly.        */
/* HWG16  18-09-91       There appears to be a printing error in Bornat(141)*/
/*                       regarding vector offset calculations. The correct  */
/*                       method was hand derived, and also confirmed in the */
/*                       Barrett and Couch compiler book.                   */
/*                       Both 'build_vector_index' and 'multiply_dims' were */
/*                       modified.                                          */          
/* HWG17  20-09-91       build_oe_tree was incorrectly processing call nodes*/
/*                       when it should not process these references.       */
/* HWG18  23-09-91       Undeclared parameters were reported as errors they */
/*                       are now reported as warnings.                      */
/* HWG19  28-09-91       Calls to internal procedures were being resolved   */
/*                       incorrectly when an internal proc had same name as */
/*                       parent proc. find_inner_block now looks for an     */
/*                       internal proc first, before looking at parent.     */
/* HWG20  12-10-91       The structure of the parse-tree was altered so that*/
/*                       the tree root is now held in the block-node of a   */
/*                       PL/1 block. All phases now have to walk the tree   */
/*                       by starting at the block node.                     */ 
/* HWG21  22-10-91       When refs to internal procs are resolved we now    */
/*                       setup the line-number and other fields correctly.  */
/*                                                                          */
/* HWG22  23-10-91       The 'main' procedure cannot have any parameters.   */
/*                                                                          */
/* HWG23  29-10-91       Arg/Param compatibility included for internal proc */
/*                       as well as external procs.                         */
/* HWG24  02-11-91       When an undeclared name was compiler-declared as a */
/*                       bin(15,0) its precision was not being set.         */  
/* HWG25  28-11-91       Check included to test that functions are reffered */
/*                       to correctly in expressions.                       */
/*                                                                          */
/* HWG26  30-11-91       The conversion for decimal to binary has been has  */
/*                       been incorporated, with conversion attributes      */
/*                       calculated and stored for use by the code generator*/            
/* HWG27  19-12-91       When a qualified ref was not resolved, because of  */
/*                       a member element not existing, pass2 still tried to*/
/*                       validate etc the partially built ref tree, this    */
/*                       was causing a system hang.  See HWG15 also.        */ 
/* HWG28  20-12-91       Select's and When's were not being processed in    */
/*                       here, they are now.                                */
/* HWG29  23-12-91       Processing of the 'signal' statement included.     */
/*                                                                          */ 
/* HWG30  01-01-92       check_logical wasnt doing anything with the sub    */
/*                       expressions of a logical operator.                 */  
/* HWG31  05-01-92       When processing a put node and inserting call      */
/*                       nodes to support routines, chained put nodes were  */
/*                       causing a problem. If a put pointed to a previous  */
/*                       put and that put had been replaced, we got a prob  */
/*                       when we tried to process that second put.          */
/* HWG32  15-01-92       When building offset expressions, the compiler was */
/*                       not setting the attribs ptr in Ref nodes that it   */
/*                       created. This caused array address calculations to */
/*                       foul-up.                                           */ 
/*                                                                          */
/* HWG33  17-01-92       When building offset expressions, the compiler was */
/*                       faulting unless a subscripted qualified reference  */
/*                       had its subscripts specified in the 'normal' way   */
/*                       that is:    a.b.c.d(1,2,3,4) was not being treated */
/*                       the same as: a(1).b(2).c(3).d(4) ALL such refs are */
/*                       now converted to the latter from by using the      */
/*                       function: reorder_subscripts.                      */
/*                                                                          */       
/* HWG34  19-01-92       The function 'simplify_oe_tree' has been coded.    */
/*                       it had a very subtle bug however. It used pointers */
/*                       that it did not initialise, and the first time it  */
/*                       was called they were (probably) zero/null.         */
/*                       However when presented with certain recursive types*/
/*                       of references it was possible for an old stack     */
/*                       frame to be allocated again to this function.      */
/*                       Consequently previous VALID ptr values would exist */
/*                       that misled the function into building calculation */
/*                       trees when it shouldnt. ALWAYS INITIALISE vars !!  */     
/*                                                                          */
/* HWG35  20-01-92       'qualify_reference' was not 'moving' the sublist   */
/*                       down the ref chain as it qualified a ref.          */
/*                                                                          */
/* HWG36  07-03-93       Assigment's now have any BY NAME option verified.  */
/*                                                                          */
/* HWG37  14/08/93       Assignments to cnstants are now reported !         */
/*                                                                          */
/* HWG38  20-01-96       Conversion ops, were being declared with no line   */
/*                       number, this caused symtab printing code to go     */
/*                       down the tube, cos it expects all names to be      */
/*						 declared with a valid line number !				*/
/*						 function 'dlc_entry' was changed.                  */
/*                                                                          */
/* HWG39  27-04-96       reorder_subscripts() was trying to append a        */
/*                       non-existent subscript node to a reference.        */
/*                       This only occurs when a reference has fewer        */
/*                       subscripts than dimensions.                        */
/*                                                                          */
/* HWG40  08-05-96       Included support for PL/I RECORD I/O stuff.        */
/*                                                                          */ 
/* HWG41  02-10-02       Subscripted scalars (ie not arrays) with > 1       */
/*                       subscript caused a trap. This fix prevents the     */
/*                       array analysis being called when number of subs is */
/*                       not equal to number of dims.                       */ 
/*                                                                          */
/* HWG42  02-10-02       check_loop should only scrutinize loop counter if  */
/*                       one is present! The code was assuming one.         */
/*                                                                          */ 
/* HWG43  26-04-06       Added a flag to Ref node that indicates whether    */
/*                       the reference is/was resolved. We should check     */
/*                       this before assuming the ref tree is useable.      */
/*                       The func: 'check_reference' is used a lot and this */
/*                       should be changed to return true/false.            */
/****************************************************************************/


/****************************************************************************/
/*                         Functional Description                           */
/****************************************************************************/
/* This source file contains all PL/1 semantic checking, and consists of a  */
/* set of mutually recursive functions that walk the parse-trees built by   */
/* pass1.                                                                   */
/* All refs are resolved to their declarations and validated in this phase. */
/* This pass will (for each type of tree node) verify that all sub-trees of */
/* a node are valid, and that all PL/1 references have the correct PL/1     */
/* attributes for the context of their use. Where appropriate, conversion   */
/* operators are inserted into the parse-tree.                              */    
/* Result attributes are deduced, and propagated up each expression tree.   */
/* After a reference has been resolved, its offset expression is calculated */
/* as an expression tree, and its root held in ref->ofx_ptr.                */
/* The offset expression reflects a calculation that must be performed at   */
/* runtime in order to access the desired location. The offset expression   */
/* will be translated to memory accessing code by the code generator.       */ 
/****************************************************************************/

# include "math.h"
# include "setjmp.h"
# include "stdlib.h"
# include "stdio.h"
# include "string.h"
# include "c_types.h"
# include "tokens.h"
# include "nodes.h"
# include "symtab.h"
# include "convert.h"

# define  chur		unsigned char
# define _LINE_     ((short)__LINE__)

jmp_buf exit_pass2;

Any_ptr allocate_node            (short );
Any_ptr free_node                (Any_ptr);
void    insert_node              (Any_ptr,Any_ptr);
static Any_ptr get_next_node            (void);
static short   process_nodes            (Any_ptr);
short   nodetype                 (Any_ptr);
       void   report                   (short ,char *,short );
static void   check_select             (void);
static void   check_when               (void);
static void   check_other              (void);
static void   check_on                 (void);
static void   check_entry              (void);
static void   check_procedure          (void);
static void   check_if                 (void);
static void   check_stop               (void);
static void   check_call               (void);
static void   test_arguments           (Ref_ptr);
static void   check_put                (void);
static void   check_open               (void);
static void   check_return             (void);
static void   check_loop               (void);
static void   check_do                 (void);
static void   check_begin              (void);
static void   check_allocate           (void);
static void   check_open               (void);
static void   check_close              (void);
static void   check_read               (void);
static void   check_write              (void);
static void   check_rewrite            (void);
static void   check_delete             (void);
static void   check_get                (void);
static void   check_intarg             (Intarg_ptr);
static void   check_outsrc             (Outsrc_ptr);
static void   check_format             (Format_ptr);
static void   check_free               (void);
static void   check_goto               (void);
static void   check_signal             (void); 
static void   check_end                (void);
static void   check_label              (void);
static void   check_assignment         (void); 
static Any_ptr check_concat             (Any_ptr);
static Any_ptr check_arithmetic         (Any_ptr);
static Any_ptr check_logical            (Any_ptr);
static Any_ptr check_expression         (Any_ptr);
static void    check_operand            (Any_ptr); 
       void    set_error_line           (Any_ptr);
static chur    get_common_scale         (Oper_ptr);
static chur    get_common_base          (Oper_ptr);
static void    get_precision            (Oper_ptr);
static short   next_node_type           (Any_ptr);
static void    resolve_qualification    (Block_ptr,Ref_ptr);
static void    check_leave              (void);
static short   arithmetic               (short);
static short   qualified                (Ref_ptr);

/***************************************************************************/
/*                      Reference procesing functions.                     */
/***************************************************************************/

static void        validate_reference    (Ref_ptr);   
static void        resolve_reference     (Ref_ptr);
static Symbol_ptr  find_declaration      (Block_ptr, char *);
static Symbol_ptr  find_inner_dcl        (Block_ptr, char *);
static Symbol_ptr  search_structure      (Symbol_ptr, char *); 
       Block_ptr   find_inner_block      (Block_ptr, char *);
static void        resolve_qualification (Block_ptr,Ref_ptr); 
static void        resolve_struc_ref     (char *,Symbol_ptr,Ref_ptr);
static void        qualify_reference     (Ref_ptr);
static void        reorder_subscripts    (Ref_ptr);
       void        append_subscript      (Ref_ptr,Sub_ptr);
static Sub_ptr     remove_subscript      (Ref_ptr); 
       chur        check_reference       (Ref_ptr);
static Ref_ptr     get_reference_tail    (Ref_ptr);
static short       subscript_count       (Ref_ptr);
static short       dimension_count       (Ref_ptr);
static short       num_inherited_dims    (Symbol_ptr);
static short       check_sublist         (Sub_ptr);
static void        build_oe_tree         (Ref_ptr);
static Any_ptr     build_component_oe    (Ref_ptr);
static Any_ptr     build_vector_index    (Ref_ptr);
static Any_ptr     subtract_one          (Symbol_ptr,Any_ptr);
static Any_ptr     multiply_dims         (Symbol_ptr,short );
static Any_ptr     build_dim_tree        (Symbol_ptr,Dim_ptr);
static Symbol_ptr  create_constant       (Block_ptr,char *,short );
static Any_ptr     simplify_oe_tree      (Any_ptr);
       short       builtin               (char *);

/****************************************************************************/
/*                    Conversion processing functions.                      */
/****************************************************************************/

static Any_ptr      apply_conversion      (Oper_ptr);
static Any_ptr      select_conversion     (Any_ptr,short ,short ,short ,short );
static Any_ptr      insert_conversion     (Symbol_ptr,Any_ptr,Symbol_ptr);
static Any_ptr      cv_fixed_bin          (Any_ptr);
static Any_ptr      cv_fixed_dec          (Any_ptr);
static Any_ptr      cv_float_bin          (Any_ptr);
static Any_ptr      cv_float_dec          (Any_ptr);  
static Symbol_ptr   dcl_entry             (char *);
static Symbol_ptr   dcl_temp              (void);

/* flags used to record the conversions used */

short           conversions_in_use = 0;
short           cv_fixed_bin_r     = 0;
short           cv_fixed_dec_r     = 0; 
short           cv_float_dec_r     = 0;
short           cv_float_bin_r     = 0;
short           cv_arithmetic_r    = 0;

/****************************************************************************/
/*                     Miscellaneous declarations.                          */
/****************************************************************************/

short         ref_was_resolved = 0;   
Any_ptr       curr_ptr = NULL;
long          unique_val = 0;   
extern char   line_no[10]; /* used by error report function */
extern short  trace_pass2; /* for compiler debugging */
extern short  semantic_reqd;
long          matches;
extern        short  optimize_reqd;
/*extern*/        Block_ptr block_root;


                                 /*****************************************/
Block_ptr     curr_block = NULL; /* This is also set by dcl phase when it */
                                 /* tries to resolve refs used in the     */
                                 /* targets of based/defined variables.   */
                                 /* dcl sets it to the declarator of the  */
                                 /* based/defined symbol it is processing */
                                 /*****************************************/
   
/****************************************************************************/
/*         This is the main function entry for pass2 processing.            */
/****************************************************************************/

void 

   enter_pass2 (void)

   {

   Any_ptr  ptr;

   if (setjmp(exit_pass2) == 0) /* Exit pass2, if we get a null node-ptr */
      {                         /* this may happen if pass1 exited early */
      ptr = get_next_node();    /* due to early EOF. In this case the    */
      process_nodes (ptr);      /* parse-tree may be incomplete.         */
      }
   else
      return;

   }

/***************************************************************************/
/* This function will take a node-ptr and call a pass2 function to process */
/* that node !                                                             */
/***************************************************************************/

static
short   

   process_nodes (Any_ptr node_ptr)

   {

   Begin_ptr      bg_ptr;
   Procedure_ptr  p_ptr;
   Block_ptr      b_saved;
   Block_ptr      cb_ptr; /* child block */
   Any_ptr        here_ptr;
   short          ntype;
   char           ntypestr[32];

   if (node_ptr == NULL)
      return (0);

   ntype = nodetype(node_ptr);

   switch (ntype) {

   case (PROCEDURE):
        b_saved = curr_block;
        p_ptr   = node_ptr;
        curr_block = p_ptr->proc;
        here_ptr   = curr_ptr;
        curr_ptr   = p_ptr->proc->first_stmt;
        check_procedure ();
        curr_ptr   = here_ptr;
        curr_block = b_saved;
        break;
   case (ENTRY):
        check_entry();
        break;
   case (IF):
        check_if();
        break;
   case (SELECT):
        check_select();
        break;
   case (WHEN):
        check_when();
        break;
   case (OTHER):
        check_other();
        break;
   case (STOP):
        check_stop();
        break;
   case (CALL):
        check_call();
        break;
   case (RETURN):
        check_return();
        break;
   case (LOOP):
        check_loop();
        break;
   case (DO):
        check_do();
        break;
   case (BEGIN):
        b_saved = curr_block;
        bg_ptr   = node_ptr;
        curr_block = bg_ptr->block;
        here_ptr   = curr_ptr;
        curr_ptr   = bg_ptr->block->first_stmt;
        check_begin ();
        curr_ptr   = here_ptr;
        curr_block = b_saved;
        break;
   case (ALLOCATE):
        check_allocate();
        break;
   case (FREE):
        check_free();
        break;
   case (GOTO):
        check_goto();
        break;
   case (SIGNAL):
        check_signal();
        break; 
   case (LEAVE):
        check_leave();
        break;
   case (END):
        return(END);
   case (LABEL):
        check_label();
        break;
   case (ASSIGNMENT):
        check_assignment();
        break;
   case (PUT):
        check_put();
        break;
   case (ON):
        check_on();
        break;
   case (OPEN):
        check_open();
        break;
   case (CLOSE):
        check_close();
        break; 
   case (READ):
        check_read();
        break;
   case (WRITE):
        check_write();
        break;
   case (DELETE):
        check_delete();
        break;
   case (REWRITE):
        check_rewrite();
        break;
   case (GET):
        check_get();
        break; 
   default:
        {
        set_error_line(node_ptr); /* set global error line number, for diagnostics */
        itoa(ntype,ntypestr,10);
        report (62,ntypestr,_LINE_);   /* weird node found ! */
        return(0);
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
            process_nodes(cb_ptr->first_stmt);
            cb_ptr = cb_ptr->sister;
            }
       }

   return(0);

   }

/****************************************************************/
/* This function is called to check/process an expression tree. */
/* It returns a pointer to a (possibly new) expression tree.    */ 
/****************************************************************/

static
Any_ptr 

   check_expression (Any_ptr node_ptr)

   {

   short                nt;
   char                 ntypestr[32];

   if (node_ptr == NULL)
      return(NULL);

   nt = nodetype(node_ptr);

   switch (nt) {
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
       return(check_logical (node_ptr));
   case(CONCAT):
       return(check_concat (node_ptr)); 
   case(PLUS):
   case(MINUS):
   case(TIMES):
   case(DIVIDE):
       return(check_arithmetic (node_ptr));
   case(REFERENCE):
       check_reference (node_ptr); 
       check_operand (node_ptr); 
       return(node_ptr);
   case(SYMBOL):
       return(node_ptr);  /* check_symbol not implemented yet */
   default:
       {
       itoa(nt,ntypestr,10);
       set_error_line(node_ptr);
       report(62,ntypestr,_LINE_);
       }
   }

   return(node_ptr); /* return same expression node */

   }

/****************************************************************************/
/*            Process the operands of a logical PL/I operator.              */
/****************************************************************************/

static
Any_ptr

   check_logical (Any_ptr g_ptr)

   {

   Oper_ptr       o_ptr;

   o_ptr = g_ptr;

   o_ptr->left_ptr = check_expression (o_ptr->left_ptr);
   o_ptr->rite_ptr = check_expression (o_ptr->rite_ptr);

   return(g_ptr);

   }

/****************************************************************************/
/*          Process the operands of miscellaneous PL/I operators.           */
/****************************************************************************/

static
Any_ptr

   check_concat (Any_ptr g_ptr)

   {

   return(g_ptr);

   }     

/*************************************************************************/
/* Analyse and process a PL/1 arithmetic operator node, and any subnodes */
/*************************************************************************/ 

static
Any_ptr 

   check_arithmetic (Any_ptr g_ptr)

   {

   Ref_ptr     r_ptr    = NULL;
   Oper_ptr    ptr      = NULL;
   Symbol_ptr  a_ptr    = NULL;
   Any_ptr     new_expr = NULL;
   chur        s;
   chur        b;

   ptr = g_ptr;

   if (nodetype(ptr->left_ptr) == REFERENCE)
      {
      r_ptr = ptr->left_ptr;
      if (check_reference(r_ptr))
         {
         a_ptr = r_ptr->attribs;
         if (a_ptr->computational == 0)
            report (76,a_ptr->spelling,_LINE_);
         }
      }
   else
      ptr->left_ptr = check_expression (ptr->left_ptr);

   if (nodetype(ptr->rite_ptr) == REFERENCE)
      {
      r_ptr = ptr->rite_ptr;
      if (check_reference(r_ptr))
         {
         a_ptr = r_ptr->attribs;
         if (a_ptr->computational == 0)
            report (76,a_ptr->spelling,_LINE_);
         }
      }
   else
      ptr->rite_ptr = check_expression (ptr->rite_ptr);

   /*********************************************************************/
   /*               A NOTE ON ARITHMETIC OPERAND CONVERSION             */
   /*********************************************************************/
   /* The ANSI rules say (ie Page 320 Infix-Add) that either or both of */
   /* the arguments for an arithmetic operator may  be converted if they*/
   /* are different.                                                    */
   /* We cannot determine the conversions required for these arguments  */
   /* until the base and scale has been determined for the result.      */
   /* Once we have done this we convert none, one or both arguments to  */
   /* the base and scale (derived earlier) for the result.              */
   /* So if we have   dec + bin, the result type is bin, so we must     */
   /* convert the dec to a bin before adding.                           */
   /* So the processing proceeds in three steps:                        */
   /* 1. Determine the results derived base and scale.                  */
   /* 2. Apply conversion operators to those arguments that need it.    */
   /* 3. Using the the arguments new precisions, determine result prec. */
   /*********************************************************************/

   /*********************************************************************/
   /* Now determine the base and scale attributes of each sub-result in */
   /* the expression-tree.                                              */
   /*********************************************************************/

   b = get_common_base  (ptr);  /* bin/dec */
   s = get_common_scale (ptr);  /* float/fixed */

   /***********************************************************************/
   /* Set this operator nodes base & scale, from the common base/scale.   */
   /***********************************************************************/

   ptr->base  = b;
   ptr->scale = s;

   /***********************************************************************/
   /* Now examine the operator nodes to see if their arguments need to be */
   /* subjected to a conversion.                                          */
   /***********************************************************************/     

   new_expr = apply_conversion(ptr);

   /************************************************************************/
   /* Having applied conversion operators, we can now deduce the precision */
   /* of this operators arithmetic result using the ANSI rules.            */
   /************************************************************************/

   get_precision (ptr);

   return (new_expr);

   }

/*************************************************************************/
/*               Analyse and process a PROCEDURE node.                   */
/*************************************************************************/ 

static
void 
   
   check_procedure (void)

   {

   Procedure_ptr   p_ptr = NULL;
   Symbol_ptr      s_ptr = NULL;
   Argument_ptr    a_ptr = NULL;
   Any_ptr     next;

   p_ptr = curr_ptr;
   next = get_next_node();
 
   /***************************************************************/
   /* Report any undeclared parameters, that this block may have. */
   /***************************************************************/

   a_ptr = p_ptr->argument;
   
   while (a_ptr != NULL)
         {
         s_ptr = a_ptr->arg_ptr;
         if (s_ptr->declared == 0)
            {
            /***********************************************************/
            /* This undeclared name must now be declared as bin(15).   */
            /***********************************************************/ 
            s_ptr->type     = BINARY;
            s_ptr->declared = 1;
            s_ptr->scale    = FIXED;
            s_ptr->prec_1   = 15;
            s_ptr->prec_2   = 0;
            s_ptr->bytes    = 2;
            s_ptr->class    = PARAMETER;
            report (-77,s_ptr->spelling,_LINE_);
            }
         a_ptr = a_ptr->next_arg;
         }

   if ((curr_block->function) && (curr_block->num_rets == 0))
      report(112,curr_block->block_name,_LINE_);

   if ((curr_block->function) && (curr_block->main))
      report(120,curr_block->block_name,_LINE_);

   if ((curr_block->main) && (curr_block->num_args > 0))
      report(121,curr_block->block_name,_LINE_);

   while (process_nodes (next) == 0)
         {
         next = get_next_node();
         }

   return; /* An END was read so were done ! */
   
   }

/*************************************************************************/
/*               Analyse and process an IF node                          */
/*************************************************************************/ 

static
void 

   check_if (void)

   {

   If_ptr       ptr;
   Any_ptr  next;

   ptr = curr_ptr;

   next = curr_ptr;  /* save current location in tree */

   set_error_line (ptr);

   ptr->expression = check_expression (ptr->expression);

   curr_ptr = ptr->then_ptr;

   process_nodes (curr_ptr);

   if (ptr->else_ptr != ptr->stmt_ptr)
      {
      curr_ptr = ptr->else_ptr;
      process_nodes (curr_ptr);
      }

   curr_ptr = next;

   return;
 
   }


/*************************************************************************/
/*               Analyse and process a STOP node.                        */
/*************************************************************************/ 

static
void 

   check_stop (void)

   {



   return;

   }


/*************************************************************************/
/*               Analyse and process a CALL node.                        */
/*************************************************************************/ 

static
void 

   check_call (void)

   {

   Call_ptr   ptr;
   Ref_ptr    r_ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   /*******************************************************************/
   /* The argument of a call statement MUST be an ENTRY !             */
   /*******************************************************************/

   r_ptr = ptr->entry; 

   if (check_reference (r_ptr) == 0)
       return;

   if (r_ptr->symbol->structure)
      {
      report (73,r_ptr->symbol->spelling,_LINE_);
      return;
      }

   if (r_ptr->symbol->type != ENTRY)
      {
      report (64,r_ptr->symbol->spelling,_LINE_); 
      goto no_further_tests; /* Dont bother checking nout else */
      }

   /****************************************************************/
   /* See wether the entry is internal or external, then check arg */
   /* counts match.                                                */
   /****************************************************************/

   if ((r_ptr->symbol->proc_ptr) == NULL) /* external */
      {
      if (r_ptr->num_subs > r_ptr->symbol->num_dims)
         report(98,r_ptr->symbol->spelling,_LINE_);
      if (r_ptr->num_subs < r_ptr->symbol->num_dims)
         report(86,r_ptr->symbol->spelling,_LINE_);
      }
   else 
      {
      /************************************************************/
      /* If this is an immediate recursive call, then the block   */
      /* MUST be declared as recursive.                           */
      /************************************************************/

      if (r_ptr->symbol->declarator == r_ptr->symbol->proc_ptr)
         if (r_ptr->symbol->proc_ptr->recursive == 0)
            report(104,r_ptr->spelling,_LINE_);

      if ((r_ptr->num_subs) > (r_ptr->symbol->proc_ptr->num_args))
         report (98,r_ptr->symbol->spelling,_LINE_);
      if ((r_ptr->num_subs) < (r_ptr->symbol->proc_ptr->num_args))
         report (86,r_ptr->symbol->spelling,_LINE_);
      if (r_ptr->symbol->proc_ptr->function)
         report (97,r_ptr->symbol->spelling,_LINE_); /* cant call a function */
/*      else
         r_ptr->symbol->proc_ptr->called = 1; */ /* Block is called ! */
      }

   /********************************************************************/
   /* If the entry takes parameters, then ensure that data-types match */
   /********************************************************************/

   if (r_ptr->num_subs > 0)
      test_arguments (r_ptr);
        
no_further_tests:

   return;
     

   }

/***************************************************************************/
/* This function test that arguments and parameters match in a CALL stmt.  */
/***************************************************************************/

static
void 

   test_arguments (Ref_ptr r_ptr)

   {

   Data_ptr        parm_ptr;
   Sub_ptr         sub_ptr;
   Ref_ptr         arg_ptr;
   Argument_ptr    argt_ptr;
   short              arg_type;
   Symbol_ptr      attribs;
   Any_ptr     proc_node_ptr;
   Procedure_ptr   p_ptr;

   /********************************************************************/
   /* Internal and external procedures have their arg descriptors held */
   /* differently, so the analysis is in two forms.                    */
   /********************************************************************/

   if (r_ptr->symbol->proc_ptr == NULL)
      {
      parm_ptr = r_ptr->symbol->array_ptr;
      sub_ptr  = r_ptr->sublist;

      while (parm_ptr != NULL)
            {
            if (sub_ptr == NULL)
               return;

            arg_type = nodetype(sub_ptr->expression);
   
            if (arg_type != REFERENCE)
               ; /* report(107,"",_LINE_); expressions are passed by val anyway */
            else
               {
               arg_ptr = sub_ptr->expression;
               attribs = arg_ptr->attribs; 
               /***********************************************************/
               /* If the arg isnt a constant, then verify the bloody ting */
               /***********************************************************/
               if ((attribs->class != CONSTANT) && (attribs->class != BUILTIN))
                  {
                  if ((parm_ptr->data_type == CHARACTER) &&
                      (parm_ptr->asterisk))
                     {
                     if ((attribs->varying != parm_ptr->varying) ||
                         (attribs->type    != CHARACTER))
                        /**************************************************/
                        /*                 We have a mismatch !           */
                        /**************************************************/
                        {
                        sub_ptr->pass_by = VALUE;
                        report(-107,arg_ptr->symbol->spelling,_LINE_);
                        goto next_iteration_1;
                        }
                     }
                  else
                     {
                     if ((attribs->type    != parm_ptr->data_type) ||
                         (attribs->prec_1  != parm_ptr->prec_1)    ||
                         (attribs->prec_2  != parm_ptr->prec_2)    ||
                         (attribs->scale   != parm_ptr->scale)     ||
                         (attribs->varying != parm_ptr->varying))
                         /**************************************************/
                         /*                 We have a mismatch !           */
                         /**************************************************/
                         {
                         sub_ptr->pass_by = VALUE;
                         report(-107,arg_ptr->symbol->spelling,_LINE_);
                         goto next_iteration_1;
                         }
                     }
                  }
               else
                  sub_ptr->pass_by = VALUE;
               }

next_iteration_1:

            sub_ptr = sub_ptr->next_ptr;
            parm_ptr = parm_ptr->next_ptr;
            
            }
      }
   else
      /****************************************************************/
      /* For internal procedures, things are held slighly differently */
      /* we effectively compare the arguments attributes, with those  */
      /* of the variable declared as a parameters for the internal    */
      /* procedure.                                                   */
      /****************************************************************/
      {
      proc_node_ptr = r_ptr->symbol->proc_ptr->first_stmt;
      p_ptr         = proc_node_ptr;
      argt_ptr      = p_ptr->argument;
      sub_ptr       = r_ptr->sublist;

      while (argt_ptr != NULL)
            {
            if (sub_ptr == NULL)
               return;

            arg_type = nodetype(sub_ptr->expression);
   
            if (arg_type != REFERENCE)
               ; /* report(107,"",_LINE_); expressions are passed by val anyway */
            else
               {
               arg_ptr = sub_ptr->expression;
               attribs = arg_ptr->attribs; 
               /***********************************************************/
               /* If the arg isnt a constant, then verify the bloody ting */
               /***********************************************************/
               if ((attribs->class != CONSTANT) && (attribs->class != BUILTIN))
                  {
                  if ((argt_ptr->arg_ptr->type == CHARACTER) &&
                      (argt_ptr->arg_ptr->asterisk))
                     {
                     if ((attribs->varying != argt_ptr->arg_ptr->varying) ||
                         (attribs->type    != CHARACTER))
                        /**************************************************/
                        /*                 We have a mismatch !           */
                        /**************************************************/
                        {
                        sub_ptr->pass_by = VALUE;
                        report(-107,arg_ptr->symbol->spelling,_LINE_);
                        goto next_iteration_2;
                        }
                     }
                  else
                     {
                     if ((attribs->type    != argt_ptr->arg_ptr->type) ||
                         (attribs->prec_1  != argt_ptr->arg_ptr->prec_1)    ||
                         (attribs->prec_2  != argt_ptr->arg_ptr->prec_2)    ||
                         (attribs->scale   != argt_ptr->arg_ptr->scale)     ||
                         (attribs->varying != argt_ptr->arg_ptr->varying))
                         /**************************************************/
                         /*                 We have a mismatch !           */
                         /**************************************************/
                         {
                         sub_ptr->pass_by = VALUE;
                         report(-107,arg_ptr->symbol->spelling,_LINE_);
                         goto next_iteration_2;
                         }
                     }
                  }
               else
                  sub_ptr->pass_by = VALUE;
               }

next_iteration_2:

            sub_ptr = sub_ptr->next_ptr;
            argt_ptr = argt_ptr->next_arg;
            
            }
 


      }

   }

/*************************************************************************/
/*               Analyse and process a RETURN node                       */
/*************************************************************************/ 

static
void 

   check_return (void)

   {

   Return_ptr ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   if (curr_block->function)
      if (ptr->value != NULL)
         ptr->value = check_expression (ptr->value);
      else
         report(110,"",_LINE_);
   else
      if (ptr->value != NULL)
         {
         report(111,"",_LINE_); 
         ptr->value = check_expression (ptr->value);
         }

   return;

   }

/*************************************************************************/
/*               Analyse and process a SELECT node                       */
/*************************************************************************/ 

static
void 

   check_select (void)

   {

   Select_ptr  ptr;
   Any_ptr next;

   ptr = curr_ptr;

   next = get_next_node();

   set_error_line (ptr);

   if (ptr->expr_ptr != NULL)
      ptr->expr_ptr = check_expression(ptr->expr_ptr);

   while (process_nodes(next) == 0)
         {
         next = get_next_node();
         }

   /* and END node caused the above loop to end */
   /* so thats it ! */

   return;

   }

/*************************************************************************/
/*               Analyse and process a WHEN node                         */
/*************************************************************************/ 

static
void 

   check_when (void)

   {

   When_ptr    ptr;
   Any_ptr next;
   short          I = 0;

   ptr = curr_ptr;

   next = curr_ptr;

   set_error_line (ptr);

   /******************************************************************/
   /* A 'when' clause in this compiler has up to 16 expression ptr   */
   /* hanging off it (an array of them in fact) so we must test each */
   /******************************************************************/

   while (ptr->expr_ptr[I] != NULL)
         {
         ptr->expr_ptr[I] = check_expression(ptr->expr_ptr[I]);
         I++;
         }

   curr_ptr = ptr->unit_ptr;

   process_nodes (curr_ptr);

   curr_ptr = next;

   return;

   }

/*************************************************************************/
/*               Analyse and process a OTHER node                        */
/*************************************************************************/ 

static
void 

   check_other (void)

   {

   Other_ptr ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   return;

   }

/*************************************************************************/
/*               Analyse and process an ENTRY node                       */
/*************************************************************************/ 

static
void 

   check_entry (void)

   {

   Entry_ptr ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   return;

   }


/****************************************************************************/
/*  Verify that all aspects of this required do-loop are valid.             */
/****************************************************************************/ 

static 
void 

   check_loop (void)

   {

   Loop_ptr    ptr;
   Any_ptr next;

   ptr = curr_ptr;

   set_error_line(ptr);

   /******************************************************************/
   /* Loop counter MUST be a scalar numeric, fixed point variable.   */
   /******************************************************************/

   if (ptr->counter != NULL)  /* HWG42 */
      {
      if (check_reference (ptr->counter))
         {
         if ((ptr->counter->attribs->type != BINARY) &&
             (ptr->counter->attribs->type != DECIMAL))
            report (63,ptr->counter->symbol->spelling,_LINE_);
         else /* Only report SCALE error, if value IS numeric */
            if (ptr->counter->attribs->scale != FIXED)
               report (69,ptr->counter->symbol->spelling,_LINE_);
         }
      }

   /***************************************************************/
   /*          Verify the loop start/finish expressions.          */
   /***************************************************************/

   if (ptr->start != NULL) 
      ptr->start  = check_expression(ptr->start);

   if (ptr->finish != NULL) 
      ptr->finish = check_expression(ptr->finish);

   if (ptr->step != NULL)
      ptr->step   = check_expression(ptr->step);

   if (ptr->repeat != NULL)
      ptr->repeat = check_expression(ptr->repeat);

   if (ptr->while_expr != NULL)
      ptr->while_expr = check_expression(ptr->while_expr);

   if (ptr->until_expr != NULL)
      ptr->until_expr = check_expression(ptr->until_expr);


   if (ptr->stmt_ptr == NULL)
      return; /* Not a real do loop, but a do in a PUT/GET LIST option !! */

   next = get_next_node();

   while (process_nodes(next) == 0)
         {
         next = get_next_node();
         }
 
   /* OK An END was read, so thats the END of THIS LOOP ! */
   
   return;

   }

/*************************************************************************/
/*               Analyse and process a do   node.                        */
/*************************************************************************/ 

static
void 

   check_do (void)

   {

   Any_ptr  next;

   next = get_next_node();

   while (process_nodes(next) == 0)
         {
         next = get_next_node();
         }
   
   /* An end must have been read, so its the end of this loop */

   return;

   }

/*************************************************************************/
/*               Analyse and process a BEGIN node                        */
/*************************************************************************/ 

static
void 

   check_begin (void)

   {

   Any_ptr     next;

   next = get_next_node();

   while (process_nodes(next) == 0)
         {
         next = get_next_node();
         }
   
   /* An end must have been read, so its the end of this loop */

   return;

   }

/*************************************************************************/
/*               Analyse and process an ALLOCATE node                    */
/*************************************************************************/ 

static
void 

   check_allocate (void)

   {

   Allocate_ptr ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   /****************************************************************/
   /* Check that the variable being allocated is declared as BASED */
   /****************************************************************/ 


   if (check_reference (ptr->area))
      if (ptr->area->attribs->class != BASED)
         report (65,ptr->area->spelling,_LINE_);

   /****************************************************************/
   /* Check that variable being SET is of the POINTER data type    */
   /****************************************************************/


   if (check_reference (ptr->target))
      {
      if (ptr->target->attribs->structure)
         {
         report (74,ptr->target->spelling,_LINE_);
         return;
         }

      if (ptr->target->attribs->type != POINTER)
         report (66,ptr->target->spelling,_LINE_);
      }

   return;

   }

/*************************************************************************/
/*               Analyse and process an OPEN node                        */
/*************************************************************************/ 

static
void 

   check_open (void)

   {

   Open_ptr ptr;
   short    count;

   ptr = curr_ptr;

   /****************************************************************/
   /*        Check that the variable being opened is a FILE        */
   /****************************************************************/ 

   while (ptr != NULL)
         {
         set_error_line (ptr);

         if (check_reference(ptr->file_ref))
            if (ptr->file_ref->attribs->type != FILE_TYPE)
               report(165,ptr->file_ref->spelling,_LINE_);

         if (ptr->title_expr != NULL)
            check_expression(ptr->title_expr);

         if (ptr->linesize_expr != NULL)
            check_expression(ptr->linesize_expr);

         if (ptr->pagesize_expr != NULL)
            check_expression(ptr->pagesize_expr);

         /* Check for compatible options */

         if (ptr->stream || ptr->print)
            {
            if (ptr->record)
               report(166,"RECORD",_LINE_);
            if (ptr->direct)
                report(166,"DIRECT",_LINE_);
            if (ptr->sequential)
                report(166,"SEQUENTIAL",_LINE_);
            if (ptr->update)
                report(166,"UPDATE",_LINE_);   
            if (ptr->keyed)
                report(166,"KEYED",_LINE_);
            }

        if (ptr->print)
            if (ptr->input)
                report(167,"",_LINE_);

        if (ptr->direct)
            if (ptr->sequential)
                report(168,"",_LINE_);

        count = ptr->input + ptr->output + ptr->update;

        if (count > 1)
            report(169,"",_LINE_); 
        
        ptr = ptr->next_open;

        }

   return;

   }

/*************************************************************************/
/*               Analyse and process an CLOSE node                       */
/*************************************************************************/ 

static
void 

   check_close (void)

   {

   Close_ptr ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   /****************************************************************/
   /*        Check that the variable being closed is a FILE        */
   /****************************************************************/ 

   if (check_reference(ptr->file_ref))
      if (ptr->file_ref->attribs->type != FILE_TYPE)
         report(165,ptr->file_ref->spelling,_LINE_);

   
   return;

   }

/*************************************************************************/
/*               Analyse and process an READ node                        */
/*************************************************************************/ 

static
void 

   check_read (void)

   {

   Read_ptr ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   /****************************************************************/
   /*        Check that the variable being closed is a FILE        */
   /****************************************************************/ 

   if (check_reference(ptr->file_ref))
      if (ptr->file_ref->attribs->type != FILE_TYPE)
         report(165,ptr->file_ref->spelling,_LINE_);

   check_reference(ptr->into_ref);

   check_expression(ptr->key_expr);

   check_reference(ptr->keyto_ref);

   check_reference(ptr->set_ref);

   check_reference(ptr->sizeto_ref);

   /* check for comptaible options */

   return;

   }

/*************************************************************************/
/*               Analyse and process an WRITE node                       */
/*************************************************************************/ 

static
void 

   check_write (void)

   {

   Write_ptr ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   /****************************************************************/
   /*        Check that the variable being closed is a FILE        */
   /****************************************************************/ 

   if (check_reference(ptr->file_ref))
      if (ptr->file_ref->attribs->type != FILE_TYPE)
         report(165,ptr->file_ref->spelling,_LINE_);

   check_reference(ptr->from_ref);

   check_expression(ptr->keyfrom_expr);

   
   return;

   }

/*************************************************************************/
/*               Analyse and process an DELETE node                      */
/*************************************************************************/ 

static
void 

   check_delete (void)

   {

   Delete_ptr ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   /****************************************************************/
   /*        Check that the variable being closed is a FILE        */
   /****************************************************************/ 

   if (check_reference(ptr->file_ref))
      if (ptr->file_ref->attribs->type != FILE_TYPE)
         report(165,ptr->file_ref->spelling,_LINE_);

   check_expression(ptr->key_expr);
  
   return;

   }

/*************************************************************************/
/*               Analyse and process an REWRITE node                     */
/*************************************************************************/ 

static
void 

   check_rewrite (void)

   {

   Rewrite_ptr ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   /****************************************************************/
   /*        Check that the variable being closed is a FILE        */
   /****************************************************************/ 

   if (check_reference(ptr->file_ref))
      if (ptr->file_ref->attribs->type != FILE_TYPE)
         report(165,ptr->file_ref->spelling,_LINE_);

   check_expression(ptr->key_expr);

   check_reference(ptr->from_ref);

   
   return;

   }

/*************************************************************************/
/*               Analyse and process a GET node                          */
/*************************************************************************/ 

static
void 

   check_get (void)

   {

   Get_ptr       ptr;
   Getfile_ptr   fptr = NULL;
   Getstring_ptr sptr = NULL;

   ptr = curr_ptr;

   set_error_line (ptr);
  
   if (ptr->getf_ptr != NULL)
      fptr = ptr->getf_ptr;

   if (ptr->gets_ptr != NULL)
      sptr = ptr->gets_ptr;

   if (fptr != NULL)
       {
       check_reference(fptr->file_ref);
       fptr->skip_expr = check_expression(fptr->skip_expr);
       check_intarg(fptr->targ_ptr);
       check_format(fptr->fmat_ptr);
       }       
       
   if (sptr != NULL)
       {
       sptr->string_expr = check_expression(sptr->string_expr);
       check_intarg(sptr->targ_ptr);
       check_format(sptr->fmat_ptr);
       }

     
    
   return;

   }

/*************************************************************************/
/*               Analyse and process a PUT node                          */
/*************************************************************************/ 

static
void 

   check_put (void)

   {

   Put_ptr       ptr;
   Putfile_ptr   fptr = NULL;
   Putstring_ptr sptr = NULL;

   ptr = curr_ptr;

   set_error_line (ptr);
  
   if (ptr->putf_ptr != NULL)
      fptr = ptr->putf_ptr;

   if (ptr->puts_ptr != NULL)
      sptr = ptr->puts_ptr;

   if (fptr != NULL)
       {
       check_reference(fptr->file_ref);
       fptr->skip_expr = check_expression(fptr->skip_expr);
       fptr->line_expr = check_expression(fptr->line_expr);
       check_outsrc(fptr->srce_ptr);
       check_format(fptr->fmat_ptr);
       }       
       
   if (sptr != NULL)
       {
       check_reference(sptr->string_ref);
       check_outsrc(sptr->srce_ptr);
       check_format(sptr->fmat_ptr);
       }
    
   return;

   }

/***********************************************************************************************************/
/* This function checks the validity of an input target commalist as found in GET LIST/EDIT statement.     */
/***********************************************************************************************************/

static
void 

check_intarg (Intarg_ptr iptr)

{

Any_ptr           temp;

while (iptr != NULL)
      {
      if (check_reference (iptr->target_ref))
         if (iptr->intarg_ptr != NULL)
            check_intarg (iptr->intarg_ptr);
     
      if (iptr->do_ptr != NULL)
         {
         temp = curr_ptr;
         curr_ptr = iptr->do_ptr;
         check_loop();
         curr_ptr = temp;
         }
      iptr = iptr->next_ptr;
    
      }

}

/***********************************************************************************************************/
/* This function checks the validity of an output source commalist as found in PUT LIST/EDIT statement.    */
/***********************************************************************************************************/

static
void 

check_outsrc (Outsrc_ptr optr)


{

Any_ptr           temp;

while (optr != NULL)
      {
      check_expression (optr->source_expr);

      if (optr->outsrc_ptr != NULL)
         check_outsrc (optr->outsrc_ptr);
     
      if (optr->do_ptr != NULL)
         {
         temp = curr_ptr;
         curr_ptr = optr->do_ptr;
         check_loop();
         curr_ptr = temp;
         }
      optr = optr->next_ptr;
    
      }

}



/***********************************************************************************************************/
/* This function checks the validity of a 'format' commalist as found in PUT/GET EDIT statement.           */
/***********************************************************************************************************/


static
void

check_format (Format_ptr fptr)

{

while (fptr != NULL)
      {
      check_expression(fptr->fmat_itrn_expr);
     
      if (fptr->nest_ptr != NULL)
         check_format(fptr->nest_ptr);
     
      if (fptr->fixed_expr1 != NULL)
         {
         check_expression(fptr->fixed_expr1);
         check_expression(fptr->fixed_expr2);
         }

      if (fptr->float_expr1 != NULL)
         {
         check_expression(fptr->float_expr1);
         check_expression(fptr->float_expr2);
         }
       
      if (fptr->pic_str != NULL);
         {
         ;
         }

      check_expression(fptr->char_expr);

      if (fptr->bit_radix > 0)
         {
         if (fptr->bit_expr)
            {
            check_expression(fptr->bit_expr);
            }
         }

      if (fptr->l_format != 0)
         ;

      if (fptr->tab)
         {
         if (fptr->tab_expr != NULL)
            {
            check_expression(fptr->tab_expr);
            }
         }

      check_expression(fptr->line_expr);
      
      check_expression(fptr->space_expr);

      if (fptr->skip)
         {
         check_expression(fptr->skip_expr);
         }
         
      check_expression(fptr->colm_expr);
            
      check_reference(fptr->rmte_ptr);
   
      fptr = fptr->next_ptr;

      }





}


/*************************************************************************/
/*               Analyse and process a FREE node.                        */
/*************************************************************************/ 

static
void 

   check_free (void)

   {

   Free_ptr     ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   /****************************************************************/
   /* Check that variable being FREED is of the BASED class        */
   /****************************************************************/


   if (check_reference (ptr->target))
      if (ptr->target->attribs->class != BASED)
         report (65,ptr->target->spelling,_LINE_);
         
   return;

   }

/*************************************************************************/
/*               Analyse and process a GOTO node.                        */
/*************************************************************************/ 

static
void 

   check_goto (void)

   {

   Goto_ptr ptr;

   ptr = curr_ptr;

   set_error_line(ptr);
   /*******************************************************************/
   /*       We can only GOTO a label type of identifier.              */
   /*******************************************************************/

   if (check_reference(ptr->target))
      {
      if (ptr->target->symbol->structure)
         {
         report (75,ptr->target->spelling,_LINE_);
         return;
         }

      if (ptr->target->symbol->type != LABEL)
        report (67,ptr->target->spelling,_LINE_); 
      }

   return;

   }

/*************************************************************************/
/*               Analyse and process a SIGNAL node.                      */
/*************************************************************************/ 

static
void 

   check_signal (void)

   {

   Signal_ptr ptr;

   ptr = curr_ptr;

   set_error_line(ptr);

   if (check_reference(ptr->ref_ptr) == 0)
       return;

   /*******************************************************************/
   /* Verify that the programmer has used a reference compatible with */
   /* the PL/I condition type.                                        */
   /*******************************************************************/

   switch (ptr->cond_type) {
   case(USER_CONDITION): 
       if (ptr->ref_ptr->attribs->type != CONDITION)
          report(128,ptr->ref_ptr->spelling,_LINE_);
       break;
   case(IO_CONDITION):
       if (ptr->ref_ptr->attribs->type != FILE_TYPE)
          report(129,ptr->ref_ptr->spelling,_LINE_);
       break;
   default:
       ;
   }       

   return;

   }

/*************************************************************************/
/*               Analyse and process an ON node.                         */
/*************************************************************************/ 

static
void 

   check_on (void)

   {

   On_ptr ptr;

   ptr = curr_ptr;

   set_error_line(ptr);

   if (check_reference(ptr->ref_ptr) == 0)
      return;


   /*******************************************************************/
   /* Verify that the programmer has used a reference compatible with */
   /* the PL/I condition type.                                        */
   /*******************************************************************/

   switch (ptr->cond_type) {
   case(USER_CONDITION): 
       if (ptr->ref_ptr->attribs->type != CONDITION)
          report(126,ptr->ref_ptr->spelling,_LINE_);
       break;
   case(IO_CONDITION):
       if (ptr->ref_ptr->attribs->type != FILE_TYPE)
          report(127,ptr->ref_ptr->spelling,_LINE_);
       break;
   default:
       ;
   }       

   return;

   }

/*************************************************************************/
/*               Analyse and process a LEAVE node.                       */
/*************************************************************************/ 

static
void 

   check_leave (void)

   {

   Leave_ptr  ptr;

   ptr = curr_ptr;

   set_error_line(ptr);

   /*******************************************************************/
   /*       We can only LEAVE a label type of identifier.             */
   /*******************************************************************/

   if (ptr->ref == NULL)
      return;

   if (ptr->ref->structure)
      {
      report (75,ptr->ref->spelling,_LINE_);
      return;
      }

   if (ptr->ref->type != LABEL)
      report (67,ptr->ref->spelling,_LINE_); 

   return;

   }

/*************************************************************************/
/*               Analyse and process an END node.                        */
/*************************************************************************/ 

static
void 

   check_end (void)

   {

  
   /* Dont read next node, caller will (?) */
   
   return;

   }

/*************************************************************************/
/*               Analyse and process a LABEL node                        */
/*************************************************************************/ 

static
void 

   check_label (void)

   {

   
   return;

   }


/*************************************************************************/
/*               Analyse and process an ASSIGNMENT node                  */
/*************************************************************************/ 

static
void 

   check_assignment (void)

   {
   
   Ref_ptr           r_ptr;
   Assignment_ptr    ptr;

   ptr = curr_ptr;

   set_error_line (ptr);

   /**********************************************/
   /* Assignment targets CANNOT be constants !!! */
   /**********************************************/ 
   
   if (check_reference (ptr->target)) // dont bother trying to validate unresolved refs !!! HWG
      {
      if (ptr->target->attribs->class == CONSTANT)              /* HWG37 */
         {
         if (ptr->target->attribs->type == NUMERIC)
            report(148,ptr->target->attribs->spelling,_LINE_);
         else
            if (ptr->target->attribs->type == LABEL)
               report(149,ptr->target->attribs->spelling,_LINE_);
            else
               report(147,ptr->target->attribs->spelling,_LINE_);
         }
      }

   ptr->source = check_expression (ptr->source);

   /********************************************************************/
   /* if by name was specified then both sides must be strucs.         */
   /********************************************************************/

   if (ptr->by_name)
      {
      if (ptr->target->attribs->child == NULL)
         report (139,"",_LINE_); /* LHS is not a struc !! */

      if (nodetype(ptr->source) != REFERENCE)
         report(140,"",_LINE_);
      else
         {
         r_ptr = ptr->source;
         if (r_ptr->attribs->child == NULL)
            report(140,"",_LINE_);
         }
      }
         
   
   return;

   }


/***************************************************************************/
/* This function ensures that a PL/1 reference is correctly constructed.   */
/* The first step is to resolve the reference, that is associate symbols   */
/* with the reference text, the next step is to ensure that arrays are     */
/* correctly subscripted, and pointers etc correctly used.                 */
/* That is we must test certain semantic aspects of a reference that are   */
/* independent of the context in which the reference appears.              */
/* Finally we build an offset expression tree.                             */
/***************************************************************************/ 


chur 

   check_reference (Ref_ptr r_ptr)

   {

   if (r_ptr == NULL)
      return;

   ref_was_resolved = 1;        /* assume it will be resolved */

   resolve_reference (r_ptr);   /* associate real symbols with names */

   if (ref_was_resolved)  /* ie if flag still set on */
      {
      qualify_reference (r_ptr);   /* Fully qualify the reference       */
      validate_reference (r_ptr);  /* check arrays ptrs etc.            */

      /*-------------------------------------------------------------------*/
      /* If the number of subscripts for this reference is the same as the */
	  /* number of dimesnions of the symbol, then we can proceed with the  */
	  /* subscript analysis. A mismatch will already have been reported.   */
	  /*-------------------------------------------------------------------*/ /* HWG-41 */

	  if (r_ptr->num_subs == r_ptr->symbol->num_dims)
	     {
         reorder_subscripts (r_ptr);  /* Puts subscripts in right place    */ 
         build_oe_tree (r_ptr);       /* build OE tree for accessing code  */
         r_ptr->ofx_ptr = simplify_oe_tree (r_ptr->ofx_ptr); /* fold constants */ 
		 }
      }

   r_ptr->resolved = ref_was_resolved;

   return(r_ptr->resolved);

   }

/***************************************************************************/
/* This function accepts a reference tree, and resolves all identifers to  */
/* their declarations, the function obeys the language rules for scoping   */
/* and structure qualification. The function will set each ref-node's      */
/* symbol ptr to point to the correct symbol table entry.                  */
/***************************************************************************/

static
void 

   resolve_reference (Ref_ptr r_ptr)

   {

   Symbol_ptr         s_ptr = NULL;
   Block_ptr          b_ptr = NULL;

   if (r_ptr == NULL)
      return;

   if (r_ptr->dot_ptr == NULL) /* Not a struc type ref */
      {
      r_ptr->symbol = find_declaration (curr_block,r_ptr->spelling);
      if (r_ptr->symbol == NULL)
         {
         b_ptr = find_inner_block (curr_block,r_ptr->spelling);
         if (b_ptr != NULL) /* We found an inner block ! */
            {
            /**********************************************************/
            /* There is an internal procedure with this name, so we   */
            /* now allocate a symbol node and set its proc_ptr to     */
            /* point to this inner block.                             */
            /* A non-null proc_ptr therefore signifies that this      */
            /* symbol is an internal entry.                           */
            /**********************************************************/ 
            s_ptr = add_symbol (curr_block,
                                r_ptr->spelling,
                                PL1NAME,
                                0,
                                8,
                                0,
                                STATIC,
                                ENTRY,
                                INTERNAL,
                                1,
                                0,
                                NULL,
                                line_no);
            s_ptr->declared   = 1; /* Inner proc exists, ie it is declared */
            s_ptr->type       = ENTRY;
            s_ptr->scope      = INTERNAL;
            s_ptr->proc_ptr   = b_ptr; /* ptr to internal proc node */
            s_ptr->known_size = 1;
            s_ptr->known_locn = 1;
            s_ptr->offset     = s_ptr->declarator->stattic;
            s_ptr->declarator->stattic    += 8; 
            /***********************************************************/
            /* Set the line number 'declared on' to that of the proc.  */
            /***********************************************************/
            strcpy(s_ptr->line,b_ptr->line);
            r_ptr->symbol = s_ptr;
            }
         else
            {
            if (builtin(r_ptr->spelling))
               {
               r_ptr->symbol = add_symbol (curr_block,
                                           r_ptr->spelling,
                                           PL1NAME,
                                           0,
                                           0,
                                           0,
                                           BUILTIN,
                                           0,
                                           INTERNAL,
                                           1,
                                           0,
                                           NULL,
                                           line_no);
 
               }
            else
               {
               /***********************************************************/
               /* This undeclared name must now be declared as bin(15).   */
               /***********************************************************/ 
               r_ptr->symbol = add_symbol (curr_block,
                                           r_ptr->spelling,
                                           PL1NAME,
                                           0,
                                           2,
                                           FIXED,
                                           AUTOMATIC,
                                           BINARY,
                                           INTERNAL,
                                           1,
                                           0,
                                           NULL,
                                           line_no);
               r_ptr->symbol->computational = 1; 
               r_ptr->symbol->prec_1        = 15;
               r_ptr->symbol->prec_2        = 0; 
               report (-33,r_ptr->spelling,_LINE_);
               }
            }
         }

      /*******************************************************************/
      /* The attribs pointer in a ref node will always point to a symbol */
      /* node after the ref has been resolved.                           */
      /* If the ref is not qualified, then then the symbol is simply the */
      /* symbol that the ref was resolved to. If it IS qualified then    */
      /* the symbol is that of the final component in the qualified ref. */
      /*******************************************************************/ 
      r_ptr->attribs = r_ptr->symbol;
      r_ptr->symbol->referenced = 1;
      }
   else
      resolve_qualification (curr_block,r_ptr);

   /*********************************************************************/
   /* OK We have resolved all symbols that appear in a ref or qualified */
   /* ref, but not in a ptr ref, ie p->a                                */
   /*********************************************************************/       

   resolve_reference (r_ptr->ptr_ptr);
   
   }

/**************************************************************************/
/* This function searches the symbol tree upwards looking for the named   */
/* variable that is DECLARED. This corresponds to the PL/1 scoping rules. */
/* This function walks up the tree in a recursive manner, simple really ! */
/**************************************************************************/

static
Symbol_ptr 

   find_declaration (Block_ptr current_block,
                     char *    symbol_name)

   {

   Symbol_ptr   temp_ptr = NULL;
  
   /****************************************************************/
   /* Check current_block pointer first, if NULL get out of here ! */
   /****************************************************************/

   if (current_block == NULL)
      return(NULL);

   /*****************************************************************/
   /*        Scan the symbol list for this PL/1 block first.        */
   /*****************************************************************/

   temp_ptr = current_block -> first_symbol;

   while (temp_ptr != NULL)
         {
         if (strcmp(symbol_name,(temp_ptr->spelling))==0)
            if (temp_ptr->declared) 
               return (temp_ptr);
         temp_ptr = temp_ptr -> next_ptr;
         }

   /******************************************************************/
   /* Right we have just searched all SIMPLE names in this block but */
   /* we did not find a declaration. We now search for the name in   */
   /* the subcomponents of any structures in this block.             */
   /******************************************************************/ 

   temp_ptr = find_inner_dcl (current_block,symbol_name);

   if (temp_ptr != NULL)
      return(temp_ptr);


   /*****************************************************************/
   /* OK It wasnt in this blocks list, so scan our parent block !   */
   /*****************************************************************/

   return (find_declaration(current_block->parent,
                            symbol_name));

   }

    
/**************************************************************************/
/* Invoke the recursive function, 'search_structure' to walk thru a struc */
/**************************************************************************/

static    
Symbol_ptr 

   find_inner_dcl (Block_ptr b_ptr, char * name)

   {

   Symbol_ptr   st_ptr = NULL;
   Symbol_ptr   sr_ptr = NULL;
   short           count = 0;
   Symbol_ptr   result;

   st_ptr = b_ptr->first_symbol;

   while (st_ptr != NULL)
         {   
         if (st_ptr->structure)
            {
            if (strcmp(st_ptr->spelling,name) == 0)   
               {
               count++;
               result = st_ptr;
               }

            sr_ptr = search_structure (st_ptr,name);

            if (sr_ptr != NULL)
               {
               count++;
               result = sr_ptr;
               }
            }
         st_ptr = st_ptr->next_ptr;
         }

   /*******************************************************************/
   /*               Was the name ambiguous or not ?                   */
   /*******************************************************************/

   if (count == 1)
      return(result);

   if (count > 0)
      {
      report (87,name,_LINE_);
      ref_was_resolved = 0;
      return(result);   /* ? */
      }

   return(NULL);

   }
   
/***************************************************************************/
/* Search all (any !) members of the structure recursively, stopping when  */
/* we find the same spelled identifer.                                     */
/***************************************************************************/

static 
Symbol_ptr 

   search_structure (Symbol_ptr s_ptr,char * name)

   /* s_ptr; Parent struc ptr */

   {

   Symbol_ptr   temp_sym = NULL;



   if (s_ptr == NULL)
      return(NULL);

   /*****************************************************************/
   /* Process all names (structure or simple) that are children  of */
   /* symbol passed in to this call.                                */
   /*****************************************************************/

   s_ptr = s_ptr->child;

   while (s_ptr != NULL)
         {
         if (strcmp(s_ptr->spelling,name) == 0)
            return (s_ptr); /* this symbol is the one intended */

         /***************************************************/
         /* OK We must now search all sub-names of this one */
         /***************************************************/
    
         temp_sym = search_structure (s_ptr,name);

         if (temp_sym != NULL)
            return(temp_sym);

         s_ptr = s_ptr->sister;
         }

   return (NULL);

   }

/************************************************************************/
/* This function searches a blocks symbol table for the existence of an */
/* internal procedure called 'name'.                                    */
/* It first looks for an internal procedure called 'name' if one isnt   */
/* found then the name is tested for a match against the current block  */
/* if it matches, then the ref is to the parent block itself.           */
/************************************************************************/        


Block_ptr 

   find_inner_block (Block_ptr b_ptr,
                     char * name)

   {

   Block_ptr  child_ptr = NULL;

   if (b_ptr == NULL)
      return(NULL);

   /***********************************************************/
   /* Search all immediately contained internal procedures.   */
   /***********************************************************/ 

   child_ptr = b_ptr->child;

   while (child_ptr != NULL)
         {
         if (strcmp(name,child_ptr->block_name) == 0)
            return (child_ptr);
         child_ptr = child_ptr->sister;
         }

   /*************************************************************/
   /* Block 'b_ptr' has no internal proc called 'name', but is  */
   /* 'b_ptr' itself called 'name' ?                            */
   /*************************************************************/ 

   if (strcmp(name,b_ptr->block_name) == 0)
       return (b_ptr);

   return (NULL);

   }      

/***************************************************************************/
/* Validate a structure qualified reference, for ambiguity, and existence  */
/***************************************************************************/

static     
void 

   resolve_qualification (Block_ptr b_ptr,Ref_ptr r_ptr)

   {

   Symbol_ptr    s_ptr = NULL;   
   char          name[128] = "";

   if (b_ptr == NULL)
      return;

   matches = 0;

   s_ptr = b_ptr->first_symbol;

   while (s_ptr  != NULL)
         {
         if (s_ptr->structure)
            {
            if (strcmp(s_ptr->spelling,r_ptr->spelling) == 0)
               {
               r_ptr->symbol = s_ptr;
               resolve_struc_ref (name,s_ptr->child,r_ptr->dot_ptr);
               }
            else
               resolve_struc_ref (name,s_ptr->child,r_ptr);
            }
         /**********************************************/
         /* OK Get the next symbol in this PL/1 block  */
         /**********************************************/


         s_ptr = s_ptr->next_ptr;
         }

   /****************************************************************/
   /* If it couldnt be resolved to a struc in this block, try next */
   /* outer block, ie our parent.                                  */
   /****************************************************************/

   if (matches == 0)
      resolve_qualification (b_ptr->parent,r_ptr);

   /**************************************************************/
   /* If we had NO matches then, the qualified reference is crap */
   /* If we had more than one, then its ambiguous !              */
   /**************************************************************/ 

   if (matches == 0)      /* no known strucs can satisfy reference */
      {
      ref_was_resolved = 0;
      if (strlen(name) > 0)
         report (79,name,_LINE_);
      else
         report (132,r_ptr->spelling,_LINE_);
         
      }
   else
   if (matches > 1)
      {
      ref_was_resolved = 0;
      report (87,name,_LINE_);   /* ambiguous struc ref */
      }
   else
      {
      /*************************************************/
      /* the type and scale of this ref node, must be  */
      /* copied from its partner.                      */
      /*************************************************/
      r_ptr->attribs = r_ptr->dot_ptr->attribs;
      r_ptr->symbol->referenced = 1;
      }
   }

/***************************************************************************/
/* Recursively, walk-down the reference tree trying to match the qualified */
/* reference with the structure parent passed in (s_ptr).                  */
/***************************************************************************/

static  
void 

   resolve_struc_ref (char * name,Symbol_ptr s_ptr,Ref_ptr r_ptr)
                               
   /* name;  used only for error reporting */

   {

   if (r_ptr == NULL)
      {
      matches++;
      return;
      }

   if (r_ptr->dot_ptr == NULL) /* ie last component of the name */
      {
      name[0] = '\x00';
      strcat (name,r_ptr->spelling);
      } 

   while (s_ptr != NULL)
         {
         if (strcmp(s_ptr->spelling,r_ptr->spelling) == 0)
            {
            /********************************************************/
            /* set the attributes of this ref, from the simple name */
            /********************************************************/
            r_ptr->attribs = s_ptr;
            r_ptr->symbol  = s_ptr;
            resolve_struc_ref (name,s_ptr->child,r_ptr->dot_ptr);
            }
         else
            resolve_struc_ref (name,s_ptr->child,r_ptr);
         
         /**********************************************/
         /* OK Get the next symbol in this PL/1 block  */
         /**********************************************/

         s_ptr = s_ptr->sister;
         }

   /*****************************************************************/
   /* Before we return, we check to see if 'matches' is set to one  */
   /* if it is, then we know that the ref-node following this one   */
   /* MUST have had its attributes set, so we set ours now !        */
   /*****************************************************************/

   if ((matches == 1) && (r_ptr->dot_ptr != NULL))
      {
      r_ptr->attribs = r_ptr->dot_ptr->attribs;
      }
   }

/***************************************************************************/
/* This function accepts a ref tree, and appends new ref nodes to it, if   */
/* the ref tree does NOT represent a full qualification of the reference.  */
/* This is required before offset expressions can be determined for a ref. */
/* Note that if qualification is missing from the 'middle' rather than the */
/* 'front' then this algorith fails. This must be fixed at some point.     */
/***************************************************************************/

static
void 

   qualify_reference (Ref_ptr r_ptr)

   {

   Symbol_ptr         s_ptr = NULL;
   Ref_ptr            n_ptr = NULL; /* new ref node ptr */

   if (r_ptr->symbol == NULL)
      return;

   s_ptr = r_ptr->symbol->parent;

   while (s_ptr != NULL)
         {  
         n_ptr = allocate_node (REFERENCE);
         /*************************************************************/
         /* Copy the original ref node to the new one, cos were gonna */
         /* change the symbol pointed to by the new one.              */
         /*************************************************************/
         n_ptr->type      = r_ptr->type;
         n_ptr->data_type = r_ptr->data_type;
         n_ptr->scale     = r_ptr->scale;
         n_ptr->spelling  = r_ptr->spelling;
         n_ptr->symbol    = r_ptr->symbol;
         n_ptr->attribs   = r_ptr->attribs;
         n_ptr->dot_ptr   = r_ptr->dot_ptr;
         n_ptr->num_subs  = r_ptr->num_subs;
         n_ptr->sublist   = r_ptr->sublist;  /* HWG35 */
         /************************************************************/
         /*       Now link the new node to the TAIL of the chain.    */
         /************************************************************/
         r_ptr->num_subs = 0;      /* Dont leave this set ! */ 
         r_ptr->sublist  = NULL;   /* HWG35                 */
         r_ptr->dot_ptr  = n_ptr;
         r_ptr->symbol   = s_ptr;
         r_ptr->spelling = s_ptr->spelling;
         s_ptr           = s_ptr->parent;
         }
   }

/***************************************************************************/
/* This function accepts a ptr to a Ref node, and verifes all semantic     */
/* aspects of the reference.                                               */
/* The attributes of the reference are also determined, by propagating     */
/* the attributes of the last referenced symbol, up the reference chain.   */
/***************************************************************************/

static
void 

   validate_reference (Ref_ptr r_ptr)

   {

   Symbol_ptr       s_ptr    = NULL;
/*   Sub_ptr          sb_ptr   = NULL;   */
/*   Ref_ptr          dt_ptr   = NULL;   */
/*   Ref_ptr          ptr_ptr  = NULL;   */
   short               sub_cnt;
   short               dim_cnt; 

   if (r_ptr == NULL)
      return;

   s_ptr = r_ptr->symbol;

   /*******************************************************/
   /* If for any reason the ref was not resolved then the */
   /* symbol pointer will be NULL, so we dont attempt to  */
   /* validate the bloody thing !                         */ 
   /*******************************************************/

   if (s_ptr == NULL)
      return;

   /*************************************************************/
   /* If this is the first part of a struc ref, then verify the */
   /* subscript count equals the dimension count.               */
   /* However if the ref has NO subscripts, then it is legal    */
   /* irrespective of the number of reqd dimensions.            */
   /* ie it may be a reference to the entire array.             */
   /*************************************************************/

   if ((r_ptr->symbol->parent == NULL) && (r_ptr->dot_ptr != NULL))
      {
      sub_cnt = subscript_count(r_ptr);
      if (sub_cnt != 0)
         {
         dim_cnt = dimension_count(r_ptr);
         if (sub_cnt != dim_cnt)
            report(29,r_ptr->spelling,_LINE_);
         }
      }

   /*****************************************************/
   /* Builtin functions are not fully implemented yet.  */
   /*****************************************************/

   if (s_ptr->proc_ptr != NULL)
      {
      if (s_ptr->proc_ptr->function)
         {
         if (s_ptr->proc_ptr->num_args < r_ptr->num_subs)
            report(99,s_ptr->spelling,_LINE_); /* too many args */
         if (s_ptr->proc_ptr->num_args > r_ptr->num_subs)
            report(100,s_ptr->spelling,_LINE_); /* too few args */
         }
      }
   else
   if (s_ptr->class != BUILTIN)
      {
      if (qualified(r_ptr) == 0)
         {
         if ((s_ptr->num_dims > 0) && (s_ptr->type != ENTRY))
            {
            if (s_ptr->num_dims > r_ptr->num_subs)
               report (85,s_ptr->spelling,_LINE_); /* subscript mismatch */
            if (s_ptr->num_dims < r_ptr->num_subs)
               report (103,s_ptr->spelling,_LINE_); /* subscript mismatch */
            }
         if ((s_ptr->structure == 0) && (r_ptr->dot_ptr != NULL))
            report (83,s_ptr->spelling,_LINE_); /* Non struc sym, used in struc ref */
   
         if ((s_ptr->array_ptr == NULL) && (r_ptr->sublist != NULL))
            report (84,s_ptr->spelling,_LINE_); /* Non Array is subscripted */
   
         if ((s_ptr->type != POINTER) && (r_ptr->ptr_ptr != NULL))
            report (66,s_ptr->spelling,_LINE_); /* Non Ptr used in -> ref */
         }
      else
         {
         /*****************************************************/
         /* If the ref IS part of a qualified name, then we   */
         /* must ensure that no single component has too many */
         /* subscripts, too few are acceptable though.        */
         /*****************************************************/
         if (s_ptr->type != ENTRY)
            {
            if (r_ptr->num_subs > num_inherited_dims(s_ptr))
               report(103,s_ptr->spelling,_LINE_);
            }
         }
      }

   /* sub_cnt is returned by check_sublist */ 

   check_sublist (r_ptr->sublist);

   validate_reference (r_ptr->dot_ptr);

   validate_reference (r_ptr->ptr_ptr);


   }

/**************************************************************************/
/*        Validate the expressions used in each subscript reference.      */
/**************************************************************************/

static
short  

   check_sublist (Sub_ptr sb_ptr)

   {

   short             count;

   count = 0;

   while (sb_ptr != NULL)
         {
         count++;
         sb_ptr->expression = check_expression (sb_ptr->expression);
         sb_ptr = sb_ptr->next_ptr;
         }

   return (count);

   
   } 

/**************************************************************************/
/* This function counts the total number of subscripts appearing in a ref */
/**************************************************************************/

static
short  

   subscript_count (Ref_ptr r_ptr)

   {

   short               total_subs;

   total_subs = 0;

   while (r_ptr != NULL)
         {
         total_subs += r_ptr->num_subs;
         r_ptr = r_ptr->dot_ptr;
         }

   return (total_subs);

   }   

/**************************************************************************/
/* This function counts the total number of dimensions reqd by a ref      */
/**************************************************************************/

static
short  

   dimension_count (Ref_ptr r_ptr)

   {

   short               total_dims;

   total_dims = 0;

   while (r_ptr != NULL)
         {
         total_dims += r_ptr->symbol->num_dims;
         r_ptr = r_ptr->dot_ptr;
         }

   return (total_dims);

   }   


/**************************************************************************/
/* Count the number of array dimensions from the level 1 name to the      */
/* symbol passed in here.                                                 */
/**************************************************************************/

static
short  

   num_inherited_dims (Symbol_ptr s_ptr)

   {

   
   short         dim_count;


   if (s_ptr == NULL)
      return(0);

   dim_count = s_ptr->num_dims;

   s_ptr = s_ptr->parent;

   while (s_ptr != NULL)
         {
         dim_count += s_ptr->num_dims;
         s_ptr = s_ptr->parent;
         }  

   return(dim_count);

   } 

/**************************************************************************/
/* This function will move any subscripts, to the logically correct place */
/* in a reference, ie it will convert the ref: struc.inner.leaf(I,J,K)    */
/* into: struc(I).inner(J).leaf(K).                                       */
/* This greatly simplifies subsequent processing of offset expressions.   */
/**************************************************************************/

static
void

   reorder_subscripts (Ref_ptr r_ptr)

   {

   Ref_ptr           t_ptr   = NULL;
   short                n_dims;
   short                n_subs;
   Sub_ptr           sb_ptr  = NULL;

   /********************************************************************/
   /* If this is NOT a struc ref (ie no dot ref) then exit immediately */
   /********************************************************************/

   if (r_ptr->dot_ptr == NULL)
      return;

   t_ptr = r_ptr;

   while (t_ptr != NULL)
         {
         n_dims = t_ptr->symbol->num_dims;
         n_subs = t_ptr->num_subs;

         /******************************************************************/
         /* If this ref node has less subscripts than its dimensionality   */
         /* requires, then 'pluck' a 'Sub' node from further down the list */
         /* and insert it, as many times as needed.                        */
         /******************************************************************/

         while (n_subs < n_dims)
               {
               sb_ptr = remove_subscript (t_ptr->dot_ptr); 

               /***************************************************/
               /* If this ref has less subscripts than dimensions */
               /* then dont attempt to append any more.     HWG39 */
               /***************************************************/ 

               if (sb_ptr == NULL)
                  break;

               append_subscript (t_ptr,sb_ptr);
               n_subs++;
               }

         t_ptr = t_ptr->dot_ptr;

         }
   }

/****************************************************************************/
/* This function will search along a Ref chain, until it finds a Sub node.  */
/* It will then unlink it from its position, and returns it ptr.            */
/****************************************************************************/

static
Sub_ptr

   remove_subscript (Ref_ptr r_ptr)

   {

   Sub_ptr           sb_ptr = NULL;

   while (r_ptr != NULL)
         {
         if (r_ptr->num_subs > 0)
            {
            sb_ptr = r_ptr->sublist;
            r_ptr->sublist = sb_ptr->next_ptr;
            if (r_ptr->sublist != NULL)
               r_ptr->sublist->prev_ptr = NULL;
            r_ptr->num_subs--;
            return(sb_ptr);
            }

         r_ptr = r_ptr->dot_ptr;

         }      

   /* The error below has been supressed, cos          */   
   /* dcl a defined(x.y) where x (or y) is dimensioned */
   /* gave this error, but is valid PL/I               */

   /* report(131,r_ptr->spelling,_LINE_); */ /* Too few subs in ref */

   return(NULL);

   }

/*************************************************************************/
/* This function will append a subscript node to the sublist of the Ref  */
/* specified.                                                            */
/*************************************************************************/


void

   append_subscript (Ref_ptr r_ptr,Sub_ptr sb_ptr)

   {
   
   Sub_ptr           ts_ptr = NULL; /* temp sub ptr */

   if (r_ptr->sublist == NULL)
      {
      r_ptr->sublist = sb_ptr;
      sb_ptr->next_ptr = NULL;
      sb_ptr->prev_ptr = NULL;
      r_ptr->num_subs++;
      return;
      }

   ts_ptr = r_ptr->sublist;

   while (ts_ptr->next_ptr != NULL)
         ts_ptr = ts_ptr->next_ptr;

   /*********************************************************************/
   /* OK We (ts_ptr) are now pointing at the last Sub node in the list. */
   /*********************************************************************/

   ts_ptr->next_ptr = sb_ptr;
   sb_ptr->prev_ptr = ts_ptr;
   sb_ptr->next_ptr = NULL;

   r_ptr->num_subs++;

   }

/*************************************************************************/
/* This function accepts a reference tree, and constructs an offset      */
/* expression tree for use by the code generator, to generate accessing  */
/* code.                                                                 */
/*************************************************************************/

static
void 

   build_oe_tree (Ref_ptr r_ptr)

   {

   Ref_ptr         root_ptr = NULL;
/* Any_ptr         ec_ptr   = NULL; */ /* component expression */  
   Oper_ptr        ep_ptr   = NULL; /* + operator node      */
   Any_ptr         er_ptr   = NULL; /* resultant expression */
   Any_ptr         et_ptr   = NULL; /* temp expression ptr  */
   Ref_ptr         tail_ptr = NULL;
   Oper_ptr        sum_ptr  = NULL;
   Ref_ptr         ofx_ptr  = NULL;
   char            char_ofx[16]= "";

   /*************************************************************/
   /* Process the reference a component at a time, appending    */
   /* non-null component expressions with + operators as we go. */
   /* Components are those parts of the ref that are seperated  */
   /* by dots.                                                  */ 
   /*************************************************************/

   if (r_ptr == NULL)  /* bad news, weve been called in error ! */
      return;

   if (r_ptr->symbol->type == ENTRY)
      return;
    
   root_ptr = r_ptr;

   er_ptr = build_component_oe (r_ptr); /* Just this component */  

   r_ptr = r_ptr->dot_ptr;

   while (r_ptr != NULL)  /* while there are more components */
         {
         et_ptr           = build_component_oe(r_ptr);

         /* If we get two expr trees then combine them with a + */

         if ((er_ptr != NULL) && (et_ptr != NULL))
            {
            ep_ptr           = allocate_node (PLUS);
            ep_ptr->left_ptr = er_ptr;
            ep_ptr->rite_ptr = et_ptr;
            er_ptr           = ep_ptr;
            }

         /* if only et_ptr has a tree, then put it into er_ptr */

         if ((et_ptr != NULL) && (er_ptr == NULL))
            er_ptr = et_ptr;

         r_ptr            = r_ptr->dot_ptr;
         } 

   /*******************************************************************/
   /* The last stage of this OE tree building is to add on the offset */
   /* of the referenced member.                                       */
   /* Its offset within parent + its parents offset within parent...  */
   /* all the way to the level 1 name, is what we want here.          */
   /* But this sum is the same as the fields offset from the level 1  */
   /* name, which is already held.                                    */
   /*******************************************************************/

   tail_ptr = get_reference_tail (root_ptr);

   if ((er_ptr != NULL) && (tail_ptr->symbol->parent != NULL))
      {
      sum_ptr = allocate_node (PLUS);
      ofx_ptr = allocate_node (REFERENCE);
      sum_ptr->left_ptr = er_ptr;
      er_ptr = sum_ptr;
      itoa((int)tail_ptr->symbol->offset,char_ofx,(int)10);
      ofx_ptr->symbol = create_constant(tail_ptr->symbol->declarator,
                                        char_ofx,
                                        NUMERIC);
      ofx_ptr->attribs  = ofx_ptr->symbol;
      ofx_ptr->spelling = ofx_ptr->symbol->spelling;
      sum_ptr->rite_ptr = ofx_ptr;
      }

   root_ptr->ofx_ptr = er_ptr;

   }

/*************************************************************************/
/* This function constructs an expression tree for a single component of */
/* a qualified reference.                                                */
/*************************************************************************/

static   
Any_ptr 

   build_component_oe (Ref_ptr r_ptr)

   {

   Oper_ptr     prod_ptr = NULL;
   char         char_size[32] = "";
   Ref_ptr      temp_ptr = NULL;

   Any_ptr  vi_ptr; /* vector index computation tree */

   vi_ptr = build_vector_index (r_ptr);

   /*******************************************************************/
   /* We must now create a multiply node, to reflect the fact that    */
   /* the vector index is a multiple of the array element size.       */
   /*******************************************************************/

   if (vi_ptr != NULL)
      {
      prod_ptr = allocate_node (TIMES);
      temp_ptr = allocate_node (REFERENCE);
      prod_ptr->left_ptr = vi_ptr;
      vi_ptr = prod_ptr;
      itoa(r_ptr->symbol->size,char_size,10);
      temp_ptr->symbol = create_constant(r_ptr->symbol->declarator,
                                         char_size,
                                         NUMERIC);
      temp_ptr->attribs  = temp_ptr->symbol;
      temp_ptr->spelling = temp_ptr->symbol->spelling;
      prod_ptr->rite_ptr = temp_ptr;
      }
      

   return (vi_ptr);

   }

/***************************************************************************/
/* This function builds a computation tree that represents the calculation */
/* required to determine the offset of a specified array reference.        */
/* We effectively translate an 'n' dimensional array reference to a vector */
/* access in an imaginary vector, that has the same number of elements as  */
/* the array being processed.                                              */
/* Note: This computation tree is translated to code, that is executed at  */
/* runtime by the generated code.                                          */
/***************************************************************************/

static
Any_ptr 

   build_vector_index (Ref_ptr r_ptr)

   {

   Sub_ptr             sb_ptr      = NULL;
   short               subs_count;
   Any_ptr             term1_ptr   = NULL;
   Any_ptr             dims_ptr    = NULL;
   Oper_ptr            mult_ptr    = NULL;
   Any_ptr             running_ptr = NULL;
   Oper_ptr            add_ptr     = NULL;
   Any_ptr             vec_ptr     = NULL;

   if (r_ptr->num_subs == 0) /* this ref is unsubscripted ! */
      {
      r_ptr->ofx_ptr = NULL;
      return(NULL);
      }

   sb_ptr = r_ptr->sublist;
   subs_count = (r_ptr->num_subs - 1);

   while (sb_ptr != NULL)
         {
         term1_ptr = subtract_one (r_ptr->symbol,sb_ptr->expression); /* ie exp-1 */
         dims_ptr  = multiply_dims (r_ptr->symbol,subs_count);

         if (term1_ptr != NULL)
            running_ptr = term1_ptr;

         if (dims_ptr != NULL)
            running_ptr = dims_ptr;

         if ((term1_ptr != NULL) && (dims_ptr != NULL))
            {
            mult_ptr = allocate_node(TIMES);
            mult_ptr->left_ptr = term1_ptr;
            mult_ptr->rite_ptr = dims_ptr;
            running_ptr = mult_ptr;
            }
         /****************************************************************/
         /* OK At this point 'running_ptr' will point to the computation */
         /* tree for the current subscript. We must now add to main tree */
         /****************************************************************/
         if (vec_ptr == NULL)
            vec_ptr = running_ptr;
         else
            {
            add_ptr = allocate_node(PLUS);
            add_ptr->left_ptr = vec_ptr;
            add_ptr->rite_ptr = running_ptr;
            vec_ptr = add_ptr;
            } 
         sb_ptr = sb_ptr->next_ptr;
         subs_count -= 1;
         }

   return(vec_ptr);

   }

/*************************************************************************/
/* This function simply takes an expression tree, and returns a pointer  */
/* to a new tree that represents (expr - 1).                             */
/*************************************************************************/

static
Any_ptr 

   subtract_one (Symbol_ptr s_ptr,Any_ptr exp_ptr)

   {

   Oper_ptr           minus_ptr = NULL;
   Symbol_ptr         one_ptr   = NULL;
   Ref_ptr            r_ptr     = NULL;

   r_ptr     = allocate_node(REFERENCE);
   minus_ptr = allocate_node(MINUS);
   one_ptr   = create_constant(s_ptr->declarator,"1",NUMERIC);

   r_ptr->symbol   = one_ptr;
   r_ptr->attribs  = one_ptr;
   r_ptr->spelling = one_ptr->spelling;
   minus_ptr->left_ptr = exp_ptr;
   minus_ptr->rite_ptr = r_ptr;
   
   return(minus_ptr);

   }

/*************************************************************************/
/* This function builds a computation tree, that consists of a sequence  */
/* of multiplications of the last 'dim_count' dimensions of the symbol.  */
/*************************************************************************/

static
Any_ptr 

   multiply_dims (Symbol_ptr s_ptr,short dim_count)

   {

   Any_ptr         root_ptr  = NULL;   /* the final expression tree root */
   Dim_ptr         bound_ptr = NULL;   /* Dim node                       */
   Oper_ptr        mul_ptr   = NULL;   /* * operator node                */
   Any_ptr         dim_ptr   = NULL;
   short              number_of_dimensions;
   short              skip_count;
   short              I;

   if (s_ptr == NULL)  /* bad news, weve been called in error ! */
      return(NULL);
    
   if (dim_count == 0)
      return(NULL);

   /***********************************************************/
   /* The bound_ptr points to a Dim node that describes the   */
   /* upper & lower bounds of this dimension. We must use the */
   /* function 'build_dim_tree' to derive an expression that  */
   /* represents the actual size of the dimension. This is    */
   /* ((upper - lower) + 1).                                  */
   /***********************************************************/

   number_of_dimensions = s_ptr->num_dims;

   skip_count = number_of_dimensions - dim_count;

   bound_ptr = s_ptr->array_ptr;

   /*****************************************************/
   /* Skip past the first 'skip_count' dimension nodes. */
   /*****************************************************/

   for (I=0; I<skip_count; I++)
       bound_ptr = bound_ptr->next_ptr; 

   /*****************************************************/
   /* OK Process all remaining dim nodes on this symbol */
   /*****************************************************/

   dim_ptr   = build_dim_tree(s_ptr,bound_ptr); /* expr representing dim */
   
   root_ptr  = dim_ptr;

   if (dim_count == 1)
      return(root_ptr);

   dim_count--;

   bound_ptr = bound_ptr->next_ptr;
   dim_ptr = build_dim_tree(s_ptr,bound_ptr);

   while (dim_count > 0)  /* while there are still dims to process */
         {
         mul_ptr           = allocate_node (TIMES);
         mul_ptr->left_ptr = root_ptr;
         mul_ptr->rite_ptr = build_dim_tree(s_ptr,bound_ptr);
         root_ptr          = mul_ptr;
         bound_ptr         = bound_ptr->next_ptr;
         dim_ptr           = build_dim_tree(s_ptr,bound_ptr);
         dim_count--;
         }   

   return(root_ptr);

   }

/*************************************************************************/
/* This function returns a ptr to an expression tree that represents the */
/* the total span of an array dimension.                                 */
/*************************************************************************/

static
Any_ptr 

   build_dim_tree (Symbol_ptr array_ptr,Dim_ptr bound_ptr)

   { 

   Oper_ptr        minus_ptr = NULL;
   Oper_ptr        plus_ptr  = NULL;
   Symbol_ptr      one_ptr   = NULL;
   Ref_ptr         r_ptr     = NULL;

   if (bound_ptr->lower == NULL)
      return(bound_ptr->upper);

   /*********************************************************/
   /* A lower bound has been specified, so we must set up   */
   /* the expression ((upper - lower) + 1)                  */
   /*********************************************************/

   minus_ptr = allocate_node(MINUS);
   plus_ptr  = allocate_node(PLUS);
   r_ptr     = allocate_node(REFERENCE);
   one_ptr   = create_constant(array_ptr->declarator,"1",NUMERIC);

   r_ptr->symbol       = one_ptr;
   r_ptr->attribs      = one_ptr;
   r_ptr->spelling     = one_ptr->spelling;
   minus_ptr->left_ptr = bound_ptr->upper;
   minus_ptr->rite_ptr = bound_ptr->lower;
   plus_ptr->left_ptr  = minus_ptr;
   plus_ptr->rite_ptr  = r_ptr;

   return(plus_ptr); /* Not fully implemented ! */

   }

/*************************************************************************/
/* This function accepts a ref node, and attempts to simplify the tree   */
/* by evaluating constant terms.                                         */
/*************************************************************************/

static
Any_ptr 

   simplify_oe_tree (Any_ptr ofx_ptr)

   {

   Oper_ptr          o_ptr = NULL;
   Ref_ptr           l_ref = NULL; /* HWG34 */
   Ref_ptr           r_ref = NULL;
   Ref_ptr           a_ptr = NULL; /* answer ptr */
   Symbol_ptr        s_ptr = NULL;
   short                l_type;
   short                r_type;
   short                oper;
   short                l_val = 0;
   short                r_val = 0;
   short                result;
   char              answer[16]="";

   if (ofx_ptr == NULL)
      return(NULL); /* no expression, so get out of here !  */

   if (nodetype(ofx_ptr) == REFERENCE)
      return(ofx_ptr); /* this cant possibly be simplified  */

   if (optimize_reqd == 0)
      return(ofx_ptr);  /* make this process optional for now */

   oper  = nodetype(ofx_ptr);
   o_ptr = ofx_ptr;

   l_type = nodetype(o_ptr->left_ptr);
   r_type = nodetype(o_ptr->rite_ptr);

   if (l_type != REFERENCE)
      o_ptr->left_ptr = simplify_oe_tree (o_ptr->left_ptr);

   if (r_type != REFERENCE)
      o_ptr->rite_ptr = simplify_oe_tree (o_ptr->rite_ptr);

   l_type = nodetype(o_ptr->left_ptr);
   r_type = nodetype(o_ptr->rite_ptr);

   if (l_type == REFERENCE) 
      l_ref = o_ptr->left_ptr;

   if (r_type == REFERENCE)
      r_ref = o_ptr->rite_ptr;

   if ((l_ref != NULL) && (r_ref != NULL))
      if ((l_ref->symbol->class == CONSTANT) && 
          (r_ref->symbol->class == CONSTANT))
         {
         /*********************************************/
         /* Convert these constants into integer form */
         /*********************************************/ 
         l_val  = atoi(l_ref->symbol->spelling);
         r_val  = atoi(r_ref->symbol->spelling);
  
         switch (oper) {

         case(PLUS):
             result = l_val + r_val;
             break;
         case(MINUS):
             result = l_val - r_val;
             break;
         case(TIMES):
             result = l_val * r_val;
             break;
         default:
             report(132,"",_LINE_);

         } /* end switch */

         itoa (result,answer,10);
         s_ptr = create_constant(l_ref->symbol->declarator,answer,NUMERIC);
         a_ptr = allocate_node (REFERENCE);
         a_ptr->symbol       = s_ptr;
         a_ptr->attribs      = s_ptr;
         a_ptr->spelling     = s_ptr->spelling;

         return(a_ptr);

         } /* end if     */
         
   return (ofx_ptr);

   }

/*************************************************************************/
/*         Return the node-type of the node pointed to by g_ptr.         */
/*************************************************************************/

static
short   

   next_node_type (Any_ptr g_ptr)

   {

   Dummy_ptr       d_ptr;

   if (g_ptr == NULL)
      return (UNKNOWN);

   d_ptr = g_ptr;

   return (nodetype(d_ptr->next_ptr));

   }   

/**************************************************************************/
/*       Return ptr to the 'next' node in the current parse-tree.         */
/**************************************************************************/

static
Any_ptr 

   get_next_node (void)

   {

   Dummy_ptr             d_ptr;

   if (curr_ptr == NULL)
      curr_ptr = block_root->first_stmt;
   else
      {  
      d_ptr    = curr_ptr;
      curr_ptr = d_ptr->next_ptr;
      }

   if (curr_ptr == NULL)
      longjmp (exit_pass2,1);

   return (curr_ptr);

   }
     
/**************************************************************************/
/* Determine the arithmetic SCALE of the result of this operator node     */
/* This is dependent on the SCALE of the two children of this node.       */
/**************************************************************************/ 

static
chur  

   get_common_scale (Oper_ptr ptr)

   {

   Ref_ptr    r_ptr;
   Oper_ptr   o_ptr;

   short      l_scale;
   short      r_scale;

   if (nodetype(ptr->left_ptr) == REFERENCE)
      {
      r_ptr = ptr->left_ptr;
      l_scale = r_ptr->attribs->scale;
      }
   else
      {
      o_ptr = ptr->left_ptr;
      l_scale = o_ptr->scale;
      }

   if (nodetype(ptr->rite_ptr) == REFERENCE)
      {
      r_ptr = ptr->rite_ptr;
      r_scale = r_ptr->attribs->scale;
      }
   else
      {
      o_ptr = ptr->rite_ptr;
      r_scale = o_ptr->scale;
      }

   if ((l_scale == D_FLOAT) || (r_scale == D_FLOAT))
      return(D_FLOAT);
   else
      return(FIXED); 
   
   }

/**************************************************************************/
/* Determine the arithmetic BASE  of the result of this operator node     */
/* This is dependent on the BASE  of the two children of this node.       */
/**************************************************************************/ 

static
chur 

   get_common_base (Oper_ptr ptr)

   {

   Ref_ptr    r_ptr;
   Oper_ptr   o_ptr;

   short      l_base;
   short      r_base;

   if (nodetype(ptr->left_ptr) == REFERENCE)
      {
      r_ptr = ptr->left_ptr;
      l_base = r_ptr->attribs->type;
      }
   else
      {
      o_ptr = ptr->left_ptr;
      l_base = o_ptr->base;
      } 

   if (nodetype(ptr->rite_ptr) == REFERENCE)
      {
      r_ptr = ptr->rite_ptr;
      r_base = r_ptr->attribs->type;
      }
   else
      {
      o_ptr = ptr->rite_ptr;
      r_base = o_ptr->base;
      }

   if ((l_base == BINARY) || (r_base == BINARY))
      return(BINARY);
   else
      return(DECIMAL);

   }

/**************************************************************************/
/* Determine the arithmetic Precision of the result of this operator node */
/* This is dependent on the precision of the two children of this node as */
/* well as the type of arithmetic operator.                               */
/* See page 319 for more information on (add for example) this matter.    */
/**************************************************************************/ 

static
void 

   get_precision (Oper_ptr ptr)

   {       

   Ref_ptr    r_ptr;
   Oper_ptr   o_ptr;

   short      p;
   short      q;
   short      r;
   short      s;
   short      N;  

   if (nodetype(ptr->left_ptr) == REFERENCE)
      {
      r_ptr = ptr->left_ptr;
      p = r_ptr->attribs->prec_1;
      q = r_ptr->attribs->prec_2;
      }
   else
      {
      o_ptr = ptr->left_ptr;
      get_common_base (o_ptr); 
      p = o_ptr->prec_1;
      q = o_ptr->prec_2;
      } 

   if (nodetype(ptr->rite_ptr) == REFERENCE)
      {
      r_ptr = ptr->rite_ptr;
      r = r_ptr->attribs->prec_1;
      s = r_ptr->attribs->prec_2;
      }
   else
      {
      o_ptr = ptr->rite_ptr;
      get_common_base (o_ptr); 
      r = o_ptr->prec_1;
      s = o_ptr->prec_2;
      }

   /*********************************************************************/
   /*        Find out the base so we can set the max precision (N).     */
   /*********************************************************************/

   if (ptr->base == BINARY)
      N = 31;
   else
      N = 18;

   /*********************************************************************/
   /* Now use the formula defined by ANSI for determination of result   */
   /* attributes.                                                       */
   /*********************************************************************/

   if (ptr->scale == FIXED)
      {
      switch(ptr->type) {

      case(PLUS):
          {
          ptr->prec_1 = min(N,max(p-q,r-s)+max(q,s)+1);
          ptr->prec_2 = max(q,s);
          return;
          }
      case(MINUS):
          {
          ptr->prec_1 = min(N,max(p-q,r-s)+max(q,s)+1);
          ptr->prec_2 = max(q,s);
          return;
          }
      case(TIMES):
          {
          ptr->prec_1 = min(N,p+r+1);
          ptr->prec_2 = q+s;
          return;
          }
      case(DIVIDE):
          {
          ptr->prec_1 = N;
          ptr->prec_2 = N-p+q-s;
          return;
          }

      } /* switch */

      } 

   return;

   }

/**************************************************************************/
/* This function returns true if the ref node is part of a qualified ref  */
/**************************************************************************/

static
short  

qualified (Ref_ptr r_ptr)

    {

    if ((r_ptr->symbol->parent) || (r_ptr->dot_ptr))
       return(1);

    return(0);

    }       

/**************************************************************************/
/* This function sets the external variable 'line_no' to that held in the */
/* program node currently being processesed. The variable is used to      */
/* report error line numbers in report.c                                  */
/**************************************************************************/    


void 

   set_error_line (Any_ptr g_ptr)

   {

   Dummy_ptr       d_ptr;

   d_ptr = g_ptr;

   strcpy (line_no,d_ptr->line_no);

   if ((trace_pass2) && (semantic_reqd))
      printf("Beggining analysis of line: %s\n",line_no);

   }

/**************************************************************************/
/* This function takes a ref chain, and returns the last element on it.   */
/**************************************************************************/

static
Ref_ptr 

   get_reference_tail (Ref_ptr r_ptr)

   {

   Ref_ptr           nxt_ptr;

   nxt_ptr = NULL;

   while (r_ptr != NULL)
         {
         nxt_ptr = r_ptr;
         r_ptr = r_ptr->dot_ptr;
         }

   return(nxt_ptr);

   }        

/**************************************************************************/
/* This function is used to create a new constant within the symbol table */
/* of the specified PL/1 block.                                           */
/**************************************************************************/

static
Symbol_ptr 

   create_constant (Block_ptr block,char * text,short token)

   {  

   Symbol_ptr        s_ptr;
   char             *copy_text;

   s_ptr = get_symbol (/* curr_ */ block,text);

   if (s_ptr == NULL)
      {
      /***************************************************************/
      /*    Allocate some space to hold the text for the constant.   */
      /***************************************************************/ 

      copy_text = (char *)malloc((strlen(text) + 1));

      strcpy(copy_text,text);

      s_ptr = add_symbol (block,
                          copy_text,
                          token,
                          0,
                          0,          
                          0,          
                          CONSTANT, 
                          0, 
                          INTERNAL,
                          1,         
                          0,
                          NULL,
                          "");
      }

   return(s_ptr);

   }

/****************************************************************************/
/* This function returns true if the node type passed in represents one of  */
/* the arithmetic operations.                                               */
/****************************************************************************/

static
short  

   arithmetic (short type)

   {

   if ((type == PLUS)  || 
       (type == MINUS) || 
       (type == TIMES) || 
       (type == DIVIDE))
       return(1);

   return(0);

   }

/***************************************************************************/
/* This function checks those aspects of a reference that only have a      */
/* meaning when the reference is part of an expression.                    */
/***************************************************************************/

static
void

   check_operand (Any_ptr node_ptr)

   {

   Ref_ptr        r_ptr;
   Symbol_ptr     s_ptr;

   r_ptr = node_ptr;

   s_ptr = r_ptr->symbol;

   /*********************************************************************/
   /* If the reference is to a PL/I procedure, then the procedure MUST  */
   /* be a function, if it has a null subscript list or if it has some  */
   /* subscripts. If it has NO subscripts at all (not even empty) then  */
   /* it may be a reference to an entry value, which is not tested here */
   /*********************************************************************/

   if (s_ptr->type == ENTRY)
      {
      if (s_ptr->scope == EXTERNAL)
         if (s_ptr->ret_ptr == NULL)
            {
            if ((r_ptr->null_list) || (r_ptr->sublist != NULL))
               {
               report(125,r_ptr->spelling,_LINE_);
               return;
               }
            }
         
      }

   if (s_ptr->type == ENTRY)
      {
      if (s_ptr->scope != EXTERNAL)
         if (s_ptr->proc_ptr->function == 0)
            {
            if ((r_ptr->null_list) || (r_ptr->sublist != NULL))
               {
               report(125,r_ptr->spelling,_LINE_);
               return;
               }
            }
         
      }

   }

/****************************************************************************/
/* This function accepts an operator node, and applies whatever conversions */
/* are required, in order to convert the operands to the same base & scale  */
/* as that held (previously determined) in the operator node itself.        */
/****************************************************************************/

static
Any_ptr

   apply_conversion (Oper_ptr o_ptr)

   {

   Oper_ptr          op_ptr;
   Ref_ptr           r_ptr;
   short             t_base;    /* t for target */
   short             t_scale;
   short             s_base;    /* s for source */
   short             s_scale;
   Any_ptr           operand;

   t_base  = o_ptr->base;
   t_scale = o_ptr->scale;

   /**********************************************************************/
   /* Determine the source base and scale, from the operators left child */
   /**********************************************************************/

   operand = o_ptr->left_ptr;

   if (nodetype(operand) == REFERENCE)
      {
      r_ptr   = operand; 
      s_base  = r_ptr->attribs->type;
      s_scale = r_ptr->attribs->scale;
      }
   else
      {
      op_ptr  = operand;
      s_base  = op_ptr->base;
      s_scale = op_ptr->scale;
      }

   o_ptr->left_ptr = select_conversion(operand,t_base,t_scale,s_base,s_scale);

   /**********************************************************************/
   /* Determine the source base and scale, from the operators rite child */
   /**********************************************************************/

   operand = o_ptr->rite_ptr;

   if (nodetype(operand) == REFERENCE)
      {
      r_ptr   = operand; 
      s_base  = r_ptr->attribs->type;
      s_scale = r_ptr->attribs->scale;
      }
   else
      {
      op_ptr  = operand;
      s_base  = op_ptr->base;
      s_scale = op_ptr->scale;
      }

   o_ptr->rite_ptr = select_conversion(operand,t_base,t_scale,s_base,s_scale);

   return(o_ptr);

   }

/***************************************************************************/
/* This function will select a conversion operator to convert the source   */
/* data type to its required target data type.                             */
/***************************************************************************/

static
Any_ptr

   select_conversion (Any_ptr op,short t_base,short t_scale,short s_base,short s_scale)

   /* op;         operand or operator */
   /* t_base;     t for target */
   /* t_scale;
   /* s_base;     s for source */
   /* s_scale;    */

{
      
   /***********************************************************************/
   /* Now examine the source and target attributes to determine the req'd */
   /* conversion operators.                                               */
   /***********************************************************************/    

   if ((s_base == DECIMAL) && (s_scale == FIXED))
      {
      if ((t_base == DECIMAL) && (t_scale == D_FLOAT))
         return(cv_float_dec(op));
      if ((t_base == BINARY) &&  (t_scale == D_FLOAT))
         return(cv_float_bin(op));
      if ((t_base == BINARY) &&  (t_scale == FIXED))
         return(cv_fixed_bin(op));
      }

   if ((s_base == BINARY) && (s_scale == FIXED))
      {
      if ((t_base == DECIMAL) && (t_scale == D_FLOAT))
         return(cv_float_dec(op));
      if ((t_base == BINARY) &&  (t_scale == D_FLOAT))
         return(cv_float_bin(op));
      if ((t_base == DECIMAL) && (t_scale == FIXED))
         return(cv_fixed_dec(op));
      }

   if ((s_base == DECIMAL) && (s_scale == D_FLOAT))
      {
      if ((t_base == DECIMAL) && (t_scale == FIXED))
         return(cv_fixed_dec(op));
      if ((t_base == BINARY) &&  (t_scale == D_FLOAT))
         return(cv_float_bin(op));
      if ((t_base == BINARY) &&  (t_scale == FIXED))
         return(cv_fixed_bin(op));
      }

   if ((s_base == BINARY) && (s_scale == D_FLOAT))
      {
      if ((t_base == DECIMAL) && (t_scale == D_FLOAT))
         return(cv_float_dec(op));
      if ((t_base == BINARY) &&  (t_scale == FIXED))
         return(cv_fixed_bin(op));
      if ((t_base == DECIMAL) && (t_scale == FIXED))
         return(cv_fixed_dec(op));
      }

   return(op);
      
   }

/****************************************************************************/
/* These functions will modify the expression tree passed in, by inserting  */
/* function reference nodes, that refer to PL/I conversion functions.       */
/* Each function below, will return a pointer to the new expression.        */
/****************************************************************************/

static
Any_ptr

   cv_fixed_bin (Any_ptr op)     /* convert to fixed binary */

   {
   
   Ref_ptr           r_ptr;
   Oper_ptr          op_ptr;
   Symbol_ptr        cv_temp;
   Symbol_ptr        cv_call;
   short             s_base;
   short             s_scale;
   short             s_prec_1;
   short             s_prec_2;
   short             t_prec_1;
   short             t_prec_2;
   

   /*********************************************************************/
   /* We must examine the source attributes (held in op) and determine  */
   /* 1) The attributes of the conversion result (ie the temporary)     */
   /* 2) The name(s) of any conversion function(s)                      */
   /*********************************************************************/ 

   /* determine the source attributes */
   
   if (nodetype(op) == REFERENCE)
      {
      r_ptr   = op; 
      s_base  = r_ptr->attribs->type;
      s_scale = r_ptr->attribs->scale;
      s_prec_1 = r_ptr->attribs->prec_1;
      s_prec_2 = r_ptr->attribs->prec_2;
      }
   else
      {
      op_ptr   = op;
      s_base   = op_ptr->base;
      s_scale  = op_ptr->scale;
      s_prec_1 = op_ptr->prec_1;
      s_prec_2 = op_ptr->prec_2;
      }

   if ((s_base == DECIMAL) && (s_scale == FIXED))
      {
      /* fixed dec to fixed bin */
      t_prec_1 = min((short)ceil(s_prec_1 * 3.322) + 1,31);
      t_prec_2 = (short)ceil(s_prec_2 * 3.322);
      cv_fixed_bin_r++ ; /* flag the use of this conversion */
      cv_call = dcl_entry(CV_DEC_TO_SHORT);
      cv_temp = dcl_temp();

      cv_temp->type = BINARY;
      cv_temp->prec_1 = t_prec_1;
      cv_temp->prec_2 = t_prec_2;
      cv_temp->scale  = FIXED;
      return(insert_conversion(cv_call,op,cv_temp));
      }

   return(op); /* the original expression */

   }


static
Any_ptr

   cv_fixed_dec (Any_ptr op)     /* convert to fixed decimal */


   {

   Ref_ptr           r_ptr;
   Any_ptr           i_ptr; /* intermmediate result */
   Oper_ptr          op_ptr;
   Symbol_ptr        cv_temp;
   Symbol_ptr        cv_call;
   short             s_base;
   short             s_scale;
   short             s_prec_1;
   short             s_prec_2;
   short             t_prec_1;
   short             t_prec_2;
   short             data_type;

   /*********************************************************************/
   /* We must examine the source attributes (held in op) and determine  */
   /* 1) The attributes of the conversion result (ie the temporary)     */
   /* 2) The name(s) of any conversion function(s)                      */
   /*********************************************************************/ 

   /* determine the source attributes */
   
   if (nodetype(op) == REFERENCE)
      {
      r_ptr   = op; 
      s_base  = r_ptr->attribs->type;
      s_scale = r_ptr->attribs->scale;
      s_prec_1 = r_ptr->attribs->prec_1;
      s_prec_2 = r_ptr->attribs->prec_2;
      data_type = r_ptr->attribs->type;
      }
   else
      {
      op_ptr   = op;
      s_base   = op_ptr->base;
      s_scale  = op_ptr->scale;
      s_prec_1 = op_ptr->prec_1;
      s_prec_2 = op_ptr->prec_2;
      data_type = op_ptr->data_type;
      }

   if ((s_base == BINARY) && (s_scale == FIXED))
      {
      /* fixed bin to fixed dec */
      t_prec_1 = min((short )ceil(s_prec_1 / 3.322) + 1,18);
      t_prec_2 = (short)ceil(s_prec_2 / 3.322);
      cv_fixed_dec_r++;
      cv_call = dcl_entry(CV_SHORT_TO_DEC);
      cv_temp = dcl_temp();

      cv_temp->type = DECIMAL;
      cv_temp->prec_1 = t_prec_1;
      cv_temp->prec_2 = t_prec_2;
      cv_temp->scale  = FIXED;
      return(insert_conversion(cv_call,op,cv_temp));
      }

   if (data_type == CHARACTER)
      {
      cv_arithmetic_r++;
      cv_call = dcl_entry(CV_CAR_TO_ATC);
      cv_temp = dcl_temp();
      cv_temp->type = DECIMAL;
      /**********************************************************/
      /* Note the precision of the resulting fixed dec is NOT   */
      /* Known to the compiler, since it depends on the data    */
      /* held in the string. The code generator MUST use the    */
      /* values returned by the code generator, as the input    */
      /* to the second conversion step.                         */
      /* We will therefore create a 10 byte temp to hold result */
      /**********************************************************/ 
      cv_temp->prec_1 = 0; 
      cv_temp->prec_2 = 0;
      cv_temp->scale  = FIXED;
      cv_temp->bytes  = 10;
      i_ptr = insert_conversion(cv_call,op,cv_temp);
      /**********************************************************/
      /* Now convert this value to an adjusted FIXED DEC form   */
      /**********************************************************/
      cv_fixed_dec_r++;
      cv_call = dcl_entry(CV_DEC_TO_DEC);
      cv_temp = dcl_temp();
      cv_temp->type = DECIMAL;
      cv_temp->prec_1 = t_prec_1;
      cv_temp->prec_2 = t_prec_2;
      cv_temp->scale  = FIXED;
      return(insert_conversion(cv_call,i_ptr,cv_temp));
      }
      

   return(op);

   }


static
Any_ptr

   cv_float_bin (Any_ptr op)     /* convert to float binary */

   {

   /*********************************************************************/
   /* We must examine the source attributes (held in op) and determine  */
   /* 1) The attributes of the conversion result (ie the temporary)     */
   /* 2) The name(s) of any conversion function(s)                      */
   /*********************************************************************/ 

   return(op);

   }


static
Any_ptr

   cv_float_dec (Any_ptr op)     /* convert to float decimal */

   {

   /*********************************************************************/
   /* We must examine the source attributes (held in op) and determine  */
   /* 1) The attributes of the conversion result (ie the temporary)     */
   /* 2) The name(s) of any conversion function(s)                      */
   /*********************************************************************/ 

   return(op);

   }

/****************************************************************************/
/* This function will 'create' a declaration for a PL/I library routine.    */
/* It will appear in the symtab as if it were declared by the user in the   */
/* same block as that containing the expression to which the conversion     */
/* refers.                                                                  */
/****************************************************************************/

static
Symbol_ptr

   dcl_entry (char * name)

   {

   char           *copy_text;
   Symbol_ptr     s_ptr;

   /***************************************************************/
   /*    Allocate some space to hold the text for the constant.   */
   /***************************************************************/ 

   copy_text = (char *)malloc((strlen(name) + 1));

   strcpy(copy_text,name);

   s_ptr = get_symbol (curr_block,name);

   if (s_ptr == NULL)
      s_ptr = add_symbol (curr_block,
                          copy_text,
                          ENTRY,
                          0,
                          8,          
                          0,          
                          STATIC, 
                          ENTRY, 
                          EXTERNAL,
                          1,         
                          0,
                          NULL,
                          "");
   

   s_ptr->referenced = 1;
   s_ptr->known_size = 1;
   s_ptr->known_locn = 1;
   s_ptr->bytes      = 8;
   s_ptr->offset     = curr_block->stattic;
   strcpy(s_ptr->line,"0 (N/A)");  /* HWG38 */
   curr_block->stattic += 8; 

   return(s_ptr);

   }

/***************************************************************************/
/* This function simply declares an automatic variable in the current block*/
/* that will be used as a temporary during the evaluation of an expression */
/***************************************************************************/

static
Symbol_ptr

   dcl_temp (void)

   {

   char           num[16]="";
   char           *tmp_name;
   Symbol_ptr     s_ptr;

   /***************************************************************/
   /*    Allocate some space to hold the text for the constant.   */
   /***************************************************************/ 

   tmp_name = (char *)malloc(33);

   strcpy(tmp_name,"Temp_");
   strcat(tmp_name,line_no);
   ltoa(unique_val,num,10);
   strcat(tmp_name,num);
   unique_val ++;

   s_ptr = add_symbol (curr_block,
                       tmp_name,
                       0,
                       0,
                       0,          
                       0,          
                       AUTOMATIC, 
                       0, 
                       INTERNAL,
                       1,         
                       0,
                       NULL,
                       "");

   s_ptr->temporary = 1;

   return(s_ptr);

   }



/****************************************************************************/
/* This function takes a symbol ptr to the conversion entry, and a pointer  */
/* to an expression. It then modifies the expression tree, so that the      */
/* original expression becomes an argument of the conversion function, and  */
/* the conversion function then replaces the original expression.           */
/* Thus if we are passed 'pli$bin_to_dec' and an expression 'val1 + val2'   */
/* then the original expression is recast as if the user had actually coded */
/* the expression: 'pli$bin_to_dec (val1 + val2).                           */
/****************************************************************************/

static
Any_ptr

   insert_conversion (Symbol_ptr function,
                      Any_ptr    expression,
                      Symbol_ptr temporary)

   {

   Sub_ptr           arg_ptr;
   Ref_ptr           cv_ptr;
   Any_ptr           temp;

   conversions_in_use = 1;

   cv_ptr = allocate_node(REFERENCE);

   arg_ptr = allocate_node(SUBSCRIPT);

   cv_ptr->symbol   = function;
   cv_ptr->attribs  = function;
   cv_ptr->spelling = function->spelling;
   cv_ptr->temp     = temporary;
   cv_ptr->sublist  = arg_ptr;
   cv_ptr->num_subs++;

   arg_ptr->expression = expression;

   temp = cv_ptr;

   return(temp);

   } 
