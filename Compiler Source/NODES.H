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
/* This file defines all node-types used to build the parse-tree for the   */
/* processed source file.                                                  */
/* Each PL/1 statement type has a corresponding node type.                 */
/* The symbol table nodes are also defined in here.                        */
/* Each node that corresponds to a PL/I statement (unlike arg nodes etc)   */
/* begins with: type, line_no, stmt_ptr, the latter points to the next     */
/* PL/I statement.                                                         */
/*                                                                         */
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
/*  26-04-96   HWG       Enhanced loop_node                                */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

typedef unsigned				         Bit;

typedef         void                    *Any_ptr; 
typedef struct  dummy_node              *Dummy_ptr;
typedef struct  leave_node              *Leave_ptr;
typedef struct  procedure_node          *Procedure_ptr;
typedef struct  assignment_node         *Assignment_ptr;
typedef struct  argument_node           *Argument_ptr;
typedef struct  do_node                 *Do_ptr;
typedef struct  begin_node              *Begin_ptr;
typedef struct  end_node                *End_ptr;
typedef struct  call_node               *Call_ptr;
typedef struct  if_node                 *If_ptr;
typedef struct  while_node              *While_ptr;
typedef struct  until_node              *Until_ptr;
typedef struct  loop_node               *Loop_ptr;
typedef struct  return_node             *Return_ptr;
typedef struct  allocate_node           *Allocate_ptr;
typedef struct  free_node               *Free_ptr; 
typedef struct  label_node              *Label_ptr;
typedef struct  goto_node               *Goto_ptr;
typedef struct  stop_node               *Stop_ptr;
typedef struct  expr_node               *Expr_ptr;
typedef struct  oper_node               *Oper_ptr;
typedef struct  dim_node                *Dim_ptr;
typedef struct  symbol_node             *Symbol_ptr;
typedef struct  block_node              *Block_ptr;
typedef struct  procedure_node          *Entry_ptr;
typedef struct  ref_node                *Ref_ptr;
typedef struct  sub_node                *Sub_ptr;
typedef struct  put_node                *Put_ptr;
typedef struct  on_node                 *On_ptr;
typedef struct  signal_node             *Signal_ptr;
typedef struct  select_node             *Select_ptr;
typedef struct  when_node               *When_ptr;
typedef struct  other_node              *Other_ptr; /* otherwise */
typedef struct  data_node               *Data_ptr;
typedef struct  init_node               *Init_ptr;
typedef struct  iarray_node             *Iarray_ptr;
typedef struct  icon1_node		        *Icon1_ptr;
typedef struct  icon2_node		        *Icon2_ptr;
typedef struct  ielem_node		        *Ielem_ptr; 
typedef struct  env_node                *Env_ptr;
typedef struct  open_node               *Open_ptr;
typedef struct  close_node              *Close_ptr;
typedef struct  delete_node             *Delete_ptr;
typedef struct  read_node               *Read_ptr;
typedef struct  rewrite_node            *Rewrite_ptr;
typedef struct  write_node              *Write_ptr;
typedef struct  get_node                *Get_ptr;
typedef struct  getfile_node            *Getfile_ptr;
typedef struct  getstring_node          *Getstring_ptr;
typedef struct  putfile_node            *Putfile_ptr;
typedef struct  putstring_node          *Putstring_ptr;
typedef struct  editin_node             *Editin_ptr;
typedef struct  intarg_node             *Intarg_ptr; 
typedef struct  outsrc_node             *Outsrc_ptr;
typedef struct  format_node             *Format_ptr;
typedef struct  formatitem_node         *Formatitem_ptr;
typedef struct  formaiteration_node     *Formatiteration_ptr;
typedef struct  dataformat_node         *Dataformat_ptr;
typedef struct  ctrlformat_node         *Ctrlformat_ptr;
typedef struct  remoteformat_node       *Remoteformat_ptr;
typedef struct  picformat_node          *Picformat_ptr;
typedef struct  strformat_node          *Strformat_ptr;
typedef struct  realformat_node         *Realformat_ptr;
typedef struct  fixedformat_node        *Fixedformat_ptr;
typedef struct  floatformat_node        *Floatformat_ptr;
 
