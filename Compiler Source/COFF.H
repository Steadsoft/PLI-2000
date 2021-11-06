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

//
// File header format.
//
#if defined(_M_MRX000) || defined(_M_ALPHA) || defined(_M_PPC) || (defined(_M_IA64) && !defined(__ICL))
#define UN_ALIGNED __unaligned
#else
#define UN_ALIGNED
#endif

typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef short               SHORT;
typedef unsigned char       BYTE;
typedef BYTE               *PBYTE;

#define IMAGE_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved externel references).
#define IMAGE_FILE_LINE_NUMS_STRIPPED        0x0004  // Line nunbers stripped from file.
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Local symbols stripped from file.
#define IMAGE_FILE_AGGRESIVE_WS_TRIM         0x0010  // Agressively trim working set
#define IMAGE_FILE_LARGE_ADDRESS_AWARE       0x0020  // App can handle >2gb addresses
#define IMAGE_FILE_BYTES_REVERSED_LO         0x0080  // Bytes of machine word are reversed.
#define IMAGE_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.
#define IMAGE_FILE_DEBUG_STRIPPED            0x0200  // Debugging info stripped from file in .DBG file
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400  // If Image is on removable media, copy and run from the swap file.
#define IMAGE_FILE_NET_RUN_FROM_SWAP         0x0800  // If Image is on Net, copy and run from the swap file.
#define IMAGE_FILE_SYSTEM                    0x1000  // System File.
#define IMAGE_FILE_DLL                       0x2000  // File is a DLL.
#define IMAGE_FILE_UP_SYSTEM_ONLY            0x4000  // File should only be run on a UP machine
#define IMAGE_FILE_BYTES_REVERSED_HI         0x8000  // Bytes of machine word are reversed.


#define IMAGE_FILE_MACHINE_UNKNOWN           0
#define IMAGE_FILE_MACHINE_I386              0x014c  // Intel 386.
#define IMAGE_FILE_MACHINE_R3000             0x0162  // MIPS little-endian, 0x160 big-endian
#define IMAGE_FILE_MACHINE_R4000             0x0166  // MIPS little-endian
#define IMAGE_FILE_MACHINE_R10000            0x0168  // MIPS little-endian
#define IMAGE_FILE_MACHINE_WCEMIPSV2         0x0169  // MIPS little-endian WCE v2
#define IMAGE_FILE_MACHINE_ALPHA             0x0184  // Alpha_AXP
#define IMAGE_FILE_MACHINE_POWERPC           0x01F0  // IBM PowerPC Little-Endian
#define IMAGE_FILE_MACHINE_SH3               0x01a2  // SH3 little-endian
#define IMAGE_FILE_MACHINE_SH3E              0x01a4  // SH3E little-endian
#define IMAGE_FILE_MACHINE_SH4               0x01a6  // SH4 little-endian
#define IMAGE_FILE_MACHINE_ARM               0x01c0  // ARM Little-Endian
#define IMAGE_FILE_MACHINE_THUMB             0x01c2
#define IMAGE_FILE_MACHINE_IA64              0x0200  // Intel 64
#define IMAGE_FILE_MACHINE_MIPS16            0x0266  // MIPS
#define IMAGE_FILE_MACHINE_MIPSFPU           0x0366  // MIPS
#define IMAGE_FILE_MACHINE_MIPSFPU16         0x0466  // MIPS
#define IMAGE_FILE_MACHINE_ALPHA64           0x0284  // ALPHA64
#define IMAGE_FILE_MACHINE_AXP64             IMAGE_FILE_MACHINE_ALPHA64
#define IMAGE_FILE_MACHINE_CEF               0xC0EF

#define IMAGE_SIZEOF_SHORT_NAME              8


//
// Section characteristics.
//
//      IMAGE_SCN_TYPE_REG                   0x00000000  // Reserved.
//      IMAGE_SCN_TYPE_DSECT                 0x00000001  // Reserved.
//      IMAGE_SCN_TYPE_NOLOAD                0x00000002  // Reserved.
//      IMAGE_SCN_TYPE_GROUP                 0x00000004  // Reserved.
#define IMAGE_SCN_TYPE_NO_PAD                0x00000008  // Reserved.
//      IMAGE_SCN_TYPE_COPY                  0x00000010  // Reserved.

#define IMAGE_SCN_CNT_CODE                   0x00000020  // Section contains code.
#define IMAGE_SCN_CNT_INITIALIZED_DATA       0x00000040  // Section contains initialized data.
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA     0x00000080  // Section contains uninitialized data.

