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
/*                    PL/1 Compiler Release 1.0                             */
/****************************************************************************/

/****************************************************************************/
/*                         Modification History                             */
/****************************************************************************/
/*  Who    When                           What                              */
/* ------------------------------------------------------------------------ */
/* HWG1   13-07-90       Initial Prototype.                                 */
/* HWG2   22-02-91       Parse tree construction implemented                */
/* HWG3   12-04-91       Duplicate internal procedures are now detected !   */
/* HWG4   22-04-91       Full PL/1 reference parsing implemented.           */
/* HWG5   22-04-91       Array declarations parsed and stored in tree.      */
/* HWG6   17-04-91       Keywords are no longer reserved.                   */
/* HWG7   19-04-91       Level 1 name attributes incorporated, ie class.    */
/* HWG8   04-06-91       Parser was in error for: do dcl = 1 to 10;         */
/* HWG9   17-06-91       Parser error when 'end nnn' and nnn was keyword.   */
/* HWG10  19-06-91       Parsing of secondary entry points included.        */
/* HWG11  24-06-91       Leave statement incorporated.                      */ 
/* HWG12  27-06-91       Operator precedence parsing of expressions.        */
/* HWG13  29-06-91       Parsing of PL/1 select groups implemented.         */
/* HWG14  30-06-91       Change HWG6 was weak, but is now fully implemented */
/* HWG15  07-07-91       When parse finished, we ensure source is ended.    */
/* HWG16  14-07-91       Volatile attribute now accepted by parser.         */
/* HWG17  18-07-91       Parameters named after keywords caused a fault.    */
/* HWG18  18-07-91       Returns option now accepted on procs/entries.      */
/* HWG19  22-07-91       Bug in parsing SELECT's fixed.                     */
/* HWG20  22-07-91       Count of no of returns stmts per block now held.   */
/* HWG21  03-08-91       Constants are now added to symtab when encountered */
/* HWG22  08-09-91       Labels followed by a stmt containing PROC or ENTRY */
/*                       were always assumed to be procedure or entry block */
/*                       a valid assignment ie entry(a)=2 gave syntax error */
/* HWG23  10-09-91       If an expression doesnt begin with value or ident  */
/*                       then parse_expression now returns failure.         */
/* HWG24  16-09-91       Subscripts may now take the form: name(I)(J)..etc  */
/* HWG25  20-09-91       The correct form of based/defined was NOT being    */
/*                       accepted by pass1, it was only allowing a single   */
/*                       identifier to appear as the arg of defined/based   */
/*                       it now accepts any PL/1 reference.                 */    
/* HWG26  23-09-91       The recursive option was only being recognised by  */
/*                       this phase, when the procedure had parameters.     */
/* HWG27  28-09-91       parser was reporting a syntax error if an internal */
/*                       procedure was encountered with the same name as    */
/*                       its parent procedure. This is legal PL/1 however.  */
/* HWG28  10-10-91       Procedure nodes were not having their line number  */
/*                       set, and this was causing problems in later phases */
/*                       specificaly error reporting in the coding phase.   */
/* HWG29  18-10-91       A small change was made in parse_statement so that */
/*                       when we have parsed a procedure, build a parse     */
/*                       tree on a per/block basis.                         */
/* HWG30  23-10-91       The line number in a proc/block node is now set    */
/*                       in the parse_procedure function.                   */   
/* HWG31  24-10-91       The procedure options option is now accepted.      */
/* HWG32  30-10-91       Parameter address offsets, are easy to determine   */
/*                       and this is now done in here, as we parse a blocks */
/*                       parameter list.                                    */
/* HWG33  21-12-91       The parsing of 'on condition(X)' etc was not quite */  
/*                       correct.                                           */ 
/* HWG34  23-12-91       Parsing of the 'signal' statement incorporated.    */
/*                                                                          */
/* HWG35  01-01-92       parse_if was at fault, when we had a stmt like:    */
/*                       if a = b then                                      */
/*                          a = 0;                                          */
/*                       else = 1;                                          */  
/*                       this syntax bug has now been fixed.                */   
/*                                                                          */
/* HWG36  07-01-92       For some totally obscure reason 'parse_expression' */
/*                       was reporting error 98 at the end, this is very    */
/*                       wrong, and I have assumed it should be 46.         */ 
/* HWG37  11-01-92       parse_sublist changed so that 'Sub' nodes are now  */
/*                       doubly linked.                                     */
/* HWG38  13-01-92       parse_go was introduced to parse the alternative   */
/*                       syntactic form of the PL/I goto statement.         */
/* HWG39  16-01-92       When parse_subname created a symbol entry for any  */
/*                       constants, it failed to set the Refs attribs ptr   */
/*                       this caused codegen to fault when it tried to      */
/*                       reference them (for arrays).                       */   
/* HWG40  17-01-92       parse_sublist had a major BUG !, although it was   */
/*                       syntactically accepting say: a.b.c(1)(2)(3) it was */
/*                       not appending the subscripts to the sublist for c  */
/*                       this function now calls the pass2 function         */
/*                       append_subscript to do this. Note a.b.c(1,2,3) was */
/*                       being processed correctly all along however.       */    
/* HWG41  19-01-92       Varying strings appearing within a structure were  */
/*                       not being flagged as such correctly.               */  
/*                                                                          */
/* HWG42  15-02-92       parse_parmlist was NOT chaining parameters onto    */
/*                       the PROC nodes list correctly, only the last para- */
/*                       meter was held. Result ? bad diagnostics produced  */
/*                       in pass2 by 'test_arguments'.                      */
/* HWG43  12-07-92       parse_declare only reported a duplicate dcl for    */
/*                       (ie dcl (x,y,...) ) the 'x' the others were skipped*/
/*                                                                          */
/* HWG44  07-03-93       The BY NAME option is now recognised for the       */
/*                       assignment statement.                              */
/*                                                                          */
/* HWG45  11/11/93       parse_declare,parse_structure & parse_attribute    */
/*                       modified, so that declares can be a comma-list     */
/*                       (of strucs or scalars). A declare of a struc with  */
/*                       a scalar immediately following does not yet work   */
/*                       correctly however (<struc>,<struc> etc does and    */
/*                       <scalar>,<scalar> does too).                       */
/*                                                                          */
/* HWG46 21/02/96        Implement parsing of the INITIAL option in a dcl.  */
/*       26/02/96        Completed the above.							    */
/*                                                                          */
/* HWG47 10/03/96        Parse the OPEN statement.                          */
/*                                                                          */
/* HWG48 28/04/96        Completely reimplemented the parsing of iterative  */
/*                       loops (now done in parse_group). The full ANSI     */
/*                       syntax is now supported (includes UNTIL. WHILE).   */
/*                                                                          */
/* HWG49 04/04/96        Parse the CLOSE statement.                         */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                         Functional Description                           */
/****************************************************************************/
/* This large collection of functions constitues the recursive descent      */
/* parser for the PL/1 language. This phase performs the bulk of the syntax */
/* checking and error reporting.                                            */
/* The language is parsed in a top-down recursive descent manner, with PL/1 */
/* expressions being parsed using operator precedence.                      */   
/* A parse tree is gradually built up, as the parser proceeds. Each time a  */
/* valid Non-terminal has been 'recognised' a corresponding 'node' is       */
/* created and inserted into the tree.                                      */
/* The subsequent 'pass2' phase will perform all semantic checking.         */
/* The code generator accepts the resulting parse-tree and translates it to */
/* target object code. The optimizer can optionally be run prior to the     */
/* generation of code.                                                      */
/* Before attempting to parse a statement, the parser determines wether or  */
/* not the statement is a legal assignment stmt beginning with a keyword.   */
/* This is done to fully support the language's lack of reserved words.     */
/* The approach is simple (to state in english !) if a statement begins     */
/* with a keyword, then if it is NOT an assignment (ie parse_assigment      */
/* fails) then assume (that is attempt to parse) that it IS a keyword.      */ 
/****************************************************************************/

#include "stdlib.h"
#include "c_types.h"
#include "tokens.h"
#include "token.h"
#include "stdio.h"
#include "string.h"
#include "nodes.h"
#include "symtab.h"
#include "setjmp.h"

# define _LINE_     ((short)__LINE__)

/****************************************************************************/
/*             E X T E R N A L    F U N C T I O N S                         */
/****************************************************************************/

short       builtin               (char *);
void        report                (short ,char *,short);
Any_ptr     allocate_node         (short );        /* Build up parse tree */
Any_ptr     free_node			  (Any_ptr);
short       nodetype              (Any_ptr);
void        insert_node           (Any_ptr,Any_ptr);
Any_ptr     next_stmt             (Any_ptr);

extern FILE  *in_port;
extern char  line_no[10];
extern short Err_count;
       short nesting;

extern Procedure_ptr parse_root;

#define PARSER		   static short	

#define FAIL           0
#define OK             1

#define CHAIN_SIZE     4000  /* should NEVER be less than the number of */
                             /* tokens in the largest structure allowed */

#define push_operand(p)      operand_stack[operand_stack_ptr++] = p
#define push_operator(p)     operator_stack[operator_stack_ptr++] = p
#define pop_operand()        operand_stack[--operand_stack_ptr]
#define pop_operator()       operator_stack[--operator_stack_ptr]

/****************************************************************************/
/*                L O C A L     F U N C T I O N S                           */
/****************************************************************************/

short  parse_program                    (void);
PARSER parse_block                      (Block_ptr,Any_ptr);
PARSER parse_entrypoint                 (char[]);
PARSER parse_secondary                  (Block_ptr,Any_ptr);
PARSER parse_entry_list                 (Symbol_ptr);
PARSER parse_initial					(Symbol_ptr);
PARSER parse_descriptor                 (Data_ptr);
PARSER parse_procedure                  (Procedure_ptr,char[]);
PARSER parse_parmlist                   (Any_ptr);
PARSER parse_statement                  (Any_ptr);
PARSER parse_signal                     (Any_ptr);
PARSER parse_declare                    (void);
PARSER parse_if                         (Any_ptr);
PARSER parse_assignment                 (Any_ptr);
PARSER parse_call                       (Any_ptr);
PARSER parse_do                         (Any_ptr);
PARSER parse_put                        (Any_ptr);
PARSER parse_open                       (Any_ptr);
PARSER parse_close                      (Any_ptr);
PARSER parse_delete                     (Any_ptr);
PARSER parse_read                       (Any_ptr);
PARSER parse_rewrite                    (Any_ptr);
PARSER parse_write                      (Any_ptr);
PARSER parse_get                        (Any_ptr);
static Format_ptr parse_format          (void);
static Intarg_ptr parse_get_list        (void);
static Outsrc_ptr parse_put_list        (void);
PARSER parse_until                      (Any_ptr);
PARSER parse_do_spec                    (Loop_ptr); 
PARSER parse_while                      (Any_ptr);
PARSER parse_group                      (Any_ptr);
PARSER parse_allocate                   (Any_ptr);
PARSER parse_null                       (void);
PARSER parse_attribute                  (Symbol_ptr);
PARSER parse_returns                    (Symbol_ptr);
Any_ptr static parse_expression         (void);
Ref_ptr static parse_reference          (void);
PARSER parse_free                       (Any_ptr);
PARSER parse_begin                      (Block_ptr,Any_ptr);
PARSER parse_on                         (Any_ptr);
PARSER parse_goto                       (Any_ptr);
PARSER parse_go                         (Any_ptr);
PARSER parse_return                     (Any_ptr);
PARSER parse_stop                       (void);
PARSER parse_select                     (Any_ptr);    
PARSER parse_leave                      (Any_ptr);
PARSER parse_when                       (Any_ptr);
PARSER parse_other                      (Any_ptr);
PARSER parse_label                      (Any_ptr);
PARSER parse_field                      (Symbol_ptr);
PARSER parse_structure_attribute        (Symbol_ptr);
PARSER parse_structure                  (Token_ptr,Symbol_ptr);
PARSER parse_dimension                  (Symbol_ptr);
PARSER parse_subname                    (Ref_ptr);
PARSER parse_sublist                    (Ref_ptr);
PARSER parse_repfactor					(Long_ptr);
PARSER parse_array_init					(Init_ptr);
PARSER parse_initial_element_commalist  (Ielem_ptr);
PARSER parse_initial_constant_one		(Icon1_ptr);
PARSER parse_initial_constant_two		(Icon2_ptr);
short  is_operator                         (short);
PARSER parse_condition                  (On_ptr);
void static    parse_options            (Procedure_ptr);


short  condition_type                   (short );
short  priority                         (Any_ptr);
void   append_subscript                 (Ref_ptr,Sub_ptr); /* see pass2 */
long   get_pos                          (void);
void   set_pos                          (long);
Token_ptr      get_token                        (void);
static Token_ptr      next_token                       (void);
long   increment                        (long);
static void   skip_to_next                     (short );
short  value                            (Token_ptr);
static short  this_isnt_a_keyword              (void);
Block_ptr       current_parent;        

/****************************************************************************/
/*             L O C A L     S T A T I C S                                  */
/****************************************************************************/

static short         global_stack_size    = -1; /* -1 = not set by user */
short         parse_in_progress     = 1;
short         lookahead_in_progress = 0;
static short         within_a_group        = 0; /* true if parsing a do... */
jmp_buf     escape;

Token_ptr   t;
short       eof   = 0;
long        debug_pos = 0;
Token       chain [CHAIN_SIZE];
long        real_ptr = 0;
long        virt_ptr = 0;
char        saved_line_no[10];  /* Part of a Kludge to fix awkward     */
                                /* line number problems when reporting */
                                /* errors about duplicate labels       */
 
/***************************************************************************/
/*          This function parses a single PL/1 source module.              */
/***************************************************************************/

short 

   parse_program (void)

   {

   Procedure_ptr   p_ptr;    
   short             result;

   /*****************************************************************/
   /* Install a long-jump termination mechanism, to exit if things  */
   /* get really bad !                                              */
   /*****************************************************************/

   if (setjmp(escape) == 0)
      { 
      nesting = 0;
      p_ptr = allocate_node (PROCEDURE);    
      parse_root = p_ptr;    
      result = parse_block(NULL,p_ptr);
      parse_in_progress = 0;
      t = next_token();
      if (t->token != END_OF_SOURCE)
         report(96,"",_LINE_); /* excessive tokens */
      return(result);
      }
   else
      return(0);

   }

/***************************************************************************/
/*        This function parses  a  PL/1 internal procedure.                */
/***************************************************************************/

PARSER

   parse_block (Block_ptr parent_block_ptr,Any_ptr g_ptr)

   {

   char                name[128];
   Block_ptr           b_ptr;
   Procedure_ptr       p_ptr;
   Symbol_ptr          s_ptr;

   if (parse_entrypoint(name) == 0)
      {  
      return(FAIL);
      }

   /************************************************************/
   /* Does the parent block already have another declared name */
   /* with the same name as this new block ?                   */
   /************************************************************/

   s_ptr = get_symbol(parent_block_ptr,name);

   if (s_ptr != NULL)
      report (32,name,_LINE_);

   /**********************************************************/
   /* Is there already an internal block called 'name' ?     */
   /**********************************************************/ 

   b_ptr = find_inner_block (parent_block_ptr,name);

   /**************************************************************/
   /* If a block with the same name was found, we must see if it */
   /* is a duplicate internal proc, or if it is a block with the */
   /* same name as its parent. The latter is legal PL/1.         */
   /**************************************************************/    

   if (b_ptr != NULL)
      if (b_ptr != parent_block_ptr)
         report (32,name,_LINE_);

   current_parent = insert_block (parent_block_ptr,
                                  name,
                                  0,      /* NOT a func    */
                                  0,      /* NOT recursive */
                                  0,      /* NO args       */
                                  0);     /* NO symbols    */

   strcpy (current_parent->line,saved_line_no);

   p_ptr = g_ptr;

   p_ptr->proc = current_parent;

   current_parent->first_stmt = p_ptr;

   if (parse_procedure(p_ptr,name) == 0)
      {     
      current_parent = parent_block_ptr;
      return(FAIL);
      }

   current_parent = parent_block_ptr;

   return(OK);

   }

/***************************************************************************/
/*        This function parses  a  PL/1 secondary entrypoint.              */
/***************************************************************************/

PARSER

   parse_secondary (Block_ptr parent_block_ptr,Any_ptr g_ptr)

   {

   long                p;

   char                name[32];
   Entry_ptr           e_ptr;

   Symbol_ptr          temp_ptr;

   if (parse_entrypoint(name) == 0)
      {  
      return(FAIL);
      }

   /****************************************************************/
   /*            Insert this symbol into the symtab !              */
   /****************************************************************/

   temp_ptr = get_symbol(current_parent,name);

   if (temp_ptr == NULL)
      {
  	   temp_ptr = add_symbol (current_parent,
                             name,
                             ENTRY,
                             1,
                             4,          /* 32 Bit address    */
                             0,          /* Unknown scale     */
                             CONSTANT, 
                             ENTRY, 
                             INTERNAL,
                             1,          /* This a Label !    */
                             0,
                             NULL,
                             line_no);

      }

   /**********************************************************/
   /* Is there already an internal block called 'name' ?     */
   /**********************************************************/ 

   e_ptr = (Entry_ptr)find_inner_block (parent_block_ptr,name);

   if (e_ptr != NULL)
      report (32,name,_LINE_);

   e_ptr = g_ptr;

   t = next_token();

   if (t->token!= ENTRY)
      {       
      report(3,"",_LINE_);
      return(FAIL);
      }

   p = get_pos();

   t = next_token();

   if ((t->token!= SEMICOLON) &&
       (t->token!= LPAR))
      {       
      report(4,"",_LINE_);
      return(FAIL);
      }

   /******************************************************************/
   /* If we have a case of     entry (      then parse argument list */
   /******************************************************************/

   if (t->token == LPAR)
      { 
      set_pos(p); /* Pretend we never even read the (  */
      if (parse_parmlist((Procedure_ptr)e_ptr) == 0)
         {
         skip_to_next(SEMICOLON);
         return(OK);
         }
      t = next_token();
      }

   if (t->token != SEMICOLON)
      {
      report(4,"",_LINE_);
      return(FAIL);
      } 

   return(OK);

   }

/***************************************************************************/
/*         This function determines wether a PL/1 entry label exists       */
/***************************************************************************/

PARSER

   parse_entrypoint (char name[32])


   {
   
   t = next_token();

   if ((t->token != PL1NAME) &&
       (t->keyword == 0))
      {      
      report(1,"",_LINE_);
      return(FAIL);
      }

   strcpy (name,t->lexeme);

   t = next_token();


   if (t->token != COLON)
      {     
      report(2,"",_LINE_);
      return(FAIL);
      }

   return(OK);

   }

/***************************************************************************/
/*         This function determines wether a PL/1 label exists             */
/***************************************************************************/

PARSER

   parse_label (Any_ptr g_ptr)

   {


   Token_ptr   temp;
   Symbol_ptr  temp_ptr;

   char        temp_line_no[10];
   Label_ptr   l_ptr;

   l_ptr = g_ptr;

   
   t = next_token();

   if ((t->token != PL1NAME) &&
       (t->keyword == 0))
      {      
      report(1,"",_LINE_);
      return(FAIL);
      }

   temp = t;  /* Save the actual label token ! */

   t = next_token();

   if (t->token != COLON)
      {     
      report(2,"",_LINE_);
      return(FAIL);
      }

   /****************************************************************/
   /*            Insert this symbol into the symtab !              */
   /****************************************************************/

   temp_ptr = get_symbol(current_parent,temp->lexeme);

   if (temp_ptr == NULL)
      {
      /******************************************************************/
      /* We will come thru here, if a goto occurs AFTER the label       */
      /******************************************************************/
	  temp_ptr = add_symbol (current_parent,
                             temp->lexeme,
                             temp->token,
                             temp->keyword,
                             4,          /* 32 Bit address    */
                             0,          /* Unknown scale     */
                             CONSTANT, 
                             LABEL, 
                             INTERNAL,
                             1,          /* This a Label !    */
                             0,
                             NULL,
                             saved_line_no);

      }
   else /* Name already in the symtab ! */
      if (temp_ptr -> declared)
         {
         strcpy(temp_line_no,line_no);
         strcpy(line_no,saved_line_no);
         report(32,temp->lexeme,_LINE_);
         strcpy(line_no,temp_line_no);
         return (FAIL);
         }
      else
         {
         /**************************************************************/
         /* We will come thru here, if a lable is defined AFTER a goto */
         /**************************************************************/
         temp_ptr -> declared = 1;  
         temp_ptr -> type     = LABEL;
         temp_ptr -> scope    = INTERNAL;
         temp_ptr -> prec_1    = 4;
         temp_ptr -> class    = CONSTANT;
         }

   l_ptr->identity = temp_ptr;
             
   return(OK);

   }

/***************************************************************************/
/*        This function determines wether a PL/1 procedure exists          */
/***************************************************************************/

PARSER

   parse_procedure (Procedure_ptr p_ptr,char name[32])

   {

   long        p;

   End_ptr     e_ptr;
   Any_ptr stmt_ptr;



   t = next_token();

   if (t->token!= PROCEDURE)
      {       
      report(3,"",_LINE_);
      return(FAIL);
      }

   p_ptr->proc->depth = nesting;
   nesting++;

   strcpy(p_ptr->line_no,line_no);
   strcpy(p_ptr->proc->line,line_no);

   p = get_pos();

   t = next_token();

   if ((t->token != SEMICOLON) &&
       (t->token != LPAR) &&
       (t->token != RETURNS) &&
       (t->token != OPTIONS) &&
       (t->token != RECURSIVE))
      {       
      report(4,"",_LINE_);
      return(FAIL);
      }

   /******************************************************************/
   /* If we have a case of   procedure (    then parse argument list */
   /******************************************************************/

   if (t->token == LPAR)
      { 
      set_pos(p); /* Pretend we never even read the (  */
      if (parse_parmlist(p_ptr) == 0)
         {
         skip_to_next(SEMICOLON);
         return(OK);
         }
      p = get_pos();
      t = next_token();
      }

   if (t->token == OPTIONS)
      {
      t = next_token();
      if (t->token != LPAR)
         {
         report(105,"",_LINE_);
         skip_to_next(SEMICOLON);
         return(OK);
         }
      parse_options(p_ptr);
      t = next_token();
      if (t->token != RPAR)
         {
         report(105,"",_LINE_);
         skip_to_next(SEMICOLON);
         return(OK);
         }
      p = get_pos();
      t = next_token();
      }

   if (t->token == RETURNS)
      {
      t = next_token();
      if (t->token != LPAR)
         {
         report(105,"",_LINE_);
         skip_to_next(SEMICOLON);
         return(OK);
         }
      p_ptr->ret_ptr = allocate_node(DATA);
      parse_descriptor(p_ptr->ret_ptr);
      t = next_token();
      if (t->token != RPAR)
         {
         report(105,"",_LINE_);
         skip_to_next(SEMICOLON);
         return(OK);
         }
      p_ptr->proc->function = 1;
      p = get_pos();
      t = next_token();
      }

   if (t->token == RECURSIVE)
      {
      p_ptr->proc->recursive = 1;
      t = next_token();
      }

   if (t->token != SEMICOLON)
      {
      report(4,"",_LINE_);
      return(FAIL);
      } 

   stmt_ptr = p_ptr;
   
   while (parse_statement(stmt_ptr))   /* any number of */
         {
         stmt_ptr = next_stmt (stmt_ptr);
         }

   /****************************************************************/
   /* Lets see if the above loop ended because of an END statement */
   /****************************************************************/

   t = next_token();

   if (t->token!= END)
      {       
      report(5,"",_LINE_);
      return(FAIL);
      }

   /**********************/ 
   /* Create an END node */
   /**********************/ 

   e_ptr = allocate_node (END);

   p_ptr->end     = e_ptr;
   e_ptr->partner = p_ptr;
   insert_node (stmt_ptr,e_ptr);

   t = next_token();

   /****************************************************************/  
   /*         Check the label on the 'end' (if any present).       */
   /****************************************************************/

   if ((t->token == PL1NAME) || (t->keyword))
      {
      if (strcmp(name,t->lexeme) != 0)
         report(-57,"",_LINE_);
      t = next_token();
      } 

   if (t->token!= SEMICOLON)
      {       
      report(4,"",_LINE_);
      return(FAIL);
      }

   nesting--;

   return (OK);

   }

