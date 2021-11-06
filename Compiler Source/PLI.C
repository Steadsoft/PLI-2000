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
/*                             PL/1 Compiler                                */
/****************************************************************************/

/****************************************************************************/
/*                         Modification History                             */
/****************************************************************************/
/*  Who    When                           What                              */
/* ------------------------------------------------------------------------ */
/* HWG    13-07-90       Initial Prototype.                         Rel 1.0 */
/* HWG    20-07-90       Simple parser.                             Rel 1.1 */
/* HWG    18-09-90       Initial Symtab                             Rel 1.2 */
/* HWG    23-10-90       Converted to Turbo C, from MS-C.           Rel 1.3 */
/* HWG    20-06-91       Storage allocater added.                   Rel 1.5 */
/* HWG    07-07-91       Options can now be defaulted in autoexec.  Rel 1.7 */
/* HWG    08-07-91       Parse command help(F1) implemented.        Rel 1.8 */
/* HWG    02-08-91       Major re-org of tokens/refs/symbols.       Rel 2.1 */
/* HWG    18-09-91       Debugging switches trace_pass2/heap  added.        */
/* HWG    21-09-91       New switch -error added to create .err file.       */
/* HWG    22-10-91       Total static storage now printed at end.           */
/* HWG    24-10-91       External/internal procedure calls working. Rel 2.3 */
/* HWG    14-08-93       Only gen code if allocator was successful.         */
/* HWG    14-08-93       Delete any semi-built .OBJ file.                   */
/* HWG    04-02-96       Dont print symtab if unable to compile, cos it     */
/*					     will not be in any state to print !                */  
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                         Functional Description                           */
/****************************************************************************/
/* This module will extract command line arguments and verify existence of  */
/* specified source file.                                                   */
/* The various phases that constitute the compiler, are called in sequence  */
/* according to success of previous phases, and user selected options.      */
/*                                                                          */
/* Statistics for each phase are maintained and printed in here too.        */
/****************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <time.h>
#include <stdio.h>
#include "ctype.h"
#include "platform.h"
#include "token.h"
#include "intaface.h"
#include "windows.h"

# define SOURCE_HELP  "Enter the name of the source file, the 'pl1' suffix is optional."
# define LIST_HELP    "Select this option, if you require a listing file."
# define NEST_HELP    "Select this option, if you want nesting levels shown in the listing."
# define OPT_HELP     "Select this option, if you want the precode optimizer to run."
# define BND_HELP     "Select this option, if you want runtime array bound checking."
# define TABLE_HELP   "Select this option, to retain debugging information in your program."
# define BEEP_HELP    "Select this option, to beep on errors and completion."
# define SYS_HELP     "Select this option, if you require an assembly listing."
# define CODE_HELP    "Select this option, if you require an object module."
# define NDP_HELP     "Select this option, if you require math coprocessor instructions generated."
# define DN_HELP      "This option is reserved for engineering use only."
# define TH_HELP      "This option is reserved for engineering use only."
# define TC_HELP      "This option is reserved for engineering use only."
# define TP2_HELP     "This option is reserved for engineering use only."
# define DBG_HELP     "This option is reserved for engineering use only."
# define HALT_HELP    "This option is reserved for engineering use only."
# define SEM_HELP     "This option is reserved for engineering use only."
# define ERR_HELP     "Select this option, if you require an error listing file."
# define MAP_HELP     "Select this option, to accept uppercase source input."
# define UNREFS_HELP   "Warn about unreferenced variables."

/* short    CuiMenu (char * title, ...); */

# define _LINE_ ((short)__LINE__)

char     stats[25][256];

long     MilliSeconds(void);

long     phase_start;
long     phase_stop;

short    duration;
short    Err_count;
short    Warn_count;
int      able_to_compile;
int      I;
short    total_errs;
short    total_warns;
short    total_msecs;
short    total_disk;
short    unrecoverable_error = 0;