#define IMAGE_SCN_LNK_OTHER                  0x00000100  // Reserved.
#define IMAGE_SCN_LNK_INFO                   0x00000200  // Section contains comments or some other type of information.
//      IMAGE_SCN_TYPE_OVER                  0x00000400  // Reserved.
#define IMAGE_SCN_LNK_REMOVE                 0x00000800  // Section contents will not become part of image.
#define IMAGE_SCN_LNK_COMDAT                 0x00001000  // Section contents comdat.
//                                           0x00002000  // Reserved.
//      IMAGE_SCN_MEM_PROTECTED - Obsolete   0x00004000
#define IMAGE_SCN_NO_DEFER_SPEC_EXC          0x00004000  // Reset speculative exceptions handling bits in the TLB entries for this section.
#define IMAGE_SCN_GPREL                      0x00008000  // Section content can be accessed relative to GP
#define IMAGE_SCN_MEM_FARDATA                0x00008000
//      IMAGE_SCN_MEM_SYSHEAP  - Obsolete    0x00010000
#define IMAGE_SCN_MEM_PURGEABLE              0x00020000
#define IMAGE_SCN_MEM_16BIT                  0x00020000
#define IMAGE_SCN_MEM_LOCKED                 0x00040000
#define IMAGE_SCN_MEM_PRELOAD                0x00080000

#define IMAGE_SCN_ALIGN_1BYTES               0x00100000  //
#define IMAGE_SCN_ALIGN_2BYTES               0x00200000  //
#define IMAGE_SCN_ALIGN_4BYTES               0x00300000  //
#define IMAGE_SCN_ALIGN_8BYTES               0x00400000  //
#define IMAGE_SCN_ALIGN_16BYTES              0x00500000  // Default alignment if no others are specified.
#define IMAGE_SCN_ALIGN_32BYTES              0x00600000  //
#define IMAGE_SCN_ALIGN_64BYTES              0x00700000  //
#define IMAGE_SCN_ALIGN_128BYTES             0x00800000  //
#define IMAGE_SCN_ALIGN_256BYTES             0x00900000  //
#define IMAGE_SCN_ALIGN_512BYTES             0x00A00000  //
#define IMAGE_SCN_ALIGN_1024BYTES            0x00B00000  //
#define IMAGE_SCN_ALIGN_2048BYTES            0x00C00000  //
#define IMAGE_SCN_ALIGN_4096BYTES            0x00D00000  //
#define IMAGE_SCN_ALIGN_8192BYTES            0x00E00000  //
// Unused                                    0x00F00000
#define IMAGE_SCN_ALIGN_MASK                 0x00F00000

#define IMAGE_SCN_LNK_NRELOC_OVFL            0x01000000  // Section contains extended relocations.
#define IMAGE_SCN_MEM_DISCARDABLE            0x02000000  // Section can be discarded.
#define IMAGE_SCN_MEM_NOT_CACHED             0x04000000  // Section is not cachable.
#define IMAGE_SCN_MEM_NOT_PAGED              0x08000000  // Section is not pageable.
#define IMAGE_SCN_MEM_SHARED                 0x10000000  // Section is shareable.
#define IMAGE_SCN_MEM_EXECUTE                0x20000000  // Section is executable.
#define IMAGE_SCN_MEM_READ                   0x40000000  // Section is readable.
#define IMAGE_SCN_MEM_WRITE                  0x80000000  // Section is writeable.

//
// Section values.
//
// Symbols have a section number of the section in which they are
// defined. Otherwise, section numbers have the following meanings:
//

#define IMAGE_SYM_UNDEFINED           (SHORT)0          // Symbol is undefined or is common.
#define IMAGE_SYM_ABSOLUTE            (SHORT)-1         // Symbol is an absolute value.
#define IMAGE_SYM_DEBUG               (SHORT)-2         // Symbol is a special debug item.

//
// Type (fundamental) values.
//

