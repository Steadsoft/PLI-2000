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
/*  18-10-90   HWG       Initial Version.                                  */
/*  20-09-91   HWG       If a name is > 15 chars, then its attributes are  */
/*                       now printed on the next line.                     */
/*  25-11-91   HWG       Offsets of identifiers are now 3 digits long on   */
/*                       listing, with leading zeros were required.        */
/*  17-12-91   HWG       'buffer' was only 80 bytes long, and this was     */
/*                       too short for certain attribute details.          */
/*  07-01-92   HWG       Prevent runtime support routines being listed in  */
/*                       the attributes listing.                           */
/*                                                                         */
/*                                                                         */
/*  07-10-02   HWG-06    If a block had no names to print, we printed a    */
/*                       msg stating this, even if listing hadn't been     */
/*                       requested (hence list file was NULL).             */
/***************************************************************************/

/***************************************************************************/
/*                   D E F I N E D     S Y M B O L S                       */
/***************************************************************************/

#define  TEXT_1   "entry internal "   /* illegal */
#define  TEXT_2   "entry external "

/***************************************************************************/
/*                        I N C L U D E    F I L E S                       */
/***************************************************************************/

#include "string.h" 
#include <stdlib.h>
#include <stdio.h>
#include "c_types.h"
#include "nodes.h"
#include "symtab.h"
#include "tokens.h"
#include "release.h"
#include <dos.h>
/* #include <dir.h> */
#include "options.h"

/***************************************************************************/
/*       I N T E R N A L L Y    D E F I N E D    P R O T O T Y P E S       */
/***************************************************************************/

static void print_block  (Block_ptr);
int  open_listing (char[]);
void print_symtab (void);
static void clear_buffer (void);
void assign       (char *,short,char *);
void report       (int,char *,int);
static void print_symbol (Symbol_ptr);
static void print_stacks (Block_ptr);
static void print_stack  (Block_ptr);
static void print_conversions (void);
void check_print  (int);  /* auto page throw function */

/***************************************************************************/
/*            L O C A L     S T A T I C    V A R I A B L E S               */
/***************************************************************************/

extern int   conversions_in_use;
extern int   cv_fixed_bin_r;
extern int   cv_fixed_dec_r;
extern int   cv_arithmetic_r;
extern long  lines_printed;
extern char  line_no[10];
FILE  *LISTING = NULL;
/* extern FILE  *ERRFILE;   removed when rep.c rewritten */
static char   buffer[130];         /* was 80 */
char   locn[8];
char   length[8];
/*struct dostime_t     st;
struct dosdate_t     sd;  */
 
/***************************************************************************/
/*               This function will open the listing file.                 */
/***************************************************************************/

int open_listing (char name[])

     {

     char         list[256];
     size_t       pos;

     if (listing_reqd == 0)
        return(0);

     strupr (name);

     /* strcpy (path,searchpath(name)); */

     /*getdate (&sd);
     gettime (&st); */

     pos = strcspn (name,".");

     strcpy (list,name);

     strcpy(&list[++pos],"L");
     strcpy(&list[++pos],"S");
     strcpy(&list[++pos],"T");

     LISTING = fopen(list,"w");

     if (LISTING == NULL)
        return(1);

     /************************************************************/
     /* Print some useful data for the programmers sake          */
     /************************************************************/

	 fprintf(LISTING,"Win32 (NT/95) dates, times, version-no's etc not yet ported.\n");
	 fprintf(LISTING,"Please update LIST.C to remove this msg etc.\n");
	 /*
     fprintf (LISTING,"Copyright (c) Hugh Gleaves 2006\n");
     check_print(1);
     fprintf (LISTING,"SOURCE FILE: %s\n",path);
     check_print(1);
     fprintf (LISTING,"COMPILED ON: %02d-%02d-%02d ",sd.day,sd.month,(sd.year-1900));  
     fprintf (LISTING,"AT: %02d:%02d:%02d\n",st.hour,st.minute,st.second);
     check_print(1);
     fprintf (LISTING,"COMPILED BY: PL/I Release %d.%d alpha\n",RMAJ,RMIN);
     check_print(1);
     fprintf (LISTING,"OS RELEASE: %d.%d\n",_osmajor,_osminor);
     check_print(1);
	 */
     /*********************************************************************/
     /*          Print a summary of any selected compiler options.        */
     /*********************************************************************/

     fprintf (LISTING,"OPTIONS: ");

     if (listing_reqd)
        fprintf (LISTING,"+listing ");
     if (ndp_reqd)
        fprintf (LISTING,"+ndp ");
     if (mapcase_reqd)
        fprintf (LISTING,"+mapcase ");
     if (dump_reqd)
        fprintf (LISTING,"+dump ");
     if (beep_reqd)
        fprintf (LISTING,"+beep ");
     if (table_reqd)
        fprintf (LISTING,"+table ");
     if (optimize_reqd)
        fprintf (LISTING,"+optimize ");
     if (error_reqd)
        fprintf (LISTING,"+error ");
     if (nesting_reqd)
        fprintf (LISTING,"+nesting ");
     if (code_reqd)
        fprintf (LISTING,"+code ");
     if (system_reqd)
        fprintf (LISTING,"+system ");
     if (bounds_reqd)
        fprintf (LISTING,"+bounds ");

     fprintf (LISTING,"\n\n"); 
     check_print(2);

     return(0);
     
     }

