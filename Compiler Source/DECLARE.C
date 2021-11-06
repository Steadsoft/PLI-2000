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
/*  20-02-91   HWG       Initial Version.                                  */
/*  07-04-91   HWG       Resolution of references to internal blocks, was  */
/*                       not working.                                      */
/*  20-04-91   HWG       Calculation of array sizes, when array has        */
/*                       constant bounds.                                  */
/*  22-04-91   HWG       Fields within structures were not having their    */
/*                       defaults applied. Process_symbol is now invoked   */
/*                       recursively for structure type symbols.           */
/*  29-04-91   HWG       Invalid/ambiguous structures refs are no longer   */
/*                       handled in here.                                  */
/*  18-05-91   HWG       Offsets of all fields within a structure are now  */
/*                       calculated.                                       */
/*  23-06-91   HWG       Computational data-types are now flagged as such. */
/*                                                                         */
/*  02-08-91   HWG       Resolution of references to variables in now      */
/*                       managed exclusively by pass2, this phase IS NOT   */
/*                       involved in reference resoultion.                 */
/*  20-09-91   HWG       The argument of a based/defined variable must be  */
/*                       resolved in this phase.                           */ 
/*  22-09-91   HWG       The offsets of structure members were not being   */
/*                       correctly calculated in the case of arrays.       */ 
/*  23-09-91   HWG       In the case of arrays of strucs the element size  */
/*                       is held in the field 'size'.                      */     
/*  22-10-91   HWG       Size of entry values changed from 12 to 8 bytes.  */
/*  25-10-91   HWG       Structure attributes (eg storage class) were not  */
/*                       being propagated to members.                      */
/*  20-12-91   HWG       Items declared as external, were being given a    */
/*                       storage class of auto.                            */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                   D E F I N E D     S Y M B O L S                       */
/***************************************************************************/


/***************************************************************************/
/*                        I N C L U D E    F I L E S                       */
/***************************************************************************/

# include "math.h"
# include "stdlib.h"
# include "string.h"
# include "c_types.h"
# include "nodes.h"
# include "symtab.h"
# include "tokens.h"

/***************************************************************************/
/*                  I N T E R N A L      S T A T I C S                     */
/***************************************************************************/

/***************************************************************************/
/*       I N T E R N A L L Y    D E F I N E D    P R O T O T Y P E S       */
/***************************************************************************/


/***************************************************************************/
/*            EXTERNAL   S T A T I C    V A R I A B L E S                  */
/***************************************************************************/

extern Block_ptr   block_root;
extern Block_ptr   curr_block; /* see pass2 for important details on this */

/***************************************************************************/
/*       E X T E R N A L L Y    D E F I N E D    P R O T O T Y P E S       */
/***************************************************************************/

extern char            line_no[10];
void   check_reference (Ref_ptr); /* see pass2 */
void   report          (int,char *,int); 
void   set_error_line  (Any_ptr);

/***************************************************************************/
/*     I N T E R N A L L Y      D E F I N E D    P R O T O T Y P E S       */
/***************************************************************************/

void       process_declares     (void);
int        nodetype             (Any_ptr);
long       curr_member_offset;


static void       process_block        (Block_ptr);
static void       check_reservations   (char *);
static void       process_symbol       (Block_ptr,Symbol_ptr);
static void       calc_member_offsets  (Symbol_ptr);
static long       size                 (Symbol_ptr);
static long       array_mult           (Symbol_ptr);
static void       validate_picture_dcl (Symbol_ptr);
static void       complete_descriptors (Symbol_ptr);
static void       propagate_attributes (Symbol_ptr);

/***************************************************************************/
/*             S T A R T    O F    E X E C U T A B L E     C O D E         */
/***************************************************************************/


void 

   process_declares (void)

   {

   if (block_root != NULL)
      process_block (block_root);

   }

/**************************************************************************/
/* This recursive procedure will process every symbol in the symtab for   */
/* the specified block.                                                   */
/**************************************************************************/