typedef struct dummy_node  {            /* This is used to allow common   */
               short         type;        /* fields to be setup, using only */
               char        line_no[10]; /* see node.c for example usage   */
               Any_ptr     next_ptr;
               } Dummy;

typedef struct leave_node  {
              short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               Symbol_ptr  ref;
               } Leave;

typedef struct dim_node {
              short         type;
               Any_ptr     lower;
               Any_ptr     upper;
               Dim_ptr     next_ptr;
               } Dim;
                

typedef struct procedure_node {        /* ALWAYS keep this struc the SAME  */
               short        type;      /* as the entry_node below.         */
               char         line_no[10];
               Any_ptr      stmt_ptr;
               End_ptr      end; 
               Block_ptr    proc;      /* ptr to descriptor for this proc */
               Argument_ptr argument;
               Data_ptr     ret_ptr;   /* type of retruns, if func */
               } Procedure, Entry;


typedef struct assignment_node {
              short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               Ref_ptr     target;   /* The a  in a = 3;  */
               Any_ptr     source;   /* The 3  in a = 3;  */
               char        by_name;  /* Non zero if true  */
               } Assignment;

typedef struct argument_node {
              short          type;
               char         line_no[10];
               Symbol_ptr   arg_ptr;
               Argument_ptr next_arg;
               } Argument;

typedef struct do_node {
              short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               End_ptr     end;
               } Do;

typedef struct begin_node {
              short         type;
               char        line_no[10];
               Block_ptr   block;
               Any_ptr     stmt_ptr;
               End_ptr     end;
               } Begin;

typedef struct end_node {
              short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               Any_ptr     partner;   /* matching do, begin etc */
               } End;

typedef struct call_node {
              short          type;
               char         line_no[10];
               Any_ptr      stmt_ptr;
               Ref_ptr      entry;      /* The a  in call a (2);  */
               } Call;

typedef struct if_node {
               short         type;
               char          line_no[10];
               Any_ptr       stmt_ptr;
               short         action;
               Any_ptr       expression;
               Any_ptr       then_ptr;
               Any_ptr       else_ptr;
               } If;

typedef struct loop_node {
               short       type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               End_ptr     end;
               short       loop;      /* true for DO LOOP; */
               Ref_ptr     counter;
               Any_ptr     start;
               Any_ptr     finish;
               Any_ptr     step;
               Any_ptr     repeat;
               Any_ptr     while_expr;
               Any_ptr     until_expr;
               } Loop;

typedef struct return_node {
               short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               Any_ptr     value;
               } Return;

typedef struct allocate_node {
               short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               Ref_ptr     area;
               Ref_ptr     target;
               } Allocate;

typedef struct free_node {
               short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               Ref_ptr     target;
               } Free;

typedef struct label_node {
               short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               Symbol_ptr  identity;
               } Label;

typedef struct goto_node {
               short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               Ref_ptr     target;
               } Goto;

typedef struct stop_node {
               short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr;
               } Stop;

typedef struct expr_node {
               short         type;
               short         data_type;
               Any_ptr     child;
               } Expr;

typedef struct oper_node {
               short           type;
               short           data_type;
               unsigned char scale;
               unsigned char base;
               short           prec_1;
               unsigned char prec_2; 
               long          offset;
               short           bytes;
               Any_ptr       left_ptr;
               Any_ptr       rite_ptr;
               } Oper;

typedef struct sub_node {
               short         type;      /* This node refers to a single     */
               short         pass_by;   /* VALUE or REFERENCE (args only)   */
               Any_ptr     expression;  /* chained.                         */
               Sub_ptr     next_ptr;
               Sub_ptr     prev_ptr;    /* doubly linked                    */
               } Sub; 
     