/****************************************************************************/
/*        This function parses a PL/1 procedure argument list               */
/****************************************************************************/

PARSER

   parse_parmlist (Any_ptr g_ptr)

   {

   short           argcount;
   Symbol_ptr    s_ptr;
   Procedure_ptr p_ptr;
   Argument_ptr  a_ptr;

   argcount = 0;

   t = next_token();

   if (t->token != LPAR)   /* Compiler Error */
      {  
      report(3,"",_LINE_);
      return(FAIL);
      }

   t = next_token();

   if ((t->token != PL1NAME) && (t->keyword == 0))
      {
      report(49,"",_LINE_);
      return(FAIL);
      }

   argcount++;

   /****************************************************************/
   /*            Insert this symbol into the symtab !              */
   /****************************************************************/

   s_ptr = get_symbol(current_parent,t->lexeme);

   if (s_ptr == NULL)
      { 
	  s_ptr = add_symbol (current_parent,
                          t->lexeme,
                          t->token,
                          t->keyword,
                          0,          /* unknown length  */
                          0,          /* unknown scale   */
                          PARAMETER,
                          0,          /* unknown type    */
                          INTERNAL,
                          0,
                          0,
                          NULL,
                          line_no);

      }

   /*********************************************************/
   /* Create and setup an 'argument' node in the parse-tree */
   /*********************************************************/

   a_ptr = allocate_node (ARGUMENT);

   a_ptr->arg_ptr = s_ptr;

   /*******************************************************************/
   /* This parameters address is at offset zero, the code generator   */
   /* will add the fixed offset (eg 6 bytes) to this at code gen time */
   /*******************************************************************/

   s_ptr->offset = 0;

   if (nodetype(g_ptr) == PROCEDURE)
      {
      p_ptr = g_ptr;
      p_ptr->argument = a_ptr;
      }
   else
      report (58,"",_LINE_);
     
   t = next_token();

   /************************************************************************/
   /* Accept only occurences of  ,name   from here on, until a  ) is found */
   /************************************************************************/

   while (t->token == COMMA)
         {
         t = next_token();
   
         if ((t->token != PL1NAME) && (t->keyword == 0))
            {
            report(49,"",_LINE_);
            return(FAIL);
            }

         argcount++;

         /****************************************************************/
         /*            Insert this symbol into the symtab !              */
         /****************************************************************/

         s_ptr = get_symbol(current_parent,t->lexeme);

         if (s_ptr == NULL)
            {  /* 1st occurence of symbol */
     		   s_ptr = add_symbol (current_parent,
                               t->lexeme,
                               t->token,
                               t->keyword,
                               0,          /* unknown length  */
                               0,          /* unknown scale   */
                               PARAMETER,
                               0,          /* unknown type    */
                               INTERNAL,
                               0,
                               0,
                               NULL,
                               line_no);

      
            }

         /*********************************************************/
         /* Create and setup an 'argument' node in the parse-tree */
         /*********************************************************/

         a_ptr->next_arg = allocate_node (ARGUMENT);
         a_ptr = a_ptr->next_arg;

         a_ptr->arg_ptr = s_ptr;

         /*******************************************************************/
         /* This parameters address is at offset zero, the code generator   */
         /* will add the fixed offset (eg 6 bytes) to this at code gen time */
         /*******************************************************************/

         s_ptr->offset = (argcount - 1) * 4;

         if (nodetype(g_ptr) == PROCEDURE)
            {
            ;
    /*        p_ptr = g_ptr;
            p_ptr->argument = a_ptr;   */  /* Fault, not needed ! */
            }
         else
            report (58,"",_LINE_);

         /**************************************/
         /* OK Lets get the next token         */
         /**************************************/

         t = next_token();
         }

   if (t->token != RPAR)
      {
      report(46,t->lexeme,_LINE_);  /* A ? was found where a ) expected */
      return(FAIL);
      }

   current_parent->num_args   = argcount;
   current_parent->params     = argcount * 4; /* size of stack space used */

   return(OK);  /* All is well !! */

   }


/****************************************************************************/
/*  This function determines which type of PL/1 statements should be parsed */
/****************************************************************************/