static void 

   process_block (Block_ptr b_ptr)

   {

   Symbol_ptr     s_ptr;
   Symbol_ptr     t_ptr; /* temp */

     while (b_ptr != NULL)
           /************************************************/
           /*      Process This Blocks Symbol Table        */
           /************************************************/
           {

           set_error_line (b_ptr);
      
           check_reservations (b_ptr->block_name);

           s_ptr = b_ptr->first_symbol;

           while (s_ptr != NULL)
                 /******************************************/
                 /*     Process This Symbol Table Entry    */
                 /******************************************/
                 {
                 process_symbol (b_ptr,s_ptr);
                 if (s_ptr->child != NULL)  /* ie if a structure ... */
                    {
                    calc_member_offsets (s_ptr);
                    propagate_attributes (s_ptr);
                    }

				 t_ptr = s_ptr->next_ptr;
                 s_ptr = t_ptr;
                 }  
           /************************************************/
           /* If this block has a child block, process it. */
           /************************************************/
           if (b_ptr->child != NULL)
              process_block (b_ptr->child);
         
           b_ptr = b_ptr->sister;
           }

     }

/**************************************************************************/
/*       This function will process the indicated symbol table entry.     */
/**************************************************************************/

static void 

   process_symbol (Block_ptr b_ptr,Symbol_ptr s_ptr)

   {

   Symbol_ptr      temp_ptr;   
   char            tmp[10];

   /*****************************************************************/
   /* If this symbol is based or defined then we must resolve the   */
   /* argument reference in here.                                   */
   /*****************************************************************/

   if (s_ptr->defbas_ptr != NULL)
      {
      curr_block = s_ptr->declarator; /* used by pass2 for scoping etc */
      strcpy (tmp,line_no);
      strcpy (line_no,s_ptr->line);
      check_reference (s_ptr->defbas_ptr);      
      strcpy (line_no,tmp);
      curr_block = NULL; /* MUST be re-NULL-ed before pass2 is invoked */
      }

   /*****************************************************************/
   /* If s_ptr has children (ie if its a struc) then re-invoke this */
   /* function recursively, for each child.                         */
   /*****************************************************************/

   if (s_ptr->structure)
      {
      temp_ptr = s_ptr->child;

      while (temp_ptr != NULL)
            {
            process_symbol (b_ptr,temp_ptr);
            temp_ptr = temp_ptr->sister;
            }
      }  

   if (s_ptr->child != NULL)
      if (s_ptr->level > 1)
         return; /* If symbol is NOT a field or level 1 name, return */

   if ((s_ptr->declared)  &&
       (s_ptr->type == 0) &&
       (s_ptr->structure == 0) &&
       (s_ptr->qualified == 0) &&
       (s_ptr->class != BUILTIN))
       /***********************************************************/
       /* This variable has been declared without a data-type     */
       /***********************************************************/
       {
       s_ptr->type       = BINARY;
       s_ptr->prec_1     = 15;
       s_ptr->prec_2     = 0;
       s_ptr->class      = AUTOMATIC;
       s_ptr->known_size = 1;
       strcpy (tmp,line_no);
       strcpy (line_no,s_ptr->line);
       report (-68,s_ptr->spelling,__LINE__);  /* Declared with NO type ! */
       strcpy (line_no,tmp);
       }  

   if (s_ptr->type  == PICTURE)
      validate_picture_dcl(s_ptr);
              

   if (s_ptr->class == 0)
       s_ptr->class = AUTOMATIC;

   if (s_ptr->type == LABEL)
      s_ptr->known_size = 1;

   if (s_ptr->type == ENTRY)
      s_ptr->known_size = 1; 

   if ((s_ptr->known_size == 0) && /* no precision given */
       (s_ptr->prec_1     == 0))
      {
      if (s_ptr->type == DECIMAL)
         {
         s_ptr->prec_1 = 7;
         s_ptr->prec_2 = 0;
         }
      if (s_ptr->type == BINARY)
         {
         s_ptr->prec_1 = 15;
         s_ptr->prec_2 = 0;
         } 
      if (s_ptr->type == CHARACTER)
         s_ptr->prec_1 = 1;
      s_ptr->known_size = 1;
      }   

   if (s_ptr->scope != EXTERNAL)
      s_ptr->scope = INTERNAL;
   else
      s_ptr->class = STATIC;
       
   if (s_ptr->type == ENTRY)
      {
      s_ptr->scope = EXTERNAL;
      s_ptr->class = STATIC;
      complete_descriptors(s_ptr);
      }

   if ((s_ptr->type == BINARY) ||
       (s_ptr->type == DECIMAL) ||
       (s_ptr->type == PICTURE))
      if (s_ptr->scale != FLOAT)
         s_ptr->scale = FIXED;

   /******************************************************************/
   /* If this field has a computational data type then set its flag  */
   /******************************************************************/

   if ((s_ptr->type == BINARY) ||
       (s_ptr->type == DECIMAL) ||
       (s_ptr->type == BIT) ||
       (s_ptr->type == PICTURE) ||
       (s_ptr->type == CHARACTER) ||
       (s_ptr->type == NUMERIC))
       s_ptr->computational = 1;

   /******************************************************************/
   /* If this symbol is a member of a structure, then return, sizes  */
   /* of structures will be worked out for Level 1 structures.       */
   /******************************************************************/

   if (s_ptr->level > 1)
      return;

   s_ptr->bytes = size (s_ptr);

   if (s_ptr->class == STATIC)
      {
      s_ptr->known_locn = 1;
      s_ptr->offset  = b_ptr->stattic;
      b_ptr->stattic += s_ptr->bytes;
      }
   else
   if (s_ptr->class == AUTOMATIC)
      {
      s_ptr->known_locn = 1;
      s_ptr->offset  = b_ptr->stack;
      b_ptr->stack   += s_ptr->bytes;
      }
   else
   if (s_ptr->class == PARAMETER)
      {
      s_ptr->known_locn = 1;
      s_ptr->offset  = (b_ptr->params - 4) - s_ptr->offset;
      }  
  
   }

