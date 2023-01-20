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

# define NOT_YET_SET				32000 /* reserved error numbers	 */
# define INTERNAL_ERROR				32001

/****************************** tokens.h ***********************************/
/* IMPORTANT: When any changes are made to this you must update the symbol */
/* table declaration in lex.c to reflect the change exactly.               */
/************************** PL1 Language Tokens ****************************/

# define PROCEDURE         0            /* procedure                       */
# define DECLARE           1            /* declare                         */
# define OPTIONS           2            /* options                         */
# define DO                3            /* do                              */
# define END               4            /* end                             */
# define SEMICOLON         5            /* ;                               */
# define COLON             6            /* :                               */
# define BEGIN             7            /* begin                           */
# define ON                8            /* on                              */
# define BINARY            9            /* binary                          */
# define DECIMAL          10            /* decimal                         */
# define DEFINED          11            /* defined                         */
# define BASED            12            /* based                           */
# define STATIC           13            /* static                          */
# define FIXED            14            /* fixed                           */
# define D_FLOAT          15            /* float                           */
# define LPAR             16            /* (                               */
# define RPAR             17            /* )                               */
# define MINUS            18            /* -                               */
# define PLUS             19            /* +                               */
# define STAR             20            /* *                               */
# define TIMES            20            /* Alternative !                   */    
# define DIVIDE           21            /* /                               */
# define POINTER          22            /* pointer                         */
# define BCOMMENT         23            /* / 'followed by' *               */
# define ECOMMENT         24            /* * 'followed by' /               */
# define CHARACTER        25            /* character                       */
# define ENTRY            26            /* entry                           */
# define TO               27            /* to                              */
# define EQUALS           28            /* =                               */
# define IF               29            /* if                              */
# define THEN             30            /* then                            */
# define ELSE             31            /* else                            */
# define DOT              32            /* .                               */
# define CALL             33            /* call                            */
# define GOTO             34            /* goto                            */
# define WHILE            35            /* while                           */
# define RETURN           36            /* return                          */
# define BIT              37            /* bit                             */
# define VARYING          38            /* varying                         */
# define VARIABLE         39            /* variable                        */
# define NOT              40            /* ^                               */
# define GT               41            /* >                               */
# define LT               42            /* <                               */
# define AND              43            /* &                               */
# define OR               44            /* |                               */
# define CONCAT           45            /* ||                              */
# define GE               46            /* >=                              */
# define LE               47            /* <=                              */
# define NOTEQUAL         48            /* ^=                              */
# define POINTING         49            /* ->                              */
# define PICTURE          50            /* picture                         */
# define BY               51            /* by                              */
# define LIKE             52            /* like                            */
# define ALIGNED          53            /* aligned                         */
# define ADDR             54            /* addr                            */
# define ALLOCATE         55            /* allocate                        */
# define SET              56            /* set                             */
# define RETURNS          57            /* returns                         */
# define RECURSIVE        58            /* recursive                       */
# define STOP             59            /* stop                            */    
# define FREE             60            /* free                            */
# define OTHER            61            /* otherwise                       */
# define SELECT           62            /* select                          */
# define WHEN             63            /* when                            */        
# define COMMA            64            /* ,                               */
# define PREPROCESSOR     65            /* %                               */
# define AUTOMATIC        66            /* automatic                       */   
# define INTERNAL         67            /* internal                        */
# define EXTERNAL         68            /* external                        */   
# define CONDITION        69            /* condition                       */
# define _UNALIGNED       70            /* unaligned                       */
# define BUILTIN          71            /* builtin                         */    
# define PUT              72            /* put                             */
# define SKIP             73            /* skip                            */
# define LIST             74            /* list                            */    
# define LEAVE            75            /* leave                           */      
# define CONVERSION       76            /* conversion                      */
# define FIXEDOVERFLOW    77            /* fixedoverflow                   */
# define OVER_FLOW        78            /* overflow                        */
# define SIZE             79            /* size                            */
# define UNDER_FLOW       80            /* underflow                       */
# define ZERODIVIDE       81            /* zerodivide                      */
# define BEGINPAGE        82            /* beginpage                       */
# define ENDFILE          83            /* endfile                         */
# define ENDPAGE          84            /* endpage                         */
# define KEY              85            /* key                             */
# define RECORD           86            /* record                          */
# define TRANSMIT         87            /* transmit                        */
# define UNDEFINEDFILE    88            /* undefinedfile                   */
# define AREA             89            /* area                            */
# define ERROR            90            /* error                           */
# define FINISH           91            /* finish                          */
# define STORAGE          92            /* storage                         */ 
# define ALL              93            /* all                             */
# define ANY              94            /* any                             */
# define UNTIL            95            /* until                           */
# define VOLATILE         96            /* volatile                        */ 
# define PCNT_INCLUDE     97            /* % include                       */
# define PCNT_REPLACE     98            /* % replace                       */
# define PCNT_LIST        99            /* % list                          */
# define PCNT_NOLIST     100            /* % nolist                        */         
# define PCNT_PAGE       101            /* % page                          */  
# define MAIN            102            /* main                            */
# define STACK           103            /* stack                           */  
# define FILE_TYPE       104            /* file                            */
# define SIGNAL          105            /* signal                          */ 
# define GO              106            /* go                              */
# define NAME            107            /* name                            */
# define INITIAL         108			/* initial                         */
# define NOTGT           109			/* ^>							   */
# define NOTLT			 110			/* ^<							   */
# define NOTGE			 111			/* ^>=							   */
# define NOTLE			 112			/* ^<=                             */
# define OPEN            113            /* open                            */
# define CLOSE           114            /* close                           */
# define READ            115            /* read                            */
# define WRITE           116            /* write                           */
# define ENVIRONMENT     117            /* environment                     */
# define STREAM          118            /* stream                          */
# define INPUT           119            /* input                           */
# define OUTPUT          120            /* output                          */
# define UPDATE          121            /* update                          */
# define SEQUENTIAL      122            /* sequential                      */
# define DIRECT          123            /* direct                          */
# define PRINT           124            /* print                           */
# define NONPRINT        125            /* nonprint                        */
# define KEYED           126            /* keyed                           */
# define TITLE           127            /* title                           */
# define LINESIZE        128            /* linesize                        */
# define PAGESIZE        129            /* pagesize                        */
# define DO_LOOP         130            /* loop                            */
# define REPEAT          131            /* repeat                          */
# define REWRITE         132            /* rewrite                         */
# define DELETE          133            /* delete                          */ 
# define INTO            134            /* into                            */
# define KEYTO           135            /* keyto                           */
# define SIZETO          136            /* sizeto                          */
# define FROM            137
# define KEYFROM         138 
# define GET             139
# define EDIT            140
# define TAB             141
# define LINE            142
# define COLUMN          143
# define STRING_IO       144
# define PAGE            145
 
