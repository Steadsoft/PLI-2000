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
/*                    PL/1 Compiler Release 1.0                             */
/*--------------------------------------------------------------------------*/
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
# include "coff.h"
# include "time.h"
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

/*--------------------------------------------------------------------------*/
/*                  I N T E R N A L    F U N C T I O N S                    */
/*--------------------------------------------------------------------------*/

ObjFile_ptr CoffCreateObject (void)

{

ObjFile_ptr	ob_ptr;

ob_ptr = malloc(sizeof(ObjFile));

ob_ptr->hdr.NumberOfSections     = 0;
ob_ptr->hdr.NumberOfSymbols      = 0;
ob_ptr->hdr.PointerToSymbolTable = 0;
ob_ptr->hdr.Machine              = IMAGE_FILE_MACHINE_I386;
ob_ptr->hdr.SizeOfOptionalHeader = 0;
ob_ptr->hdr.TimeDateStamp        = time(NULL);
ob_ptr->hdr.Characteristics      = IMAGE_FILE_32BIT_MACHINE +
								   IMAGE_FILE_BYTES_REVERSED_LO +
								   IMAGE_FILE_LINE_NUMS_STRIPPED;

ob_ptr->first_sym_ptr            = NULL;
ob_ptr->string_ptr               = NULL;
ob_ptr->string_space_left	     = 0;

return (ob_ptr);

}

Section_ptr CoffInsertSection (ObjFile_ptr	ob_ptr)

{

Section_ptr sect_ptr;
WORD	    n;

ob_ptr->hdr.NumberOfSections++;

n = ob_ptr->hdr.NumberOfSections;

sect_ptr = malloc(sizeof(Section));

ob_ptr->sec_ptr[n] = sect_ptr;

sect_ptr->sect_num = n;

/*------------------------------------------------------------*/
/* Set the pointer to this section headers data block to NULL */
/* This will be assigned later in the compilation process.    */
/*------------------------------------------------------------*/

sect_ptr->data_ptr        = NULL;
sect_ptr->allocated_space = 0;

/*-----------------------------------------------------------------*/
/* These two pointers will point to a singly linked lits of nodes. */
/* The nodes contain a COFF relocation/symbol and a pointer to the */
/* next in the list.                                               */
/*-----------------------------------------------------------------*/

sect_ptr->reloc_ptr   = NULL;
sect_ptr->num_relocs  = 0;

/*--------------------------------------------------*/
/* Init all remaining fields in the Section Header  */
/*--------------------------------------------------*/

sect_ptr->sect.Misc.VirtualSize     = 0;
sect_ptr->sect.VirtualAddress       = 0;
sect_ptr->sect.SizeOfRawData        = 0;
sect_ptr->sect.PointerToRawData     = 0;
sect_ptr->sect.PointerToRelocations = 0;
sect_ptr->sect.PointerToLinenumbers = 0;
sect_ptr->sect.NumberOfRelocations  = 0;
sect_ptr->sect.NumberOfLinenumbers  = 0;
sect_ptr->sect.Characteristics      = 0;

/*-----------------------------------------------------------------*/
/* Finally return a pointer to this new Section Header so that the */
/* caller can carry out further processing upon it.                */
/*-----------------------------------------------------------------*/

return (sect_ptr);

}

long CoffInsertString (ObjFile_ptr ob_ptr, char * instr)

{

long	offset;
char *  next_ptr;

/*-------------------------------------------------------*/
/* If no strings have yet been stored in the string pool */
/* we will allocate 4K as our intial space.              */
/*-------------------------------------------------------*/

if (ob_ptr->string_ptr == NULL)
   {
   ob_ptr->string_ptr = malloc(4096);
   STRTABSIZE(ob_ptr) = 4;
   ob_ptr->string_space_left = 4096 - 4;
   }

/*----------------------------------------------------------*/
/* If there is insufficient space to store this string then */
/* lets increase the string pool by 4K.                     */
/*----------------------------------------------------------*/

if (ob_ptr->string_space_left <= (long)(strlen(instr)))
   {
   ob_ptr->string_ptr = realloc(ob_ptr->string_ptr,4096);
   ob_ptr->string_space_left += 4096;
   }

next_ptr = ob_ptr->string_ptr + STRTABSIZE(ob_ptr);

strcpy(next_ptr,instr);
offset = STRTABSIZE(ob_ptr);
ob_ptr->string_space_left -= (strlen(instr) + 1);
STRTABSIZE(ob_ptr) += (strlen(instr) + 1);

return(offset);

}

