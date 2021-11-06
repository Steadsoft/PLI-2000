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
/*  HWG   01-07-91        Initial version created.                          */
/*  HWG   20-09-91        If user requested -error then write the error to  */
/*                        the .err file as well as the terminal.            */
/*  HWG   23-12-91        When calculating offset into file for a given     */
/*                        error number, we were doing something along the   */
/*                        lines of: long = int * int;                       */
/*                        This fails in C, and must be recoded to work like */
/*                        this: long = (long)int * (long)int;               */
/*                        18 months down the line and I hadn't seen this one*/
/*                        before!                                           */   
/****************************************************************************/

/****************************************************************************/
/*                         Functional Description                           */
/****************************************************************************/
/* This file contains functions, that are concerned with reporting of error */
/* messages. The messages are held in a disk file, so that the size and     */ 
/* number of them, does not impact the size of the compiler program.        */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                    R E P L A C E    S T A T E M E N T S                  */
/****************************************************************************/
/****************************************************************************/
/*                    I N C L U D E    F I L E S                            */
/****************************************************************************/

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "platform.h"
# include "tokens.h"

/****************************************************************************/
/*                  E X T E R N A L    V A R I A B L E S                    */
/****************************************************************************/

       char  file[128];
extern short Err_count;
extern short Warn_count;
extern char  line_no[10];   
extern short listing_reqd;
extern short debug_reqd;
extern short beep_reqd;
extern short able_to_compile;
extern short error_reqd;
extern char  error_list[65];


void DebugTrap(void);

/****************************************************************************/
/*                  E X T E R N A L    F U N C T I O N S                    */
/****************************************************************************/

/* void get_exe_dir (char *); */

/****************************************************************************/
/*                  I N T E R N A L    V A R I A B L E S                    */
/****************************************************************************/

char   *ln;
int    file_not_yet_open = 1;
FILE   *ERR;
FILE   *ERROR_LIST = NULL;
long   position;
int    error_code;
char   errfile[64] = "\\ERRORS.BIN";
char   printout[512];

struct err_msg {
       short          severity;      /* 2 bytes   */
       short          length;        /* 2 bytes   */
       char           text[508];     /* 512 bytes */
       } message;

/****************************************************************************/
/*                  I N T E R N A L    F U N C T I O N S                    */
/****************************************************************************/

/****************************************************************************/
/*            S T A R T    O F    E X E C U T A B L E    C O D E            */
/****************************************************************************/

void report (short error_number,
             char *parameter,
             short compiler_line) /* Line number of compiler source code */


     {

     char   *pos;

     //DebugTrap();

     /********************************************************************/
     /*                Beep to user about this error !                   */
     /********************************************************************/

     if (beep_reqd)
        {
        sound (500);
        delay (150);
        nosound();  
        }

     ln = strpbrk(line_no,"0123456789");

     printf ("\n");

     if (error_number > 0)
        {
        Err_count++;
        able_to_compile = 0; /* Prevent later phases from running */
        printf ("ERROR %d FOUND, BEGINNING ON LINE %s (%d)\n",
                error_number,
                ln,
                compiler_line);
        if (error_reqd)
           fprintf (ERROR_LIST,"\nERROR %d FOUND, BEGINNING ON LINE %s (%d)\n",
                   error_number,
                   ln,
                   compiler_line);
 
        }
     else
        {
        Warn_count++;
        printf ("WARNING %d ISSUED, BEGINNING ON LINE %s (%d)\n",
                (-error_number),
                ln,
                compiler_line);
        if (error_reqd)
           fprintf (ERROR_LIST,"\nWARNING %d ISSUED, BEGINNING ON LINE %s (%d)\n",
                    (-error_number),
                    ln,
                    compiler_line);
 
        error_number = (-error_number);
        }

     /*******************************************************************/
     /* Error message 1 is at offset 0 in the file, so use the calc     */
     /* below, to calculate offset from error number !                  */
     /*******************************************************************/

	 /* Error message 57 (for example) has its text stored in line 58 */

     position = ((long)error_number * (long)sizeof(message));

     error_code = fseek (ERR,position,0);

     if (error_code != 0)
        {
        printf("Fatal error, unable to position correctly in the\n");
        printf("compiler error file, either the file is corrupted\n");
        printf("or an internal error has ocurred in the compiler.\n");
        }
 
     /*******************************************************************/
     /* Now read the error record, and display the message on screen    */
     /*******************************************************************/

 	 switch (error_number) 
	 {
	 case(NOT_YET_SET):
	     {
		 strcpy(message.text,"Compiler Error: The 'not-yet-set' error code has been encountered*");
		 strcat(message.text,"please inform technical support.\n");
		 break;
		 }
	 case(INTERNAL_ERROR):
	     {
	     strcpy(message.text,"Compiler Error: An internal failure has ocurred, please inform*");
		 strcat(message.text,"technical support.\n");
	     break;
		 }
	 default:
	     fread (&message,512,1,ERR);
	 }

     /******************************************************************/
     /* Scan the string for "*" and replace each ocurrence with a 0Ax  */
     /******************************************************************/

     pos = strstr(message.text,"*");

     while (pos != NULL)
           {  
           strnset(pos,'\n',1);
           pos = strstr(message.text,"*");
           }              

     printf(message.text,parameter);

     if (ERROR_LIST != NULL)
        fprintf(ERROR_LIST,message.text,parameter);



     }

void open_error (char * program_path)

     {

     char      exe_dir[128];
     char      err_dir[128];

     strcpy (exe_dir,program_path);
     
     strrev (exe_dir);
     
     strcpy (err_dir,&(exe_dir[7]));
     
     strrev (err_dir);
     
     strcat (err_dir,"ERRORS.BIN");      
     
     

     /*get_exe_dir(exe_dir);

     strcat(exe_dir,errfile);

     strcpy(errfile,exe_dir); */

     ERR = fopen(err_dir,"rb");

     if (ERR == NULL)
        {
        printf("Fatal, the compiler was unable to open the error message\n");
        printf("file: %s\n",err_dir);
        printf("Please refer to the documentation for more information about\n");
        printf("this error.\n");
        exit(0);
        }   
	 else
	    if (debug_reqd)
	       printf("Opened file: %s\n",err_dir);

     if (error_reqd == 0)
        return;

     ERROR_LIST = fopen(error_list,"w");

     if (ERROR_LIST == NULL)
        {
        printf("Fatal error, unable to open the required error listing\n");
        printf("file, please check that there is sufficient free disk\n");
        printf("space.\n");
        exit(0);
        }   

     }