PARSER

   parse_statement (Any_ptr prev_node)

   {

   long        p,q;
   Any_ptr     stmt_ptr;
   short       result; 

   /*******************************************************************/
   /* This function only returns OK when a PL/1 end statement is read */
   /*******************************************************************/

   p = get_pos() ;

   if (this_isnt_a_keyword())  /* check uses of keywords */
      {
      set_pos(p);
      t = next_token();
      t->keyword = 0;
      t->token   = PL1NAME;
      }
   else
      {
      set_pos(p);
      t = next_token();
      }

   set_pos (p);  /* pretend we didnt look at the token ! */

   switch (t->token)
         {
   case(DECLARE):
       if (parse_declare() == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK);
          }
       else
          return(OK);
   case(ALLOCATE):
       {
       stmt_ptr = allocate_node (ALLOCATE);
       if (parse_allocate(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK);
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(FREE):
       {
       stmt_ptr = allocate_node (FREE); 
       if (parse_free(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK);
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          } 
       }
   case(WHEN):
       {
       stmt_ptr = allocate_node (WHEN); 
       if (parse_when(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK);
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          } 
       }
   case(OTHER):
       {
       stmt_ptr = allocate_node (OTHER); 
       if (parse_other(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK);
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          } 
       }
   case(STOP):
       {
       stmt_ptr = allocate_node (STOP);
       if (parse_stop() == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK);
          }
       else
          {
          insert_node (prev_node,stmt_ptr);
          return(OK);
          }
       }
   case(SELECT):
       {
       stmt_ptr = allocate_node (SELECT);
       if (parse_select(stmt_ptr) == 0)
          {              
          skip_to_next(SEMICOLON);
          return(OK);
 	        }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       } /* the case */
   case(IF):
       {
       stmt_ptr = allocate_node (IF);
       if (parse_if(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK);
          }
       else
          {
          insert_node (prev_node,stmt_ptr);
          return(OK);
          }
       } 
   case(PL1NAME):
       /****************************************************/
       /* Parse either a PL/1 label, or an assignment stmt */
       /****************************************************/
       { /* Check that this isnt a label */
       t = next_token(); /* the original PL1NAME */
       strcpy(saved_line_no,line_no);   /* SEE Kludge note above */
       t = next_token(); /* possibly a : */
       set_pos(p);
       if (t->token!= COLON)
          {
          stmt_ptr = allocate_node (ASSIGNMENT);
          /************************************************/
          /*        This is definitely NOT a label !      */
          /************************************************/
          if (parse_assignment(stmt_ptr) == 0)
             {              
             skip_to_next(SEMICOLON);
             return(OK);
             /* return(FAIL); */
             }
          else
             {    
             insert_node (prev_node,stmt_ptr);   
             return(OK);
             }
          }
       else
          /*************************************************************/
          /* OK we have a label, so test to see if this is a procedure */
          /*************************************************************/
          {
          t = next_token();  /* the original PL1NAME */
          t = next_token();  /* the :                */

          q = get_pos();

          /**********************************************************/
          /* The next statement could be an assignment that has     */
          /* form;  procedure = 23,  or entry(a) = 2 etc so we must */
          /* test to see if the statement is an assignment. if so   */
          /* we simply have a label, if NOT we test further.        */
          /**********************************************************/

          if (this_isnt_a_keyword()) /* ie if an assignment */
             {
             set_pos(p);
             stmt_ptr = allocate_node(LABEL);
             /**************************************************/
             /* We have recognised a PL/1 label, so read it in */
             /* before returning, or we will loop !            */
             /**************************************************/
             if (parse_label(stmt_ptr) == 0)
                {
                insert_node (prev_node,stmt_ptr);   
                return(OK);
                } 
             insert_node (prev_node,stmt_ptr);   
             return(OK);
             }
          else
             set_pos(q);

          t = next_token();  /* maybe a PROCEDURE */

          set_pos(p);    /* reposition to before the label */

          if (t->token == PROCEDURE)
             /********************************************************/
             /*         OK This is a new internal procedure.         */
             /********************************************************/
             { /* This HAS to be new internal procedure */
             stmt_ptr = allocate_node (PROCEDURE);
             if (parse_block(current_parent,stmt_ptr) == 0)
                {                 
                skip_to_next(SEMICOLON);
                return(OK);            
	           } /* if */
             else
                {
                /* insert_node (prev_node,stmt_ptr); */
                return(OK); /* A valid block */
                }
             } /* if */
          else
             if (t->token == ENTRY)
                /********************************************************/
                /*         OK This is a new internal procedure.         */
                /********************************************************/
                { /* This HAS to be a secondary entry point */
                stmt_ptr = allocate_node (ENTRY);
                if (parse_secondary(current_parent,stmt_ptr) == 0)
                   {                 
                   skip_to_next(SEMICOLON);
                   return(OK);            
	              } /* if */
                else
                   {
                   insert_node (prev_node,stmt_ptr);
                   return(OK); /* A valid secondary entry point */
                   }
                } /* if */
             else
                {
                stmt_ptr = allocate_node(LABEL);
                /**************************************************/
                /* We have recognised a PL/1 label, so read it in */
                /* before returning, or we will loop !            */
                /**************************************************/
                if (parse_label(stmt_ptr) == 0)
                   {
                   insert_node (prev_node,stmt_ptr);   
                   return(OK);
                   } 
                insert_node (prev_node,stmt_ptr);   
                return(OK);
                }
          } /* else */
       } /* the entire case clause */
   case(CALL):
       {
       stmt_ptr = allocate_node (CALL);
       if (parse_call(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(ON):
       {
       stmt_ptr = allocate_node (ON);
       if (parse_on(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(GOTO):
       {
       stmt_ptr = allocate_node (GOTO);
       if (parse_goto(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }  
       }
   case(GO):
       {
       stmt_ptr = allocate_node (GOTO);
       if (parse_go(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }  
       }
   case(SIGNAL):
       {
       stmt_ptr = allocate_node (SIGNAL);
       if (parse_signal(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }  
       }
   case(RETURN):
       {
       stmt_ptr = allocate_node (RETURN); 
       if (parse_return(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(DELETE):
       {
       stmt_ptr = allocate_node (DELETE); 
       if (parse_delete(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(READ):
       {
       stmt_ptr = allocate_node (READ); 
       if (parse_read(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(WRITE):
       {
       stmt_ptr = allocate_node (WRITE); 
       if (parse_write(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(REWRITE):
       {
       stmt_ptr = allocate_node (REWRITE); 
       if (parse_rewrite(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(OPEN):
       {
       stmt_ptr = allocate_node (OPEN); 
       if (parse_open(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(CLOSE):
       {
       stmt_ptr = allocate_node (CLOSE); 
       if (parse_close(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(GET):
       {
       stmt_ptr = allocate_node (GET); 
       if (parse_get(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(PUT):
       {
       stmt_ptr = allocate_node (PUT); 
       if (parse_put(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }

   case(DO):
       /************************************************************/
       /* This is a PL/1 do, do while, or do x = y to z; statement */
       /************************************************************/
       {
       within_a_group++;

       /******************************************************************/
       /* If this is just an innocent DO; then allocate the much simpler */
       /* DO node !, a LOOP node is used for DO followed by any of the   */
       /* PL/I looping constructs.                                       */
       /******************************************************************/

       t = next_token(); /* re-read the DO */
       t = next_token(); /* either a ; or not. */

       set_pos(p); /* reposition to before the DO token */

       if (t->token == SEMICOLON)
          {
          stmt_ptr = allocate_node(DO);
          result = parse_do(stmt_ptr);
          }
       else
          { 
          stmt_ptr = allocate_node(LOOP);
          result = parse_group(stmt_ptr);
          }

       if (result == 0)
          {              
          skip_to_next(SEMICOLON);
          within_a_group--;
          return(OK);
          }
       else
          {
          /**********************************************/
          /* We have parsed a valid do-end group !      */
          /**********************************************/
          insert_node (prev_node,stmt_ptr);   
          within_a_group--;
          return(OK);
          }

       } /* the case */

   case(BEGIN):
       {
       t = next_token();  /* the original BEGIN */
       t = next_token();  /* Have a sneaky look ahead */

       set_pos(p);
       stmt_ptr = allocate_node (BEGIN);
       if (parse_begin(current_parent,stmt_ptr) == 0)
          {              
          skip_to_next(SEMICOLON);
          return(OK);
 	        }
       else
          {
          /**********************************************/
          /* We have parsed a valid begin-end block!    */
          /**********************************************/
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       } /* the case */
    case(LEAVE):
       {
       stmt_ptr = allocate_node (LEAVE); 
       if (parse_leave(stmt_ptr) == 0)
          {           
          skip_to_next(SEMICOLON);
          return(OK); 
          }
       else
          {
          insert_node (prev_node,stmt_ptr);   
          return(OK);
          }
       }
   case(SEMICOLON):
       {
       t = next_token();  /* we must re-read the ; or we will loop ! */
       return(OK);
       }
   case(END):
       {
       /****************************************************************/
       /* We have encountered a possible END statement, and this means */
       /* we have been called (probably) by parse_loop or parse_group  */
       /* parse_begin or parse_select.                                 */
       /* All of these expect to see an END, so reset our position     */
       /* but return failure, allowing caller to know were finished.   */
       /****************************************************************/
       set_pos(p);        
       return(FAIL);
       }
   default:
       {               
       /*****************************************************************/
       /* We have encountered and unrecogniseable stmt, so skip to next */
       /*****************************************************************/
       report(12,"",_LINE_);
       skip_to_next(SEMICOLON);
       return(OK);
       }
   } /* case */

   }

/****************************************************************************/
/*    This function determines wether any PL/1 declares exist               */
/****************************************************************************/

PARSER

   parse_declare (void)

   {

   Symbol_ptr  save_ptr[32];
   Symbol_ptr  temp_ptr;
   Block_ptr   b_ptr;
   Token_ptr   s;
   long        p;

   long        level_no;
   short         count;
   short         I;
   short         none_valid = 1; 

   t = next_token();

   if (t->token!= DECLARE)
      {       
      return (FAIL);
      }

step_along_commalist:         /* HWG45 */

   t = next_token();

   if ((t->token != PL1NAME) && 
       (t->token != NUMERIC) && 
       (t->token != LPAR) &&
       (t->keyword == 0))
      {
      report(1,"",_LINE_);
      return (FAIL);
      }

   if ((t->token == NUMERIC))
      {
      level_no = atol(t->lexeme);

      if (level_no == 1)
         {
         s = t;
         p = get_pos();
         t = next_token();
         set_pos(p);
         /*************************************************************/
         /* Is this name an attempt at a duplicate declaration ?      */
         /*************************************************************/
         temp_ptr = get_symbol (current_parent,
                                t->lexeme);
         if (temp_ptr != NULL)
            if (temp_ptr->declared)
               {
               report (32,t->lexeme,_LINE_);
               return(FAIL);
               }
        
         /**************************************************************/
         /* Is this a declaration of a variable when there is already  */
         /* an internal proc with this name ?                          */
         /**************************************************************/

         b_ptr = find_inner_block (current_parent,t->lexeme);

         if ((b_ptr != NULL) && (b_ptr != current_parent))
            {
            report(32,t->lexeme,_LINE_);
            return(FAIL);
            } 

         return(parse_structure(s,
                                NULL)); /* no parent cos level 1 !! */
         }
      else
         {
         report(38,"",_LINE_); /* strucs must begin with a '1' !!! */
         return(FAIL);
         }
      } 
   
   if (t->token == LPAR)
      {
      t = next_token();
      count = 0;
      while (t->token != RPAR)   /* stop loop when ) is found */
            {
            if ((t->token != PL1NAME) &&
                (t->keyword == 0))
                {
                report(1,"",_LINE_);
                return(FAIL);
                }

            /**************************************************************/
            /* Is this a declaration of a variable when there is already  */
            /* an internal proc with this name ?                          */
            /**************************************************************/

            b_ptr = find_inner_block (current_parent,t->lexeme);

            if ((b_ptr != NULL) && (b_ptr != current_parent))
               {
               report(32,t->lexeme,_LINE_);
               /* return(FAIL); */ /* dont exit, cos there may be other */
               }                   /* names, ie: dcl (a,b,c,d) etc      */

            temp_ptr = get_symbol(current_parent,  /* duplicate dcl ? */
                                  t->lexeme);
            if (temp_ptr != NULL)
               if (temp_ptr -> declared)
                  {
                  report(32,t->lexeme,_LINE_);
                  goto skip_dup_dcl;
                  /* return(FAIL); */ /* dont fail whole dcl just for this */
                  }
               else
                  ; /* temp_ptr -> declared = 1; */
            else
               {
               /***********************************************************/
               /*  We can now insert the symbol table entry for this name */
               /***********************************************************/

               temp_ptr = add_symbol (current_parent,
                                      t->lexeme,
                                      t->token,
                                      t->keyword,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      1,
                                      0,
                                      NULL, 
                                      line_no);
               none_valid = 0;
               }
            save_ptr[count] = temp_ptr;
            count++; 

skip_dup_dcl:

            t = next_token();
            if (t->token == COMMA)
               t = next_token();

            } /* while */

      p = get_pos();

      /***************************************************************/
      /* We now have an array of pointers to symbols, we must set    */
      /* the attributes for each one.                                */
      /***************************************************************/  

      if (none_valid)
         return(FAIL); /* make parser skip remaining text */
 
      for (I=0; I < count; I++)
          {
          set_pos(p); /* reset our pos, so we can loop here */
          if (parse_attribute(save_ptr[I]) == 0)
             return(FAIL);
          save_ptr[I]->declared = 1;
          }

      return(OK);
      }

   /**************************************************************/
   /* Is this a declaration of a variable when there is already  */
   /* an internal proc with this name ?                          */
   /**************************************************************/

   b_ptr = find_inner_block (current_parent,t->lexeme);

   if ((b_ptr != NULL) && (b_ptr != current_parent))
      {
      report(32,t->lexeme,_LINE_);
      return(FAIL);
      } 

   /***************************************************************/
   /* if name is in symtab, AND it was declared, then duplicate ! */
   /***************************************************************/ 

   temp_ptr = get_symbol(current_parent,
                         (t->lexeme));
   if (temp_ptr != NULL)
      if (temp_ptr -> declared)
         {
         report(32,t->lexeme,_LINE_);
         return (FAIL);
         }
      else
         ; /* temp_ptr -> declared = 1; */
   else
      {
      /*************************************************************/
      /* OK We can now insert the symbol table entry for this name */
      /*************************************************************/

      temp_ptr = add_symbol (current_parent,
                            t->lexeme,
                            t->token,
                            t->keyword,
                            0,
                            0,
                            0,
                            0,
                            0,
                            1,
                            0,
                            NULL, 
                            line_no);
      }

   if (parse_attribute(temp_ptr) == 0)
      {       
      return(FAIL);
      }

   temp_ptr -> declared = 1;

   p = get_pos();

   t = next_token();

   if (t->token == COMMA)                /* HWG45 */
      goto step_along_commalist;

   set_pos(p);

   return (OK);

   }

/****************************************************************************/
/*            This function will parse a PLI 'if' statement.                */
/****************************************************************************/

PARSER

   parse_if (Any_ptr g_ptr)

   {

   If_ptr      i_ptr;
   long        p;

   Expr_ptr    e_ptr;
   
   i_ptr = g_ptr;
 
   t = next_token();

   e_ptr = parse_expression();

   if (e_ptr == NULL)
      {
      return (FAIL);
      }

   i_ptr->expression = e_ptr;

   p = get_pos();
   t = next_token();

   if (t->token!= THEN)
      {
      report(23,"",_LINE_);
      if ((t->token == DO) || (t->token == BEGIN))
         set_pos(p);
      /* return(FAIL);  */
      }

   i_ptr->action = THEN; /* When If node is examined, other parts of  */
                         /* the compiler can tell what part of the if */
                         /* statement is being currently parsed !     */ 

   if (parse_statement(i_ptr) == 0)   /* then_ptr  */
      return(FAIL);

   p = get_pos(); /* we can exit here if there is NO else clause */

   /********************************************************************/
   /* OK We have detected a valid IF expr THEN statement, lets see if  */
   /* there is an else clause.                                         */
   /********************************************************************/

   p = get_pos() ;

   if (this_isnt_a_keyword())  /* check uses of keywords */
      {
      set_pos(p);  /* an assignment */
      i_ptr->action = STMT;
      return(OK);
      }

   set_pos (p);  /* pretend we didnt look at the token ! */

   t = next_token();

   if (t->token != ELSE)
      {
      i_ptr->action = STMT; /* No else, so set this up ! */ 
      set_pos (p);          /* Restore position cos else isnt present */      
      return(OK);           /* return OK for the IF expr THEN         */
      }

   i_ptr->action = ELSE; /* see above */

   if (parse_statement(i_ptr) == 0)  /* else_ptr */
      return(FAIL);     /* we have valid IF expr THEN, but bad stmt */
                        /* following an ELSE                        */

   i_ptr->action = STMT; /* see above */

   return (OK);  /* A valid IF-THEN-ELSE */

   }

/***************************************************************************/
/*     This function determines wether we have a valid assignmnet          */
/***************************************************************************/


PARSER

   parse_assignment (Any_ptr g_ptr)

   {

   long           p;


   Assignment_ptr a_ptr;
   Expr_ptr       e_ptr;
   Ref_ptr        r_ptr;

   a_ptr = g_ptr;

   r_ptr = parse_reference();

   if (r_ptr == NULL)
      return(FAIL);
    
   a_ptr->target = r_ptr;

   t = next_token();


   if (t->token != EQUALS)
      {
      report(46,t->lexeme,_LINE_);
      return(FAIL);
      }

   e_ptr = parse_expression();

   if (e_ptr == NULL)
      {       
      return(FAIL);
      }

   a_ptr->source = e_ptr; 

   t = next_token();


   if ((t->token!= SEMICOLON) && (t->token != COMMA))
      {
      report(4,"",_LINE_);
      return(FAIL);
      }

   if (t->token == SEMICOLON)
      return(OK);

   if (t->token == COMMA)
      {
      p = get_pos();

      t = next_token();

      if (t->token != BY)
         {
         set_pos(p);
         report(138,"",_LINE_);
         return(FAIL);
         }

      p = get_pos();
      t = next_token();

      if (t->token != NAME)
         {
         report(138,"",_LINE_);
         set_pos(p);
         return(FAIL);
         }

      a_ptr->by_name = 1;

      t = next_token();

      if (t->token != SEMICOLON)
         {
         report(4,"",_LINE_);
         return(FAIL);
         }

      }

   return (OK);

   }

/***************************************************************************/
/*     This function determines wether we have a valid free statement      */
/***************************************************************************/

PARSER

   parse_free (Any_ptr g_ptr)

   {

   Ref_ptr     r_ptr;
   Free_ptr    f_ptr;

   f_ptr = g_ptr;
   
   t = next_token();


   if (t->token!= FREE)
      {       
      return(FAIL);
      }

   r_ptr = parse_reference();

   if (r_ptr == NULL)
      return(FAIL);
   
   f_ptr->target = r_ptr;

   t = next_token();

   if (t->token!= SEMICOLON)
      {
      report(4,"",_LINE_);
      return(FAIL);
      }

   return (OK);

   }

/***************************************************************************/
/*          This function parses a PL/1 allocate statement                 */
/***************************************************************************/

PARSER

   parse_allocate (Any_ptr g_ptr)

   {

   Ref_ptr      r_ptr;
   Allocate_ptr a_ptr;

   a_ptr = g_ptr;

   t = next_token();

   if (t->token!= ALLOCATE)
      {       
      return(FAIL);
      }

   r_ptr = parse_reference();

   if (r_ptr == NULL)
      return(FAIL);

   a_ptr->area = r_ptr; 

   t = next_token();

   if (t->token!= SET)
      {
      report(26,"",_LINE_);
      return(FAIL);
      }

   t = next_token();

   if (t->token!= LPAR)
      {
      report(8,"",_LINE_);
      return(FAIL);
      }

   r_ptr = parse_reference();

   if (r_ptr == NULL)
      return(FAIL);

   a_ptr->target = r_ptr;

   t = next_token();

   if (t->token!= RPAR)
      {
      report(10,"",_LINE_);
      return(FAIL);
      }

   t = next_token();

   if (t->token!= SEMICOLON)
      {
      report(4,"",_LINE_);
      return(FAIL);
      }

   return (OK);

   }
               
/***************************************************************************/
/*          This function detects a null PL/1 statement                    */
/***************************************************************************/
/*
short 

   parse_null (void)

   {

   long        p,r;
   long        ln;

   t = next_token();

   if (t->token!= SEMICOLON)
      {       
      return(FAIL);
      }

   return(OK);

   }
  */
/***************************************************************************/
/*      This function detects a valid PL/1 data type attribute             */
/***************************************************************************/


PARSER

   parse_attribute (Symbol_ptr sym_ptr)

   {

   long        p;
   short       state;
   short       comma_seen = 0;
   Ref_ptr     r_ptr;

   /**************************************************************/
   /*   Set the following symbol table values to a safe value    */
   /**************************************************************/

   sym_ptr -> prec_1  = 0;
   sym_ptr -> prec_2  = 0;

   if (sym_ptr->class != PARAMETER)
      sym_ptr -> offset  = 0;

   sym_ptr -> bytes   = 0;
   sym_ptr -> bad_dcl = 0;
   sym_ptr -> aligned = 0;

   strcpy (sym_ptr -> line,line_no);

   /*****************************************************************/
   /* We must see if next token is a (, if it is, then we have an   */
   /* array on our hands !                                          */
   /*****************************************************************/

   p = get_pos();

   t = next_token();

   set_pos(p);

   if (t->token == LPAR)
      if (parse_dimension(sym_ptr) == 0)
         {
         skip_to_next(SEMICOLON);
         return(OK);
         }

   /*****************************************************************/
   /* PL/1's powerful declaration facility is implemented as an FSM */
   /*****************************************************************/

   /********************************************************************/
   /* The algorithm used here, is to repeatedly read tokens.           */
   /* We tick off various attributes, classes, scopes etc one by one   */
   /* if a flag is already set, then that token has been duplicated.   */
   /* We jump from 'case' to 'case' as attribute tokens are recognised */
   /* checking for incompatible mixtures as we go. The order in which  */
   /* type-tokens are read, is not important, ie the following are OK  */
   /* 1:  dcl variable    char(10) var static ;                        */
   /* 2:  dcl variable    var char(10) static ;                        */
   /********************************************************************/  

   state = 1;
    
   t = next_token();

   while ((t->token != SEMICOLON) && (t->token != COMMA))
       {
       switch (state) {

       case(1):
           {
           switch (t->token) {
             case(BUILTIN):
                 if (builtin(sym_ptr->spelling))
                    sym_ptr->class = BUILTIN;
                 else
                    {
                    sym_ptr->bad_dcl = 1;
                    report(101,sym_ptr->spelling,_LINE_);
                    }
                 break; /* if any attribs follow, then dcl is invalid */
             case(CONDITION):
             case(ENTRY):
             case(POINTER):
                 { /* verify data type not already seen !  */
                   /* and name already declared */
                 if (((sym_ptr->type) != 0) &&
                     ((sym_ptr->declared) == 1))
                    {
                    /* error */
                    sym_ptr->bad_dcl = 1;
                    report(35,sym_ptr->spelling,_LINE_);
                    break;
                    }
                 sym_ptr->type = t->token;
                 sym_ptr->prec_1 = 8; /* two words ! */
                 sym_ptr->known_size = 1;
                 /*****************************************************/
                 /* Right, if we have just read ENTRY, then we must   */
                 /* call parse_entry_list, to verify any argument data */
                 /* types ie dcl subr entry (char(*) var,bin(15));    */
                 /*****************************************************/ 
                 if (t->token == ENTRY)                  
                    parse_entry_list(sym_ptr);     
                 break;
                 }
             case(RETURNS):
                 {
                 parse_returns(sym_ptr);
                 break;
                 }  
			 case(INITIAL):
			     {
				 if (sym_ptr->initial)
				    {
					sym_ptr->bad_dcl = 1;
					report(0,sym_ptr->spelling,_LINE_);
					break;
					}
				 parse_initial(sym_ptr);
				 sym_ptr->initial = 1;  /* should we set this only when parse was clean ? */
				 break;
				 }
             case(VARIABLE):
                 {
                 sym_ptr->variable = 1;
                 break;
                 }

             case(PICTURE):
                 { /* Verify data type not already seen ! */
                   /* and name already declared */
                 if (((sym_ptr->type) != 0) &&
                     ((sym_ptr->declared) == 1))
                    {
                    /* Error here */
                    sym_ptr->bad_dcl = 1;
                    report(35,sym_ptr->spelling,_LINE_);
                    break;
	               }
                 state = 8; /* 1st action when in state 1   ! */
               sym_ptr->type = t->token;
                 break;
               }

             case(CHARACTER):
             case(BIT):
                 { /* Verify data type not already seen ! */
                   /* and name already declared */
                 if (sym_ptr->scale != 0)
                    {
                    sym_ptr->bad_dcl = 1;
                    report (71,sym_ptr->spelling,_LINE_); /* this type invalid */
                    }  
                 if (((sym_ptr->type) != 0) &&
                     ((sym_ptr->declared) == 1))
                    {
                    /* Error here */
                    sym_ptr->bad_dcl = 1;
                    report(35,sym_ptr->spelling,_LINE_);
                    break;
	               }
                 state = 2; /* 1st action when in state 1   ! */
               sym_ptr->type = t->token;
                 break;
               }

             case(BINARY):
             case(DECIMAL):
                 { /* Verify data type not already seen ! */
                   /* and name already declared */
                 if ((sym_ptr->varying) != 0)
                    {
                    sym_ptr->bad_dcl = 1;
                    report(45,sym_ptr->spelling,_LINE_); /* only char can be var */
                    break;
                    } 
                 if (((sym_ptr->type) != 0) &&
                     ((sym_ptr->declared) == 1))
                    {
                    /* Error here */
                    sym_ptr->bad_dcl = 1;
                    report(35,sym_ptr->spelling,_LINE_);
                    break;
	               }
                 state = 2; /* 1st action when in state 1   ! */
               sym_ptr->type = t->token;
                 break;
               }
           case(VOLATILE):
               { /* Verify volatile not already seen ! */
                 if ((sym_ptr->class) == PARAMETER)
                    {
                    /* Error parameters cant have a declared class */
                    sym_ptr->bad_dcl = 1;
                    report(101,sym_ptr->spelling,_LINE_);
                    break;
                    }         
               if (sym_ptr->vola_tile)
	               {
	               /* Error here ! */
                    sym_ptr->bad_dcl = 1;
                    report(34,sym_ptr->spelling,_LINE_);
                    break;
	               }
               state = 1;  /* This is sufficient or now */
               sym_ptr->vola_tile = 1;
               break;
               }
          	  case(STATIC):
           case(AUTOMATIC):
               { /* Verify storage class not already seen ! */
                 if ((sym_ptr->class) == PARAMETER)
                    {
                    /* Error parameters cant have a declared class */
                    sym_ptr->bad_dcl = 1;
                    report(50,sym_ptr->spelling,_LINE_);
                    break;
                    }         
               if ((sym_ptr->class) != 0)
	               {
	               /* Error here ! */
                    sym_ptr->bad_dcl = 1;
                    report(34,sym_ptr->spelling,_LINE_);
                    break;
	               }
               state = 1;  /* This is sufficient or now */
               sym_ptr->class = t->token;
               break;
               }
             case(DEFINED):
             case(BASED):
                 { /* verify storage class not already seen */
                 if ((sym_ptr->class) == PARAMETER)
                    {
                    /* Error parameters cant have a declared class */
                    sym_ptr->bad_dcl = 1;
                    report(50,sym_ptr->spelling,_LINE_);
                    state = 5;
                    break;
                    }         
                 if ((sym_ptr->class) != 0)
                    { /* error */
                    sym_ptr->bad_dcl = 1;
                    report (34,sym_ptr->spelling,_LINE_);
                    state = 5;
                    break;
                    }
                 state = 5;
                 sym_ptr->class = t->token;
                 break;
                 }
             case(INTERNAL):
             case(EXTERNAL):
                 { /* Verify scope not already seen */
                 if ((sym_ptr->scope) != 0)
                    {
                    /* Error here ! */
                    sym_ptr->bad_dcl = 1;
                    report(36,sym_ptr->spelling,_LINE_);
                    }
                 state = 1;
                 sym_ptr->scope = t->token;
                 break;
                 }

             case(ALIGNED):
             case(_UNALIGNED):
                 { /* Verify alignment not already seen */
                 if ((sym_ptr->aligned) != 0)
                    {
                    /* Error here ! */
                    sym_ptr->bad_dcl = 1;
                    report(55,sym_ptr->spelling,_LINE_);
                    }
                 state = 1;
                 sym_ptr->aligned = t->token;
                 break;
                 }
             case(FILE_TYPE):
                 { /* Verify alignment not already seen */
                 if ((sym_ptr->file) != 0)
                    {
                    /* Error here ! */
                    sym_ptr->bad_dcl = 1;
                    report(55,sym_ptr->spelling,_LINE_);
                    }
                 state = 1;
                 sym_ptr->file = t->token;
                 sym_ptr->type = t->token;
                 break;
                 }

 
             case(FIXED):
             case(D_FLOAT):
                 { /* Verify scale not already seen */
                 if ((sym_ptr->scale) != 0)
                    {
                    /* Error here */
                    sym_ptr->bad_dcl = 1;
                    report(37,sym_ptr->spelling,_LINE_);
                    }
                 if (sym_ptr->type != 0)
                    if ((sym_ptr->type != DECIMAL) &&
                        (sym_ptr->type != BINARY))
                        {
                        sym_ptr->bad_dcl = 1;
                        report (70,sym_ptr->spelling,_LINE_);
                        }  
                 state = 1;
                 sym_ptr->scale = t->token;
                 break;
                 }
             case(VARYING):
                 { /* Verify varying not already seen */
                 if (((sym_ptr->type) != 0) && 
                     ((sym_ptr->type) != CHARACTER))
                    {
                    sym_ptr->bad_dcl = 1;
                    report(45,sym_ptr->spelling,_LINE_);
                    }
                         
                 if ((sym_ptr->varying) != 0)
                    {
                    /* Error here */
                    sym_ptr->bad_dcl = 1;
                    report(44,sym_ptr->spelling,_LINE_);
                    }
                 state = 1;
                 sym_ptr->varying = 1; /* 1 bit flag ! */
                 break;
                 }
           default:
               {
               /* error */
                 report(46,t->lexeme,_LINE_);
               }
           } /* The inner switch  */
           break;  /* The outer case(1) clause ! */
           } /* case(1) */
         case(2):
             {
             switch (t->token) {

             case(LPAR):
                 {
                 state = 3;
                 break;
                 }
             default :
                 {
                 /*****************************************************/
                 /* There is no size specification for this data type */
                 /*****************************************************/
                 report(46,t->lexeme,_LINE_);
                 }
             } /* inner switch */
             break; /* case(2) */
             } /* case(2) */
         case(3):
             {
             switch (t->token) {    

             case(STAR):
                 {
                 if ((sym_ptr->type != CHARACTER) ||
                     (sym_ptr->class != PARAMETER))
                    report(51,sym_ptr->spelling,_LINE_);  
                 else
                    sym_ptr->asterisk = 1;
                 }
                        
             case(PL1NAME):
                 {
                 if (t->token == PL1NAME)
                    if (get_symbol(current_parent,t->lexeme) == NULL)
                       {
                	   add_symbol (current_parent,
                                   t->lexeme,
                                   t->token,
                                   t->keyword,
                                   0,          /* Unknown length    */
                                   0,          /* Unknown scale     */
                                   0,          /* Unknown class     */
                                   0,          /* Unknown scope     */
                                   0,
                                   0,
                                   0,
                                   NULL,
                                   line_no);

                       }
                 state = 4;
                 break;
                 }
             case(NUMERIC):
                 {
                 /************************************************/
                 /* Store the numeric value in the precision_1   */
                 /************************************************/
                 sym_ptr->known_size = 1; /* specified size */
                 if (comma_seen)
                     sym_ptr->prec_2     = atoi (t->lexeme);
                 else
                     sym_ptr->prec_1     = atoi (t->lexeme);
                 /**************************************************/
                 /* See if the next token is a   ,   if so stay in */
                 /* this state !                                   */
                 /**************************************************/
                 p = get_pos();
                 
                 t = next_token();

                 if (t->token != COMMA)
                    {
                    set_pos(p);
                    state = 4;
                    }
                 comma_seen = 1;
                 break;
                 }
             default:
                 {
                 report (9,"",_LINE_);
                 }
             } /* inner switch */ 
             break; /* case(3) */
             } /* case(3) */
         case(4):
             {
             switch (t->token) {
      
             case(RPAR):
                 { /* Good the data type is Fully specified */
                 state = 1;
                 break;   
                 }
             default:
                 {
                 report(46,t->lexeme,_LINE_);
                 /* error */
                 }
             } /* inner switch */
             break; /* case(4) */
             } /*   case(4) */
         case(5): /* defined syntax verivication */
             {
             switch (t->token) {
             case(LPAR):
                 {
                 r_ptr = parse_reference();
                 if (r_ptr == NULL)
                    break;  /* ? */
                 sym_ptr->defbas_ptr = r_ptr;
                 state = 7;
                 break;
                 }
             default :
                 {      /* based doesnt insist on: based(ref)  */
                 if (sym_ptr->class == BASED)
                    {
                    state = 1;
                    break;
                    }         
                 /*****************************************************/
                 /* There is syntax error in defined                  */
                 /*****************************************************/
                 report(46,t->lexeme,_LINE_);
                 }
             } /* inner switch */
             break; /* case 5 */
             } /* case 5 */
        
         case(7):
             {
             switch (t->token) {
      
             case(RPAR):
                 { /* Good tified */
                 state = 1;
                 break;   
                 }
             default:
                 {
                 report(46,t->lexeme,_LINE_);
                 /* error */
                 }
             } /* inner switch */
             break; /* case(7) */
             } /*   case(7) */

        case(8):
            {
            if (t->token != STRING)
               {
               report(46,t->lexeme,_LINE_);
               break;
               }
            else
               sym_ptr->pic_text = t->lexeme;
            }


        default:
             {
             }
         } /* outer switch */

         p = get_pos();

         t = next_token();

         }  /* while loop */


    if (t->token == COMMA)   /* HWG45 */
       set_pos(p);
    
    return(OK);

   }

/***************************************************************************/
/*    This function parses an entry declarations data-type commalist.      */
/***************************************************************************/

PARSER

   parse_entry_list (Symbol_ptr s_ptr)

   {

   Data_ptr          d_ptr; 
   long              p;

   p = get_pos();

   t = next_token();

   if ((t->token == SEMICOLON) || (t->token == VARIABLE))
      {
      set_pos(p);
      return(OK);
      }


   set_pos(p);

   t = next_token();

   if (t->token != LPAR)
      {
      report(46,t->lexeme,_LINE_);
      return(FAIL);
      }

   s_ptr->array_ptr = allocate_node(DATA);
   d_ptr = s_ptr->array_ptr;
   d_ptr->parent = s_ptr;

   t->token = COMMA; /* force initial entry to loop */

   while (t->token == COMMA)
         {
         if (parse_descriptor(d_ptr) != FAIL)
            {
            d_ptr->next_ptr = allocate_node(DATA);
            d_ptr = d_ptr->next_ptr;
            d_ptr->parent = s_ptr;
            t = next_token();
            }
         s_ptr->num_dims++;  
         }

   t = next_token();

   if (t->token != RPAR)
      {
      report(46,t->lexeme,_LINE_);
      return(FAIL);
      }
    
   return(OK);
   
   }

PARSER

/***************************************************************************/
/* Parse the RETURNS option for a declaration of an external ENTRY.        */
/***************************************************************************/

   parse_returns (Symbol_ptr s_ptr)

   {

   if (t->token == RETURNS)
      {
      t = next_token();
      if (t->token != LPAR)
         {
         report(105,"",_LINE_);
         skip_to_next(SEMICOLON);
         return(OK);
         }

      s_ptr->ret_ptr = allocate_node(DATA);
      parse_descriptor(s_ptr->ret_ptr);
      t = next_token();

      if (t->token != RPAR)
         {
         report(105,"",_LINE_);
         skip_to_next(SEMICOLON);
         return(OK);
         }
      }

   return(OK);

   }   

/***************************************************************************/
/*    This function parses an 'initial' specification.                     */
/***************************************************************************/

PARSER

   parse_initial (Symbol_ptr s_ptr)

   {

   long              p;
   Init_ptr          ip;
    
   /******************************************************************************/
   /* The INITIAL spec has a syntax (see ANSI) as follows:                       */
   /* <initial> ::= INITIAL [(<initial-element-commalist> | <array-init>)]	     */
   /* this means that the keyword INITIAL may be followed by nothing !, I will   */
   /* allow this, but I dont yet know why this is allowed ? 					 */
   /******************************************************************************/

   /* if next token is LPAR, then proceed, otherwise return */

   if (next_token()->token != LPAR)
      {
	  s_ptr->initial = 1;  /* ie INITIAL with no other options specified (ie dcl a bin(15) init; */
	  return(OK);
	  }

   ip = allocate_node(INITIAL);

   p = get_pos();

   if ((next_token()->token == LPAR) && (next_token()->token == STAR) && (next_token()->token == RPAR))
      {
      /*****************************************************************************/
      /* OK weve recognised the construct: (*)  so lets backup and parse this term */
      /*****************************************************************************/   
      set_pos(p);
	  if (parse_array_init(ip) == 0)
	     report(NOT_YET_SET,"",_LINE_);
	  }
   else
      {
      /**************************************************/
      /* OK We must now parse the alternative construct */
      /**************************************************/	   
      set_pos(p);
	  ip->ie_ptr = allocate_node(IELEM);
      parse_initial_element_commalist(ip->ie_ptr);
   	  }

   s_ptr->init_ptr = ip;

   t = next_token();

   if (t->token != RPAR)
      {
	  report(46,t->lexeme,_LINE_);
	  return(FAIL);
	  }

   return(OK);

   }

/************************************************************************************************/
/* This function parses the <array-init> non-terminal, part of the INITIAL option.				*/
/************************************************************************************************/

PARSER

parse_array_init (Init_ptr ip)

{

Long			  p;
Token_ptr         t2;

p = get_pos();

if ((next_token()->token != LPAR) || (next_token()->token != STAR) || (next_token()->token != RPAR))
   {
   report(INTERNAL_ERROR,"",_LINE_);
   return(FAIL);
   }

ip->ia_ptr = allocate_node(IARRAY);

/**********************************************************************************/
/* What follows must be either <initial-constant-two> or (<initial-constant-one>) */
/* or (<expression>)             												  */
/**********************************************************************************/

p = get_pos();
t =next_token();

if (t->token != LPAR)
   {
   /*********************************************************/
   /* This must be <initial-constant-two>				    */
   /*********************************************************/
   set_pos(p);
   ip->ia_ptr->ic2_ptr = allocate_node(ICON2);
   if (parse_initial_constant_two(ip->ia_ptr->ic2_ptr) == 0)
      {
      report(NOT_YET_SET,"",_LINE_);
      return(FAIL);
      }
   return(OK);    
   }
else
   {
   /**********************************************************/
   /* in this case we must have either of the following:     */
   /* (<initial-constant-one>) or (<expression>)             */
   /* we attempt to recognise the former first.				 */
   /* Note that weve just read an LPAR in order to be here.  */
   /**********************************************************/
   p = get_pos();

   t  = next_token();
   t2 = next_token();

 
   if (((t->token == PLUS) || (t->token == MINUS) || (t->token == NOT)) && (t2->token == STRING) )
      {
      ip->ia_ptr->ic1_ptr              = allocate_node(ICON1);
      ip->ia_ptr->ic1_ptr->preop       = t->token;
	  ip->ia_ptr->ic1_ptr->str_con_ptr = t2->lexeme;
	  
	  if (next_token()->token != RPAR)
	     {
		 report(NOT_YET_SET,t->lexeme,_LINE_);
		 return(FAIL);
		 }
	  return(OK);
	  }
   
   set_pos(p);

   if (next_token()->token == STRING)
      {
      if (ip->ia_ptr->ic1_ptr == NULL)
         allocate_node(ICON1);

      ip->ia_ptr->ic1_ptr->str_con_ptr = t->lexeme;
	  if (next_token()->token != RPAR)
	     {
		 report(NOT_YET_SET,t->lexeme,_LINE_);
		 return(FAIL);
		 }
	  return(OK);
	  }

   /* we didnt find [+|-|^] <simple-string-constant>, so lets try <initial-constant-two> */

   set_pos(p);
  
   ip->ia_ptr->ic2_ptr = allocate_node(ICON2);

   if (parse_initial_constant_two(ip->ia_ptr->ic2_ptr))
      if (next_token()->token != RPAR)
         {
         report(NOT_YET_SET,"",_LINE_);
         return(FAIL);
         }
	  else
         return(OK);
   else
      {
	  ip->ia_ptr->ic2_ptr = free_node(ip->ia_ptr->ic2_ptr); /* free this up */
	  set_pos(p);
      /* OK lets finally try (<expression>), the only possibility left */
	  ip->ia_ptr->expr_ptr = parse_expression();

	  if (ip->ia_ptr->expr_ptr != NULL)
	     if (next_token()->token == RPAR)
		    return(OK);

      }
      
      
          
   }

return(FAIL);

}

/**************************************************************************/
/* This function parses the <initial-constant-two> part of an INITIAL     */
/**************************************************************************/

PARSER

parse_initial_constant_two (Icon2_ptr ic2p)
      
{

Long						p;

p = get_pos();

t = next_token();

if ((t->token == PLUS) || (t->token == MINUS) || (t->token == NOT))
   {
   ic2p->preop = t->token;
   }
else
   set_pos(p);

/* OK we must now find a numeric constant, else a syntax error */

t = next_token();

if (t->token != NUMERIC)
   return(FAIL);

ic2p->num_con_ptr = t->lexeme;

return(OK);

}

/*********************************************************/
/* Parses the <initial-element-commalist> non-terminal.  */
/*********************************************************/

PARSER

parse_initial_element_commalist (Ielem_ptr ep)

{

Long						p,l;
Ielem_ptr				    iep;
Any_ptr						xp;
short						comma_seen;
Token_ptr					t2;

iep = ep;

comma_seen = 1; /* force loop entry */

p = get_pos();
t = next_token();

while (comma_seen)
      {
	  if (t->token == STAR)
	     {
		 ep->asterisk = 1;
		 goto step_forward;
		 }
 
	  if (t->token != LPAR) /* <initial-constant-one> ? */
	     {
        /* p = get_pos(); 	  */

        /* t  = next_token();	*/
         t2 = next_token();
 
         if (((t->token == PLUS) || (t->token == MINUS) || (t->token == NOT)) && (t2->token == STRING) )
            {
            iep->ic1_ptr              = allocate_node(ICON1);
            iep->ic1_ptr->preop       = t->token;
	        iep->ic1_ptr->str_con_ptr = t2->lexeme;
	  
	        if (next_token()->token != RPAR)
	           {
		       report(NOT_YET_SET,t->lexeme,_LINE_);
		       return(FAIL);
		       }
            goto step_forward;
	        }
   
         set_pos(p);

         if (next_token()->token == STRING)
            {
            if (iep->ic1_ptr == NULL)
               allocate_node(ICON1);

            iep->ic1_ptr->str_con_ptr = t->lexeme;

	        if (next_token()->token != RPAR)
	           {
		       report(NOT_YET_SET,t->lexeme,_LINE_);
		       return(FAIL);
		       }
	        goto step_forward;
	        }
		   /* we didnt find [+|-|^] <simple-string-constant>, so lets try <initial-constant-two> */

         set_pos(p);
  
         iep->ic2_ptr = allocate_node(ICON2);

         if (parse_initial_constant_two(iep->ic2_ptr) == 0)
           /* if (next_token()->token != RPAR)	 */
               {
               report(NOT_YET_SET,"",_LINE_);
               return(FAIL);
               }
	        else  
			   {
			/*   p = get_pos();	  */
               goto step_forward;
			   }

	     report(46,t->lexeme,_LINE_);
	     return(FAIL);
	     }
	     
	  /**********************************************************************************/
	  /* OK weve just read an LPAR, so lets examine the two possibilities               */
	  /* at this stage we can ONLY have the following:					                */
	  /* (<expression>) OR 														        */
	  /* (<expression>) { <initial-constant-two> | * | (<initial-element-commalist>) }  */
	  /* If simply the former, then the next token would be either COMMA or RPAR so we  */
	  /* lookahead (after parsing the expression) to help us decide.					*/
	  /**********************************************************************************/    

	  xp = parse_expression();

	  if (xp == NULL) /* syntax error */
	     {
		 return(FAIL);
		 }

	  t = next_token();

	  if (t->token == COMMA)
	     {
		 xp = free_node(xp);  /* we dont need this expression tree */
		 set_pos(l);
		 t = next_token();
		 goto this_looks_like_an_initial_element_commalist;
		 }

	  if (t->token != RPAR)
	     {
		 report(46,t->lexeme,_LINE_);
		 return(FAIL);
		 }

	  p = get_pos(); /* were gonna lookahead */

	  t = next_token();

	  if ((t->token == COMMA) || (t->token == RPAR))
	 	 {
		 /***************************************************/
		 /* OK its the former (see above) type of construct */
		 /***************************************************/
		 set_pos(p);
		 iep->expr_ptr = xp;
		 goto step_forward;
		 }

	  /********************************************************************************/
	  /* We now know that we must have an example of the latter (see above) construct */
	  /* of course a syntax error may exist, but this is always a possibility.        */
	  /********************************************************************************/

	  set_pos(p);

	  t = next_token();

	  if (t->token == STAR)
	     {
		 iep->asterisk = 1;
		 goto step_forward;
		 }

this_looks_like_an_initial_element_commalist:

	 /* p = get_pos();	  */

	  if (t->token == LPAR)	/* legal, we have a nested <initial-element-commalist> */
	     {
		 iep->ie_ptr = allocate_node(IELEM);
		 parse_initial_element_commalist (iep->ie_ptr);
		 t = next_token();

         if (t->token != RPAR)
            {
	        report(46,t->lexeme,_LINE_);
	        return(FAIL);
	        }

		 goto step_forward;
		 }

	  set_pos(p);

	  iep->ic2_ptr = allocate_node(ICON2);

	  if (parse_initial_constant_two(iep->ic2_ptr) == 0)
	     {
		 iep->ic2_ptr = free_node(iep->ic2_ptr);
	     return(FAIL);
		 }

step_forward:

      /**************************************************/
	  /* we should now encounter either a ',' or a ')'  */
	  /**************************************************/
	  p = get_pos();
      t = next_token();

	  if (t->token != COMMA) /* should be a ')', but we will check after leaving the function */
	     {
		 set_pos(p);
	     comma_seen = 0;
		 }
	  else
	     {
	     iep->next_ptr = allocate_node(IELEM); /* we need a new node for the next element */
		 iep = iep->next_ptr;
		 p = get_pos();
		 l = get_pos();
		 t = next_token();
		 }

	  }	/* while */


return(OK);

}


/******************************************************************************/
/* This function parses a PL/I repetition factor, and returns its value if it */
/* is valid.																  */
/******************************************************************************/
	
PARSER

   parse_repfactor (Long_ptr lp)
   
   {
   
   *lp = 0;

   t = next_token();

   if (t->token != LPAR)
      {
	  report(INTERNAL_ERROR,t->lexeme,_LINE_);
	  return(FAIL);
	  }

   t = next_token();

   if (t->token != NUMERIC)
      {
	  report(152,t->lexeme,_LINE_);
	  if (t->token == RPAR)
	     return(FAIL);
	  skip_to_next(RPAR);
	  return(FAIL);
	  }

   *lp = atol(t->lexeme);

   if (*lp < 1)
      {
	  *lp = 0;
	  report(153,t->lexeme,_LINE_);
	  }

   t = next_token();

   if (t->token != RPAR)
      {
	  report(46,t->lexeme,_LINE_);
	  *lp = 0;
	  return(FAIL);
	  }

   return(OK);

   }
   
   		   
/**************************************************************************/
/*  This function parses an entry declarations data-type specifiers.      */
/**************************************************************************/

PARSER

   parse_descriptor (Data_ptr d_ptr)

   {

   long        p;
   short       state;
   short         comma_seen = 0;

   /*****************************************************************/
   /* PL/1's powerful declaration facility is implemented as an FSM */
   /*****************************************************************/

   state = 1;
  
   p = get_pos();
 
   t = next_token();

   while ((t->token) != COMMA)
       {
       switch (state) {
       case(1):
           {
           switch (t->token) {
             case(ENTRY):
             case(POINTER):
                 { /* verify data type not already seen !  */
                 if ((d_ptr->data_type) != 0)
                    {
                    /* error */
                    d_ptr->bad_dcl = 1;
                    report(35,d_ptr->parent->spelling,_LINE_);
                    break;
                    }
                 d_ptr->data_type = t->token;
                 d_ptr->prec_1 = 8; /* two words ! */
                 break;
                 }  

             case(CHARACTER):
             case(BINARY):
             case(BIT):
             case(DECIMAL):
                 { /* Verify data type not already seen ! */
                 if (((d_ptr->varying) != 0) &&
                     ((t->token) == CHARACTER))
                    {
                    d_ptr->bad_dcl = 1;
                    report(45,d_ptr->parent->spelling,_LINE_);
                    break;
                    } 
                 if ((d_ptr->data_type) != 0)
                    {
                    /* Error here */
                    d_ptr->bad_dcl = 1;
                    report(35,d_ptr->parent->spelling,_LINE_);
                    break;
	               }
                 state = 2; /* 1st action when in state 1   ! */
               d_ptr->data_type = t->token;
                 break;
               }
             case(FIXED):
             case(D_FLOAT):
                 { /* Verify scale not already seen */
                 if ((d_ptr->scale) != 0)
                    {
                    /* Error here */
                    d_ptr->bad_dcl = 1;
                    report(37,d_ptr->parent->spelling,_LINE_);
                    }
                 state = 1;
                 d_ptr->scale = t->token;
                 break;
                 }
             case(VARYING):
                 { /* Verify varying not already seen */
                 if (((d_ptr->data_type) != 0) && 
                     ((d_ptr->data_type) != CHARACTER))
                    {
                    d_ptr->bad_dcl = 1;
                    report(45,d_ptr->parent->spelling,_LINE_);
                    }
                         
                 if ((d_ptr->varying) != 0)
                    {
                    /* Error here */
                    d_ptr->bad_dcl = 1;
                    report(44,d_ptr->parent->spelling,_LINE_);
                    }
                 state = 1;
                 d_ptr->varying = 1; /* used to be:  t->token; */
                 break;
                 }
             case(RPAR):
                 {
                 set_pos(p);
                 return(FAIL); /* the closing ) */
                 }
           default:
               {
               /* error */
                 report(46,t->lexeme,_LINE_);
               }
           } /* The inner switch  */
           break;  /* The outer case(1) clause ! */
           } /* case(1) */
         case(2):
             {
             switch (t->token) {

             case(LPAR):
                 {
                 state = 3;
                 break;
                 }
             default :
                 {
                 report(46,t->lexeme,_LINE_);
                 /*****************************************************/
                 /* There is no size specification for this data type */
                 /*****************************************************/
                 }
             } /* inner switch */
             break; /* case(2) */
             } /* case(2) */
         case(3):
             {
             switch (t->token) {    

             case(STAR):
                 {
                 d_ptr->asterisk = 1;
                 state = 4;
                 break;
                 }  
             case(PL1NAME): /* this needs some work */
                 {
                 if (get_symbol(current_parent,t->lexeme) == NULL)
                    {
		               add_symbol (current_parent,
                                t->lexeme,
                                t->token,
                                t->keyword,
                                0,          /* Unknown length    */
                                0,          /* Unknown scale     */
                                0,          /* Unknown class     */
                                0,          /* Unknown scope     */
                                0,
                                0,
                                0,
                                NULL,
                                line_no);
                    }

                 d_ptr->prec_1 = 0;
                 state = 4;
                 break;
                 }
             case(NUMERIC):
                 {
                 /************************************************/
                 /* convert the numeric string to a size value   */
                 /************************************************/
                 if (comma_seen)
                    d_ptr->prec_2     = atoi (t->lexeme);
                 else
                    d_ptr->prec_1     = atoi (t->lexeme); 
                 /**************************************************/
                 /* See if the next token is a   ,   if so stay in */
                 /* this state !                                   */
                 /**************************************************/
                 p = get_pos();
                 t = next_token();
                 if (t->token != COMMA)
                    {
                    set_pos(p);
                    state = 4;
                    }
                 comma_seen = 1;
                 break;
                 }
             default:
                 {
                 report (9,"",_LINE_);
                 }
             } /* inner switch */ 
             break; /* case(3) */
             } /* case(3) */
         case(4):
             {
             switch (t->token) {
      
             case(RPAR):
                 { /* Good the data type is Fully specified */
                 state = 1;
                 break;   
                 }
             default:
                 {
                 report(46,t->lexeme,_LINE_);
                 /* error */
                 }
             } /* inner switch */
             break; /* case(4) */
             } /*   case(4) */
        case(6):
             {
             switch (t->token) {    

             case(PL1NAME):
                 {
                 if (t->token == PL1NAME)
                    if (get_symbol(current_parent,t->lexeme) == NULL)
                       {
	                  add_symbol (current_parent,
                                  t->lexeme,
                                  t->token,
                                  t->keyword,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  NULL,
                                  line_no);
                       }
                 d_ptr->prec_1 = 0;
                 state = 7;
                 break;
                 }
             default:
                 {
                 report(46,t->lexeme,_LINE_);
                 } /* syntax error in defined */ 
             } /* inner switch */
             break; /* case (6) */
             } /* case(6) */
         
         case(7):
             {
             switch (t->token) {
      
             case(RPAR):
                 { /* Good tified */
                 state = 1;
                 break;   
                 }
             default:
                 {
                 report(46,t->lexeme,_LINE_);
                 /* error */
                 }
             } /* inner switch */
             break; /* case(7) */
             } /*   case(7) */


        default:
             {
             }
         } /* outer switch */

         p = get_pos();

         t = next_token();

         }  /* while loop */

   /****************************************************************/
   /* At the end of this loop we must reset our lexical position   */
   /* so that the structure parser can re-read the comma that      */
   /* caused this loop to end.Recall that parse_entry_list expects */
   /* to find this token !                                         */
   /****************************************************************/

   set_pos (p); 

   return(OK);

   }  /* parse_descriptor  */

/***************************************************************************/
/* This function parses those attributes that are associated with level 1  */
/* name declarations, ie based, static, aligned etc.                       */
/***************************************************************************/


PARSER

   parse_structure_attribute (Symbol_ptr sym_ptr)

   {

   long        p;
   short       state;
   Ref_ptr     r_ptr;

   /**************************************************************/
   /*   Set the following symbol table values to a safe value    */
   /**************************************************************/

   sym_ptr -> prec_1  = 0;
   sym_ptr -> prec_2  = 0;
   sym_ptr -> offset  = 0;
   sym_ptr -> bytes   = 0;
   sym_ptr -> bad_dcl = 0;
   sym_ptr -> aligned = 0;
  
   strcpy (sym_ptr -> line,line_no);

   state = 1;
   
   p = get_pos();
   t = next_token();

   while ((t->token) != COMMA)
       {
       switch (state) {

       case(1):
           {
           switch (t->token) {
           case(VOLATILE):
               { /* Verify volatile not already seen ! */
                 if ((sym_ptr->class) == PARAMETER)
                    {
                    /* Error parameters cant have a declared class */
                    sym_ptr->bad_dcl = 1;
                    report(101,sym_ptr->spelling,_LINE_);
                    break;
                    }         
               if (sym_ptr->vola_tile)
	               {
	               /* Error here ! */
                    sym_ptr->bad_dcl = 1;
                    report(34,sym_ptr->spelling,_LINE_);
                    break;
	               }
               state = 1;  /* This is sufficient or now */
               sym_ptr->vola_tile = 1;
               break;
               }
         	  case(STATIC):
           case(AUTOMATIC):
               { /* Verify storage class not already seen ! */
                 if ((sym_ptr->class) == PARAMETER)
                    {
                    /* Error parameters cant have a declared class */
                    sym_ptr->bad_dcl = 1;
                    report(50,sym_ptr->spelling,_LINE_);
                    break;
                    }         
               if ((sym_ptr->class) != 0)
	               {
	               /* Error here ! */
                    sym_ptr->bad_dcl = 1;
                    report(34,sym_ptr->spelling,_LINE_);
                    break;
	               }
               state = 1;  /* This is sufficient or now */
               sym_ptr->class = t->token;
               break;
               }
             case(BASED):
             case(DEFINED):
                 { /* verify storage class not already seen */
                 if ((sym_ptr->class) == PARAMETER)
                    {
                    /* Error parameters cant have a declared class */
                    sym_ptr->bad_dcl = 1;
                    report(50,sym_ptr->spelling,_LINE_);
                    state = 5;
                    break;
                    }         
                 if ((sym_ptr->class) != 0)
                    { /* error */
                    sym_ptr->bad_dcl = 1;
                    report (34,sym_ptr->spelling,_LINE_);
                    state = 5;
                    break;
                    }
                 state = 5;
                 sym_ptr->class = t->token;
                 break;
                 }
             case(INTERNAL):
             case(EXTERNAL):
                 { /* Verify scope not already seen */
                 if ((sym_ptr->scope) != 0)
                    {
                    /* Error here ! */
                    sym_ptr->bad_dcl = 1;
                    report(36,sym_ptr->spelling,_LINE_);
                    }
                 state = 1;
                 sym_ptr->scope = t->token;
                 break;
                 }

             case(ALIGNED):
             case(_UNALIGNED):
                 { /* Verify alignment not already seen */
                 if ((sym_ptr->aligned) != 0)
                    {
                    /* Error here ! */
                    sym_ptr->bad_dcl = 1;
                    report(55,sym_ptr->spelling,_LINE_);
                    }
                 state = 1;
                 sym_ptr->aligned = t->token;
                 break;
                 }

             default:
               {
               /* error */
                 report(46,t->lexeme,_LINE_);
               }
           } /* The inner switch  */
           break;  /* The outer case(1) clause ! */
           } /* case(1) */
         case(5): /* defined syntax verivication */
             {
             switch (t->token) {
             case(LPAR):
                 {
                 r_ptr = parse_reference();
                 if (r_ptr == NULL)
                    break;
                 sym_ptr->defbas_ptr = r_ptr;
                 state = 7;
                 break;
                 }
             default :
                 {
                 if (sym_ptr->class == BASED)
                    { /* based doesnt insist on: based(ref) */
                    state = 1;
                    break;
                    }
                 /*****************************************************/
                 /* There is syntax error in defined                  */
                 /*****************************************************/
                 report(46,t->lexeme,_LINE_);
                 }
             } /* inner switch */
             break; /* case 5 */
             } /* case 5 */
        
         case(7):
             {
             switch (t->token) {
      
             case(RPAR):
                 { /* Good tified */
                 state = 1;
                 break;   
                 }
             default:
                 {
                 report(46,t->lexeme,_LINE_);
                 /* error */
                 }
             } /* inner switch */
             break; /* case(7) */
             } /*   case(7) */


         default:
             {
             }
         } /* outer switch */

         p = get_pos();
         t = next_token();

         }  /* while loop */

    set_pos(p);

    return(OK);

   }

/***************************************************************************/
/* This function parses an array dimensioning specification.               */
/* As each dimension is recognised, a 'Dim' node is strung onto the symbol */
/***************************************************************************/

PARSER

   parse_dimension (Symbol_ptr s_ptr)

   {

   long             p;
   Dim_ptr          d_ptr;
   Any_ptr      temp;

   t = next_token();

   if (t->token != LPAR)
      {
      report(30,"",_LINE_);
      return(FAIL);
      }
   
   while (t->token != RPAR)
         { 
         temp = parse_expression();
         if (temp == NULL)
            {
            s_ptr->array_ptr = NULL;   /* cleanup a bit */
            return(FAIL);
            }

       s_ptr->num_dims++;

         if (s_ptr->array_ptr == NULL)
            {
            s_ptr->array_ptr = allocate_node (ARRAY);
            d_ptr = s_ptr->array_ptr;
            }
         else
            {
            d_ptr->next_ptr = allocate_node (ARRAY);
            d_ptr = d_ptr->next_ptr;
            }
         

         /*****************************************************/
         /* If the next token is a COLON, then the expression */
         /* derived above is for the lower bound, otherwise   */
         /* it is for the upper.                              */
         /*****************************************************/

         p = get_pos();

         t = next_token();

         switch (t->token) {

         case(COLON):
             {
             d_ptr->lower = temp;
             d_ptr->upper = parse_expression();
             if (d_ptr->upper == NULL)
                {
                s_ptr->array_ptr = NULL;
                return(FAIL);
                }
             p = get_pos();
             t = next_token();
             if ((t->token != COMMA) && (t->token != RPAR))
                {
                s_ptr->array_ptr = NULL;
                report(46,t->lexeme,_LINE_);
                return(FAIL);
                }
             set_pos(p);
             break;
             }
         case(COMMA):
             {
             d_ptr->upper = temp;
             set_pos(p);           /* reset, so that next read  */
             break;                /* gets this COMMA, or parse */
             }                     /* expression at top of loop */
         case(RPAR):               /* will fail.                */
             {
             d_ptr->upper = temp;
             set_pos(p);
             break;
             }
         default:
             {
             report(46,t->lexeme,_LINE_);
             return(FAIL);
             }
         } 
         
         t = next_token();

         }  /* end loop */

   return (OK);

   }     

/***************************************************************************/
/*             This function parses a PL/1 'call' statement.               */
/***************************************************************************/

PARSER

   parse_call (Any_ptr g_ptr)

   {

   Call_ptr    c_ptr;
   Ref_ptr     r_ptr;

   c_ptr = g_ptr;

   t = next_token();

   if (t->token!= CALL)
      return(FAIL);
   
   r_ptr = parse_reference();

   if (r_ptr == NULL)
      return(FAIL);
    
   c_ptr->entry = r_ptr;

   t = next_token();

   if (t->token != SEMICOLON)
      {       
      report(46,t->lexeme,_LINE_);
      return(FAIL);
      }

   return(OK);

   }

/***************************************************************************/
/*             This function parses a PL/1 'on' statement.                 */
/***************************************************************************/

PARSER

   parse_on (Any_ptr g_ptr)

   {

   On_ptr      o_ptr;

   o_ptr = g_ptr;

   t = next_token();

   if (t->token!= ON)
      return(FAIL);

   if (parse_condition(o_ptr) == 0)
      return(FAIL);

   if (parse_statement(o_ptr) == 0)
      return(FAIL);
  
   return(OK);

   }

/***************************************************************************/
/* This function parses a PL/1 condition name, as it appears in an on stmt */
/***************************************************************************/

PARSER

   parse_condition (On_ptr o_ptr)

   {

   short              c_type;

   t = next_token();

   /********************************************************************/
   /* According to ANSI the conditions allowed here are either:        */
   /* computational, named-io, programmer, or AREA, ERROR, FINISH or   */
   /* STORAGE. The latter four we refer to as OTHER_CONDITION's        */
   /********************************************************************/     

   c_type = condition_type (t->token);
  
   if (c_type == 0)
      {
      report(46,t->lexeme,_LINE_);
      return (FAIL);
      }

   if ((c_type == COMP_CONDITION) ||
       (c_type == OTHER_CONDITION))
       {
       o_ptr->cond_type = t->token;
       return(OK);
       }

   /********************************************************************/
   /* Both of these conditions must have '(' as the next token.        */
   /********************************************************************/

   t = next_token();

   if (t->token != LPAR)
      {
      report(46,t->lexeme,_LINE_);
      return(FAIL);
      }

   /********************************************************************/
   /* We have either an IO condition or user condition so . . .        */
   /********************************************************************/

   o_ptr->ref_ptr = parse_reference();
   o_ptr->cond_type = c_type;

   if (o_ptr->ref_ptr == NULL)
      return(FAIL);

   t = next_token();

   if (t->token != RPAR)
      {
      report(46,t->lexeme,_LINE_);
      return(FAIL);
      }

   return(OK);

   }

/***************************************************************************/
/*             This function parses a PL/1 'goto' statement.               */
/***************************************************************************/

PARSER

   parse_goto (Any_ptr G_ptr)

   {

   Ref_ptr     r_ptr;
   Goto_ptr    g_ptr;

   g_ptr = G_ptr;

   t = next_token();


   if (t->token!= GOTO)
      return(FAIL);

   r_ptr = parse_reference();

   if (r_ptr == NULL)
      return(FAIL);

   g_ptr->target = r_ptr; 

   t = next_token();

   if (t->token!= SEMICOLON)
      return(FAIL);

   return(OK);

   }

/***************************************************************************/
/*         This function parses the alternative 'goto' statement.          */
/***************************************************************************/

PARSER

   parse_go (Any_ptr G_ptr)

   {

   Ref_ptr     r_ptr;
   Goto_ptr    g_ptr;

   g_ptr = G_ptr;

   t = next_token();


   if (t->token!= GO)
      return(FAIL);

   t = next_token();

   if (t->token != TO)
      {
      report(12,"",_LINE_); /* unrecognised stmt */
      return(FAIL);
      } 

   r_ptr = parse_reference();

   if (r_ptr == NULL)
      return(FAIL);

   g_ptr->target = r_ptr; 

   t = next_token();

   if (t->token!= SEMICOLON)
      return(FAIL);

   return(OK);

   }

/***************************************************************************/
/*          This function parses a PL/1 'signal' statement.                */
/***************************************************************************/

PARSER

   parse_signal (Any_ptr S_ptr)

   {

   Signal_ptr  s_ptr;
   short         c_type;

   s_ptr = S_ptr;

   t = next_token();

   if (t->token != SIGNAL)
      {
      report(46,t->lexeme,_LINE_);
      return(FAIL);
      }

   t = next_token();

   /********************************************************************/
   /* According to ANSI the conditions allowed here are either:        */
   /* computational, named-io, programmer, or AREA, ERROR, FINISH or   */
   /* STORAGE. The latter four we refer to as OTHER_CONDITION's        */
   /********************************************************************/     

   c_type = condition_type (t->token);
  
   if (c_type == 0)
      {
      report(46,t->lexeme,_LINE_);
      return (FAIL);
      }

   if ((c_type == COMP_CONDITION) ||
       (c_type == OTHER_CONDITION))
       {
       s_ptr->cond_type = t->token;
       return(OK);
       }

   /********************************************************************/
   /* Both of these conditions must have '(' as the next token.        */
   /********************************************************************/

   t = next_token();

   if (t->token != LPAR)
      {
      report(46,t->lexeme,_LINE_);
      return(FAIL);
      }

   /********************************************************************/
   /* We have either an IO condition or user condition so . . .        */
   /********************************************************************/

   s_ptr->ref_ptr = parse_reference();
   s_ptr->cond_type = c_type;

   if (s_ptr->ref_ptr == NULL)
      return(FAIL);

   t = next_token();

   if (t->token != RPAR)
      {
      report(46,t->lexeme,_LINE_);
      return(FAIL);
      }

   return(OK);

   }

/***************************************************************************/
/*            This function parses a PL/1 'leave' statement.               */
/***************************************************************************/

PARSER

   parse_leave (Any_ptr G_ptr)

   {

   Symbol_ptr  s_ptr;
   Leave_ptr   l_ptr;

   l_ptr = G_ptr;

   t = next_token();


   if (t->token!= LEAVE)
      return(FAIL);
   t = next_token();

   if ((t->token != PL1NAME) &&   /* MUST be an unsubscripted ref */
       (t->keyword == 0) &&
       (t->token != SEMICOLON))
      {       
      report(1,"",_LINE_);
      return(FAIL);
      }

   /********************************************************************/
   /*       A leave is only valid if we are within a PL/I do group.    */
   /********************************************************************/

   if (!within_a_group)
      report (135,"",_LINE_);

   if (t->token != SEMICOLON)
      {
      /****************************************************************/
      /*            Insert this symbol into the symtab !              */
      /****************************************************************/
      s_ptr = get_symbol(current_parent,t->lexeme);
      if (s_ptr == NULL)
         {
         s_ptr = add_symbol (current_parent,
                             t->lexeme,
                             t->token,
                             t->keyword,
                             0,          /* Unknown length    */
                             0,          /* Unknown scale     */
                             0,          /* Unknown class     */
                             0,          /* Unknown scope     */
                             0,
                             0,
                             0,
                             NULL,
                             line_no);

         }

      l_ptr->ref = s_ptr; 
      t = next_token();
      if (t->token!= SEMICOLON)
         return(FAIL);
      }

   return(OK);

   }

/***************************************************************************/
/*          This function parses a PL/1 'return' statement.                */
/***************************************************************************/

PARSER

   parse_return (Any_ptr g_ptr)

   {

   Return_ptr  r_ptr;
   Expr_ptr    e_ptr;

   r_ptr = g_ptr; 

   t = next_token();

   if (t->token!= RETURN)
      {       
      report(30,"",_LINE_);
      return(FAIL);
      }

   t = next_token();


   if ((t->token!= SEMICOLON) && (t->token!= LPAR))
      {
      report (28,"",_LINE_);      
      return(FAIL);
      }

   if (t->token== SEMICOLON)
      {
      current_parent->num_rets++;
      return(OK);
      }

   current_parent->num_rets++;

   /****************************************************************/
   /* OK We have discovered a return (   type of construct so..... */
   /****************************************************************/

   r_ptr->value = NULL; /* temporary assignment only */

   e_ptr = parse_expression();

   if (e_ptr == NULL)
      return(FAIL);

   r_ptr->value = e_ptr;

   t = next_token();

   if (t->token!= RPAR)
      {       
      report(10,"",_LINE_);
      return(FAIL);
      }

   t = next_token();

   if (t->token!= SEMICOLON)
      {       
      report(4,"",_LINE_);
      return(FAIL);
      }

    return(OK);

   }

/***************************************************************************/
/*          This function parses a PL/1 'open' statement.                  */
/***************************************************************************/

PARSER

   parse_open (Open_ptr open_ptr)

   {
 
   Open_ptr    o_ptr;
   char        semicolon_not_seen = 1;
   long        p;
   Token_ptr   tok;
   Any_ptr     a_ptr;
   
   o_ptr = open_ptr; 

   t = next_token();

   if (t->token!= OPEN)
      {       
      report(30,"",_LINE_);
      return(FAIL);
      }

   /****************************************************************************/
   /* Parsing the open statement is very simple, everything is keyword driven  */
   /****************************************************************************/ 

 
   while (semicolon_not_seen)
         {
         t = next_token();

         switch(t->token) {
         
         case(STREAM):
             o_ptr->stream++;
             break;
         case(RECORD):
             o_ptr->record++;
             break;
         case(INPUT):
             o_ptr->input++;
             break;
         case(OUTPUT):
             o_ptr->output++;
             break;
         case(UPDATE):
             o_ptr->update++;
             break;
         case(SEQUENTIAL):
             o_ptr->sequential++;
             break;
         case(DIRECT):
             o_ptr->direct++;
             break;
         case(PRINT):
             o_ptr->print++;
             break;
         case(NONPRINT):
             o_ptr->nonprint++;
             break;
         case(KEYED):
             o_ptr->keyed++;
             break;
         case(FILE_TYPE):
         case(TITLE):
         case(LINESIZE):
         case(PAGESIZE):
             {
             tok = t;

             t = next_token();
             if (t->token != LPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }

             /*********************************************************************/
             /* These options and things all have (args) as part of their spec.   */
             /* duplicate occurences of these between comma's are detected and    */
             /* reported. For example the line below is in error cos of two FILE  */
             /* specifications:                                                   */
             /*                                                                   */
             /* open file(file_a) stream, file(feed_1) stream file(feed_2) ;      */
             /* a likely cause is a missing comma after the 2nd STREAM.           */
             /*                                                                   */
             /* A similar policy is used for other statements (ie READ,CLOSE etc) */ 
             /*********************************************************************/

             switch(tok->token) {

             case(FILE_TYPE): /* 'FILE' is already used in stdio.h !! */
                 if (o_ptr->file_ref != NULL)
                    {
                    report(163,"FILE",_LINE_);
                    a_ptr = parse_reference(); /* parse it anyway to consume tokens */
                    }
                 else
                    o_ptr->file_ref = parse_reference();
                 break;
             case(TITLE):
                 if (o_ptr->title_expr != NULL)
                    {
                    report(163,"TITLE",_LINE_);
                    a_ptr = parse_expression();
                    }
                 else
                    o_ptr->title_expr = parse_expression();
                 break;
             case(LINESIZE):
                 if (o_ptr->linesize_expr != NULL)
                    {
                    report(163,"LINESIZE",_LINE_);
                    a_ptr = parse_expression();
                    }
                 else
                    o_ptr->linesize_expr = parse_expression();
                 break;
             case(PAGESIZE):        
                 if (o_ptr->pagesize_expr != NULL)
                    {
                    report(163,"PAGESIZE",_LINE_);
                    a_ptr = parse_expression();
                    }
                 else 
                    o_ptr->pagesize_expr = parse_expression();
                 break;
                 }

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }
         default:
             report(156,t->lexeme,_LINE_);
         }

         p = get_pos();
         t = next_token();

         if (t->token == SEMICOLON)
            semicolon_not_seen = 0;
         else
            if (t->token == COMMA)
               {
               o_ptr->next_open = allocate_node(OPEN);
               o_ptr = o_ptr->next_open;
               }
            else
               set_pos(p);
         }    
         
   return(OK);

   }

/***************************************************************************/
/*          This function parses a PL/1 'close' statement.                  */
/***************************************************************************/

PARSER

   parse_close (Close_ptr close_ptr)

   {
 
   Close_ptr   c_ptr;
   char        semicolon_not_seen = 1;
   long        p;
   Token_ptr   tok;
   Any_ptr     a_ptr;   
   
   c_ptr = close_ptr; 

   t = next_token();

   if (t->token!= CLOSE)
      {       
      report(30,"",_LINE_);
      return(FAIL);
      }

   /***********************************************/
   /* Parsing the close statement is very simple. */
   /***********************************************/ 

 
   while (semicolon_not_seen)
         {
         t = next_token();

         if (t->token == FILE_TYPE)
            {
            tok = next_token();

            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (c_ptr->file_ref != NULL)
               {
               report(163,"FILE",_LINE_);
               a_ptr = parse_reference(); /* parse anyway to consume the tokens */
               }
            else  
               c_ptr->file_ref = parse_reference();

            t = next_token();

            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }

            }
   
         p = get_pos();
         t = next_token();

         if (t->token == SEMICOLON)
            semicolon_not_seen = 0;
         else
            if (t->token == COMMA)
               {
               c_ptr->next_close = allocate_node(CLOSE);
               c_ptr = c_ptr->next_close;
               }
            else
               set_pos(p);
         }    
         
   return(OK);

   }

/***************************************************************************/
/*          This function parses a PL/1 'read' statement.                  */
/***************************************************************************/

PARSER

   parse_read (Read_ptr read_ptr)

   {
 
 
   char        semicolon_not_seen = 1;
   long        p;
   Token_ptr   tok;
   Any_ptr     a_ptr;   
   
   t = next_token();

   if (t->token!= READ)
      {       
      report(30,"",_LINE_);
      return(FAIL);
      }

   /***********************************************/
   /* Parsing the READ statement is very simple.  */
   /***********************************************/ 

   /************************************************************************************/
   /* we simply read tokens looking for recognised options etc, in any order. We shout */
   /* if a duplicate options appears, but dont test for legal combinations of options  */
   /* This can be done very easily in pass2.                                           */
   /* A similar pricniple is used elsewhere in the I/O parsing functions.              */
   /************************************************************************************/   

 
   while (semicolon_not_seen)
         {
         t = next_token();

         switch (t->token) {

         case(FILE_TYPE):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }

             if (read_ptr->file_ref != NULL)
                {
                report(163,"FILE",_LINE_);
                a_ptr = parse_reference(); /* parse anyway to consume the tokens */
                }
             else  
                read_ptr->file_ref = parse_reference();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }

         case(INTO):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }

             if (read_ptr->into_ref != NULL)
                {
                report(163,"INTO",_LINE_);
                a_ptr = parse_reference(); /* parse anyway to consume the tokens */
                }
             else  
                read_ptr->into_ref = parse_reference();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }

          case(KEYTO):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }

             if (read_ptr->keyto_ref != NULL)
                {
                report(163,"KEYTO",_LINE_);
                a_ptr = parse_reference(); /* parse anyway to consume the tokens */
                }
             else  
                read_ptr->keyto_ref = parse_reference();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }

          case(KEY):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }

             if (read_ptr->key_expr != NULL)
                {
                report(163,"KEY",_LINE_);
                a_ptr = parse_expression(); /* parse anyway to consume the tokens */
                }
             else  
                read_ptr->key_expr = parse_expression();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }

           case(SIZETO):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }

             if (read_ptr->sizeto_ref != NULL)
                {
                report(163,"SIZETO",_LINE_);
                a_ptr = parse_reference(); /* parse anyway to consume the tokens */
                }
             else  
                read_ptr->sizeto_ref = parse_reference();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }
       
         default:
             /*************************************************/
             /*     An unknown keyword or option perhaps ?    */
             /*************************************************/
             report(164,t->lexeme,_LINE_);
             skip_to_next(SEMICOLON);
             goto abort_this_parse; /* dont parse rest of stmt ! */
         } /* switch */

         p = get_pos();
         t = next_token();

         if (t->token == SEMICOLON)
            semicolon_not_seen = 0;
         else
            set_pos(p);
         }    
  
   abort_this_parse:
       
   return(OK);

   }

/***************************************************************************/
/*          This function parses a PL/1 'delete' statement.                */
/***************************************************************************/

PARSER

   parse_delete (Delete_ptr delete_ptr)

   {
 
   char        semicolon_not_seen = 1;
   long        p;
   Token_ptr   tok;
   Any_ptr     a_ptr;   
   
   t = next_token();

   if (t->token!= DELETE)
      {       
      report(30,"",_LINE_);
      return(FAIL);
      }

   /*************************************************/
   /* Parsing the DELETE statement is very simple.  */
   /*************************************************/ 

 
   while (semicolon_not_seen)
         {
         t = next_token();

         switch (t->token) {

         case(FILE_TYPE):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }
             if (delete_ptr->file_ref != NULL)
                {
                report(163,"FILE",_LINE_);
                a_ptr = parse_reference(); /* parse anyway to consume the tokens */
                }
             else  
                delete_ptr->file_ref = parse_reference();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }

          case(KEY):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }

             if (delete_ptr->key_expr != NULL)
                {
                report(163,"KEY",_LINE_);
                a_ptr = parse_expression(); /* parse anyway to consume the tokens */
                }
             else  
                delete_ptr->key_expr = parse_expression();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }
       
         default:
             /*************************************************/
             /*     An unknown keyword or option perhaps ?    */
             /*************************************************/
             report(164,t->lexeme,_LINE_);
             skip_to_next(SEMICOLON);
             goto abort_this_parse; /* dont parse rest of stmt ! */


         } /* switch */

         p = get_pos();
         t = next_token();

         if (t->token == SEMICOLON)
            semicolon_not_seen = 0;
         else
            set_pos(p);
         }    
   
   abort_this_parse:
      
   return(OK);

   }


/****************************************************************************/
/*          This function parses a PL/1 'write' statement.                  */
/****************************************************************************/

PARSER

   parse_write (Write_ptr write_ptr)

   {
 
   char        semicolon_not_seen = 1;
   long        p;
   Token_ptr   tok;
   Any_ptr     a_ptr;   
   
   t = next_token();

   if (t->token!= WRITE)
      {       
      report(30,"",_LINE_);
      return(FAIL);
      }

   /************************************************/
   /* Parsing the WRITE statement is very simple.  */
   /************************************************/ 

 
   while (semicolon_not_seen)
         {
         t = next_token();

         switch (t->token) {

         case(FILE_TYPE):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }
             if (write_ptr->file_ref != NULL)
                {
                report(163,"FILE",_LINE_);
                a_ptr = parse_reference(); /* parse anyway to consume the tokens */
                }
             else  
                write_ptr->file_ref = parse_reference();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }

          case(FROM):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }
             if (write_ptr->from_ref != NULL)
                {
                report(163,"FROM",_LINE_);
                a_ptr = parse_reference(); /* parse anyway to consume the tokens */
                }
             else  
                write_ptr->from_ref = parse_reference();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }

         case(KEYFROM):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }
             if (write_ptr->keyfrom_expr != NULL)
                {
                report(163,"KEYFROM",_LINE_);
                a_ptr = parse_expression(); /* parse anyway to consume the tokens */
                }
             else  
                write_ptr->keyfrom_expr = parse_expression();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }
       
       
      
         default:
             /*************************************************/
             /*     An unknown keyword or option perhaps ?    */
             /*************************************************/
             report(164,t->lexeme,_LINE_);
             skip_to_next(SEMICOLON);
             goto abort_this_parse; /* dont parse rest of stmt ! */

         } /* switch */

         p = get_pos();
         t = next_token();

         if (t->token == SEMICOLON)
            semicolon_not_seen = 0;
         else
            set_pos(p);
         }    
   abort_this_parse:
      
   return(OK);

   }


/******************************************************************************/
/*          This function parses a PL/1 'rewrite' statement.                  */
/******************************************************************************/

PARSER

   parse_rewrite (Rewrite_ptr rewrite_ptr)

   {
 
   char        semicolon_not_seen = 1;
   long        p;
   Token_ptr   tok;
   Any_ptr     a_ptr;   
   
   t = next_token();

   if (t->token!= REWRITE)
      {       
      report(30,"",_LINE_);
      return(FAIL);
      }

   /**************************************************/
   /* Parsing the REWRITE statement is very simple.  */
   /**************************************************/ 

 
   while (semicolon_not_seen)
         {
         t = next_token();

         switch (t->token) {

         case(FILE_TYPE):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }
             if (rewrite_ptr->file_ref != NULL)
                {
                report(163,"FILE",_LINE_);
                a_ptr = parse_reference(); /* parse anyway to consume the tokens */
                }
             else  
                rewrite_ptr->file_ref = parse_reference();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }

          case(FROM):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }
             if (rewrite_ptr->from_ref != NULL)
                {
                report(163,"FROM",_LINE_);
                a_ptr = parse_reference(); /* parse anyway to consume the tokens */
                }
             else  
                rewrite_ptr->from_ref = parse_reference();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
             }

         case(KEY):
             {
             tok = next_token();

             if (tok->token != LPAR)
                {
                report(155,t->lexeme,_LINE_);
                return(FAIL);
                }
             if (rewrite_ptr->key_expr != NULL)
                {
                report(163,"KEY",_LINE_);
                a_ptr = parse_expression(); /* parse anyway to consume the tokens */
                }
             else  
                rewrite_ptr->key_expr = parse_expression();

             t = next_token();

             if (t->token != RPAR)
                {
                report(155,tok->lexeme,_LINE_);
                return(FAIL);
                }
             break;
       
         default:
             /*************************************************/
             /*     An unknown keyword or option perhaps ?    */
             /*************************************************/
             report(164,t->lexeme,_LINE_);
             skip_to_next(SEMICOLON);
             goto abort_this_parse; /* dont parse rest of stmt ! */
            }

         } /* switch */

         p = get_pos();
         t = next_token();

         if (t->token == SEMICOLON)
            semicolon_not_seen = 0;
         else
            set_pos(p);
         }    
   
   abort_this_parse:
      
   return(OK);

   }


/***************************************************************************/
/*          This function parses a PL/1 'get' statement.                   */
/***************************************************************************/

PARSER

   parse_get (Get_ptr get_ptr)

   {
 
   char          semicolon_not_seen = 1;
   long          p;
   Token_ptr     tok;
   Any_ptr       dummy_ptr;
   Any_ptr       string_expr;
   Ref_ptr       file_ref;
   Any_ptr       skip_expr;
   Intarg_ptr    list_ptr;
   Format_ptr    fmat_ptr;  
   char          null_skip;
   Getfile_ptr   getf_ptr;
   Getstring_ptr gets_ptr;
 
   dummy_ptr   = NULL;
   string_expr = NULL;
   file_ref    = NULL;
   skip_expr   = NULL;
   list_ptr    = NULL;
   fmat_ptr    = NULL; 
   getf_ptr    = NULL;
   gets_ptr    = NULL;

   null_skip   = 0;    /* set to true for SKIP without its optional expression */

   t = next_token();

   if (t->token!= GET)
      {       
      report(30,"",_LINE_);
      return(FAIL);
      }

   /********************************************************************************************************/
   /* The get statement has a rather involved syntax. The get statement does not have a single node like   */
   /* some other statements, instead it is represented as a small tree (see NODES.H) of specialized nodes  */
   /* this reflects the grammar as specified in the ANSI X3.74-1987 manual.                                */
   /* The parse algorithm is as follows:                                                                   */
   /*                                                                                                      */
   /* If we see FILE   then parse the  ref  (reqd by FILE)   and store it   in file_ref.                   */
   /* If we see STRING then parse the  exp  (reqd by STRING) and store it   in string_expr.                */
   /* If we see SKIP   then parse the  exp  (option in SKIP) and store it   in skip_expr.                  */
   /* If we see LIST   then parse the  arg  (reqd by LIST)   and store it   in list_ptr.                   */
   /* If we see EDIT   then parse both args (reqd by EDIT)   and store them in list_ptr and fmat_ptr.      */
   /*                                                                                                      */
   /* If we see anything and its associated pointer is NOT NULL then its an illegal duplicate item.        */
   /*                                                                                                      */
   /* Once we encounter a semicolon we check for legal combinations of the options by examining the above  */
   /* pointers, for example if file_ref and string_expr were BOTH non-null, then we have a syntax error.   */
   /* Finally the tree is completed by assigning pointers in the parent nodes from the relevant pointers   */
   /* discussed above.                                                                                     */
   /********************************************************************************************************/

   while (semicolon_not_seen) /* stop parsing once we see a ; */
         {
         t = next_token();

         if (t->token == FILE_TYPE)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all GET options */
            /****************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (file_ref != NULL)
               {
               report(163,"FILE",_LINE_);
               dummy_ptr = parse_reference(); /* parse anyway to consume the tokens */
               }
            else
               {  
               file_ref = parse_reference();
               getf_ptr = allocate_node(GETFILE);
               }               
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }
            goto step_forward;
            }

         if (t->token == STRING_IO)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all GET options */
            /****************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (string_expr != NULL)
               {
               report(163,"STRING",_LINE_);
               dummy_ptr = parse_expression(); /* parse anyway to consume the tokens */
               }
            else
               {  
               string_expr = parse_expression();
               gets_ptr = allocate_node(GETSTRING);
               }               
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }
            goto step_forward;
            }

         if (t->token == SKIP)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all GET options */
            /****************************************************************/
            p = get_pos();
            tok = next_token();
            
            if (tok->token == LPAR)
               {
               if (skip_expr != NULL)
                  {
                  report(163,"SKIP",_LINE_);
                  dummy_ptr = parse_expression(); /* parse anyway to consume the tokens */
                  }
               else
                  {  
                  skip_expr = parse_expression();
                  }            
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(155,tok->lexeme,_LINE_);
                  return(FAIL);
                  }
               }
            else
               {
               set_pos(p); /* go back we havent got SKIP( but simply SKIP */
               null_skip++;
               }
            goto step_forward;

            }

         if (t->token == LIST)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all GET options */
            /****************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (list_ptr != NULL)
               {
               report(163,"LIST",_LINE_);
               dummy_ptr = parse_get_list(); /* parse anyway to consume the tokens */
               }
            else
               {  
               list_ptr = parse_get_list();
               }               
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }
            goto step_forward; 
            }

         if (t->token == EDIT)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all GET options */
            /****************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (list_ptr != NULL)
               {
               report(163,"EDIT",_LINE_);
               dummy_ptr = parse_get_list(); /* parse anyway to consume the tokens */
               }
            else
               {  
               list_ptr = parse_get_list();
               }               
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }
            /************************************************************/
            /* Next parse the format part of this edit option.          */
            /************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (fmat_ptr != NULL)
               {
               report(163,"EDIT",_LINE_);
               dummy_ptr = parse_format(); /* parse anyway to consume the tokens */
               }
            else
               {  
               fmat_ptr = parse_format();
               } 
            if (fmat_ptr == NULL) /* the format list had a syntax error */
               {
               skip_to_next(SEMICOLON);
               goto parse_completed;
               }
              
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }
            goto step_forward;
            }

        
         report(179,t->lexeme,_LINE_);
         skip_to_next(SEMICOLON);
         goto parse_completed;
   