/****************************************************************************/
/* This function fills in the data description for each argument descriptor */
/* that appears in an entry declaration. ie any unspecified attributes are  */
/* filled in here.                                                          */
/****************************************************************************/

static void 

   complete_descriptors (Symbol_ptr s_ptr)

   {

   Data_ptr              d_ptr;

   d_ptr = s_ptr->array_ptr;
    
   while (d_ptr != NULL)
         {
         if ((d_ptr->data_type == BINARY) || (d_ptr->data_type == DECIMAL))
            if (d_ptr->scale == 0)
               d_ptr->scale = FIXED;

         d_ptr = d_ptr->next_ptr;
         }


   } 

/***************************************************************************/
/* This recursive procedure will calculate the storage needed for fixed    */
/* size variables, it goes recursive in the case of a structure.           */
/***************************************************************************/

static long 

   size (Symbol_ptr s_ptr)

   {

   Symbol_ptr    t_ptr;
   long          running_total;
   long          this_fields_size;
   long          multiplier;
   
   /*********************************************************************/
   /* If we have been passed a scalar variable, then calculate its size */
   /* and return that size back to caller. Also set the size in here    */
   /*********************************************************************/ 

   this_fields_size = 0;

   if (s_ptr->child == NULL)
      {
      if (s_ptr->type == POINTER)        /* pointer */
         this_fields_size = 4;

      if (s_ptr->type == ENTRY)
         this_fields_size = 12; /* 3 ptrs, static, code and display */
   
      if (s_ptr->type == LABEL)
         this_fields_size = 4;
   
      if (s_ptr->type == CHARACTER)      /* char    */
         {
         if (s_ptr->varying)
            this_fields_size = s_ptr->prec_1 + 2;
         else
            this_fields_size = s_ptr->prec_1;
         }
      if (s_ptr->type == BINARY)         /* binary  */
         {
         if (s_ptr->prec_1 > 0)
            this_fields_size += 1;
         if (s_ptr->prec_1 > 8)
            this_fields_size += 1;
         if (s_ptr->prec_1 > 16)
            this_fields_size += 1;
         if (s_ptr->prec_1 > 24)
            this_fields_size += 1;
         }

      if (s_ptr->type == BIT)            /* binary  */
         if (s_ptr->aligned)
            this_fields_size = s_ptr->prec_1;
         else
            this_fields_size = (short)(ceil((float)s_ptr->prec_1 / 8));
         


      if (s_ptr->type == DECIMAL)        /* decimal */
         {
         if ((s_ptr->prec_1 > 15)  && (s_ptr->scale == FLOAT))
            report(88,s_ptr->spelling,__LINE__);
         else
         if ((s_ptr->prec_1 > 18) && (s_ptr->scale == FIXED))
            report(105,s_ptr->spelling,__LINE__);
         else
         this_fields_size = (short)(1 + ceil((float)(s_ptr->prec_1 / 2)));
         }

      /****************************************/
      /* this is the element size if field is */
      /* an array. Used to calculate offsets. */
      /****************************************/

      s_ptr->size = this_fields_size; 

      multiplier = array_mult (s_ptr);

      if (multiplier == 0)
         s_ptr->known_size = 0;  /* ie size is variable at runtime */
      else
         {
         s_ptr->known_size = 1;
         this_fields_size  = this_fields_size * multiplier;
         }

      s_ptr->bytes = this_fields_size;

      return (this_fields_size);
      }

   /**********************************************************************/
   /* If we were passed a structure variable, then set its size to the   */
   /* sum of the sizes of its immediate child members.                   */
   /**********************************************************************/

   t_ptr = s_ptr->child;   /* get 1st child */

   running_total = 0;

   while (t_ptr != NULL)
         {
         running_total += size (t_ptr);
         t_ptr = t_ptr->sister;  /* childs sister, ie parents next child */
         }

   s_ptr->size = running_total; /* this is the element size if struc is */
                                /* an array. Used to calculate offsets. */

   multiplier = array_mult (s_ptr);

   if (multiplier != 0)
      {
      s_ptr->known_size = 1;
      running_total = running_total * multiplier;
      }

   s_ptr->bytes = running_total;

   return (running_total);

   } 