# define STMT             32            /* This is NOT a real token !      */
# define STRING          201            /* a string  constant              */
# define NUMERIC         202            /* a numeric constant              */
# define PL1NAME         203            /* a pl1 identifier                */
# define END_OF_SOURCE   204            /* end of input stream             */
# define BAD_TOKEN       205            /* error in token structure        */
# define PARAMETER       206            /* This IS NOT a real token !      */
# define LABEL           207            /* This IS NOT a real token !      */
# define CONSTANT        208            /* This IS NOT a real token !      */
# define ASSIGNMENT      209            /* This IS NOT a real token !      */
# define LOOP            210            /* This IS NOT a real token !      */    
# define ARGUMENT        211            /* This is NOT a real token !      */
# define UNKNOWN         213            /* This is NOT a real token !      */
# define EXPRESSION      214            /* This is NOT a real token !      */
# define SYMBOL          215            /* This is NOT a real token !      */ 
# define ARRAY           216            /* This is NOT a real token !      */
# define REFERENCE       217            /* This is NOT a real token !      */
# define SUBSCRIPT       218            /* This is NOT a real token !      */ 
# define BIT_STRING      219            /* ....                            */      
# define IO_CONDITION    220
# define COMP_CONDITION  221
# define OTHER_CONDITION 221
# define USER_CONDITION  222 
# define DATA            223
# define VALUE           224
# define IARRAY		     225
# define IELEM			 226
# define ICON1			 227
# define ICON2			 228
# define STRFORMAT       229
# define PICFORMAT       230
# define FLOATFORMAT     231
# define FIXEDFORMAT     232
# define REALFORMAT      233
# define REMOTEFORMAT    234
# define CTRLFORMAT      235
# define DATAFORMAT      236
# define FORMATITEM      237
# define FORMAT          238
# define INTARG          239
# define EDITIN          240
# define LISTIN          241
# define INSPEC          242
# define GETSTRING       243
# define GETFILE         244
# define PUTSTRING       245
# define PUTFILE         246
# define OUTSRC          247

# define MAX_TOKEN       247            /* Highest entry in this file      */
  
/***************************** end of file *********************************/


