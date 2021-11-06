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

/*--------------------------------------------------------------------------*/
/*                         Modification History                             */
/*--------------------------------------------------------------------------*/
/*  Who    When                           What                              */
/* ------------------------------------------------------------------------ */
/* HWG    20-06-91     Initial version of module.                           */
/* HWG    15-09-91     Dont create EXTDEF's for unreferenced entries.       */
/* HWG    23-09-91     EXTDEF records did not have the TYPE byte present.   */
/* HWG    29-09-91     COMENT records emmited with copyrite stuff etc.      */
/* HWG    30-09-91     MODEND record function implemented (called by code.c */
/* HWG    30-09-91     LEDATA records were not referring to their SEGDEF    */  
/*                     records correctly.                                   */
/* HWG    12-10-91     The stack segment is created in this module now.     */              
/*                                                                          */
/* HWG    23-10-91     All SEGDEF records are now created prior to any of   */
/*                     the LEDATA records.                                  */
/* HWG    24-10-91     Fixups emitted for internal entry values.            */
/*                                                                          */
/* HWG    25-10-91     Fixups are emitted for external entries, and public  */
/*                     and external object module refs are created for      */
/*                     the outer block and external entries respectively.   */     
/* HWG    30-11-91     Dont generate fixups for unreferenced externals.     */
/*                                                                          */
/* HWG    20-06-93     Report all unreferenced names except those declared  */
/*                     via include files.                                   */
/*																			*/
/* HWG    20-06-96     Added diag 150 about the allocation of labels.       */ 
/*                                                                          */
/* HWG    20-09-02     Major change to COFF 32 Bit support on NT.           */
/*                     DOS .OBJ support removed completely.                 */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                         Functional Description                           */
/*--------------------------------------------------------------------------*/
/* This module is the storage allocator. It walks-thru the symbol table     */
/* and creates segment definition records for each procedure, for the       */ 
/* following: static storage, external names, constant storage.             */
/* The records are written to the object file.                              */
/* LNAMES records are also produced for CODE and DATA.                      */            
/* This phase also creates PUBDEF records for the outer block of the source */
/* file being compiled, and EXTDEF records for any external entries.        */
/* The MODEND record is not written in here, but rather at the end of the   */
/* code generation phase.                                                   */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                    R E P L A C E    S T A T E M E N T S                  */
/*--------------------------------------------------------------------------*/

# define chur              unsigned char
# define COMPILER_NAME     "Win32 PL/1 Optimizing Compiler."
# define COPYRITE_TEXT     "Copyright (c) Hugh Gleaves 2006."
# define ONE_K             1024

/*--------------------------------------------------------------------------*/
/*                    I N C L U D E    F I L E S                            */
/*--------------------------------------------------------------------------*/
# include "c_types.h"
//# include "acbp.h"
# include "objdefs.h"
# include "nodes.h"
# include "tokens.h"
# include "stdio.h"
# include "stdlib.h"
# include "setjmp.h"
# include "fixup.h"
# include "string.h"
//# include "windef.h"
# include "coff.h"

/*--------------------------------------------------------------------------*/
/*                  E X T E R N A L    V A R I A B L E S                    */
/*--------------------------------------------------------------------------*/

#include "globals.h"
 
/*--------------------------------------------------------------------------*/
/*                  I M P O R T E D     F U N C T I O N S                   */
/*--------------------------------------------------------------------------*/

void report          (int,char *,int);

/*--------------------------------------------------------------------------*/
/*                  I N T E R N A L    V A R I A B L E S                    */
/*--------------------------------------------------------------------------*/

static jmp_buf                exit_allocator;
static chur                   objrec[4096];
static short                  objlen;
static short                  curr_extdef_num = 0;
             
/*--------------------------------------------------------------------------*/
/*                  I N T E R N A L    F U N C T I O N S                    */
/*--------------------------------------------------------------------------*/

void        allocator         (char*);

static void open_object       (char*);
static void build_coff_hdr    (char*);
static void build_sect_hdrs   (Block_ptr);
static void build_coff_symtab (Block_ptr);
static void build_coff_stable (Block_ptr);
static void build_sections    (Block_ptr);

static void allocate_static   (Block_ptr);
static void store_constants   (Block_ptr,short,short);
static void allocate_constants(Block_ptr);
static void allocate_extern   (Block_ptr);
static void set_checksum      (void);
static void allocate_storage  (Block_ptr);
static void create_lnames     (Block_ptr);
static void clean_buffer      (void);
static void create_stack_seg  (Block_ptr);
static void create_static_seg (Block_ptr);