step_forward:

         p = get_pos();
         t = next_token();

         if (t->token == SEMICOLON)
            semicolon_not_seen = 0;
         else
            set_pos(p);
         } 

parse_completed:
   
   /***********************************************************************************/
   /* Check for illegal combinations here, because the processing we do below depends */
   /* on which pointers are not NULL (ie which options were coded by user).           */
   /***********************************************************************************/

   if ((gets_ptr != NULL) && (getf_ptr != NULL))   
      {
      report(172,"",_LINE_);
      return(FAIL);
      }

   /***********************************************************************************/
   /* If only targ_ptr is set then we had a LIST option, if both are set then we had  */
   /* an EDIT specified. Syntactically both EDIT and LIST may be omitted so that what */
   /* we have if both of these pointers are NULL. This applies to FILE and STRING.    */
   /***********************************************************************************/

   if (getf_ptr != NULL)
      {
      getf_ptr->file_ref  = file_ref;
      getf_ptr->skip_expr = skip_expr;

      if (list_ptr != NULL)
         getf_ptr->targ_ptr = list_ptr;

      if (fmat_ptr != NULL)
         getf_ptr->fmat_ptr = fmat_ptr;
      if (null_skip)
         getf_ptr->skip = 1;
      }

   if (gets_ptr != NULL)
      {
      gets_ptr->string_expr = string_expr;

      if (list_ptr != NULL)
         gets_ptr->targ_ptr = list_ptr;

      if (fmat_ptr != NULL)
         gets_ptr->fmat_ptr = fmat_ptr;
      }

   get_ptr->getf_ptr = getf_ptr;
   get_ptr->gets_ptr = gets_ptr;
         
   return(OK);

   }