extern   FILE *LISTING;
extern   unsigned long  symtab_heap_used;
extern   unsigned long  node_heap_used;
extern   unsigned long  nodes_allocated;
extern   unsigned long  code_bytes;
extern   unsigned long  bytes_read;
         unsigned long  total_static = 0;
 
                  short  sects = 0;
                  double fsects;
                  double fbytes;
extern   long total_lines;
extern   long total_stmts;
         double total_time    = 0;
         long   rate          = 0;
		 long   srate         = 0;
         short  system_reqd   = 0;
         short  listing_reqd  = 0;
         short  nesting_reqd  = 0;
         short  error_reqd    = 0;
         short  optimize_reqd = 0;
         short  bounds_reqd   = 0;
         short  table_reqd    = 0;
         short  code_reqd     = 0;
         short  beep_reqd     = 0;
         short  dump_reqd     = 0;
         short  trace_heap    = 0;
         short  trace_pass2   = 0;
         short  trace_code    = 0;
         short  debug_reqd    = 0; 
         short  mapcase_reqd  = 0;
         short  ndp_reqd      = 0;
         short  halt_reqd     = 0;
         short  unrefs_reqd   = 0;
         short  semantic_reqd = 0; 

extern   char file[128];
         char error_list[128] = "";

void  open_error   (char *);
void  set_defaults (void);
int   parse_program (void);
void  enter_pass2 (void);
void  allocator(char*);
void  initialise (char*);
static void  begin_init(void);
static void  end_init(void);
void  process_declares();
static void  begin_declare(void);
static void  end_declare(void);
static void  begin_pass1(void);
static void  end_pass1(void);
static void  begin_pass2(void);
static void  end_pass2(void);
static void  begin_optimize(void);
/* void  enter_optimizer(void); */
static void  end_optimize(void);
static void  begin_alloc(void);
static void  end_alloc(void);
static void  begin_code(void);
void  enter_code(void);
static void  end_code(void); 
static void  totals(void);
void  report(short,char *,short);
void  print_symtab (void);
void  open_listing (char[]);
void  dump_tree (void);       /* DEBUG ONLY */
static void  calc_disk_io (void);
Token_ptr get_token(void);  
char  program_path [128];
extern char * _pgmptr;
long  except_code = 0;
void *  except_addr = NULL;
char  except_msg[32];

/***************************************************************************/
/*                  Win32 Exception handling support.                      */
/***************************************************************************/

long filterer (LPEXCEPTION_POINTERS);


/***************************************************************************/
/*             Entry point for start of compilation.                       */
/***************************************************************************/