/*--------------------------------------------------------------------------*/
/*                  E X P O R T E D     F U N C T I O N S                   */
/*--------------------------------------------------------------------------*/

int  long_index               (short,short);
chur short_index              (short,short);
void begin_fixup              (void);
void write_fixup              (void);
static void gen_fixup                (void);
void create_code_seg          (Block_ptr);

/*--------------------------------------------------------------------------*/
/*            S T A R T    O F    E X E C U T A B L E    C O D E            */
/*--------------------------------------------------------------------------*/

void allocator (char * file)

     {

     block_counter = 0;                                          
                                                                          
     if (setjmp(exit_allocator) == 0)
        {
		/*----------------------------------------*/
		/* Open the object file for binary output */
		/*----------------------------------------*/
        open_object (file);
		/*------------------------------------------------------------*/
		/* Create a COFF header. This forms the top of an internal    */
		/* tree that sections, symbols, linenums, relocs all hang off */
		/*------------------------------------------------------------*/
        build_coff_hdr (file);
		/*------------------------------------------------------------*/
		/* Now create the section headers. We create section headers  */
		/* for each blocks code and static. The number of the section */
		/* is stored in the 'block' node for later cross referencing. */
		/*------------------------------------------------------------*/
        build_sect_hdrs (block_root);
		/*---------------------------------------------------------------*/
		/* Now we build the COFF symbol table, we represent every symbol */
		/* in this table.                                                */
		/*---------------------------------------------------------------*/
		build_coff_symtab (block_root);
		/*---------------------------------------*/
		/* Next we create the COFF string table. */
		/*---------------------------------------*/
		build_coff_stable (block_root);
		/*-------------------------------------------------------------*/
		/* We must now create all per-section data for every section   */
		/* This consists of: the data block itself, relocations and    */
		/* line numbers. The latter is actually created by the codegen */
		/*-------------------------------------------------------------*/
        build_sections (block_root);  /* alloc store for root proc. */
        }
     else
        return;

     }

/*-------------------------------------------------------------------------*/
/* This function creates the COFF header and other one-off section headers */
/*-------------------------------------------------------------------------*/

static void build_coff_hdr (char * name)


     {

     obj_root = CoffCreateObject();

     }


/*--------------------------------------------------------------------------------------*/
/* This function creates a new empty section header using the name and number passed in */
/* by the caller.                                                                       */
/*--------------------------------------------------------------------------------------*/

static Section_ptr create_section (char * name)

{

Section_ptr		sect_ptr;
long            nindex;

sect_ptr = CoffInsertSection(obj_root);

if (strlen(name) <= 8)
   strncpy(sect_ptr->sect.Name,name,8);
else
   {
   nindex = CoffInsertString(obj_root,name);
   sect_ptr->sect.Name[0] = '/';
   ltoa(nindex,&(sect_ptr->sect.Name[1]),10);
   }

return(sect_ptr);

}

/*---------------------------------------------------------------------------*/
/* This function walks the symtab and creates section headers for each block */
/*---------------------------------------------------------------------------*/

static void build_sect_hdrs (Block_ptr b_ptr)


     {

     Section_ptr		sect_ptr;
	 CoffSym_ptr		csym_ptr;
     char               sname[64];

     if (b_ptr == NULL)
        return;

     block_counter++;

     /*--------------------------------------------*/
	 /* Create section header for this blocks code */
	 /*--------------------------------------------*/

     strcpy(sname,".text$");
     strcat(sname,b_ptr->block_name);
	 sect_ptr = create_section(sname);
	 b_ptr->code_idx = sect_ptr->sect_num;
	 sect_ptr->sect.Characteristics      = IMAGE_SCN_CNT_CODE + 
	                                       IMAGE_SCN_MEM_READ + 
										   IMAGE_SCN_MEM_EXECUTE +
										   IMAGE_SCN_ALIGN_16BYTES;
										   
     if (b_ptr->depth > 0)
        {
	    sect_ptr->sect.Characteristics   = IMAGE_SCN_CNT_CODE + 
	                                       IMAGE_SCN_MEM_READ + 
										   IMAGE_SCN_MEM_EXECUTE +
										   IMAGE_SCN_ALIGN_1BYTES;
        }
        										    

     /* Create a COFF symtab entry for the sections name */

     csym_ptr = CoffCreateSymbol (obj_root);
     strncpy(csym_ptr->sym.Name.ShortName,".text",8);
     csym_ptr->sym.SectionNumber = b_ptr->code_idx;
     csym_ptr->sym.StorageClass  = IMAGE_SYM_CLASS_STATIC;
     csym_ptr->sym.Type          = 0;
	 csym_ptr->sym.Value         = 0;

     /*----------------------------------------------*/
     /* Create section header for this blocks static */
	 /*----------------------------------------------*/

     strcpy(sname,".data$");
     strcat(sname,b_ptr->block_name);
	 sect_ptr = create_section(sname);
	 b_ptr->data_idx = sect_ptr->sect_num;
	 sect_ptr->sect.Characteristics      = IMAGE_SCN_CNT_INITIALIZED_DATA +
	                                       IMAGE_SCN_MEM_READ +
										   IMAGE_SCN_MEM_WRITE;
     /* Create a COFF symtab entry for the sections name */

     csym_ptr = CoffCreateSymbol (obj_root);
     strncpy(csym_ptr->sym.Name.ShortName,".data",8);
     csym_ptr->sym.SectionNumber = b_ptr->data_idx;
     csym_ptr->sym.StorageClass  = IMAGE_SYM_CLASS_STATIC;
     csym_ptr->sym.Type          = 0;
	 csym_ptr->sym.Value         = 0;


     b_ptr = b_ptr->child;

     while (b_ptr != NULL)
           {
           build_sect_hdrs (b_ptr);
           b_ptr = b_ptr->sister;
           }
     }