/****************************************************************************/
/* This function will calculate the total array extent of an array variable */
/****************************************************************************/

static long

   array_mult (Symbol_ptr s_ptr)

   {

   Dim_ptr     d_ptr;
   Ref_ptr     r_ptr; 
   short       lbound = 1;
   short       hbound = 1;
   long        total; 

   d_ptr = s_ptr->array_ptr;

   if (d_ptr == NULL) /* Not an array */
      return(1);

   if (nodetype(d_ptr) != ARRAY)
      return(1); /* Not an array, array_ptr points to entry descriptors */

   total = 1;

   while (d_ptr != NULL)
         {
         /*************************************************/
         /* Process the lower bound of this dimension     */
         /*************************************************/

         if (d_ptr->lower == NULL)
            lbound = 1;
         else
            if (nodetype(d_ptr->lower) == REFERENCE) 
               {
               r_ptr = d_ptr->lower;
               s_ptr = r_ptr->symbol;
               if (s_ptr->class == CONSTANT)
                  if (s_ptr->type == NUMERIC)
                     lbound = (short)atol(s_ptr->spelling);
                  else
                     {
                     /**********************************************/
                     /* A non Numeric constant, in an array spec ! */
                     /**********************************************/
                     report (85,s_ptr->spelling,__LINE__);
                     return(0);
                     } 
               }

         /*************************************************/
         /* Process the lower bound of this dimension     */
         /*************************************************/

         if (d_ptr->upper == NULL)
            hbound = 1;
         else
            if (nodetype(d_ptr->upper) == REFERENCE)  
               {
               r_ptr = d_ptr->upper;
               s_ptr = r_ptr->symbol;
               if (s_ptr->class == CONSTANT)
                  if (s_ptr->type == NUMERIC)
                     hbound = (short)atol(s_ptr->spelling);
                  else
                     {
                     /**********************************************/
                     /* A non Numeric constant, in an array spec ! */
                     /**********************************************/
                     report (85,s_ptr->spelling,__LINE__);
                     return(0);
                     } 
               }

         /*********************************************/
         /* Ok now update the array multiplier total  */
         /*********************************************/
   
         total = total * ((hbound - lbound) + 1);

         d_ptr = d_ptr->next_ptr;

         }

   return (total);

   }