typedef struct ref_node {
               short       type;
               short       num_subs;    /* No of subscripts                 */
               short       data_type;   /* type of this entire ref          */
               short       scale;       /* scale (if any) of entire ref     */
               short       null_list;   /* Does this ref have a () ?        */
               char        *spelling;   /* The lexical text of this ref node*/
               unsigned    resolved: 1; /* entry, label etc.                */ 
               Symbol_ptr  symbol;      /* The resolved symbol for this ref */
               Symbol_ptr  attribs;     /* A symbol node for attributes only*/ 
               Symbol_ptr  temp;        /* A temporary for conversion use   */
               Sub_ptr     sublist;     /* Ptr to subscript list            */
               Ref_ptr     dot_ptr;     /* Ptr to next ref if qualified     */
               Ref_ptr     ptr_ptr;     /* Ptr to next ref if a   ->   ref  */
               Any_ptr     ofx_ptr;     /* Ptr to offset expression         */ 
               } Ref;

typedef struct on_node {
               short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr; 
               Any_ptr     unit_ptr;
               Ref_ptr     ref_ptr;  /* ie condition(abc), key(str.data) */
               short         cond_type;
               short         snap;
               short         system;
               } On;   

typedef struct signal_node {
               short         type;
               char        line_no[10];
               Any_ptr     stmt_ptr; 
               Ref_ptr     ref_ptr;  /* ie, the 'abc' in signal condition(abc) */
               short         cond_type;
               } Signal;   

typedef struct env_node {
               short        type;
               char         line_no[10];
               /* no env options have yet been thought of */
               } Env; 

typedef struct open_node {
               short        type;
               char         line_no[10];
               Any_ptr      stmt_ptr;
               Ref_ptr      file_ref;
               Any_ptr      title_expr;
               Any_ptr      linesize_expr;
               Any_ptr      pagesize_expr;
               char         stream;
               char         record;
               char         input;
               char         output;
               char         update;
               char         sequential;
               char         direct;
               char         print;
               char         nonprint;
               char         keyed;
               Env_ptr      env_ptr;  
               Open_ptr     next_open; /* opens can form a commalist */
               } Open;  

typedef struct close_node {
               short        type;
               char         line_no[10];
               Any_ptr      stmt_ptr;
               Ref_ptr      file_ref;
               Env_ptr      env_ptr;
               Close_ptr    next_close; /* closes can form a commalist */
               } Close; 

typedef struct delete_node {
               short        type;
               char         line_no[10];
               Any_ptr      stmt_ptr;
               Ref_ptr      file_ref;
               Any_ptr      key_expr;
               Env_ptr      env_ptr;
               } Delete;

typedef struct read_node {
               short        type;
               char         line_no[10];
               Any_ptr      stmt_ptr;
               Ref_ptr      file_ref;
               Ref_ptr      into_ref;
               Env_ptr      env_ptr;
               Any_ptr      key_expr;
               Ref_ptr      keyto_ref;
               Ref_ptr      set_ref;
               Ref_ptr      sizeto_ref;
               } Read;

typedef struct rewrite_node {
               short        type;
               char         line_no[10];
               Any_ptr      stmt_ptr;
               Ref_ptr      file_ref;
               Env_ptr      env_ptr;
               Any_ptr      key_expr;
               Ref_ptr      from_ref;
               } Rewrite;

typedef struct write_node {
               short        type;
               char         line_no[10];
               Any_ptr      stmt_ptr;
               Ref_ptr      file_ref;
               Ref_ptr      from_ref;
               Any_ptr      keyfrom_expr;
               Env_ptr      env_ptr;
               } Write; 

typedef struct get_node {
               short            type;
               char             line_no[10];
               Any_ptr          stmt_ptr;
               Getfile_ptr      getf_ptr;
               Getstring_ptr    gets_ptr;
               } Get;

