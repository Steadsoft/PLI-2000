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

#include "wtypes.h"
#include "winbase.h"

long MilliSeconds (void)

{

return(GetTickCount());

}

void DebugTrap (void)

{

DebugBreak();

}

void sound (short f)    

{

return;

}

void nosound (void)

{

return;

}      

void gotoxy (int x,int y)

{

return;

}

void clreol (void)

{

return;

}  

void _setcursortype (int type)

{

return;

}


void gettime (struct dostime_t * p)


{

return;

}

void getdate (struct dosdate_t * p)


{

return;

}

void delay (short t)


{

return;

}

void textcolor (int c)

{

return;

}

void os_sleep (int interval)

{

if (interval == -1)
   interval = INFINITE;

Sleep (interval);

}