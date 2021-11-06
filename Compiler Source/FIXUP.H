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
/* This structure is used for creating fixup entries by the code generator. */
/****************************************************************************/

# define FRAME_METHOD_0    0
# define FRAME_METHOD_1    1
# define FRAME_METHOD_2    2
# define FRAME_METHOD_3    3
# define FRAME_METHOD_4    4
# define FRAME_METHOD_5    5

# define TARGT_METHOD_0    0
# define TARGT_METHOD_1    1
# define TARGT_METHOD_2    2
# define TARGT_METHOD_3    3
# define TARGT_METHOD_4    4
# define TARGT_METHOD_5    5
# define TARGT_METHOD_6    6
# define TARGT_METHOD_7    7

# define LOCN_TYPE_0       0
# define LOCN_TYPE_1       1
# define LOCN_TYPE_2       2
# define LOCN_TYPE_3       3
# define LOCN_TYPE_4       4
# define LOCN_TYPE_5       5



typedef struct fixup_st {
               Chur     M;          /* segment relative/self relative */
               Chur     S;          /* MUST ALWAYS BE SET TO ZERO.    */
               Chur     loc;        /* type of locn to fixup          */
               long     offset;     /* offset to fixup in data        */
               Chur     fix_frame;  /* frame fixup method             */
               Chur     fix_target; /* target fixup method            */
               long     frame;      /* index of seg to be fixed       */
               long     target;     /* index of seg containing target */
               long     disp;       /* offset of target in target seg */
               char     frame_present;
               char     target_present;
               char     disp_present;  
               } fixup;

/***************************** end of include ******************************/
               