/***************************************************************************/
/*     This function prints a formatted listing of the symbol table        */
/***************************************************************************/

void print_symtab (void)

     {

     extern Block_ptr   block_root;

     if (listing_reqd) /* Only actually print if requested by user */
        {
        lines_printed = 0;
        fprintf (LISTING,"\n\fATTRIBUTES AND CROSS REFERENCE TABLE\n");
        fprintf (LISTING,"\n");
        check_print(2);
        }

     print_block (block_root);

     print_conversions();

     print_stacks (block_root);

     }

/***************************************************************************/
/* This procedure will recursively start printing blocks at the bottom of  */
/* the symbol tree, and walk its way up.                                   */
/* This may not print the symbol table in the most useable order, but WFC  */
/***************************************************************************/

static void print_block (Block_ptr b_ptr)

     {
 
     Block_ptr    p_ptr;
     Symbol_ptr   v_ptr;
     short        length;

     /*****************************************************/
     /*       Check the pointer before using it !         */
     /*****************************************************/

     if (b_ptr == NULL)
        return;

     p_ptr = b_ptr;

     while (p_ptr != NULL)
           {
           clear_buffer();
           if (p_ptr->parent != NULL)
              {
              if (p_ptr->begin)
                 {
                 assign (buffer,0,(p_ptr->block_name));
                 length = strlen(p_ptr->block_name) + 1;
                 }
              else
                 {  
                 assign (buffer,0,"\n\nPROCEDURE ");
                 assign (buffer,12,(p_ptr->block_name));
                 length = strlen(p_ptr->block_name) + 13;
                 }
              }
           else
              {
              assign (buffer,0,"PROCEDURE ");
              assign (buffer,10,(p_ptr->block_name));
              length = strlen(p_ptr->block_name) + 11;
              }
           assign (buffer,length,"ON LINE");
           length += 7;
           assign (buffer,length,strrchr(p_ptr->line,' '));    
           if (listing_reqd)
              {
              fprintf (LISTING,"%s",buffer);
              fprintf (LISTING,"%s","\n\n");
              check_print(2);
              }

           if (listing_reqd) /* Only actually print if requested by user */
              {
              fprintf (LISTING,"NAME            CLASS     SIZE         OFX   ATTRIBUTES\n");
              fprintf (LISTING,"\n");
              check_print(2);
              }

           /********************************************/
           /* OK Now print all variables in this block */
           /********************************************/    
           v_ptr = p_ptr -> first_symbol;

           if ((v_ptr == NULL) && (listing_reqd))  /* HWG-06 */
              {
              fprintf(LISTING,"This block has no declared names.\n");
              check_print(1);
              }

           while (v_ptr != NULL)
                 {
                 print_symbol(v_ptr);
                 v_ptr = v_ptr -> next_ptr;
                 }
           p_ptr = p_ptr -> sister;
           }

     if ((b_ptr -> child) != NULL)
        print_block (b_ptr -> child);

     /*********************************************************/
     /*       This block has children, so print them !        */
     /*********************************************************/ 

     return;

     }

/***************************************************************************/
/*          This function will print the details of a given name           */
/***************************************************************************/