typedef struct put_node {
               short            type;
               char             line_no[10];
               Any_ptr          stmt_ptr;
               Putfile_ptr      putf_ptr;
               Putstring_ptr    puts_ptr;
               } Put;

typedef struct getfile_node {
               short            type;
               char             line_no[10];
               Ref_ptr          file_ref;
               short            skip; /* true or false */
               Any_ptr          skip_expr;
               Intarg_ptr       targ_ptr; /* non NULL for either LIST or EDIT */
               Format_ptr       fmat_ptr; /* NULL for GET LIST..., not NULL for GET EDIT... */
               } Getfile;

typedef struct getstring_node {
               short            type;
               char             line_no[10];
               Any_ptr          string_expr;
               Intarg_ptr       targ_ptr; /* non NULL for either LIST or EDIT */
               Format_ptr       fmat_ptr; /* NULL for GET LIST..., not NULL for GET EDIT... */
               } Getstring;

typedef struct putfile_node {
               short            type;
               char             line_no[10];
               Ref_ptr          file_ref;
               short            skip; /* true or false */
               Any_ptr          skip_expr;
               Any_ptr          line_expr;
               short            page;
               Outsrc_ptr       srce_ptr; /* non NULL for either LIST or EDIT */
               Format_ptr       fmat_ptr; /* NULL for PUT LIST..., not NULL for PUT EDIT... */
               } Putfile;

typedef struct putstring_node {
               short            type;
               char             line_no[10];
               Ref_ptr          string_ref;
               Outsrc_ptr       srce_ptr; /* non NULL for either LIST or EDIT */
               Format_ptr       fmat_ptr; /* NULL for PUT LIST..., not NULL for PUT EDIT... */
               } Putstring;

typedef struct intarg_node {
               short            type;
               char             line_no[10];
               Ref_ptr          target_ref; /* a reference for the I/P data */
               Intarg_ptr       intarg_ptr;
               Loop_ptr         do_ptr;
               Intarg_ptr       next_ptr;
               } Intarg;  

typedef struct outsrc_node {
               short            type;
               char             line_no[10];
               Any_ptr          source_expr; /* an expression for the O/P data */
               Outsrc_ptr       outsrc_ptr;
               Loop_ptr         do_ptr;
               Outsrc_ptr       next_ptr;
               } Outsrc;



/*************************************************************************************************************/
/*                                                  Formats !!!                                              */
/*                                                                                                           */
/* The format spec is not too complex, but could do with some explanation. The format may have a preceeding  */
/* iteration factor, if so its expression tree is rooted in 'fmat_itrn_expr'. Following an iteration factor  */
/* is either a format item OR a nested format spec, if the latter then the format spec is rooted in the      */
/* 'nest_ptr' and all format fields in the node are IGNORED.                                                 */
/*************************************************************************************************************/

typedef struct format_node {
               short                type;
               char                 line_no[10];
               Any_ptr              fmat_itrn_expr;  /* set for (<expression>)  */
               long                 fmat_itrn_value; /* set for <integer>       */
               Format_ptr           nest_ptr;   /* see comment above */
               /* F (exp[,exp])                 */
               Any_ptr              fixed_expr1;
               Any_ptr              fixed_expr2;
               /* E (exp[,exp])                 */
               Any_ptr              float_expr1;
               Any_ptr              float_expr2;
               /* P 'zz9.9'  (for example)      */
               Char_ptr             pic_str; /* The expression MUST be a char string constant ! */
               /* A [(exp)]                     */
               short                charf;     /* char format present */
               Any_ptr              char_expr;
               /* ie B or B(4) or B3(21) etc    */
               short                bit_radix;      /* 1,2,3 or 4 */
               Any_ptr              bit_expr;
               /* L                             */
               short                l_format;   /* tue or false */
               /* TAB [(exp)]                   */ 
               short                tab;
               Any_ptr              tab_expr;
               /* LINE (exp)                    */
               Any_ptr              line_expr;
               /* X (exp)                       */
               Any_ptr              space_expr;
               /* SKIP [(exp)]                  */
               short                skip;
               Any_ptr              skip_expr;
               /* COLUMN (exp)                  */
               Any_ptr              colm_expr;
               /* R (identifier)                */
               Ref_ptr              rmte_ptr;   /* MUST be a simple identifier */
               Format_ptr           next_ptr;   /* Next in commalist */
               } Format;
                   