/***************************************************************************/
/*          This function parses a PL/1 'put' statement.                   */
/***************************************************************************/

PARSER

   parse_put (Put_ptr put_ptr)

   {
 
   char          semicolon_not_seen = 1;
   long          p;
   Token_ptr     tok;
   Any_ptr       dummy_ptr;
   Ref_ptr       string_ref;
   Ref_ptr       file_ref;
   Any_ptr       skip_expr;
   Any_ptr       line_expr;
   Outsrc_ptr    list_ptr;
   Format_ptr    fmat_ptr;  
   char          null_skip;
   short         page = 0;
   Putfile_ptr   putf_ptr;
   Putstring_ptr puts_ptr;
 
   dummy_ptr   = NULL;
   string_ref  = NULL;
   file_ref    = NULL;
   skip_expr   = NULL;
   line_expr   = NULL;
   list_ptr    = NULL;
   fmat_ptr    = NULL; 
   putf_ptr    = NULL;
   puts_ptr    = NULL;
   page        = 0;

   null_skip   = 0;    /* set to true for SKIP without its optional expression */

   t = next_token();

   if (t->token!= PUT)
      {       
      report(30,"",_LINE_);
      return(FAIL);
      }

   /********************************************************************************************************/
   /* The put statement has a rather involved syntax. The put statement does not have a single node like   */
   /* some other statements, instead it is represented as a small tree (see NODES.H) of specialized nodes  */
   /* this reflects the grammar as specified in the ANSI X3.74-1987 manual.                                */
   /* The parse algorithm is as follows:                                                                   */
   /*                                                                                                      */
   /* If we see FILE   then parse the  ref  (reqd by FILE)   and store it   in file_ref.                   */
   /* If we see STRING then parse the  ref  (reqd by STRING) and store it   in string_ref.                 */
   /* If we see SKIP   then parse the  exp  (option in SKIP) and store it   in skip_expr.                  */
   /* If we see LIST   then parse the  arg  (reqd by LIST)   and store it   in list_ptr.                   */
   /* If we see EDIT   then parse both args (reqd by EDIT)   and store them in list_ptr and fmat_ptr.      */
   /*                                                                                                      */
   /* If we see anything and its associated pointer is NOT NULL then its an illegal duplicate item.        */
   /*                                                                                                      */
   /* Once we encounter a semicolon we check for legal combinations of the options by examining the above  */
   /* pointers, for example if file_ref and string_ref were BOTH non-null, then we have a syntax error.    */
   /* Finally the tree is completed by assigning pointers in the parent nodes from the relevant pointers   */
   /* discussed above.                                                                                     */
   /********************************************************************************************************/

   while (semicolon_not_seen) /* stop parsing once we see a ; */
         {
         t = next_token();

         if (t->token == FILE_TYPE)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all PUT options */
            /****************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (file_ref != NULL)
               {
               report(163,"FILE",_LINE_);
               dummy_ptr = parse_reference(); /* parse anyway to consume the tokens */
               }
            else
               {  
               file_ref = parse_reference();
               putf_ptr = allocate_node(PUTFILE);
               }               
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }
            goto step_forward;
            }

         if (t->token == STRING_IO)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all PUT options */
            /****************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (string_ref != NULL)
               {
               report(163,"STRING",_LINE_);
               dummy_ptr = parse_reference(); /* parse anyway to consume the tokens */
               }
            else
               {  
               string_ref = parse_reference();
               puts_ptr = allocate_node(PUTSTRING);
               }               
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }
            goto step_forward;
            }

         if (t->token == PAGE)
            {
            page = 1;
            goto step_forward;
            } 

         if (t->token == SKIP)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all PUT options */
            /****************************************************************/
            p = get_pos();
            tok = next_token();
            
            if (tok->token == LPAR)
               {
               if (skip_expr != NULL)
                  {
                  report(163,"SKIP",_LINE_);
                  dummy_ptr = parse_expression(); /* parse anyway to consume the tokens */
                  }
               else
                  {  
                  skip_expr = parse_expression();
                  }            
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(155,tok->lexeme,_LINE_);
                  return(FAIL);
                  }
               }
            else
               {
               set_pos(p); /* go back we havent got SKIP( but simply SKIP */
               null_skip++;
               }
            goto step_forward;
            }

         if (t->token == LINE)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all PUT options */
            /****************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (line_expr != NULL)
               {
               report(163,"FILE",_LINE_);
               dummy_ptr = parse_expression(); /* parse anyway to consume the tokens */
               }
            else
               line_expr = parse_expression();
               
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }
            goto step_forward;
            }

         if (t->token == LIST)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all PUT options */
            /****************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (list_ptr != NULL)
               {
               report(163,"LIST",_LINE_);
               dummy_ptr = parse_put_list(); /* parse anyway to consume the tokens */
               }
            else
               {  
               list_ptr = parse_put_list();
               }               
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }
            goto step_forward;
            }

         if (t->token == EDIT)
            {
            /****************************************************************/
            /* The block of statements here, is similar for all PUT options */
            /****************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (list_ptr != NULL)
               {
               report(163,"EDIT",_LINE_);
               dummy_ptr = parse_put_list(); /* parse anyway to consume the tokens */
               }
            else
               {  
               list_ptr = parse_put_list();
               }               
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }
            /************************************************************/
            /* Next parse the format part of this edit option.          */
            /************************************************************/
            tok = next_token();
            if (tok->token != LPAR)
               {
               report(155,t->lexeme,_LINE_);
               return(FAIL);
               }
            if (fmat_ptr != NULL)
               {
               report(163,"EDIT",_LINE_);
               dummy_ptr = parse_format(); /* parse anyway to consume the tokens */
               }
            else
               {  
               fmat_ptr = parse_format();
               } 
            if (fmat_ptr == NULL) /* the format list had a syntax error */
               {
               skip_to_next(SEMICOLON);
               goto parse_completed;
               }
              
            t = next_token();
            if (t->token != RPAR)
               {
               report(155,tok->lexeme,_LINE_);
               return(FAIL);
               }

            goto step_forward;
            }

         if (strcmp(t->lexeme,"data") == 0)
            {
            report(177,"",_LINE_);
            goto step_forward;
            }
        
         report(178,t->lexeme,_LINE_);
         skip_to_next(SEMICOLON);
         goto parse_completed;