/***************************************************************************/
/* This function calculates the offsets (WRT the structure level 1 name)   */
/* of all structure members (fields or substructures).                     */
/***************************************************************************/

static void 

   calc_member_offsets (Symbol_ptr s_ptr)

   {

   Symbol_ptr         ch_ptr; 

   if (s_ptr->level == 1)
      curr_member_offset = 0;

   ch_ptr = s_ptr->child;

   while (ch_ptr != NULL)
         {
         if (ch_ptr->child == NULL) /* ie a non-struc type member */
            {
            ch_ptr->offset = curr_member_offset;
            curr_member_offset += ch_ptr->bytes;
            ch_ptr->known_locn = 1;
            }
         else
            {
            ch_ptr->offset = curr_member_offset;
            calc_member_offsets (ch_ptr);
            curr_member_offset = (ch_ptr->bytes + ch_ptr->offset);
            ch_ptr->known_locn = 1;
            }
         ch_ptr = ch_ptr->sister;
         }

   }

/****************************************************************************/
/* This function will propagate any structures attributes (ie class) to all */
/* of its members.                                                          */
/****************************************************************************/

static void 

   propagate_attributes (Symbol_ptr s_ptr)

   {

   Symbol_ptr            temp_ptr;

   /************************************************/
   /* If we have a parent then copy its attributes */
   /************************************************/

   if (s_ptr->parent != NULL)
      {
      s_ptr->class      = s_ptr->parent->class;
      s_ptr->vola_tile  = s_ptr->parent->vola_tile;
      s_ptr->defbas_ptr = s_ptr->parent->defbas_ptr;
      }

   if (s_ptr->child == NULL)
      return;

   temp_ptr = s_ptr->child;

   /************************************************************/
   /*     Process all children of this particular structure.   */
   /************************************************************/

   while (temp_ptr != NULL)
         {
         propagate_attributes (temp_ptr);
         temp_ptr = temp_ptr->sister;
         } 

   }

/***************************************************************************/
/* Check for use of a reserved Vulkan procedure prefix.                    */
/***************************************************************************/

static void 

   check_reservations (char * name)

   {

   if (strncmp(name,"dos$",4) == 0)
      {
      report(-122,name,__LINE__);
      return;
      }

   if (strncmp(name,"pli$",4) == 0)
      {
      report(-122,name,__LINE__);
      return;
      }

   if (strncmp(name,"bios$",4) == 0)
      {
      report(-122,name,__LINE__);
      return;
      }

   if (strncmp(name,"lim$",4) == 0)
      {
      report(-122,name,__LINE__);
      return;
      }

   if (strncmp(name,"io$",4) == 0)
      {
      report(-122,name,__LINE__);
      return;
      }

   if (strncmp(name,"net$",4) == 0)
      {
      report(-122,name,__LINE__);
      return;
      }

   if (strncmp(name,"sys$",4) == 0)
      {
      report(-122,name,__LINE__);
      return;
      }


   } 

/****************************************************************************/
/* This function validates a picture specification for picture declaration  */
/****************************************************************************/

static void 

   validate_picture_dcl (Symbol_ptr s_ptr)

   {

   s_ptr = s_ptr; /* just suppress compiler warning for now ! */


   }
   