#define IMAGE_SYM_TYPE_NULL                 0x0000  // no type.
#define IMAGE_SYM_TYPE_VOID                 0x0001  //
#define IMAGE_SYM_TYPE_CHAR                 0x0002  // type character.
#define IMAGE_SYM_TYPE_SHORT                0x0003  // type short integer.
#define IMAGE_SYM_TYPE_INT                  0x0004  //
#define IMAGE_SYM_TYPE_LONG                 0x0005  //
#define IMAGE_SYM_TYPE_FLOAT                0x0006  //
#define IMAGE_SYM_TYPE_DOUBLE               0x0007  //
#define IMAGE_SYM_TYPE_STRUCT               0x0008  //
#define IMAGE_SYM_TYPE_UNION                0x0009  //
#define IMAGE_SYM_TYPE_ENUM                 0x000A  // enumeration.
#define IMAGE_SYM_TYPE_MOE                  0x000B  // member of enumeration.
#define IMAGE_SYM_TYPE_BYTE                 0x000C  //
#define IMAGE_SYM_TYPE_WORD                 0x000D  //
#define IMAGE_SYM_TYPE_UINT                 0x000E  //
#define IMAGE_SYM_TYPE_DWORD                0x000F  //
#define IMAGE_SYM_TYPE_PCODE                0x8000  //

//
// Type (derived) values.
//

#define IMAGE_SYM_DTYPE_NULL                0       // no derived type.
#define IMAGE_SYM_DTYPE_POINTER             1       // pointer.
#define IMAGE_SYM_DTYPE_FUNCTION            2       // function.
#define IMAGE_SYM_DTYPE_ARRAY               3       // array.

//
// Storage classes.
//

#define IMAGE_SYM_CLASS_END_OF_FUNCTION     (BYTE )-1
#define IMAGE_SYM_CLASS_NULL                0x0000
#define IMAGE_SYM_CLASS_AUTOMATIC           0x0001
#define IMAGE_SYM_CLASS_EXTERNAL            0x0002
#define IMAGE_SYM_CLASS_STATIC              0x0003
#define IMAGE_SYM_CLASS_REGISTER            0x0004
#define IMAGE_SYM_CLASS_EXTERNAL_DEF        0x0005
#define IMAGE_SYM_CLASS_LABEL               0x0006
#define IMAGE_SYM_CLASS_UNDEFINED_LABEL     0x0007
#define IMAGE_SYM_CLASS_MEMBER_OF_STRUCT    0x0008
#define IMAGE_SYM_CLASS_ARGUMENT            0x0009
#define IMAGE_SYM_CLASS_STRUCT_TAG          0x000A
#define IMAGE_SYM_CLASS_MEMBER_OF_UNION     0x000B
#define IMAGE_SYM_CLASS_UNION_TAG           0x000C
#define IMAGE_SYM_CLASS_TYPE_DEFINITION     0x000D
#define IMAGE_SYM_CLASS_UNDEFINED_STATIC    0x000E
#define IMAGE_SYM_CLASS_ENUM_TAG            0x000F
#define IMAGE_SYM_CLASS_MEMBER_OF_ENUM      0x0010
#define IMAGE_SYM_CLASS_REGISTER_PARAM      0x0011
#define IMAGE_SYM_CLASS_BIT_FIELD           0x0012

#define IMAGE_SYM_CLASS_FAR_EXTERNAL        0x0044  //

#define IMAGE_SYM_CLASS_BLOCK               0x0064
#define IMAGE_SYM_CLASS_FUNCTION            0x0065
#define IMAGE_SYM_CLASS_END_OF_STRUCT       0x0066
#define IMAGE_SYM_CLASS_FILE                0x0067

// new

#define IMAGE_SYM_CLASS_SECTION             0x0068
#define IMAGE_SYM_CLASS_WEAK_EXTERNAL       0x0069

// type packing constants

#define N_BTMASK                            0x000F
#define N_TMASK                             0x0030
#define N_TMASK1                            0x00C0
#define N_TMASK2                            0x00F0
#define N_BTSHFT                            4
#define N_TSHIFT                            2

// MACROS

// Basic Type of  x

#define BTYPE(x) ((x) & N_BTMASK)

// Is x a pointer?

#ifndef ISPTR
#define ISPTR(x) (((x) & N_TMASK) == (IMAGE_SYM_DTYPE_POINTER << N_BTSHFT))
#endif

// Is x a function?

#ifndef ISFCN
#define ISFCN(x) (((x) & N_TMASK) == (IMAGE_SYM_DTYPE_FUNCTION << N_BTSHFT))
#endif

// Is x an array?

#ifndef ISARY
#define ISARY(x) (((x) & N_TMASK) == (IMAGE_SYM_DTYPE_ARRAY << N_BTSHFT))
#endif

// Is x a structure, union, or enumeration TAG?

#ifndef ISTAG
#define ISTAG(x) ((x)==IMAGE_SYM_CLASS_STRUCT_TAG || (x)==IMAGE_SYM_CLASS_UNION_TAG || (x)==IMAGE_SYM_CLASS_ENUM_TAG)
#endif

