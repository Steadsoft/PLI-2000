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
/*                  Symbol Table Function Declarations                      */
/****************************************************************************/

Symbol_ptr get_symbol   (Block_ptr,
                         char[]);

static Symbol_ptr find_declaration  (Block_ptr,
                              char[]);

Block_ptr  find_inner_block  (Block_ptr,
                              char[]);


Block_ptr  insert_block (Block_ptr,
                         char[],
                         short,
                         short,
                         short,
                         short);

Symbol_ptr add_symbol   (Block_ptr,
                         char *,
                         short,
                         short,
                         short,
                         short,
                         short,
                         short,     
                         short,
                         short,
                         short,
                         Symbol_ptr, 
                         char *);             


/***************************** end of include *******************************/