static void print_symbol (Symbol_ptr v_ptr)

     {

     short        posn;
     char         tmp[10];
     short        indent;

     if (strncmp(v_ptr->spelling,"pli$",4) == 0)
        return; 

     if (v_ptr->class == CONSTANT) /* Why print these ? */
        if ((v_ptr->type != ENTRY) &&
            (v_ptr->type != LABEL))
        return;

     if (v_ptr->temporary)
        return;

     while (v_ptr != NULL)
 
        {
   
        clear_buffer();
        indent = v_ptr->level;

        if (indent > 1)    /* indent structure memebers */
           assign (buffer,2,(v_ptr->spelling));
        else
           assign (buffer,0,(v_ptr->spelling));
        posn = 15;
   
        /**************************************************************/
        /* If the length of the name is > 15 chars, then print it but */
        /* print its attributes on the next line.                     */
        /**************************************************************/

        if (listing_reqd)
           if (((indent <= 1) && (strlen(v_ptr->spelling) > 15)) ||
               ((indent >  1) && (strlen(v_ptr->spelling) > 13)))
              {
              fprintf (LISTING,"%s\n",buffer);
              check_print(1);
              clear_buffer();
              }
 
        if (indent > 1) 
           assign(buffer,posn," member ");
        else
           { /* Structures and scalars have a class printed */
           switch (v_ptr->class) {
   
           case(BASED): 
               {
               assign (buffer,posn," based ");
               break;
               }
           case(STATIC): 
               {
               assign (buffer,posn," static ");
               break;
               }
           case(AUTOMATIC): 
               {
               assign (buffer,posn," automatic ");
               break;
               }
           case(DEFINED): 
               {
               assign (buffer,posn," defined ");
               break;
               }
           case(PARAMETER): 
               {
               assign (buffer,posn," parameter ");
               break;
               }
           case(CONSTANT): 
               {
               assign (buffer,posn," constant ");
               break;
               }
           case(BUILTIN): 
               {
               assign (buffer,posn," builtin ");
               break;
               }
           } /* switch */
           } /* else   */


        posn = 26; /* size */
   
        if ((v_ptr->known_size))
           {
           itoa ((v_ptr->bytes),length,10); /* print size in dec */
           assign(buffer,posn,length);
           /*************************************************************/
           /* If this is an array of strucs, then print out its element */
           /* size.                                                     */
           /*************************************************************/
           if ((v_ptr->array_ptr != NULL) &&
               (v_ptr->structure) &&
               (v_ptr->child != NULL))
               {
               posn += strlen(length);
               assign(buffer,posn,",");
               posn += 1;
               itoa ((v_ptr->size),length,10);
               assign(buffer,posn,length);
               }
           }
        else
           if (v_ptr->class != BUILTIN)
              assign(buffer,posn,"*");
       
        posn = 39; /* location */
   
        if ((v_ptr->known_locn))
           {   
           sprintf(locn,"%04X",v_ptr->offset);
           assign(buffer,posn,locn);
           }
        else
           if (v_ptr->class != BUILTIN)
              if (v_ptr->class == BASED)
                 assign(buffer,posn,"-u-");
 
        posn = 45;  /* attributes */

        if (v_ptr->child != NULL)
           { 
           assign (buffer,posn,"struc ");
           posn += 6;  
           }
        else  
           {
           switch (v_ptr->type) {
   
           case(BINARY):
               {
               assign (buffer,posn,"bin(");
               posn += 4;
               itoa ((v_ptr->prec_1),tmp,10);  
               assign (buffer,posn,tmp);
               posn += strlen(tmp);
               if (v_ptr->scale == FIXED)
                  {
                  assign (buffer,posn,",");
                  posn += 1;  
                  itoa ((v_ptr->prec_2),tmp,10);  
                  assign (buffer,posn,tmp);
                  posn += strlen(tmp);
                  }    
               assign (buffer,posn,") ");
               posn += 2;  
               break;
               }
           case(POINTER):
               {
               assign (buffer,posn,"ptr ");
               posn += 4;
               break;
               }
           case(CHARACTER):
               {
               if (v_ptr->varying)
                  {
                  assign (buffer,posn,"char var ");
                  posn += 9;
                  }
               else
                  {
                  assign (buffer,posn,"char ");
                  posn += 5;
                  }
               break;
               }
           case(DECIMAL):
               {
               assign (buffer,posn,"dec(");
               posn += 4;
               itoa ((v_ptr->prec_1),tmp,10);  
               assign (buffer,posn,tmp);
               posn += strlen(tmp);
               if (v_ptr->scale == FIXED)
                  {
                  assign (buffer,posn,",");
                  posn += 1;  
                  itoa ((v_ptr->prec_2),tmp,10);  
                  assign (buffer,posn,tmp);
                  posn += strlen(tmp);
                  }    
               assign (buffer,posn,") ");
               posn += 2;
               break;
               }
           case(BIT):
               {
               assign (buffer,posn,"bit ");
               posn += 4;
               break;
               }
           case(PICTURE):
               {
               assign (buffer,posn,"pic ");
               posn += 4;
               break;
               }
           case(ENTRY):
               {
               assign (buffer,posn,"entry ");
               posn += 6;
               if (v_ptr->variable)
                  {
                  assign (buffer,posn,"variable ");
                  posn += 9;
                  }
               break;
               }
           case(CONDITION):
               {
               assign (buffer,posn,"condition ");
               posn += 10;
               break;
               }
           case(LABEL):
               {
               assign (buffer,posn,"label ");
               posn += 6;
               break;
               }
           case(NUMERIC):
               {
               assign (buffer,posn,"numeric ");
               posn += 8;
               break;
               }
           case(STRING):
               {
               assign (buffer,posn,"string ");
               posn += 7;
               break;
               }
           } /* switch */
           } /* else */

        if (v_ptr->level <= 1)
           {
           switch (v_ptr->scope) {
   
           case(INTERNAL):
               {
               assign (buffer,posn,"internal ");
               posn += 9;  
               break;
               }
           case(EXTERNAL):
               {
               assign (buffer,posn,"external ");
               posn += 9;
               break;
               }
           }
           }

        switch (v_ptr->scale) {
   
        case(FIXED):
            {
            assign (buffer,posn,"fixed ");
            posn += 6;  
            break;
            }
        case(FLOAT):
            {
            assign (buffer,posn,"float ");
            posn += 6;
            break;
            }
         }

        if (v_ptr->array_ptr != NULL)
           if (v_ptr->type != ENTRY)
              {
              assign (buffer,posn,"array ");
              posn += 6;
              } 

		if (v_ptr->initial)
		   {
		   assign (buffer,posn,"init ");
		   posn += 5;
		   }

        /***********************************************************/
        /* Print out the line the symbol was declared on           */
        /***********************************************************/

        if (v_ptr->parent == NULL)
           {
           assign (buffer,posn,"dcl ");
           posn += 4;
           assign (buffer,posn,strpbrk(v_ptr->line,"0123456789"));
           }
   
        if (listing_reqd)
           {       
           fprintf (LISTING,"%s",buffer);
           fprintf (LISTING,"%s","\n\n");
           check_print(2);
           }

        /************************************************************/
        /* If this is a structure, then print any children it has   */
        /************************************************************/

        if (v_ptr->child != NULL)
           print_symbol(v_ptr->child);

        v_ptr = v_ptr->sister;  /* Skip to sister (if any) */

        } 
   
   }