/*-------------------------------------------------------------------*/
/* Allocate and append a new CoffSym node to the list in the obj hdr */
/*-------------------------------------------------------------------*/

CoffSym_ptr CoffCreateSymbol   (ObjFile_ptr ob_ptr)

{

CoffSym_ptr		last_sym_ptr;

if (ob_ptr->first_sym_ptr == NULL)
   {
   ob_ptr->first_sym_ptr = malloc(sizeof(CoffSym));
   ob_ptr->first_sym_ptr->sym_idx = 0;
   ob_ptr->first_sym_ptr->next_ptr = NULL;
   memset(ob_ptr->first_sym_ptr->sym.Name.ShortName,0,8);
   ob_ptr->first_sym_ptr->sym.NumberOfAuxSymbols = 0;
   ob_ptr->first_sym_ptr->sym.SectionNumber      = 0;
   ob_ptr->first_sym_ptr->sym.StorageClass       = 0;
   ob_ptr->first_sym_ptr->sym.Type               = 0;
   ob_ptr->first_sym_ptr->sym.Value              = 0;
   return(ob_ptr->first_sym_ptr);
   }

last_sym_ptr = ob_ptr->first_sym_ptr;

while (last_sym_ptr->next_ptr != NULL)
      {
	  last_sym_ptr = last_sym_ptr->next_ptr;
	  }

last_sym_ptr->next_ptr = malloc(sizeof(CoffSym));
last_sym_ptr->next_ptr->sym_idx = last_sym_ptr->sym_idx + 1;
last_sym_ptr->next_ptr->next_ptr = NULL;

memset(last_sym_ptr->next_ptr->sym.Name.ShortName,0,8);
last_sym_ptr->next_ptr->sym.NumberOfAuxSymbols = 0;
last_sym_ptr->next_ptr->sym.SectionNumber      = 0;
last_sym_ptr->next_ptr->sym.StorageClass       = 0;
last_sym_ptr->next_ptr->sym.Type               = 0;
last_sym_ptr->next_ptr->sym.Value              = 0;

return(last_sym_ptr->next_ptr);

}


/*-----------------------------------------------------------------*/
/* This function returns a pointer to the specified section header */
/*-----------------------------------------------------------------*/

Section_ptr CoffGetSectionPtr (ObjFile_ptr ob_ptr,short sect_num)

{

return(ob_ptr->sec_ptr[sect_num]);

}

/*--------------------------------------------------------------------------*/
/* Write all object file data to the file. This is the header, sections etc */
/*--------------------------------------------------------------------------*/
   
void CoffWriteObjFile (ObjFile_ptr	ob_root)