step_forward:
   
         p = get_pos();
         t = next_token();

         if (t->token == SEMICOLON)
            semicolon_not_seen = 0;
         else
            set_pos(p);
         } 

parse_completed:
   
   /***********************************************************************************/
   /* Check for illegal combinations here, because the processing we do below depends */
   /* on which pointers are not NULL (ie which options were coded by user).           */
   /***********************************************************************************/

   if ((puts_ptr != NULL) && (putf_ptr != NULL))   
      {
      report(172,"",_LINE_);
      return(FAIL);
      }

   /***********************************************************************************/
   /* If only srce_ptr is set then we had a LIST option, if both are set then we had  */
   /* an EDIT specified. Syntactically both EDIT and LIST may be omitted so that what */
   /* we have if both of these pointers are NULL. This applies to FILE and STRING.    */
   /***********************************************************************************/

   if (putf_ptr != NULL)
      {
      putf_ptr->file_ref  = file_ref;
      putf_ptr->skip_expr = skip_expr;
      putf_ptr->line_expr = line_expr;
      putf_ptr->page      = page;

      if (list_ptr != NULL)
         putf_ptr->srce_ptr = list_ptr;

      if (fmat_ptr != NULL)
         putf_ptr->fmat_ptr = fmat_ptr;
      if (null_skip)
         putf_ptr->skip = 1;
      
      }

   if (puts_ptr != NULL)
      {
      puts_ptr->string_ref = string_ref;

      if (list_ptr != NULL)
         puts_ptr->srce_ptr = list_ptr;

      if (fmat_ptr != NULL)
         puts_ptr->fmat_ptr = fmat_ptr;
      }

   put_ptr->putf_ptr = putf_ptr;
   put_ptr->puts_ptr = puts_ptr;
         
   return(OK);

   }

/***************************************************************************/
/*         This function parse the FORMAT option of a GET/PUT statement.   */
/***************************************************************************/

static
Format_ptr

   parse_format (void)

   {

   Format_ptr       head_ptr = NULL; 
   Format_ptr       fmat_ptr = NULL;
   short            comma_seen = 1;
   long             p;
 
   fmat_ptr = allocate_node(FORMAT);
   head_ptr = fmat_ptr; /* head_ptr is returned by this function */

   while (comma_seen)
         {
         p = get_pos();
         t = next_token();

         /***********************************************************************************************/
         /* OK we may have a format-iteration.                                                          */
         /* this being <integer> or (<expression>) (I think the syntax would be better if just          */
         /* (<expression>) were allowed). We set either fmat_itrn_value or fmat_itrn_expr as a result.  */
         /***********************************************************************************************/

         if ((t->token == NUMERIC) || (t->token == LPAR))
            {
            if (t->token == NUMERIC)
               {
               fmat_ptr->fmat_itrn_value = atol(t->lexeme);
               t = next_token();
               }
           else
           if ((t->token == LPAR))
              {
              fmat_ptr->fmat_itrn_expr = parse_expression();
              t = next_token();
              if (t->token != RPAR)
                 {
                 report(10,"",_LINE_);
                 return(NULL);
                 } 
              t = next_token();
              }
            }

         if (t->token == LPAR)
            if (fmat_ptr->fmat_itrn_expr != NULL)
               {
               fmat_ptr->nest_ptr = parse_format();    
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(10,"",_LINE_);
                  return(NULL);
                  }
               }
            else
               {
               /***************************************************************/
               /* Nested (format-commalist) MUST be preceded by an iteration. */
               /* See P50 ANSI 87.                                            */
               /***************************************************************/
               report(174,"",_LINE_);
               return(NULL);
               }   
         
         /************************************************************/
         /* OK We must now see a <format-item> so lets get going !   */
         /* Incidentally whilst coding this stuff I was listening to */
         /* "Fantaisie" (In a continuous loop on the NT CD Player)   */
         /* this is a track from "Pelleas et Melisande" by the well  */
         /* known french composer "(Gabriel) Faure" (1845-1924).     */
         /* Now back to the action........                           */
         /************************************************************/    

         /*****************************************************************/
         /* Theres no point having special tokens for 'F', 'P' etc etc so */
         /* examine the raw lexeme in these cases.                        */
         /*****************************************************************/     

         if (strcmp(t->lexeme,"f") == 0)
            {
            t = next_token();

            if (t->token != LPAR)
               {
               report(8,"",_LINE_);
               return(NULL);
               }
            fmat_ptr->fixed_expr1 = parse_expression();

            t = next_token();

            if (t->token == COMMA)
               {
               fmat_ptr->fixed_expr2 = parse_expression();
               t = next_token();
               }

            if (t->token != RPAR)
               {
               report(10,"",_LINE_);
               return(NULL);
               }
            goto step_forward;
            } 
         
         if (strcmp(t->lexeme,"e") == 0)
            {
            t = next_token();

            if (t->token != LPAR)
               {
               report(8,"",_LINE_);
               return(NULL);
               }
            fmat_ptr->float_expr1 = parse_expression();

            t = next_token();

            if (t->token == COMMA)
               {
               fmat_ptr->float_expr2 = parse_expression();
               t = next_token();
               }

            if (t->token != RPAR)
               {
               report(10,"",_LINE_);
               return(NULL);
               }
            goto step_forward;
            } 
         
         if (strcmp(t->lexeme,"p") == 0)
            {
            t = next_token();
            if (t->token != STRING)
               {
               report(175,t->lexeme,_LINE_);
               return(NULL);
               }
            fmat_ptr->pic_str = t->lexeme;
            goto step_forward;
            } 
         
         if (strcmp(t->lexeme,"a") == 0)
            {
            fmat_ptr->charf = 1;
            p = get_pos(); /* the parenthesized expression is optional */
            t = next_token();
            if (t->token == LPAR)
               {
               fmat_ptr->char_expr = parse_expression();
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(10,"",_LINE_);
                  return(NULL);
                  }
               }
            else
               set_pos(p); 
            goto step_forward;
            }
         
         if (strcmp(t->lexeme,"b") == 0)
            {
            fmat_ptr->bit_radix = 1;
            p = get_pos(); /* the parenthesized expression is optional */
            t = next_token();
            if (t->token == LPAR)
               {
               fmat_ptr->bit_expr = parse_expression();
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(10,"",_LINE_);
                  return(NULL);
                  }
               }
            else
               set_pos(p); 
            goto step_forward;
            }

         if (strcmp(t->lexeme,"b1") == 0)
            {
            fmat_ptr->bit_radix = 1;
            p = get_pos(); /* the parenthesized expression is optional */
            t = next_token();
            if (t->token == LPAR)
               {
               fmat_ptr->bit_expr = parse_expression();
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(10,"",_LINE_);
                  return(NULL);
                  }
               }
            else
               set_pos(p); 
            goto step_forward;
            }

         if (strcmp(t->lexeme,"b2") == 0)
            {
            fmat_ptr->bit_radix = 2;
            p = get_pos(); /* the parenthesized expression is optional */
            t = next_token();
            if (t->token == LPAR)
               {
               fmat_ptr->bit_expr = parse_expression();
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(10,"",_LINE_);
                  return(NULL);
                  }
               }
            else
               set_pos(p); 
            goto step_forward;
            }

         if (strcmp(t->lexeme,"b3") == 0)
            {
            fmat_ptr->bit_radix = 3;
            p = get_pos(); /* the parenthesized expression is optional */
            t = next_token();
            if (t->token == LPAR)
               {
               fmat_ptr->bit_expr = parse_expression();
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(10,"",_LINE_);
                  return(NULL);
                  }
               }
            else
               set_pos(p); 
            goto step_forward;
            }

         if (strcmp(t->lexeme,"b4") == 0)
            {
            fmat_ptr->bit_radix = 4;
            p = get_pos(); /* the parenthesized expression is optional */
            t = next_token();
            if (t->token == LPAR)
               {
               fmat_ptr->bit_expr = parse_expression();
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(10,"",_LINE_);
                  return(NULL);
                  }
               }
            else
               set_pos(p); 
            goto step_forward;
            }

         if (strcmp(t->lexeme,"x") == 0)
            {
            t = next_token();
            if (t->token != LPAR)
               {
               report(8,"",_LINE_);
               return(NULL);
               }
            fmat_ptr->space_expr = parse_expression();
            t = next_token();
            if (t->token != RPAR)
               {
               report(10,"",_LINE_);
               return(NULL);
               }
            goto step_forward;
            }

         if (strcmp(t->lexeme,"l") == 0)
            {
            fmat_ptr->l_format = 1;
            goto step_forward;
            }

         if (strcmp(t->lexeme,"r") == 0)
            {
            t = next_token();
            if (t->token != LPAR)
               {
               report(8,"",_LINE_);
               return(NULL);
               }
            fmat_ptr->rmte_ptr = parse_reference();
            t = next_token();
            if (t->token != RPAR)
               {
               report(10,"",_LINE_);
               return(NULL);
               }
            goto step_forward;
            }

         if (t->token == TAB)   
            {
            p = get_pos(); /* the parenthesized expression is optional */
            t = next_token();
            fmat_ptr->tab = 1;
            if (t->token == LPAR)
               {
               fmat_ptr->tab_expr = parse_expression();
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(10,"",_LINE_);
                  return(NULL);
                  }
               }
            else
               set_pos(p); 
            goto step_forward;
            }


         if (t->token == LINE)
            {
            t = next_token();
            if (t->token != LPAR)
               {
               report(8,"",_LINE_);
               return(NULL);
               }
            fmat_ptr->line_expr = parse_expression();
            t = next_token();
            if (t->token != RPAR)
               {
               report(10,"",_LINE_);
               return(NULL);
               }
            goto step_forward;
            }

         if (t->token == SKIP)
            {
            p = get_pos(); /* the parenthesized expression is optional */
            t = next_token();
            fmat_ptr->skip = 1;
            if (t->token == LPAR)
               {
               fmat_ptr->skip_expr = parse_expression();
               t = next_token();
               if (t->token != RPAR)
                  {
                  report(10,"",_LINE_);
                  return(NULL);
                  }
               }
            else
               set_pos(p); 
            goto step_forward;
            }

         if (t->token == COLUMN)
            {
            t = next_token();
            if (t->token != LPAR)
               {
               report(8,"",_LINE_);
               return(NULL);
               }
            fmat_ptr->colm_expr = parse_expression();
            t = next_token();
            if (t->token != RPAR)
               {
               report(10,"",_LINE_);
               return(NULL);
               }
            goto step_forward;
            }

        /**********************************************************************/
        /* If we havent reached the label below, then we have an unknown edit */
        /* specification !                                                    */
        /**********************************************************************/

        report(176,t->lexeme,_LINE_);
        return(NULL);

step_forward: /* come here after weve processed a single format item, ie A, A(23), F(6,2), P '999'  etc etc */

        p = get_pos();        
        t = next_token();

        if (t->token == COMMA)
           {
           fmat_ptr->next_ptr = allocate_node(FORMAT);
           fmat_ptr = fmat_ptr->next_ptr;
           }
        else
           {
           set_pos(p);
           comma_seen = 0;
           }

        } /* while */
 
   return (head_ptr);

   }

/***************************************************************************/
/*         This function parse the LIST option of a GET statement.         */
/***************************************************************************/

static
Intarg_ptr

   parse_get_list (void)

   {

   Intarg_ptr       list_ptr   = NULL;
   Intarg_ptr       iarg_ptr   = NULL;
   char             comma_seen = 1;
   long             p;

   list_ptr = allocate_node(INTARG);
   iarg_ptr = list_ptr;

   while (comma_seen)
         {
         p = get_pos();
         t = next_token();
         
         if (t->token == LPAR)
            {
            /************************************************************************/
            /* We have (or should have !) ( <input-target-commalist> DO <do-spec> ) */
            /************************************************************************/
            iarg_ptr->intarg_ptr = parse_get_list();
            /*****************************************************/
            /* Ok now we need to see a DO followed by <do-spec>  */
            /*****************************************************/
            p = get_pos();
            t = next_token();

            if (t->token != DO)
               {
               report(170,"GET",_LINE_);
               skip_to_next(RPAR);
               }
            else
               {
               set_pos(p);
               iarg_ptr->do_ptr = allocate_node(LOOP);
               parse_group(iarg_ptr->do_ptr);
               }

            /*********************************************************/
            /* Ok finally an RPAR to close this DO target option.    */
            /*********************************************************/

            t = next_token();
            
            if (t->token != RPAR)
               {
               report(171,"GET",_LINE_);
               skip_to_next(SEMICOLON);
               }
            }
         else
            {
            set_pos(p);
            iarg_ptr->target_ref = parse_reference();
            }

         /****************************************************************/
         /* Ok we will now get either a ; or a ), the latter is expected */
         /* by the caller so we will backtrack.                          */
         /****************************************************************/

         p = get_pos();

         t = next_token();

         if ((t->token == RPAR) || (t->token == DO) || (t->token == SEMICOLON))
            set_pos(p);
         
         if (t->token != COMMA)
            comma_seen = 0;   
         else            
            {
            /*****************************************************/
            /* Chain another INTARG node onto this ones next_ptr */
            /*****************************************************/ 
            iarg_ptr->next_ptr = allocate_node(INTARG);
            iarg_ptr = iarg_ptr->next_ptr;
            }
            
         }   
         

   return (list_ptr); /* this is the root of any tree we may have built */

   }

/***************************************************************************/
/*         This function parse the LIST option of a PUT statement.         */
/***************************************************************************/