/*------------------------------------------------------------------------*/
/* Generate symbol table entries for every relevant symbol in every block */
/*------------------------------------------------------------------------*/

static void build_coff_symtab (Block_ptr b_ptr)

{

CoffSym_ptr		csym_ptr;
Symbol_ptr	    s_ptr;

if (b_ptr == NULL)
   return;

/*---------------------------------------------------------------*/
/* Generate a symtab entry for the outermost blocks name.        */
/* Internal blocks are added as internal declarations of their   */
/* parent, but the outer block has no parent to do this.         */
/*---------------------------------------------------------------*/

if (b_ptr->parent == NULL)
   {
   csym_ptr = CoffCreateSymbol (obj_root);

   if (strlen(b_ptr->block_name) <= 8)
      strncpy(csym_ptr->sym.Name.ShortName,b_ptr->block_name,8);
   else
      csym_ptr->sym.Name.Name.Offset = CoffInsertString(obj_root,b_ptr->block_name);
 
   csym_ptr->sym.SectionNumber = b_ptr->code_idx;
   csym_ptr->sym.StorageClass  = IMAGE_SYM_CLASS_EXTERNAL;
   csym_ptr->sym.Type          = (IMAGE_SYM_TYPE_NULL /* + (256 * IMAGE_SYM_DTYPE_FUNCTION)*/);
   }

/*-------------------------------------------------*/
/* Now get a ptr to this blocks first symbol entry */
/* process it and then iterate all symbols for the */
/* PL/I block.                                     */
/*-------------------------------------------------*/

s_ptr = b_ptr->first_symbol;

while (s_ptr != NULL)
      {
	  if ((s_ptr->referenced) && (s_ptr->class != CONSTANT)) 
	     {
	     csym_ptr = CoffCreateSymbol (obj_root);

		 s_ptr->coff_symtab_idx = csym_ptr->sym_idx;

		 if (strlen(s_ptr->spelling) <= 8)
	        strncpy(csym_ptr->sym.Name.ShortName,s_ptr->spelling,8);
		 else
		    csym_ptr->sym.Name.Name.Offset = CoffInsertString (obj_root,s_ptr->spelling);

	     csym_ptr->sym.SectionNumber = b_ptr->data_idx;
	  
	     /* Set the storage class */
	     
	     if ((s_ptr->type == ENTRY) && (s_ptr->scope == INTERNAL))
	        {
	        csym_ptr->sym.StorageClass  = IMAGE_SYM_CLASS_STATIC;
	        csym_ptr->sym.SectionNumber = s_ptr->proc_ptr->code_idx; // b_ptr->code_idx;
	        csym_ptr->sym.Value         = 0; // entry points always begin at offset zero in their section
	        }
	     else
  	     if ((s_ptr->type == ENTRY) && (s_ptr->scope == EXTERNAL))
            {
	        csym_ptr->sym.StorageClass  = IMAGE_SYM_CLASS_EXTERNAL;
	        csym_ptr->sym.Type          = 0x2000;
	        csym_ptr->sym.SectionNumber = IMAGE_SYM_UNDEFINED; // externally defined names have no section number!!
	        csym_ptr->sym.Value         = s_ptr->bytes;
	        }
	     else

	     switch (s_ptr->class) {
	     case(STATIC):
	         {
		     csym_ptr->sym.StorageClass = IMAGE_SYM_CLASS_STATIC; //IMAGE_SYM_CLASS_STATIC;
		     csym_ptr->sym.Value        = s_ptr->offset;
		     break;
		     }
	     case(AUTOMATIC):
	         {
	         csym_ptr->sym.StorageClass = IMAGE_SYM_CLASS_AUTOMATIC;
	         csym_ptr->sym.Value        = s_ptr->offset;
		     break;
		     }
	         }

	     /*------------------------------------------------------*/
	     /* If sym is external, we must set this in StorageClass */
	     /*------------------------------------------------------*/

	        
         /* Now we set the Type */

	     if (s_ptr->type == BINARY)
	        if (s_ptr->scale == FIXED)
		       {
		       if (s_ptr->prec_1 == 15)
			      csym_ptr->sym.Type = IMAGE_SYM_TYPE_SHORT;
			   if (s_ptr->prec_1 == 31)
			      csym_ptr->sym.Type = IMAGE_SYM_TYPE_LONG;
               }
	     if (s_ptr->type == POINTER)
	        csym_ptr->sym.Type = (IMAGE_SYM_TYPE_VOID + (256 * IMAGE_SYM_DTYPE_POINTER));

         /* Is this a structure ? */

	     if (s_ptr->structure)
	        csym_ptr->sym.Type = IMAGE_SYM_TYPE_STRUCT;

         /* Is this an array ? */

	     if ((s_ptr->num_dims > 0) && (s_ptr->type != ENTRY))
	        csym_ptr->sym.Type = (unsigned short)(csym_ptr->sym.Type + (256 * IMAGE_SYM_DTYPE_ARRAY));
         }

      s_ptr = s_ptr->next_ptr;
	  }

/*-------------------------------------------------------------*/
/* OK, now do the same for any child blocks of this PL/I block */
/*-------------------------------------------------------------*/

b_ptr = b_ptr->child;

while (b_ptr != NULL)
      {
      build_coff_symtab (b_ptr);
      b_ptr = b_ptr->sister;
      }
}

