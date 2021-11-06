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
/* This is used to create OBJ file from the VC tools. This allows us to     */
/* cross check certain machine code generation questions.                   */
/* This is not part of the compiler, just insert code and compile it then   */
/* examine the disassembly listing for insight into the code generation.    */
/* This is used mainly to examine COFF object module records and aids in    */
/* debugging the compilers own COFF management code.                        */
/*--------------------------------------------------------------------------*/

static short exty (short);

long mains (void)

{

static	char	x;
static	short	y;
static  long	z;

x = 1;
y = 2;
z = 3;

exty(y);

return(x + y + z);

}

short exty (short a)

{



}