void main (long argcount,char * argvector[])

     {                                                                      

     short              status;
     char               obj_file [128] = "";
   

__try {

    listing_reqd = 0;

    strcpy (file,"");

    set_defaults();   
    
    /***********************************************************************/
    /* Because the compiler error file must be in the same dir as the      */
    /* compiler executable, we need the full path of the executable to     */
    /* work this out. MS Visual C++ (for DOS projects) puts the full path  */
    /* in argv[0], other implementations put only the program name.        */
    /***********************************************************************/
     
    strcpy(program_path,_pgmptr);   

    status = AquireCmdLineArgs  
                      ("PL/I 32 Optimizing Compiler",argcount,argvector,
                       "posn(source_file),length(64),string,required",SOURCE_HELP,file,
                       "switch(list),color(9)",LIST_HELP,&listing_reqd,
                       "switch(nesting),color(9)",NEST_HELP,&nesting_reqd,
                       "switch(mapcase),color(9)",MAP_HELP,&mapcase_reqd,
                       "switch(error),color(9)",ERR_HELP,&error_reqd,
                       "switch(optimize),color(9)",OPT_HELP,&optimize_reqd,
                       "switch(bounds),color(9)",BND_HELP,&bounds_reqd,  
                       "switch(ndp),color(9)",NDP_HELP,&ndp_reqd,
                       "switch(table),color(9)",TABLE_HELP,&table_reqd,
                       "switch(beep),color(9)",BEEP_HELP,&beep_reqd,
                       "switch(system),color(9)",SYS_HELP,&system_reqd,
                       "switch(code),color(9)",CODE_HELP,&code_reqd,
                       "switch(dump_nodes),secret,color(9)",DN_HELP,&dump_reqd,
                       "switch(trace_heap),color(9),secret",TH_HELP,&trace_heap,
                       "switch(trace_pass2),secret,color(9)",TP2_HELP,&trace_pass2,
                       "switch(trace_code),secret,color(9)",TC_HELP,&trace_code,
                       "switch(debug),secret,color(9)",TC_HELP,&debug_reqd,
                       "switch(halt),secret,color(9)",HALT_HELP,&halt_reqd, 
                       "switch(unrefs),color(9)",UNREFS_HELP,&unrefs_reqd, 
                       "switch(semantic),color(9)",SEM_HELP,&semantic_reqd,
                       "end,wide,color(14)");



     if (status == 1019)  /* Escape from form */
        exit(0);

     if (status != 0)
	     {
	     printf ("An error ocurred processing the command line arguments.\n");
	     exit(0);
	     }

     if (nesting_reqd || system_reqd)    /* nesting implies listing */
        listing_reqd = 1;

     if (system_reqd)
        code_reqd = 1;

     if (ndp_reqd)
        code_reqd = 1;

     if ((code_reqd) || (unrefs_reqd) || (trace_pass2))
        semantic_reqd = 1;
  
     /***********************************************************/
     /*          Does the input file name end in .pl1 ?         */
     /***********************************************************/

     strrev (file);

     if (file[3] != '.') 
        {
        strrev (file);
        strcat (file,".pl1");
        }
     else
        strrev (file);

     /********************************************************************/
     /* Set the pathname for the error listing to the same as the source */
     /* file, but set the suffix to .err  We must always delete any .err */
     /* and any .obj                                                     */
     /********************************************************************/

     strcpy (error_list,file);
     strrev (error_list);
     error_list[0] = 'R';
     error_list[1] = 'R';
     error_list[2] = 'E';
     strrev (error_list);

     remove(error_list);

     if (code_reqd)
        {
        strcpy (obj_file,file);
        strrev (obj_file);
        obj_file[0] = 'J';
        obj_file[1] = 'B';
        obj_file[2] = 'O';
        strrev (obj_file);
        remove(obj_file);
        }

     /*******************************************************************/
     /*               Initialise various bits and pieces.               */
     /*******************************************************************/ 

     duration        = 0;
     Err_count       = 0;
     Warn_count      = 0;
     able_to_compile = 1;
     total_errs      = 0;
     total_warns     = 0;
     total_msecs      = 0;
     total_disk      = 0;

	 if (debug_reqd)
	    printf("Program pathname is: %s\n",program_path);

     open_error(program_path);

     /***********************************************************/
     /*      perform all required initialisation first.         */
     /***********************************************************/

     begin_init();
     initialise(file);
     end_init();

    
     if (listing_reqd)
        {
        open_listing (file);
        }

     /*************************************************************/
     /* parse_program, is the main entry to the syntax phase.     */
     /* This phase reads the source file, parses the input, saves */
     /* symbol entries and declarations, and constructs a parse   */
     /* tree.                                                     */
     /*************************************************************/

     begin_pass1();
     parse_program();
     end_pass1();   

     /**************************************************************/
     /* The declare phase processes the symbol table constructed   */
     /* by the syntax phase.It reports on undeclared parameters.   */
     /* It also calcuates storage requiremenst, reports undeclared */
     /* variables, and applies language defaults where required.   */
     /**************************************************************/ 

     if (unrecoverable_error == 0)
        {
        begin_declare();
        process_declares();
        end_declare();
        }

     /**************************************************************/
     /* The semantic phase walks-thru the parse-tree.              */
     /* This phase resolves all references to their declarations.  */
     /* It verifies that all variables are of the right data type  */
     /* for the context of their use.                              */
     /* It creates temporary variables, and determines conversion  */
     /* requirements for different operations.                     */
     /* Resultant data-types for conversions and expressions are   */
     /* also determined.                                           */
     /**************************************************************/

     if ((unrecoverable_error == 0) && (semantic_reqd))
        {
        begin_pass2();
        enter_pass2();  
        end_pass2();
        }

     /************************************************************/
     /* Optimize the intermediate parse tree                     */
     /************************************************************/

     if (unrecoverable_error == 0)
        if (able_to_compile)
           if (optimize_reqd)
              {
              begin_optimize();
              /* enter_optimizer(); */
              end_optimize();
              }

     /************************************************************/
     /* Print identifier attributes and xref information.        */
     /************************************************************/

     if (unrecoverable_error == 0)
	    if (able_to_compile) 
           print_symtab();


     /************************************************************/
     /* The next two phases are only concerned with the creation */
     /* of an object module.                                     */
     /* The allocator builds all the required parts of the obj   */
     /* module with the exception of code, it is concerned with  */
     /* the creation of external reference information, and the  */
     /* allocation and initialisation of static storage.         */
     /* The code generator analyses the parse-tree for each PL/1 */
     /* block and generates machine instructions for each type   */
     /* of tree node. The actual creation of the raw instruction */
     /* stream is performed by the code emitter, which is called */
     /* from the code generator for each instruction required.   */
     /************************************************************/  

     if (unrecoverable_error == 0)
        if ((able_to_compile) && (code_reqd))
           {
           begin_alloc();
           allocator(file);
           end_alloc(); 

           if ((unrecoverable_error == 0) && (able_to_compile))
              {
              begin_code();
              enter_code(); 
              end_code();
              }

           /* Delete any semi-built .obj file. */

           if ((unrecoverable_error) || (!able_to_compile))
              remove(obj_file);

           }

   
     if (dump_reqd)
        dump_tree();    

     /************************************************************/
     /* If no errors or warning were produced, then delete any   */
     /* error file (one may exist from a previous compile).      */
     /************************************************************/

     if (error_reqd)
        if ((total_errs == 0) && (total_warns == 0))
           remove (error_list);  

     /************************************************************/
     /*           print the compilation totals.                  */
     /************************************************************/

     totals();

     if (beep_reqd)
        {
        sound (1500);
        delay (100);
        sound (1000);
        delay (100);  
        }

     nosound ();

     if (able_to_compile == 0)
        {
        sprintf (stats[19],"\nTranslation Aborted\n");    
        printf (stats[19]);
        }

     if (halt_reqd)
        {
        printf("Halt requested, compiler has been suspended...\n");
        os_sleep(-1);
        }

     exit(0);

     } /* __try */

__except (filterer(GetExceptionInformation() ))
	{
    sprintf(except_msg,"(Code=%08lX, Addr=%08lX)",except_code,except_addr);
   	report(181,except_msg,(short)_LINE_);
    exit(0);
	}


}