typedef struct select_node {
               short            type;
               char             line_no[10];
               Any_ptr          stmt_ptr;     /* Must point to a when */
               Any_ptr          expr_ptr;
               } Select;

typedef struct when_node {
               short         type;
               char          line_no[10];
               Any_ptr       stmt_ptr;     /* can be another when etc..    */
               Any_ptr       expr_ptr[16]; /* list of exprs (max 16 limit) */
               Any_ptr       unit_ptr;     /* stuff to do if the when true */  
               short         any;          /* these two mutually exclusive */
               short         all;
               short         setting_unit; /* see also IF node             */
               } When;

typedef struct other_node {
               short         type;
               char          line_no[10];
               Any_ptr       stmt_ptr;
               Any_ptr       unit_ptr;
               short         setting_unit; 
               } Other;

typedef struct init_node {
               short		type;
			   char			line_no[10];
			   Iarray_ptr	ia_ptr;
			   Ielem_ptr	ie_ptr;
			   } Init;

typedef struct iarray_node {
			   short		type;
			   Icon1_ptr	ic1_ptr;
			   Icon2_ptr	ic2_ptr;
			   Any_ptr		expr_ptr;
			   } Iarray;

typedef struct icon1_node {
			   short		type;
			   short		preop;           /* code of <prefix-operator> (if any)      */
			   Any_ptr		str_con_ptr;     /* ptr to string constant, in lexeme table */
			   Icon2_ptr	ic2_ptr;
			   } Icon1;

typedef struct icon2_node {
			   short		type;
			   short		preop;			 /* code of <prefix-operator> (if any) */
			   Ref_ptr		ref_ptr;
			   Any_ptr		num_con_ptr;	 /* ptr to numeric constant, in lexeme table */
			   } Icon2;

typedef struct  ielem_node {
				short		type;
				short		asterisk; /* true/false */
				Any_ptr		expr_ptr; /* ptr to expression */
				long		ifactor;
				Icon2_ptr	ic2_ptr;				
				short		star;		/* true/false */
				Ielem_ptr   next_ptr;   /* the commalist is chained */
				Ielem_ptr	ie_ptr;
				Icon1_ptr	ic1_ptr;
				} Ielem;
			
typedef struct data_node {
               short         type;
               char          line_no[10];
               short         data_type;
               short         prec_1;
               short         prec_2;
               short         scale;
               Bit           varying;
               short         asterisk; /* ie char(*) var   etc */
               short         bad_dcl;
               Symbol_ptr    parent;
               Data_ptr      next_ptr;
               } Data; 
           