#ifndef INCREF
#define INCREF(x) ((((x)&~N_BTMASK)<<N_TSHIFT)|(IMAGE_SYM_DTYPE_POINTER<<N_BTSHFT)|((x)&N_BTMASK))
#endif
#ifndef DECREF
#define DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))
#endif



typedef struct {
    WORD    Machine;
    WORD    NumberOfSections;
    DWORD   TimeDateStamp;
    DWORD   PointerToSymbolTable;
    DWORD   NumberOfSymbols;
    WORD    SizeOfOptionalHeader;
    WORD    Characteristics;
} CoffHdr, *CoffHdr_ptr;

typedef struct {
    char    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
      DWORD   PhysicalAddress;
      DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} SectHdr, *SectHdr_ptr;

typedef struct {
    DWORD   Zero;     // if 0, use ShortName
    DWORD   Offset;   // offset into string table
} SymName;

typedef union {
    char    ShortName[8];
	SymName Name;
} UName;


typedef struct {
    UName   Name;
    DWORD   Value;
    SHORT   SectionNumber;
    WORD    Type;
    BYTE    StorageClass;
    BYTE    NumberOfAuxSymbols;
} CoffSymbol, * CoffSymbol_ptr;

typedef struct coffsym    *CoffSym_ptr;

typedef struct coffsym {
   CoffSymbol	  sym;      /* The COFF symbol itself. This is what gets written to OBJ file */
   long           sym_idx;  /* The zero-based index of this symbol, not part of OBJ.         */
   CoffSym_ptr    next_ptr; /* Pointer to next node in memory, not part of OBJ.              */
} CoffSym;

typedef struct {
    union {
        DWORD   VirtualAddress;
        DWORD   RelocCount;             // Set to the real count when IMAGE_SCN_LNK_NRELOC_OVFL is set
        }RelUn;
    DWORD   SymbolTableIndex;
    WORD    Type;
} CoffReloc, * CoffReloc_ptr;

typedef struct coffrel_tag * CoffRel_ptr; 

typedef struct coffrel_tag {
   CoffReloc	    reloc;
   CoffRel_ptr	    next_ptr;
} CoffRel, * CoffRel_ptr;
//
// Auxiliary entry format.
//

typedef union {
    struct {
        DWORD    TagIndex;                      // struct, union, or enum tag index
        union {
            struct {
                WORD    Linenumber;             // declaration line number
                WORD    Size;                   // size of struct, union, or enum
            } LnSz;
           DWORD    TotalSize;
        } Misc;
        union {
            struct {                            // if ISFCN, tag, or .bb
                DWORD    PointerToLinenumber;
                DWORD    PointerToNextFunction;
            } Function;
            struct {                            // if ISARY, up to 4 dimen.
                WORD     Dimension[4];
            } Array;
        } FcnAry;
        WORD    TvIndex;                        // tv index
    } Sym;
    struct {
        BYTE    Name[sizeof(CoffSymbol)];
    } File;
    struct {
        DWORD   Length;                         // section length
        WORD    NumberOfRelocations;            // number of relocation entries
        WORD    NumberOfLinenumbers;            // number of line numbers
        DWORD   CheckSum;                       // checksum for communal
        SHORT   Number;                         // section number to associate with
        BYTE    Selection;                      // communal selection type
    } Section;
} CoffAuxSym;

typedef CoffAuxSym UN_ALIGNED * CoffAuxSym_ptr;

/*-----------------------------------------*/
/* These are the COFFLIB library functions */
/*-----------------------------------------*/

typedef struct {
	SectHdr  		sect;
	short			sect_num;
	DWORD		    allocated_space;
	Chur_ptr		data_ptr;
	long            num_relocs;
	CoffRel_ptr     reloc_ptr;
} Section, *Section_ptr;

typedef struct {
	CoffHdr		    hdr;
	Section_ptr		sec_ptr[100];
    CoffSym_ptr     first_sym_ptr;
	char *			string_ptr;
	long            string_space_left;
} ObjFile, *ObjFile_ptr;

#define     STRTABSIZE(X)      (*(long *)((X)->string_ptr))

ObjFile_ptr CoffCreateObject   (void);
Section_ptr CoffInsertSection  (ObjFile_ptr);
void        CoffWriteObjFile   (ObjFile_ptr);
Section_ptr CoffGetSectionPtr  (ObjFile_ptr,short);
CoffSym_ptr CoffCreateSymbol   (ObjFile_ptr);
long        CoffInsertString   (ObjFile_ptr ob_ptr, char * instr);
void        CoffInsertReloc    (Section_ptr, CoffReloc_ptr);