static
Outsrc_ptr

   parse_put_list (void)

   {

   Outsrc_ptr       list_ptr   = NULL;
   Outsrc_ptr       oarg_ptr   = NULL;
   char             comma_seen = 1;
   long             p;

   list_ptr = allocate_node(OUTSRC);
   oarg_ptr = list_ptr;

   while (comma_seen)
         {
         p = get_pos();
         t = next_token();
         
         if (t->token == LPAR)
            {
            /*************************************************************************/
            /* We have (or should have !) ( <output-source-commalist> DO <do-spec> ) */
            /*************************************************************************/
            oarg_ptr->outsrc_ptr = parse_put_list();
            /*****************************************************/
            /* Ok now we need to see a DO followed by <do-spec>  */
            /*****************************************************/
            p = get_pos();
            t = next_token();

            if (t->token != DO)
               {
               report(170,"PUT",_LINE_);
               skip_to_next(RPAR);
               }
            else
               {
               set_pos(p);
               oarg_ptr->do_ptr = allocate_node(LOOP);
               parse_group(oarg_ptr->do_ptr);
               }

            /*********************************************************/
            /* Ok finally an RPAR to close this DO target option.    */
            /*********************************************************/

            t = next_token();
            
            if (t->token != RPAR)
               {
               report(171,"PUT",_LINE_);
               skip_to_next(SEMICOLON);
               }
            }
         else
            {
            set_pos(p);
            oarg_ptr->source_expr = parse_expression();
            }

         /****************************************************************/
         /* Ok we will now get either a ; or a ), the latter is expected */
         /* by the caller so we will backtrack.                          */
         /****************************************************************/

         p = get_pos();

         t = next_token();

         if ((t->token == RPAR) || (t->token == DO) || (t->token == SEMICOLON))
            set_pos(p);
         
         if (t->token != COMMA)
            comma_seen = 0;   
         else            
            {
            /*****************************************************/
            /* Chain another INTARG node onto this ones next_ptr */
            /*****************************************************/ 
            oarg_ptr->next_ptr = allocate_node(OUTSRC);
            oarg_ptr = oarg_ptr->next_ptr;
            }
            
         }   
         

   return (list_ptr); /* this is the root of any tree we may have built */

   }



/***************************************************************************/
/*        This function parses a PL/1 DO statement                         */
/***************************************************************************/

PARSER

   parse_do (Any_ptr g_ptr)

   {

   Do_ptr      d_ptr;
   Any_ptr     stmt_ptr;
   End_ptr     e_ptr;

   d_ptr = g_ptr;

   t = next_token();

   if (t->token!= DO)
      return(FAIL);

   t = next_token();

   if (t->token!= SEMICOLON)
      {
      report(4,"",_LINE_);       
      return(FAIL);
      } 

   /****************************************************/
   /*              A new loop level !                  */
   /****************************************************/

   nesting++;
        
   stmt_ptr = d_ptr;
   
   while (parse_statement(stmt_ptr))   /* any number of */
         {
         stmt_ptr = next_stmt (stmt_ptr);
         /*******************************************************/
         /* If the statement just parsed resulted in any nodes  */
         /* being chained to the node passed in, then pass the  */
         /* last node in next time round.                       */
         /*******************************************************/
         }


   /******************************************************************/
   /* lets see if the above loop ended because of an END ocurring    */
   /******************************************************************/

   t = next_token();


   if (t->token!= END)
      {
      report(5,"",_LINE_);       
      return(FAIL);
      }

   t = next_token();


   if (t->token!= SEMICOLON)
      {
      report(4,"",_LINE_);       
      return(FAIL);
      }

   /******************************************************/
   /* Insert an appropriate END node into the parse tree */
   /******************************************************/

   e_ptr = allocate_node (END);
   e_ptr->partner = d_ptr;
   insert_node (stmt_ptr,e_ptr);

   nesting--;

   return(OK);

   }

/***************************************************************************/
/*          This function parses a PL/1 'do group' construct.              */
/***************************************************************************/

