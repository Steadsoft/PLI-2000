/****************************************************************************/
/*                           COPYRIGHT NOTICE                               */
/****************************************************************************/
/* Vulkan CONFIDENTIAL INFORMATION:                           Category:  1  */
/* COPYRIGHT (c) 1990 Vulkan Technologies Ltd.                              */
/*                                                                          */
/* This data file contains confidential and proprietary information of      */
/* Vulkan Technologies Ltd, and any reproduction, disclosure, or other use  */
/* in whole or in part, is hereby expressly prohibited.                     */
/*                                                                          */
/* This restriction applies unless, prior written agreement or prior        */
/* written permission has been given by Vulkan Technologies Ltd.            */
/*                                                                          */
/* Unlawful use of this material shall render the using party liable to     */
/* prosecution.                                                             */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                    PL/1 Compiler Release 1.0                             */
/****************************************************************************/
/****************************************************************************/
/*                         Modification History                             */
/****************************************************************************/
/*  Who    When                           What                              */
/* ------------------------------------------------------------------------ */
/* HWG  01-07-91      Initial version created                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                         Functional Description                           */
/****************************************************************************/
/* This is a command, that reads a text file, that contains all PL/1 error  */
/* messages, and constructs the compiler error text file. The text file is  */ 
/* maintained by using an editor, and this command reads it to build the    */
/* compiler readable error text file.                                       */
/****************************************************************************/

/****************************************************************************/
/*                    R E P L A C E    S T A T E M E N T S                  */
/****************************************************************************/
/****************************************************************************/
/*                    I N C L U D E    F I L E S                            */
/****************************************************************************/

# include <stdio.h>
# include "intaface.h"
# include "errmask.h"

/****************************************************************************/
/*                  E X T E R N A L    V A R I A B L E S                    */
/****************************************************************************/
/****************************************************************************/
/*                  E X T E R N A L    F U N C T I O N S                    */
/****************************************************************************/
/****************************************************************************/
/*                  I N T E R N A L    V A R I A B L E S                    */
/****************************************************************************/

FILE     *IN;
FILE     *OUT;
size_t   bytes;
char     inbuffer[512];

/****************************************************************************/
/*                  I N T E R N A L    F U N C T I O N S                    */
/****************************************************************************/

void  fail (char *);

/****************************************************************************/
/*            S T A R T    O F    E X E C U T A B L E    C O D E            */
/****************************************************************************/

void main (argc,argv)

     int          argc;
     char       **argv;

     {

     short     status;
     char      infile[256]  = "errors.txt";
     char      outfile[256] = "errors.bin";
 
     status = AquireCmdLineArgs ("build_compiler_error_file",argc,argv,
                                 "posn(input_file),length(60),string,required","",infile,
                                 "posn(output_file),length(60),string,required","",outfile,
                                 "end","");
     if (status != 0)
        exit(0);

     /******************************************************************/
     /*      Open both files, but check that they are different !      */
     /******************************************************************/

     if (strcmp(infile,outfile) == 0)
        fail ("The input and output files have the same name. %s\n");

     IN = fopen (infile,"r");
 
     if (IN == NULL)
        fail("Unable to open input file. %s\n");

     OUT = fopen (outfile,"wb");

     if (OUT == NULL)
        fail("Unable to open output file. %s\n");

     /*********************************************************************/
     /* Now read each text record, format it correctly and write it out   */
     /*********************************************************************/

     while (fgets(inbuffer,512,IN))
           {
           message.length = strlen(inbuffer);
           message.severity = 1;
           strcpy(message.text,inbuffer);
           fwrite(&message,512,1,OUT);
           } 

     }

void fail (msg)

     char   *msg;

     {

     printf (msg,"Request aborted.");
     exit (0);

     }