typedef struct symbol_node {
        short         node_type;        /* Allows pass2 to recognise symbols*/
        char          line[10];         /* line no, declared on             */
        unsigned      bad_dcl:       1; /* syntax error in declaration      */
        unsigned      known_size:    1; /* size is determinable by compiler */
        Bit           varying:       1; /* varying string                   */
        unsigned      variable:      1; /* entry, label etc.                */ 
        unsigned      known_locn:    1; /* locn is determinable by compiler */
        unsigned      structure:     1; /* 0 = scalar, 1 = structure/member */
        unsigned      keyword:       1; /* set to true if possible keyword  */
        unsigned      qualified:     1; /* is name a '.' name ?             */
        unsigned      declared:      1; /* appeared in a dcl statement      */
        unsigned      computational: 1; /* as defined in ANSI document      */
        unsigned      vola_tile:     1; /* for concurrent O/S.              */
        unsigned      referenced:    1; /* is this symbol actually used ?   */
        unsigned      asterisk:      1; /* is the sym a char(*) parameter ? */
        unsigned      temporary:     1; /* this is a compiler created temp  */
        unsigned      aligned:       1; /* alignment given when declared ?  */
		unsigned	  initial:       1; /* did 'init' appear on declare ?   */
        unsigned      file:          1; /* is symbola a file ?              */ 
        short         ext_code_idx;     /* If extrnl, the idx of the EXTDEF */
        short         ext_static_idx;   /* If extrnl, the idx of the EXTDEF */
		short         cons_idx;         /* Index of .rdata section, if applicable */
		long          coff_symtab_idx;  /* Index in COFF symbol table.      */
		long          coff_strtab_idx;  /* Index in COFF string table.      */
        short         level;            /* structure level of this name     */
     /*short          value_reg;           Is this syms val in register ?   */
     /*short          locn_reg;            Is the syms offset in a reg ?    */ 
        long          bytes;            /* required storage in bytes        */
        long          size;             /* if an array, the element size    */
        short         prec_1;           /* The a in  fixed dec (a,b)        */
        short         prec_2;           /* The b in  fixed dec (a,b)        */
        long          offset;           /* offset in stack/static           */
        short         token;            /* token id                         */ 
        short         class;            /* based, static, defined, auto     */ 
        short         type;             /* char, numeric, pointer, pic      */
        short         scope;            /* internal or external             */   
        short         scale;            /* fixed or float                   */
        short         num_dims;         /* number of declared dims/args     */
        char         *pic_text;         /* ptr to pic specification text    */
        char         *spelling;         /* ascii spelling of name           */
        Ref_ptr       defbas_ptr;       /* Ref that symbol is based/def on  */
        Block_ptr     declarator;       /* PL/1 block in which declared     */
        Block_ptr     proc_ptr;         /* ptr to block if internal proc    */
        Any_ptr       array_ptr;        /* first array-bound/arg descriptor */
		Init_ptr	  init_ptr;			/* ptr to initial descriptor        */
        Data_ptr      ret_ptr;          /* ptr to returns descriptor if any */	
        Symbol_ptr    parent;           /* structure parent if applicable   */
        Symbol_ptr    child;            /* ptr to 1st structure member      */
        Symbol_ptr    sister;           /* ptr to next field at this level  */
        Symbol_ptr    prev_ptr;         /* previous symbol in list          */
        Symbol_ptr    next_ptr;         /* next symbol in list              */
        } Symbol;

typedef struct block_node {
               short       type;
               char        line[10];       /* line no, declared on            */ 
               Block_ptr   parent;         /* pointer to parent PL/1 block    */
               Block_ptr   child;          /* pointer to 1st child block      */
               Block_ptr   sister;         /* pointer to adjacent sister      */  
               Any_ptr     first_stmt;     /* 1st stmt node in this block     */
               Symbol_ptr  first_symbol;   /* first declared name in block    */
               Data_ptr    ret_ptr;        /* ie returns (char(32))           */  
               char        block_name[32]; /* ascii spelling of procname      */
               short       depth;          /* zero for outermost block        */
               short       begin;          /* True if this is a begin block   */
               short       function;       /* True if block is function       */
               short       recursive;      /* True if block is recursive      */
               short       main;           /* Set if block is main entry      */
               short       num_args;       /* How many parameters             */
               short       num_rets;       /* No of returns stmts in block    */
               short       called;         /* Is the block referenced ?       */
               long        stack;          /* Size of stack frame             */
               long        stattic;        /* Size of static area             */
               long        seg_pos;        /* pos'n in file of SEGDEF         */
               short       params;         /* Size of stack for arg ptrs      */
               short       data_idx;       /* Section number of blocks static */
               short       code_idx;       /* Section number of blocks code   */
               short       stack_idx;      /* SEGMENT Index of blocks stack   */
               short       sym_count;      /* Number of names in this proc    */
               } Block;