static void build_coff_stable (Block_ptr b_ptr)

{



}

static void build_sections (Block_ptr b_ptr)

{


allocate_storage (block_root);



}
/*-------------------------------------------------------------------------*/
/* This recursive function allocates various 80286 segments for the block  */
/* and all of its child blocks.                                            */
/*-------------------------------------------------------------------------*/

static void allocate_storage (Block_ptr b_ptr)

     {

     Block_ptr          temp_ptr;

     if (b_ptr == NULL)
	    return;

	 allocate_constants (b_ptr);
	 //allocate_extern    (b_ptr);
     allocate_static    (b_ptr);

     temp_ptr = b_ptr->child;

     while (temp_ptr != NULL)
           {
           allocate_storage(temp_ptr);
           temp_ptr = temp_ptr->sister;
           }
     }




/*-------------------------------------------------------------------------*/
/* This function creates the obj records needed to define the data segment */
/* for a given blocks static storage.                                      */
/* Constants are held in the static segment as well.                       */
/*-------------------------------------------------------------------------*/

static void allocate_static (Block_ptr b_ptr)


{

Symbol_ptr	    sym_ptr;
Section_ptr		sect_ptr;
CoffReloc		reloc;

sect_ptr = CoffGetSectionPtr (obj_root,b_ptr->data_idx);

sect_ptr->data_ptr = malloc(b_ptr->stattic);
memset(sect_ptr->data_ptr,0,b_ptr->stattic);
sect_ptr->sect.SizeOfRawData = b_ptr->stattic;
total_static += b_ptr->stattic;

sym_ptr = b_ptr->first_symbol;

while (sym_ptr != NULL)
      {
	  if ((sym_ptr->type == ENTRY) && (sym_ptr->scope == INTERNAL))
	     {
         reloc.RelUn.VirtualAddress = sym_ptr->offset;
	     reloc.SymbolTableIndex     = sym_ptr->coff_symtab_idx;
	     reloc.Type                 = 0x0006;
         CoffInsertReloc(sect_ptr,&reloc);
		 }
		 
	  if ((sym_ptr->type == ENTRY) && (sym_ptr->scope == EXTERNAL))
	     {
         reloc.RelUn.VirtualAddress = sym_ptr->offset;
	     reloc.SymbolTableIndex     = sym_ptr->coff_symtab_idx;
	     reloc.Type                 = 0x0006;
         CoffInsertReloc(sect_ptr,&reloc);
		 }

	  sym_ptr = sym_ptr->next_ptr;
	  }

}