/****************************************************************************/
/* Print the names and usage count of any PL/I conversion operators used    */
/****************************************************************************/

static void

   print_conversions (void)

   {

   if ((conversions_in_use) && (listing_reqd))
      {
      lines_printed = 0;
      fprintf (LISTING,"\f\n");
      fprintf (LISTING,"LANGUAGE CONVERSIONS REQUIRED BY THIS COMPILATION\n\n");
      fprintf (LISTING,"OPERATOR             USAGE COUNT\n\n");
      check_print(5);

      if (cv_fixed_bin_r)
         fprintf (LISTING,"cv_fixed_bin                   %d\n\n",cv_fixed_bin_r);
      if (cv_fixed_dec_r)
         fprintf (LISTING,"cv_fixed_dec                   %d\n\n",cv_fixed_dec_r);
      if (cv_arithmetic_r)
         fprintf (LISTING,"cv_arithmetic                  %d\n\n",cv_fixed_dec_r);
      } 



   }

/**************************************************************************/
/*            Print stack sizes for every internal block.                 */
/**************************************************************************/

static void print_stacks (Block_ptr root)

     {

     if (listing_reqd)
        {
        lines_printed = 0;
        fprintf (LISTING,"\f\n"); 
        fprintf (LISTING,"LENGTH OF STACK FRAMES:\n\n");
        fprintf (LISTING,"BLOCK NAME                        LINE No     STACK\n\n");
        check_print(5);
        }

     print_stack (root);

     }

static void print_stack (Block_ptr root)

     {
     
     while (root != NULL)
           {
           clear_buffer();
           assign (buffer,0,(root->block_name));
           assign (buffer,29,"%s        %5d");
           if (listing_reqd)
              {
              fprintf(LISTING,buffer,(root->line),(root->stack));
              fprintf(LISTING,"\n");
              check_print(1); 
              } 
           print_stack (root->child);
           root = root->sister;
           }
     
     }      
     
   
   
void assign (char * string,
             short start,
             char * text)

     {
     
     strncpy (&string[start],text,strlen(text));

     }


static void clear_buffer (void)

     {
 
     memset (buffer,' ',100);
     buffer[99] = '\0';
      
     }
