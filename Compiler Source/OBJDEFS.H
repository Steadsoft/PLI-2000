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
/*               DOS Object module, record type names.                     */
/***************************************************************************/

   # define THEADR    (unsigned char)'\x80' 
   # define COMENT    (unsigned char)'\x88' 
   # define MODEND    (unsigned char)'\x8A' 
   # define EXTDEF    (unsigned char)'\x8C' 
   # define TYPDEF    (unsigned char)'\x8E' 
   # define PUBDEF    (unsigned char)'\x90' 
   # define LINNUM    (unsigned char)'\x94' 
   # define LNAMES    (unsigned char)'\x96' 
   # define SEGDEF    (unsigned char)'\x98' 
   # define GRPDEF    (unsigned char)'\x9A' 
   # define FIXUPP    (unsigned char)'\x9C' 
   # define LEDATA    (unsigned char)'\xA0' 
   # define LIDATA    (unsigned char)'\xA2' 
   # define COMDEF    (unsigned char)'\xB0' 

   # define CODE_CLASS        1     /* these four are used to generate */
   # define CODE_NAME         2     /* index numbers for seg refs      */
   # define DATA_CLASS        3
   # define DATA_NAME         4
   # define STACK_CLASS       5
   # define STACK_NAME        6

/*   # define DATA_SEGDEF       5
   # define CODE_SEGDEF       6    */
   

   /*********************************************************************/
   /* These defines mark the offset values within an object record for  */
   /* various object record fields.                                     */
   /*********************************************************************/

   # define REC_TYPE                0
   # define MODEND_MODULE_TYPE      3  
   # define REC_LENGTH              1

/****************************  end of include ******************************/
     
