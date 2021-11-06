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

/**************************************************************************/
/* This include file, simplifies the clumsy syntax of C. It defines a new */
/* set of pointers for all C data types, and removes the need to use *    */
/**************************************************************************/

typedef char          *char_ptr;
typedef int           *int_ptr;
typedef long          *long_ptr;
typedef short         *short_ptr;
typedef unsigned char *uchar_ptr;
typedef unsigned int  *uint_ptr;
typedef double        *double_ptr;
typedef float         *float_ptr;
typedef void          *any_ptr;

/**************************  end of include ******************************/