PARSER

   parse_group (Any_ptr g_ptr)

   {

   Loop_ptr    lp_ptr;
   Any_ptr     stmt_ptr;
   End_ptr     e_ptr;
   Ref_ptr     r_ptr;
   long        p;

   lp_ptr = g_ptr;

   t = next_token();

   if (t->token!= DO)
      {    
      return(FAIL);
      }

/************************************************************************/
/* Following the 'DO' are several possible options, including nothing ! */
/* we can have iterative specs, while and until in various combinations */
/************************************************************************/

   p = get_pos();

   t = next_token();

   if (t->token == SEMICOLON) /* this is an innocent DO;  */
      goto parse_complete;

   if (t->token == DO_LOOP)
      {
      p = get_pos();
      t = next_token();
      if (t->token == SEMICOLON)
         {
         lp_ptr->loop = 1; /* this is: DO LOOP ;  */
         goto parse_complete;
         }

      /**************************************************************/
      /* If we see a ) then assume we are part of a do-spec in a    */
      /* GET or PUT, LIST statement. Backtrack cos the caller needs */
      /* to see the ) token.                                        */
      /**************************************************************/

      if (t->token == RPAR)
         {
         set_pos(p);
         goto parse_complete;
         }
      }
    
   
   set_pos(p);

   r_ptr = parse_reference();

   /**************************************************************************/
   /* whatever comes next, it must actually have the syntax of a reference   */
   /* if not then there is a definite syntax error.                          */
   /* Note that even if we have a WHILE(expression) (or UNTIL) these clauses */
   /* still look like references to the parser, if we did have one of these  */
   /* then the presence of an = is all we need to decide if the WHILE is a   */
   /* keyword or a variable of some sort.                                    */
   /**************************************************************************/  

   if (r_ptr == NULL) /* bad ! */    
      {
      /* we cant possibly parse, so try to continue parsing the do groups conmtents */
      skip_to_next(SEMICOLON);
      goto parse_complete;
      } 

   p = get_pos();

   t = next_token();

   if (t->token == EQUALS)
      /*********************************************************/
      /* we have encountered DO <reference> =   so we must now */
      /* find a <spec> (see p 47 of ANSI-87.                   */
      /*********************************************************/        
      {
      if (parse_do_spec(lp_ptr) == FAIL)
         {
         skip_to_next(SEMICOLON);
         goto parse_complete;
         }

      lp_ptr->counter = r_ptr;
      }
   else
      set_pos(p);
    
   /*************************************************************************/
   /* It is permissible to have either or both, a WHILE and UNTIL clause    */
   /* these can appear in isolation OR they can follow an iterative do-spec */
   /* which may have been parsed above.                                     */  
   /* The syntax could be written as follows:                               */
   /* DO [<iterative-spec>] {[<while-spec>] . [<until-spec>]} ;             */
   /* so the while/until may appear in any order (if at all) but may NOT    */
   /* precede any <iterative-spec> if one is present.                       */  
   /*************************************************************************/

   p = get_pos();
   t = next_token();

   if (t->token == SEMICOLON) 
      goto parse_complete;

   if (t->token == RPAR) /* this can happen in a PUT or GET stmt */
      {
      set_pos(p);
      goto parse_complete;
      }

   if ((t->token == WHILE) || (t->token == UNTIL))
      {
      if (t->token == WHILE)
         {
         t = next_token();

         if (t->token != LPAR)
            {
            report(161,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }

         lp_ptr->while_expr = parse_expression();

         if (lp_ptr->while_expr == NULL) 
            {
            report(161,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }
         t = next_token();

         if (t->token != RPAR)
            {
            report(161,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }
         
         /* does an UNTIL follow ? */

         t = next_token();

         if ((t->token == SEMICOLON) || (t->token == RPAR))
            goto parse_complete;

         if (t->token != UNTIL)
            {
            report(46,t->lexeme,_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }
        
         t = next_token();

         if (t->token != LPAR)
            {
            report(162,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }

         lp_ptr->until_expr = parse_expression();

         if (lp_ptr->until_expr == NULL) 
            {
            report(162,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }

         t = next_token();

         if (t->token != RPAR)
            {
            report(162,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }

         }
     else
         {  /* we have an UNTIL */
         t = next_token();

         if (t->token != LPAR)
            {
            report(162,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }

         lp_ptr->until_expr = parse_expression();

         if (lp_ptr->until_expr == NULL) 
            {
            report(162,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }
         t = next_token();

         if (t->token != RPAR)
            {
            report(162,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }
         
         /* does a WHILE follow ? */

         p = get_pos();
         t = next_token();

         if (t->token == SEMICOLON)
            goto parse_complete;

         if (t->token == RPAR)
            {
            set_pos(p);
            goto parse_complete;
            }

         if (t->token != WHILE)
            {
            report(46,t->lexeme,_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }
        
         t = next_token();

         if (t->token != LPAR)
            {
            report(161,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }

         lp_ptr->while_expr = parse_expression();

         if (lp_ptr->while_expr == NULL) 
            {
            report(161,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }

         t = next_token();

         if (t->token != RPAR)
            {
            report(161,"",_LINE_);
            skip_to_next(SEMICOLON);
            goto parse_complete;
            }

         }

      }
   else
      {
      report(46,t->lexeme,_LINE_);
      skip_to_next(SEMICOLON);
      goto parse_complete;
      }

   p = get_pos();
   t = next_token();

   if (t->token == RPAR)
      {
      set_pos(p);
      goto parse_complete;
      } 

   if (t->token != SEMICOLON) 
      {
      report(46,t->lexeme,_LINE_);
      skip_to_next(SEMICOLON);
      }

   


parse_complete:

   /***********************************************************************/
   /* If the current token (ie t) is an RPAR, then assume that we are     */
   /* not a PL/I loop but rather a <do-spec> as found in the GET/PUT LIST */
   /* statement.                                                          */
   /***********************************************************************/   

   if (t->token == RPAR)
      return(OK);

   /* new loop */

   nesting++;

   stmt_ptr = lp_ptr;
   
   while (parse_statement(stmt_ptr))   /* any number of */
         {
         stmt_ptr = next_stmt (stmt_ptr);
         /*******************************************************/
         /* If the statement just parsed resulted in any nodes  */
         /* being chained to the node passed in, then pass the  */
         /* last node in next time round.                       */
         /*******************************************************/
         }

   /******************************************************/
   /* OK lets see if the call failed because of an END   */
   /******************************************************/

   t = next_token();

   if (t->token!= END)
      {
      report(5,"",_LINE_);       
      return(FAIL);
      }

   t = next_token();

   if (t->token!= SEMICOLON)
      {
      report(4,"",_LINE_);       
      return(FAIL);
      }

   /******************************************************/
   /* Insert an appropriate END node into the parse tree */
   /******************************************************/

   e_ptr = allocate_node (END);
   e_ptr->partner = lp_ptr;
   insert_node (stmt_ptr,e_ptr);

   nesting--;

   return(OK);

   }


/***************************************************************************/
/* Parse that part of a <do-spec> that follows the = token.                */
/***************************************************************************/

PARSER

parse_do_spec (Loop_ptr loop_ptr)

{

long            p;

/***********************************************************************/
/* NOTE: When we get called the token EQUALS has already been consumed */
/***********************************************************************/

loop_ptr->start = parse_expression();

if (loop_ptr->start == NULL)
   {
   report(160,"",_LINE_);
   return(FAIL);
   }

t = next_token();

/***********************************************************************/
/* If a BY or TO was encountered, then process its expression and test */
/* for the presence of the optional other BY or TO.                    */
/***********************************************************************/ 

if ((t->token == TO) || (t->token == BY))
   {
   if (t->token == TO)
      {
      loop_ptr->finish = parse_expression();
      if (loop_ptr->finish == NULL)
         {
         report(157,"",_LINE_);
         return(FAIL);
         }

      p = get_pos();

      t = next_token();

      if (t->token == BY)
         {
         loop_ptr->step = parse_expression();
         if (loop_ptr->step == NULL)
            {
            report(158,"",_LINE_);
            return(FAIL);
            }
         }
      else
         {
         set_pos(p);
         goto end_of_by_to_stuff;
         } 
      } 

   if (t->token == BY)
      {
      loop_ptr->step = parse_expression();
      if (loop_ptr->step == NULL)
         {
         report(158,"",_LINE_);
         return(FAIL);
         }

      p = get_pos();

      t = next_token();

      if (t->token == TO)
         {
         loop_ptr->finish = parse_expression();
         if (loop_ptr->finish == NULL)
            {
            report(157,"",_LINE_);
            return(FAIL);
            }
         }
      else
         {
         set_pos(p);
         goto end_of_by_to_stuff;
         } 

      }

   }
else
   {
   /*****************************************************************/
   /* The tokens TO or BY were not present, so lets test for REPEAT */
   /*****************************************************************/   
   if (t->token == REPEAT)
      {
      loop_ptr->repeat = parse_expression();
      if (loop_ptr->repeat == NULL)
         {
         report(159,"",_LINE_);
         return(FAIL);
         } 
      }
   }


end_of_by_to_stuff:


return(OK);

}



/***************************************************************************/
/*          This function parses a PL/1 'begin block' construct.           */
/***************************************************************************/

PARSER

   parse_begin (Block_ptr parent_block_ptr,Any_ptr g_ptr)

   {

   Begin_ptr   b_ptr;
   Any_ptr     stmt_ptr;
   End_ptr     e_ptr;
   char        name[32]="";

   b_ptr = g_ptr;

   t = next_token();

   if (t->token!= BEGIN)
      {    
      return(FAIL);
      }

   t = next_token();

   if (t->token!= SEMICOLON)
      {       
      return(FAIL);
      }

   /* new block */

   
   strcat(name,"BEGIN.");
   strcat(name,strpbrk(line_no,"1234567890"));

   /*****************************************/
   /* Create a new block node.              */
   /*****************************************/

   current_parent = insert_block (parent_block_ptr,
                                  name,
                                  0,      /* NOT a func    */
                                  0,      /* NOT recursive */
                                  0,      /* NO args       */
                                  0);     /* NO symbols    */

   b_ptr = g_ptr;

   b_ptr->block = current_parent;
   b_ptr->block->begin = 1;

   current_parent->first_stmt = b_ptr;

   strcpy(b_ptr->line_no,line_no);
   strcpy(b_ptr->block->line,line_no);

   nesting++;

   stmt_ptr = b_ptr;
   
   while (parse_statement(stmt_ptr))   /* any number of */
         {
         stmt_ptr = next_stmt (stmt_ptr);
         /*******************************************************/
         /* If the statement just parsed resulted in any nodes  */
         /* being chained to the node passed in, then pass the  */
         /* last node in next time round.                       */
         /*******************************************************/
         }

   /******************************************************/
   /* OK lets see if the call failed because of an END   */
   /******************************************************/


   t = next_token();


   if (t->token!= END)
      {
      report(5,"",_LINE_);       
      return(FAIL);
      }

   t = next_token();


   if (t->token!= SEMICOLON)
      {
      report(4,"",_LINE_);       
      return(FAIL);
      }

   /******************************************************/
   /* Insert an appropriate END node into the parse tree */
   /******************************************************/

   e_ptr = allocate_node (END);
   e_ptr->partner = b_ptr;
   b_ptr->end = e_ptr; 
   insert_node (stmt_ptr,e_ptr);

   nesting--;

   current_parent = parent_block_ptr;

   return(OK);

   }


/***************************************************************************/
/*  This function parses a PL/1 'expression'.                              */
/*  The technique is based on operator precedence.                         */ 
/***************************************************************************/

Any_ptr 
static

   parse_expression (void)

   {

   long        p;
   Any_ptr     el_ptr;
   Any_ptr     er_ptr;
   Oper_ptr    nextop = NULL;
   Any_ptr     topop  = NULL;
   Ref_ptr     r_ptr;
   Any_ptr     operator_stack[64] = {NULL};
   Any_ptr     operand_stack [64] = {NULL};
   short       operator_stack_ptr = 0;
   short       operand_stack_ptr  = 0;


   /*******************************************************************/
   /*        Read the first token in this expression string.          */
   /*******************************************************************/

   p = get_pos();
   t = next_token();
   set_pos(p); /* Pretend we never even read the PL1NAME above */

   if ((value(t) == 0) && (t->token != LPAR))
      {
	  if (t->token != SEMICOLON)
         t = next_token();
      report(46,t->lexeme,_LINE_);
      return(NULL);
      }

   while (value(t) || (t->token == LPAR))
         {
         if (t->token == LPAR)
            {
            t = next_token(); /* we MUST consume the token */    
            r_ptr = parse_expression();
            t = next_token(); /* This HAS to be an )   */
            }
         else
            {          
            r_ptr = parse_reference();
            if (r_ptr == NULL)  /* invalid variable reference  */
               return(NULL);
            }

         push_operand(r_ptr);  /* save the ptr on the operand stack */

         p = get_pos();
         t = next_token();

         /*************************************************************/
         /* Read the next token, this will be either another operator */
         /* or a semicolon etc, or keyword.                           */
         /*************************************************************/
         if ((t->token != SEMICOLON) && 
             (t->token != COLON) && 
             (t->token != COMMA) &&
             (t->keyword == 0) &&
             (t->token != RPAR))     
            if (is_operator(t->token) == 0)
               {
               report (46,t->lexeme,_LINE_);
               return(NULL);
               }
            else
               {                
               nextop = allocate_node(t->token);
               nextop -> type = t->token; /*  + - * / etc  */
               }
         else
            { /* a semicolon etc, a keyword or an RPAR, was read */
            nextop = NULL;
            set_pos(p);
            /* goto end_parse; */
            }

         while ((priority(topop) >= priority(nextop)) && (topop != NULL))
               {
               er_ptr = pop_operand();
               el_ptr = pop_operand();
               ((Oper_ptr)topop)-> left_ptr = el_ptr;
               ((Oper_ptr)topop)-> rite_ptr = er_ptr;
               push_operand (topop);     
               topop = pop_operator();
               }

         push_operator (topop);
         topop = nextop;
         if (topop != NULL)
            {
            p = get_pos();
            t = next_token();
            set_pos(p);    
            }  
         else
            goto end_parse;
         }

end_parse:
  
   if (topop != NULL)
      {
      if (t->keyword)
         {
         set_pos(p);
         return (pop_operand());
         }
      report (46,t->lexeme,_LINE_); /* was:  report(98,"",_LINE_);  */
      return(NULL);
      }
   else
      return (pop_operand());           

   }

/*************************************************************************/
/*                   This parses a stop statement.                       */
/*************************************************************************/

PARSER

   parse_stop (void)

   {

   t = next_token();

   if (t->token!= STOP)
      {
      report(30,"",_LINE_);
      return(FAIL);
      }

   t = next_token();

   if (t->token!= SEMICOLON)
      {
      report(4,"",_LINE_);
      return(FAIL);
      }

   return(OK);

   }

/**************************************************************************/
/*           This function parses a PL/1 select statement.                */
/**************************************************************************/

PARSER

   parse_select (Any_ptr g_ptr)

   {
 
   Select_ptr    s_ptr;
   Expr_ptr      e_ptr;
   Any_ptr       stmt_ptr;
   End_ptr       en_ptr;
   short         tp;

   s_ptr = g_ptr;

   t = next_token();

   if (t->token!= SELECT)
      {    
      report(30,"",_LINE_);
      return(FAIL);
      }

   /*******************************************************************/
   /*           Either a semicolon or a LPAR may now follow           */
   /*******************************************************************/

   t = next_token();

   if ((t->token != SEMICOLON) && (t->token != LPAR))
      {
      report(46,t->lexeme,_LINE_);
      return(FAIL);
      }
  
   nesting++; 

   if (t->token == LPAR)
      {
      /******************************************************************/
      /* OK We MUST have an expression in here, so lets check for one ! */
      /******************************************************************/
      e_ptr = parse_expression();

      if (e_ptr == NULL)
         return(FAIL);
        
      t = next_token();

      if (t->token != RPAR)
         {
         report(46,t->lexeme,_LINE_);
         return(FAIL);
         }
    
      t = next_token();

      if (t->token != SEMICOLON)
         {
         report(4,"",_LINE_);
         return(FAIL);
         }
      s_ptr->expr_ptr = e_ptr;
      }
   /*********************************************************************/
   /*          OK Now parse all (any) embedded WHEN clauses.            */
   /*********************************************************************/

   stmt_ptr = s_ptr;

   /***********************************************************************/
   /* We can have any number of statements in here, but pass2 will check  */
   /* that they are all WHEN's and possibly a final OTHERWISE.            */
   /***********************************************************************/

   while (parse_statement(stmt_ptr))
         {
         /*******************************************************/
         /* If the statement just parsed resulted in any nodes  */
         /* being chained to the node passed in, then pass the  */
         /* last node in next time round.                       */
         /*******************************************************/
         stmt_ptr = next_stmt (stmt_ptr);
         tp = nodetype(stmt_ptr);
         if ((tp != WHEN) & (tp != OTHER))
            report(95,"",_LINE_);
         }

   /******************************************************/
   /* OK lets see if the call failed because of an END   */
   /******************************************************/

   t = next_token();

   if (t->token!= END)
      {
      report(5,"",_LINE_);       
      return(FAIL);
      }

   t = next_token();

   if (t->token!= SEMICOLON)
      {
      report(4,"",_LINE_);       
      return(FAIL);
      }

   /******************************************************/
   /* Insert an appropriate END node into the parse tree */
   /******************************************************/

   en_ptr = allocate_node (END);
   en_ptr->partner = s_ptr;
   insert_node (stmt_ptr,en_ptr);

   nesting--;

   return(OK);

   }

/**************************************************************************/
/*            This function parses a PL/1 WHEN clause.                    */
/**************************************************************************/

PARSER

   parse_when (Any_ptr g_ptr)

   {

   When_ptr    w_ptr;
   Expr_ptr    e_ptr;
   short       ecount = 0;
      
   w_ptr = g_ptr;
              
   t = next_token();

   if (t->token != WHEN)
      {
      report(30,"",_LINE_);   /* Probably the OTHERWISE or END */
      return(FAIL);
      }

   t = next_token();

   if ((t->token != LPAR) && 
       (t->token != ANY) &&
       (t->token != ALL))
      {
      report(8,"",_LINE_);
      return(FAIL);
      }

   if (t->token == ANY)
      {
      w_ptr->any = 1;
      t = next_token();
      }
   else
      if (t->token == ALL)
         { 
         w_ptr->all = 1;
         t = next_token();
         }

   if (t->token != LPAR)
      {
      report(8,"",_LINE_);
      return(FAIL);
      }

   e_ptr = parse_expression();

   if (e_ptr == NULL)
      return(FAIL);

   w_ptr->expr_ptr[ecount] = e_ptr;

   t = next_token() ;

   while (t->token == COMMA)
         {
         ecount++;
         e_ptr = parse_expression();
         if (e_ptr == NULL)
            return(FAIL);
         w_ptr->expr_ptr[ecount] = e_ptr;
         t = next_token();
         }

   if (t->token!= RPAR) 
      {
      report(10,"",_LINE_);
      return(FAIL);
      }

   /*********************************************************************/
   /* OK We have a valid WHEN (expr), so we MUST have a valid stmt next */
   /*********************************************************************/

   w_ptr->setting_unit = 1;

   if (parse_statement(w_ptr) == 0)
      return(FAIL);

   w_ptr->setting_unit = 0;

   return(OK);

   } 

/**************************************************************************/
/*         This function parses a PL/1 OTHERWISE clause.                  */
/**************************************************************************/

PARSER

   parse_other (Any_ptr g_ptr)

   {

   Other_ptr   o_ptr;
      
   o_ptr = g_ptr;
              
   t = next_token();

   if (t->token != OTHER)
      {
      report(30,"",_LINE_);   
      return(FAIL);
      }

   o_ptr->setting_unit = 1;

   if (parse_statement(o_ptr) == 0)
      return(FAIL);

   o_ptr->setting_unit = 0; 

   return(OK);

   } 

/**************************************************************************/
/*   This function parses a PL/1 structure declaration.                   */
/**************************************************************************/
    
PARSER

   parse_structure (Token_ptr  curr_ptr, Symbol_ptr struc_parent)

   /* Token is first field in new struc */

   {

   long        p;
   Token       save;
   Token_ptr   save_ptr;
   Token_ptr   tokn_ptr;
   Symbol_ptr  temp_ptr; 
   short       result;
   short       level; 
   short       a,b;

   level = (short)(atol(curr_ptr->lexeme));  /* get the level number */

   save = *curr_ptr;

   save_ptr = &save; /* curr_ptr; */

   p = get_pos();

   while ((save_ptr->token) == NUMERIC)
         {
         if (strcmp((curr_ptr->lexeme),(save_ptr->lexeme)) > 0)
            {
            set_pos (p);
            return(OK);
            } 

         tokn_ptr = next_token();

         if (tokn_ptr->keyword)
            {
            tokn_ptr->keyword = 0;
            tokn_ptr->token   = PL1NAME;
            }
         
         if (tokn_ptr->token   != PL1NAME)
            {      
            report(39,"",_LINE_);  /* bad syntax in structure dcl */
            return(FAIL);
            }

         /*************************************************************/
         /* OK We can now insert the symbol table entry for this name */
         /*************************************************************/


         /* Debugging only. */
         /* printf("Struc %s at level %d found.\n",tokn_ptr->lexeme,level); */

         temp_ptr = add_symbol (current_parent,
	                             tokn_ptr->lexeme,
	                             tokn_ptr->token,
	                             tokn_ptr->keyword,
	                             0,
	                             0,
	                             0,
	                             0,
	                             0,
	                             1,
                                level,
                                struc_parent,
	                            line_no);
         
         /*****************************************************************/
         /* We must see if next token is a (, if it is, then we have an   */
         /* array on our hands !                                          */
         /*****************************************************************/

         p = get_pos();

         t = next_token();

         set_pos(p);

         if (t->token == LPAR)
            if (parse_dimension(temp_ptr) == 0)
               {
               skip_to_next(SEMICOLON);
               return(FAIL);
               }

         /**********************************************************/
         /* If this is a level 1 parse, then check to see if the   */
         /* struc is based, defined etc.                           */
         /**********************************************************/ 

         if (level == 1)
            {
            p = get_pos();
            t = next_token();
            set_pos(p);
            
            if (t->token != COMMA)
               if (parse_structure_attribute(temp_ptr) == 0)
                  {
                  skip_to_next(SEMICOLON);
                  return(FAIL);
                  }  
            }


         /**********************************************************/
         /* temp_ptr (above) will become the parent of any nested  */
         /* structure members .                                    */
         /**********************************************************/

         p = get_pos();

         tokn_ptr = next_token();

         if ((tokn_ptr->token) == COMMA)
            {
            tokn_ptr = next_token();
            
            if (tokn_ptr->token == PL1NAME)  /* HWG45 */
               {
               set_pos(p);
               return(SEMICOLON); /* tell recursive caller weve done */
               }                  /* parsing this struc.             */ 

            if ((tokn_ptr->token) != NUMERIC)
               {
               report(40,"",_LINE_); /* bad syntax . . .  */
               return(FAIL);
               }

            a = atoi(tokn_ptr->lexeme);
            b = atoi(save_ptr->lexeme);

            if (a <= b)
               {
               report(41,"",_LINE_);
               return(FAIL);
               }

            /*********************************************************/
            /* Now recursively invoke this function, to parse a new  */
            /* sub-structure.                                        */
            /*********************************************************/

            result = parse_structure(tokn_ptr,
                                     temp_ptr);

            if (result == SEMICOLON)
               return(SEMICOLON);

            if (result == FAIL)
               return(FAIL);
        
            }
         else   
            {
            /****************************************************/
            /* Verify that each scalar field has SAME level num */
            /****************************************************/

            if (strcmp((save_ptr->lexeme),(curr_ptr->lexeme)) != 0)
               {
               report(43,"",_LINE_);
               return(FAIL);
               }
      
            set_pos(p);

            if (parse_field(temp_ptr) != OK)
               return(FAIL);

            tokn_ptr = next_token();

            if ((tokn_ptr->token) == SEMICOLON)
	            return(SEMICOLON);

            if ((tokn_ptr->token) != COMMA)
	            {
	            report(42,"",_LINE_);
	            return(FAIL);
	            }
            }

         p = get_pos();

         save = *(next_token());

         save_ptr = &save; /* next_token(); */
           
         } /* while */

        /***********************************************************/
        /* We end up here if a non-numeric token leads struc field */
        /***********************************************************/ 

        if (save_ptr->token == PL1NAME)    /* HWG45 */
           {
           set_pos(p);
           return(SEMICOLON); /* tell recursive caller weve done */
           }                  /* parsing this struc.             */ 

        report (40,"",_LINE_);   /* was 39 but i altered it after testing */
        return(FAIL);     /* with strucy.pl1                       */ 

   }
   
/**************************************************************************/
/*  This function parses a structure fields attributes.                   */
/**************************************************************************/

PARSER

   parse_field (Symbol_ptr sym_ptr)

   {

   long        p;
   short       state;
   short         comma_seen = 0;

   /**************************************************************/
   /*   Set the following symbol table values to a safe value    */
   /**************************************************************/

   sym_ptr -> prec_1   = 0;
   sym_ptr -> offset  = 0;
   sym_ptr -> bad_dcl = 0;
   strcpy(sym_ptr -> line,line_no);

   /*****************************************************************/
   /* PL/1's powerful declaration facility is implemented as an FSM */
   /*****************************************************************/

   state = 1;
  
   p = get_pos();
 
   t = next_token();

   while (((t->token) != SEMICOLON) && ((t->token) != COMMA))
       {
       switch (state) {

       case(1):
           {
           switch (t->token) {

             case(ENTRY):
             case(POINTER):
                 { /* verify data type not already seen !  */
                 if ((sym_ptr->type) != 0)
                    {
                    /* error */
                    sym_ptr->bad_dcl = 1;
                    report(35,sym_ptr->spelling,_LINE_);
                    break;
                    }
                 sym_ptr->type = t->token;
                 sym_ptr->prec_1 = 8; /* two words ! */
                 sym_ptr->known_size = 1;
                 break;
                 }

             case(PICTURE):
                 { /* Verify data type not already seen ! */
                   /* and name already declared */
                 if (((sym_ptr->type) != 0) &&
                     ((sym_ptr->declared) == 1))
                    {
                    /* Error here */
                    sym_ptr->bad_dcl = 1;
                    report(35,sym_ptr->spelling,_LINE_);
                    break;
	               }
                 state = 8; /* 1st action when in state 1   ! */
               sym_ptr->type = t->token;
                 break;
               }

             case(CHARACTER):
             case(BINARY):
             case(BIT):
             case(DECIMAL):
                 { /* Verify data type not already seen ! */
                 if (((sym_ptr->varying) != 0) &&
                     ((t->token) == CHARACTER))
                    {
                    sym_ptr->bad_dcl = 1l;
                    report(45,sym_ptr->spelling,_LINE_);
                    break;
                    } 
                 if ((sym_ptr->type) != 0)
                    {
                    /* Error here */
                    sym_ptr->bad_dcl = 1;
                    report(35,sym_ptr->spelling,_LINE_);
                    break;
	               }
                 state = 2; /* 1st action when in state 1   ! */
               sym_ptr->type = t->token;
                 break;
               }
             case(FIXED):
             case(D_FLOAT):
                 { /* Verify scale not already seen */
                 if ((sym_ptr->scale) != 0)
                    {
                    /* Error here */
                    sym_ptr->bad_dcl = 1;
                    report(37,sym_ptr->spelling,_LINE_);
                    }
                 state = 1;
                 sym_ptr->scale = t->token;
                 break;
                 }
             case(VARYING):
                 { /* Verify varying not already seen */
                 if (((sym_ptr->type) != 0) && 
                     ((sym_ptr->type) != CHARACTER))
                    {
                    sym_ptr->bad_dcl = 1;
                    report(45,sym_ptr->spelling,_LINE_);
                    }
                         
                 if ((sym_ptr->varying) != 0)
                    {
                    /* Error here */
                    sym_ptr->bad_dcl = 1;
                    report(44,sym_ptr->spelling,_LINE_);
                    }
                 state = 1;
                 sym_ptr->varying = 1; /* t->token; */
                 break;
                 }
           default:
               {
               /* error */
                 report(46,t->lexeme,_LINE_);
               }
           } /* The inner switch  */
           break;  /* The outer case(1) clause ! */
           } /* case(1) */
         case(2):
             {
             switch (t->token) {

             case(LPAR):
                 {
                 state = 3;
                 break;
                 }
             default :
                 {
                 report(46,t->lexeme,_LINE_);
                 /*****************************************************/
                 /* There is no size specification for this data type */
                 /*****************************************************/
                 }
             } /* inner switch */
             break; /* case(2) */
             } /* case(2) */
         case(3):
             {
             switch (t->token) {    

             case(STAR):
             case(PL1NAME):
                 {
                 if (t->token == PL1NAME)
                    if (get_symbol(current_parent,t->lexeme) == NULL)
                       {
  		               add_symbol (current_parent,
                       t->lexeme,
                       t->token,
                       t->keyword,
                       0,          /* Unknown length    */
                       0,          /* Unknown scale     */
                       0,          /* Unknown class     */
                       0,          /* Unknown scope     */
                       0,
                       0,
                       0,
                       NULL,
                       line_no);

                       }
                 sym_ptr->prec_1 = 0;
                 state = 4;
                 break;
                 }
             case(NUMERIC):
                 {
                 /************************************************/
                 /* convert the numeric string to a size value   */
                 /************************************************/
                 sym_ptr->known_size = 1; /* specified size */
                 if (comma_seen)
                    sym_ptr->prec_2     = atoi (t->lexeme);
                 else
                    sym_ptr->prec_1     = atoi (t->lexeme); 
                 /**************************************************/
                 /* See if the next token is a   ,   if so stay in */
                 /* this state !                                   */
                 /**************************************************/
                 p = get_pos();
                 t = next_token();
                 if (t->token != COMMA)
                    {
                    set_pos(p);
                    state = 4;
                    }
                 comma_seen = 1;
                 break;
                 }
             default:
                 {
                 report (9,"",_LINE_);
                 }
             } /* inner switch */ 
             break; /* case(3) */
             } /* case(3) */
         case(4):
             {
             switch (t->token) {
      
             case(RPAR):
                 { /* Good the data type is Fully specified */
                 state = 1;
                 break;   
                 }
             default:
                 {
                 report(46,t->lexeme,_LINE_);
                 /* error */
                 }
             } /* inner switch */
             break; /* case(4) */
             } /*   case(4) */
        case(6):
             {
             switch (t->token) {    

             case(PL1NAME):
                 {
                 if (t->token == PL1NAME)
                    if (get_symbol(current_parent,t->lexeme) == NULL)
                       {
                       add_symbol (current_parent,
                                   t->lexeme,
                                   t->token,
                                   t->keyword,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   NULL,
                                   line_no);
                       }
                 sym_ptr->prec_1 = 0;
                 state = 7;
                 break;
                 }
             default:
                 {
                 report(46,t->lexeme,_LINE_);
                 } /* syntax error in defined */ 
             } /* inner switch */
             break; /* case (6) */
             } /* case(6) */
         
         case(7):
             {
             switch (t->token) {
      
             case(RPAR):
                 { /* Good tified */
                 state = 1;
                 break;   
                 }
             default:
                 {
                 report(46,t->lexeme,_LINE_);
                 /* error */
                 }
             } /* inner switch */
             break; /* case(7) */
             } /*   case(7) */

        case(8):
            {
            if (t->token != STRING)
               {
               report(46,t->lexeme,_LINE_);
               break;
               }
            else
               sym_ptr->pic_text = t->lexeme;
            }

        default:
             {
             }
         } /* outer switch */

         p = get_pos();

         t = next_token();

         }  /* while loop */

   /****************************************************************/
   /* At the end of this loop we must reset our lexical position   */
   /* so that the structure parser can re-read the , or ; that     */
   /* caused this loop to end. Recall that parse_structure expects */
   /* to find these tokens !                                       */
   /****************************************************************/

   set_pos (p); 

   return(OK);

   }  /* parse_field  */

/**************************************************************************/
/* This function parses the comma-list that appears in the procedures     */
/* 'options' option.                                                      */
/**************************************************************************/

void 
static

   parse_options (Procedure_ptr p_ptr)

   {

   long             p;

   if (p_ptr->proc->depth > 0)
      {
      report(119,"",_LINE_);
      } 

   p = get_pos();

   t = next_token();

   while (t->token != RPAR)
         {
         switch (t->token) {

         case(MAIN):
             {
             p_ptr->proc->main = 1;
             break;
             }
         case(STACK):
             {
             t = next_token();
             if (t->token != LPAR)
                {
                report(46,t->lexeme,_LINE_);
                break;
                }
             else
                {
                t = next_token();
                if (t->token != NUMERIC)
                   {
                   report(46,t->lexeme,_LINE_);
                   break;
                   }
                else
                   {
                   global_stack_size = atoi(t->lexeme);
                   t = next_token();
                   if (t->token != RPAR)
                      {
                      report(46,t->lexeme,_LINE_);
                      break;
                      }
                   }
                }
             break;
             }
         default:  
             {
             report(118,"",_LINE_);
             }
         }

         p = get_pos();
         t = next_token();

         if (t->token != RPAR)
            if (t->token != COMMA)
               report(46,t->lexeme,_LINE_);   
            else
               {
               p = get_pos();
               t = next_token();
               }
         }

   set_pos(p);

   } 
   
/**************************************************************************/
/*       Parse a PL/1 reference, and construct a reference tree.          */
/**************************************************************************/

Ref_ptr 
static

   parse_reference (void)

   {

   Ref_ptr      r_ptr;
   long         p;
  
   r_ptr = allocate_node (REFERENCE);
  
   if (parse_subname(r_ptr))
      {
  
      p = get_pos();
  
      t = next_token();
  
      switch (t->token) {
      
      case(DOT):
          r_ptr->dot_ptr = parse_reference();
          break;
      case(POINTING):
          r_ptr->ptr_ptr = parse_reference();
          break;
      default:
          {
          set_pos(p);
          }            
      }
  
      }

   return(r_ptr);

   }

/**************************************************************************/
/* This function parses a 'subname' ie:  IDENTIFIER or IDENTIFIER() or    */
/* IDENTIFIER(expr,...) or IDENTIFIER(expr)(expr)(expr).....              */
/**************************************************************************/     

PARSER

   parse_subname (Ref_ptr r_ptr)

   {

   long           p;
 
   t = next_token();

   if (lookahead_in_progress == 0)
      if (t->keyword)
         {
         t->keyword = 0;
         t->token   = PL1NAME;
         }

   if ((value(t) == 0) && (builtin(t->lexeme) == 0))
      {
      report(46,t->lexeme,_LINE_);  /* Expected identifier not found */
      return(FAIL);
      }

   r_ptr->spelling = t->lexeme;

   /********************************************************************/
   /* If the symbol is a constant, then this is actually a declaration */
   /* of the constant, and can thus be resolved Now.                   */
   /********************************************************************/

   if (t->token != PL1NAME)  /* ie a constant */
      {
      r_ptr->symbol = get_symbol(current_parent,t->lexeme);

      if (r_ptr->symbol == NULL)
         r_ptr->symbol = add_symbol (current_parent,
                                     t->lexeme,
                                     t->token,
                                     0,
                                     0,          /* 32 Bit address    */
                                     0,          /* Unknown scale     */
                                     CONSTANT, 
                                     0, 
                                     INTERNAL,
                                     1,          /* This a Label !    */
                                     0,
                                     NULL,
                                     line_no);
      r_ptr->attribs = r_ptr->symbol;
      }


   /*****************************************************************/
   /* If the next token is an LPAR then repeatedly parse arg exprs  */
   /*****************************************************************/

   p = get_pos();

   t = next_token();

   while (t->token == LPAR)
         {
         p = get_pos();

         t = next_token();

         if (t->token == RPAR)
            {
            r_ptr->null_list = 1;
            return(FAIL); /* null arglist, so exit */
            }

         set_pos (p);

         if (parse_sublist(r_ptr) == 0)
            return(FAIL);
   
         t = next_token();

         if (t->token != RPAR)
            {
            report(46,t->lexeme,_LINE_);
            return(FAIL);
            }

         p = get_pos();

         t = next_token();
         }

   set_pos(p);


   return(OK);

   }

/***************************************************************************/
/* This function parses a subscript list, that appears in a PL/1 reference */
/***************************************************************************/

PARSER

   parse_sublist (Ref_ptr r_ptr)

   {

   Sub_ptr        sb_ptr;
   Any_ptr        e_ptr;
   long           p; 


   e_ptr = parse_expression();

   if (e_ptr == NULL)
      return(FAIL);

   sb_ptr = allocate_node (SUBSCRIPT);
   sb_ptr->expression = e_ptr;

   append_subscript (r_ptr,sb_ptr); /* defined in pass2.c */

   p = get_pos();

   t = next_token();

   while (t->token == COMMA)
         {
         e_ptr = parse_expression();

         if (e_ptr == NULL)
            return(FAIL);
         
         sb_ptr = allocate_node (SUBSCRIPT);
         sb_ptr->expression = e_ptr;

         append_subscript (r_ptr,sb_ptr); 

         p = get_pos();

         t = next_token();

         }

   set_pos(p);

   return(OK);

   }
           
     
/**************************************************************************/
/*  This func scans the token stream exiting when a 'token' has been read */
/**************************************************************************/

void 
static

   skip_to_next (short token)

   {

   Token_ptr    t;

   t = next_token();

   while ((t->token != token) & (t->token!= END_OF_SOURCE))
         t = next_token();

   }

/***************************************************************************/
/*          This function retrieves the next 'logical' token.              */
/***************************************************************************/

Token_ptr 
static

   next_token (void)

   {

   Token_ptr     temp_ptr;

   if (real_ptr == virt_ptr)
      {
      real_ptr = increment(real_ptr);
      virt_ptr = increment(virt_ptr);
      temp_ptr = get_token();
      /**********************************************/
      /* Save the contents of this token variable   */
      /**********************************************/ 
      chain[real_ptr].token   = temp_ptr->token;
      chain[real_ptr].keyword = temp_ptr->keyword; 
      chain[real_ptr].lexeme = temp_ptr->lexeme;
      temp_ptr = &chain[real_ptr];
      if (temp_ptr->token == END_OF_SOURCE)
         if (parse_in_progress)
            {
            eof = 1;
            report(22,"",_LINE_);
            longjmp(escape,1); /* This causes pass1 to exit */
            }
      return (temp_ptr);
      }

   /****************************************************/
   /* OK The token held in the buffer must be returned */
   /****************************************************/

   virt_ptr = increment(virt_ptr);

   return (&chain[virt_ptr]);

   }

/****************************************************************************/
/* This function will increment a 'pointer' to a buffer in a circular chain */
/****************************************************************************/

long 

   increment (long pr)

   {

   if (pr == (CHAIN_SIZE - 1))
      return(0);

   return (++pr);

   }

/****************************************************************************/
/* This function returns the current position in the circular buffer        */
/****************************************************************************/

long 

   get_pos(void)

   {
   
   return (virt_ptr);

   }

/***************************************************************************/
/* This function sets the 'current' position in the circular input buffer  */
/***************************************************************************/
 
void 

   set_pos (long p)

   {

   virt_ptr = p;

   }

/***************************************************************************/
/* This function will return TRUE if its argument is a relation operator   */
/***************************************************************************/

short relop (short t)

    {

    switch (t) {

    case(EQUALS):
        return(1);
    case(GT):
        return(1);
    case(GE): 
        return(1);
    case(LT):
        return(1);
    case(LE):
        return(1);
    case(NOT):
        return(1);
    case(NOTEQUAL):
        return(1);
    default:
        return(0);
    }

    }

/***************************************************************************/
/*     Return true if the supplied token is a PL/1 language operator.      */
/***************************************************************************/

short is_operator (short token)

    {

    switch (token) {
    case(OR):
        return(1);
    case(AND):
        return(1);
	case(NOT):
	    return(1);
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
        return(1);
    case(CONCAT):
        return(1);
    case(PLUS):
    case(MINUS):
        return(1);
    case(TIMES):
    case(DIVIDE):
        return(1);
    default:
        return(0);

    }
    }
        
/***************************************************************************/
/* This function returns true if the token passed-in represents a value.   */
/***************************************************************************/

short value (Token_ptr tkn)

    {

    if ((tkn->token == PL1NAME) ||
        (tkn->token == NUMERIC) ||
        (tkn->token == STRING)  ||
        (tkn->token == BIT_STRING) ||
        (tkn->keyword))
        return(1);

    return (0);

    } 

/***************************************************************************/
/* This function attempts to determine wether or not a statement is an     */
/* assignment, or a valid PL/1 construct. To do this requires an arbitrary */
/* degree of analysis, but essentially involves reading tokens until we    */
/* either get a syntax error (in an expression), or prove that we have a   */
/* valid language construct.                                               */
/* For example these two statements are both legal PL/1, but only the      */
/* latter is an assignment:                                                */
/* when (a,b) return (z);                                                  */
/* when (a,b) = 23.4;                                                      */
/* The biggest problem we have here, is not the recognition, but the side  */
/* effect of it, namely the unrequired tree-building caused by calls to    */
/* parse_expression and parse_reference. The flag 'lookahead_in_progress'  */
/* Is used to signify that nodes should not REALLY be, allocated etc.      */
/***************************************************************************/

short 
static

   this_isnt_a_keyword (void)

   {

   Ref_ptr      r_ptr;
   long         p;

   p = get_pos();
   t = next_token();
   set_pos(p);

   if (t->token == SEMICOLON) /* ie a null statement */
      return(0); 

   lookahead_in_progress = 1; /* the node allocators will only */
                              /* allocate dummy-static-nodes   */

   r_ptr = parse_reference();

   /******************************************************************/
   /* A Null ref-ptr probably means a syntax error in an assignment  */
   /******************************************************************/

   if (r_ptr == NULL)
      {
      lookahead_in_progress = 0;
      return(1); /* yes this is probably an assignment */
      }

   /***********************************************************************/
   /* OK We have a valid reference, so if the next token is an = then     */
   /* we have an assignment, otherwise we assume its a language construct */
   /***********************************************************************/

   t = next_token();

   if ((t->token == EQUALS) || (t->token == COLON)) /* ie a label */
      {
      lookahead_in_progress = 0;
      return(1);
      }
   else
      {
      lookahead_in_progress = 0;
      return(0);
      } 

   } 

/* end */