/****************************************************************************/
/* These functions handle simple statistical totalling and reporting        */
/****************************************************************************/

static void begin_init(void)

     {

     Err_count  = 0;
     Warn_count = 0;

     phase_start = MilliSeconds();

     memset(stats,0,sizeof(stats));

     }

static void end_init (void)

     {

     double elapsed;

     calc_disk_io();

     phase_stop = MilliSeconds();;

     elapsed = phase_stop - phase_start;

     duration = (short) elapsed;

     if ((Err_count > 0) || (Warn_count > 0))
        sprintf (stats[3],"\ninitialize      %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);
     else
        sprintf   (stats[3],"initialize      %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);

     printf (stats[3]);

     total_time += duration;

     duration = 0;
     total_disk  += sects;
     total_errs  += Err_count;
     total_warns += Warn_count;
     Err_count = 0; Warn_count = 0;

     }

static void begin_declare(void)

     {

     Err_count = 0; Warn_count = 0;

     phase_start = MilliSeconds();;

     }

static void begin_pass1(void)

     {

     Err_count = 0; Warn_count = 0;

     phase_start = MilliSeconds();;

     }

static void end_pass1 (void)

     {

     double elapsed;

     calc_disk_io();

     phase_stop = MilliSeconds();;

     elapsed = phase_stop - phase_start;

     duration = (short) elapsed;

     if ((Err_count > 0) || (Warn_count > 0))
        sprintf (stats[4],"\nsyntax          %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);
     else
        sprintf   (stats[4],"syntax          %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);

     printf(stats[4]);

     total_time += duration;

     total_disk  += sects;
     total_errs  += Err_count;
     total_warns += Warn_count;
     duration = 0;
     Err_count = 0; Warn_count = 0;

     }

static void end_declare (void)

     {

     double elapsed;

     calc_disk_io();

     phase_stop = MilliSeconds();;

     elapsed = phase_stop - phase_start;

     duration = (short) elapsed;

     if ((Err_count > 0) || (Warn_count > 0))
        sprintf (stats[5],"\ndeclare         %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);
     else
        sprintf   (stats[5],"declare         %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);

     printf(stats[5]); 

     total_disk  += sects;
     total_time += duration;
     total_errs  += Err_count;
     total_warns += Warn_count;
     duration = 0;
     Err_count = 0; Warn_count = 0;

     }


static void begin_pass2(void)

     {

     Err_count = 0; Warn_count = 0;

     phase_start = MilliSeconds();;

     }

static void end_pass2 (void)

     {

     double elapsed;

     calc_disk_io();

     phase_stop = MilliSeconds();;

     elapsed = phase_stop - phase_start;

     duration = (short) elapsed;

     if ((Err_count > 0) || (Warn_count > 0))
        sprintf (stats[6],"\nsemantic        %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);
     else
        sprintf   (stats[6],"semantic        %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);

     printf(stats[6]);
   
     total_disk  += sects;
     total_time += duration;
     total_errs  += Err_count;
     total_warns += Warn_count;
     duration = 0;
     Err_count = 0; Warn_count = 0;

     }

static void begin_optimize(void)

     {

     Err_count = 0; Warn_count = 0;

     phase_start = MilliSeconds();;

     }

static void end_optimize (void)

     {

     double elapsed;

     calc_disk_io();

     phase_stop = MilliSeconds();;

     elapsed = phase_stop - phase_start;

     duration = (short) elapsed;


     if ((Err_count > 0) || (Warn_count > 0))
        sprintf (stats[7],"\noptimize        %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);
     else
        sprintf   (stats[7],"optimize        %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);

     printf(stats[7]); 

     total_disk  += sects;
     total_time += duration;
     total_errs  += Err_count;
     total_warns += Warn_count;
     duration = 0;
     Err_count = 0; Warn_count = 0;

     }

static void begin_alloc(void)

     {

     Err_count = 0; Warn_count = 0;

     phase_start = MilliSeconds();;

     }

static void end_alloc (void)

     {

     double elapsed;

     calc_disk_io();

     phase_stop = MilliSeconds();;

     elapsed = phase_stop - phase_start;

     duration = (short) elapsed;

     if ((Err_count > 0) || (Warn_count > 0))
        sprintf (stats[8],"\nallocator       %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);
     else
        sprintf   (stats[8],"allocator       %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);

     printf(stats[8]);

     total_disk  += sects;
     total_time += duration;
     total_errs  += Err_count;
     total_warns += Warn_count;
     duration = 0;
     Err_count = 0; Warn_count = 0;

     }

static void begin_code(void)

     {

     Err_count = 0; Warn_count = 0;

     phase_start = MilliSeconds();;

     }

static void end_code (void)

     {

     double elapsed;

     calc_disk_io();

     phase_stop = MilliSeconds();;

     elapsed = phase_stop - phase_start;

     duration = (short) elapsed;

     if ((Err_count > 0) || (Warn_count > 0))
        sprintf (stats[9],"\ntranslate       %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);
     else
        sprintf   (stats[9],"translate       %4d        %4d        %4d        %4d\n",duration,Err_count,Warn_count,sects);

     printf(stats[9]);

     total_disk  += sects;
     total_time  += duration;
     total_errs  += Err_count;
     total_warns += Warn_count;
     duration = 0;
     Err_count = 0; Warn_count = 0;

     }



static void totals (void)

     {

     calc_disk_io();

     total_msecs = (short)total_time;

     sprintf (stats[10],"totals          %4d        %4d        %4d        %4d\n",total_msecs,total_errs,total_warns,total_disk);

     printf(stats[10]);

     if (total_time == 0)
        total_time = total_lines/24; /* approx extrapolated value */

     if (total_time == 0)
        total_time = 1;

     rate =  (long)(total_lines / (total_time / 1000)) ;
	 srate = (long)(total_stmts / (total_time / 1000)) ;

     sprintf (stats[11],"\nSOURCE STMTS %d\n", total_stmts);
     sprintf (stats[12],  "SYMBOL TABLE %ld\n", symtab_heap_used);

     if (optimize_reqd)
        {
        sprintf (stats[13],  "NODE STORAGE %ld\n", node_heap_used);
        sprintf (stats[14],  "NODES NEEDED %ld\n", nodes_allocated);
        }

     printf(stats[11]);
     printf(stats[12]);

     if (optimize_reqd)
        {
        printf(stats[13]);
        printf(stats[14]);
        }
    
     if (code_bytes > 0)
        {
        sprintf (stats[15],  "CODE SIZE    %ld\n", code_bytes);
        printf(stats[15]);
        }

     if (total_static > 0)
        {
        sprintf (stats[16],  "STATIC SIZE  %ld\n", total_static);
        printf(stats[16]);
        }

     sprintf (stats[17],  "LINES/SECOND %ld\n", rate); 
     sprintf (stats[18],  "STMTS/SECOND %ld\n", srate);
     printf(stats[17]);
	 printf(stats[18]);

     /*******************************************************************/
     /* Print the compiler statistics and totals to the listing file    */
     /*******************************************************************/ 

     if (listing_reqd)
        {
        fprintf(LISTING,"\f\n"); 
        for (I = 0; I <= 19; I++)
            {
            /*************************************************************/
            /* Some of these lines of stats info, may have a newline     */
            /* char at the front (for clean spacing on CRT whenreporting */
            /* errors etc, BUT in the .lst file we do not want to see    */
            /* such spacing (cos errors dont get written to the .lst)    */
            /*************************************************************/
            if ((I >= 3) && (I <=9)) /* only these lines are relevant */
               if (isalpha(stats[I][0]))
                  fprintf (LISTING,stats[I]);
               else
                  fprintf (LISTING,&(stats[I][1]));  /* skip the \n */
            else
               fprintf (LISTING,stats[I]); 
            }
        }
      
     }

static void calc_disk_io (void)

     {

     fbytes = bytes_read;
     fbytes = fbytes / 512;
     fsects = ceil (fbytes);
     sects  = (short) fsects;
     bytes_read = 0;

     }

/************************************************************/
/*       This is the Win32 exception filter function.       */
/************************************************************/

long filterer (LPEXCEPTION_POINTERS machine_info)

{

/* if (machine->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
   printf("Exceptioncode: %08lX\n",machine->ExceptionRecord->ExceptionCode); */

except_code = machine_info->ExceptionRecord->ExceptionCode;
except_addr = machine_info->ExceptionRecord->ExceptionAddress;

return(EXCEPTION_EXECUTE_HANDLER);

}