{

short		    S,N;
Section_ptr	    sect_ptr,prev_ptr;
CoffSym_ptr		sym_ptr;
CoffRel_ptr		rel_ptr;
CoffReloc_ptr   reloc_ptr;
long			size_of_relocs;
size_t          length;
//long			file_posn;

/* Write the header */

fwrite (&(ob_root->hdr),sizeof(CoffHdr),1,OB);

N = ob_root->hdr.NumberOfSections;

sect_ptr = ob_root->sec_ptr[1];

sect_ptr->sect.PointerToRawData     = sizeof(CoffHdr) + (sizeof(SectHdr) * N);

if (sect_ptr->sect.NumberOfRelocations > 0)
   sect_ptr->sect.PointerToRelocations = sect_ptr->sect.PointerToRawData + sect_ptr->sect.SizeOfRawData;

fwrite (&(sect_ptr->sect),sizeof(SectHdr),1,OB);

for (S=2; S <= N; S++)
    {
	prev_ptr = sect_ptr;
	size_of_relocs = prev_ptr->sect.NumberOfRelocations * sizeof(CoffReloc);
    sect_ptr = ob_root->sec_ptr[S];
    sect_ptr->sect.PointerToRawData     = prev_ptr->sect.PointerToRawData + prev_ptr->sect.SizeOfRawData + size_of_relocs;

	if (sect_ptr->sect.NumberOfRelocations > 0)
	   sect_ptr->sect.PointerToRelocations = sect_ptr->sect.PointerToRawData + sect_ptr->sect.SizeOfRawData;
	else
	   sect_ptr->sect.PointerToRelocations = 0;

    fwrite (&(sect_ptr->sect),sizeof(SectHdr),1,OB);
	}

for (S=1; S <= N; S++)
    {
	sect_ptr =  ob_root->sec_ptr[S];

	if (sect_ptr->data_ptr != NULL)
	   {
       fwrite ((sect_ptr->data_ptr),sect_ptr->sect.SizeOfRawData,1,OB);

  	   rel_ptr = sect_ptr->reloc_ptr;

       length = sizeof(CoffReloc);

	   while (rel_ptr != NULL)
	         {
		     reloc_ptr = &(rel_ptr->reloc);
			 fwrite(reloc_ptr,length,1,OB);
             rel_ptr = rel_ptr->next_ptr;
	         }
	   }
	}

/*------------------------------------------------------------------------------------*/
/* OK We now write out the symbol table entries. We first do an ftell to get the byte */
/* offset at which we are going to begin doing this, this needs to go in the header.  */
/*------------------------------------------------------------------------------------*/

if (ob_root->first_sym_ptr != NULL)
   {
   ob_root->hdr.PointerToSymbolTable = ftell(OB);
   fwrite (&(ob_root->first_sym_ptr->sym),18,/* sizeof(CoffSymbol) */ 1,OB);

   sym_ptr = ob_root->first_sym_ptr->next_ptr;

   while (sym_ptr != NULL)
         {
         fwrite (&(sym_ptr->sym),18,/* sizeof(CoffSymbol) */ 1,OB);
		 ob_root->hdr.NumberOfSymbols = sym_ptr->sym_idx + 1;
         sym_ptr = sym_ptr->next_ptr;
		 }
   }

/* OK Now write the string table */

if (ob_root->string_ptr != NULL)
   fwrite (ob_root->string_ptr,STRTABSIZE(ob_root),1,OB);

/*-------------------------------------------------------------------------------------------*/
/* Finally reposition to the start of the file and re-write the header with its final values */
/*-------------------------------------------------------------------------------------------*/

//file_posn = ftell(OB);

fseek (OB,0,SEEK_SET);

fwrite (&(ob_root->hdr),sizeof(CoffHdr),1,OB);

//fseek (OB,file_posn,SEEK_SET);

}

void CoffInsertReloc (Section_ptr sect_ptr, CoffReloc_ptr reloc_ptr)

{

CoffRel_ptr	temp_ptr;

if (sect_ptr->reloc_ptr == NULL)
   {
   sect_ptr->reloc_ptr                = malloc(sizeof(CoffRel));
   sect_ptr->sect.NumberOfRelocations = 1;
   sect_ptr->num_relocs               = 1;
   sect_ptr->reloc_ptr->reloc         = *(reloc_ptr);
   sect_ptr->reloc_ptr->next_ptr      = NULL;
   return;
   } 

temp_ptr                            = malloc(sizeof(CoffRel));
temp_ptr->next_ptr                  = sect_ptr->reloc_ptr;
sect_ptr->sect.NumberOfRelocations += 1;
sect_ptr->num_relocs               += 1;
sect_ptr->reloc_ptr                 = temp_ptr;
sect_ptr->reloc_ptr->reloc          = *(reloc_ptr);

}