/*-------------------------------------------------------------------------*/
/* This function creates obj records that define all external names for    */
/* the specified PL/1 block.                                               */
/*-------------------------------------------------------------------------*/

static void allocate_extern (Block_ptr b_ptr)

     {

     Symbol_ptr      s_ptr;

     s_ptr = b_ptr->first_symbol;

     while (s_ptr != NULL)
           {
           if ((s_ptr->scope == EXTERNAL) && (s_ptr->referenced)) 
              /*---------------------------------------------------------*/
              /* Create an EXTDEF record, and append to the obj module.  */
              /*---------------------------------------------------------*/
//              create_extdef(s_ptr);                             
           s_ptr = s_ptr->next_ptr;
           }
     }

  
/*-------------------------------------------------------------------------*/
/* This function opens the output object module, for binary, write mode.   */
/*-------------------------------------------------------------------------*/

static void open_object (char * name)

     {

     char         path[256];
     size_t       pos;

     strupr (name);
     pos = strcspn (name,".");
     strcpy (path,name);

     strcpy(&path[++pos],"O");
     strcpy(&path[++pos],"B");
     strcpy(&path[++pos],"J");

     OB = fopen(path,"wb+");    /* THis should be a BINARY UPDATE file ! */

     if (OB == NULL)
        {
        report(92,name,__LINE__);
        longjmp(exit_allocator,2);
        }
   
     }





/*-------------------------------------------------------------------------*/
/* This function will examine all symbols in the specified block. If it    */
/* finds a constant then it is assigned an offset into the static of its   */
/* owning block and its data attributes are completed.                     */
/* This attribute completion is handled here rather than declare.c so as   */
/* to allow future upgrading of pass2 to preconvert constants.             */
/*-------------------------------------------------------------------------*/

static void 
   
   allocate_constants (Block_ptr b_ptr)

   {

   Symbol_ptr         s_ptr;
   Section_ptr        sect_ptr;
   short			  length;
   
   s_ptr = b_ptr->first_symbol;

   while (s_ptr != NULL)
         {
		 /*---------------------------------*/
		 /* Read only, constant string data */
		 /*---------------------------------*/
  	     if (s_ptr->class == CONSTANT)
	        if (s_ptr->type == STRING)
		       {
   		   	   sect_ptr = create_section(".rdata");
	           sect_ptr->sect.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA+IMAGE_SCN_MEM_READ;
			   length = (short)strlen(s_ptr->spelling);
               sect_ptr->data_ptr = malloc(length);
               memcpy(sect_ptr->data_ptr,s_ptr->spelling,length);
               sect_ptr->sect.SizeOfRawData = length;
			   total_static += length;
			   s_ptr->cons_idx = sect_ptr->sect_num;
			   }

         /*------------------------------------------------------------*/
         /* If this symbol isnt referenced, and it was not declared in */
         /* an include file (ie line contains a '-') then report it.   */
         /*------------------------------------------------------------*/

         s_ptr = s_ptr->next_ptr;  /* look at next symbol */

         }

   }

/*--------------------------------------------------------------------------*/
/* This function will examine all constants held in the specified block and */
/* assign their values to physical storage in the LEDATA segment currently  */
/* being built.                                                             */
/*--------------------------------------------------------------------------*/

static void 
   
   store_constants (Block_ptr b_ptr, short oblen, short offset)

   {

   Symbol_ptr       s_ptr;
   short              recoffset;
   short             *vcs_len;
   chur            *pointer;

   s_ptr = b_ptr->first_symbol;

   while (s_ptr != NULL)
         {
         if (s_ptr->class == CONSTANT)
            {
            switch(s_ptr->type) {

            case(STRING):
                {
                /*-----------------------------------------------*/
                /* OK We now examine the offset of this constant */
                /* to see if it should go into the curren LEDATA */
                /*-----------------------------------------------*/
                if ((s_ptr->offset >= offset) &&
                    (s_ptr->offset <= (offset + ONE_K)))
                   {
                   recoffset = (int)((s_ptr->offset + oblen) - 1); /* skip over hdr */
                   pointer = &(objrec[recoffset]);
                   vcs_len = (short *)pointer;
                   *vcs_len = s_ptr->prec_1;
                   strcpy(&objrec[recoffset+2],s_ptr->spelling); 
                   }
                }
            case(NUMERIC):
                {
                break;
                }
     	    case(LABEL):
		        {
			    report(150,s_ptr->spelling,__LINE__);
			    break;
			    }
            default:
                report(124,"",__LINE__);
            }
            } /* if */
         s_ptr = s_ptr->next_ptr;
         }

   }
