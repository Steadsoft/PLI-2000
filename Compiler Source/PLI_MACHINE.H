/*# define ABSOLUTE_ALIGNED  0x00
# define BYTE_ALIGNED      0x20
# define WORD_ALIGNED      0x40
# define PARAGRAPH_ALIGNED 0x60
# define PAGE_ALIGNED      0x80

# define PRIVATE_COMBINE   0x00
# define UNUSED_COMBINE    0x04
# define PUBLIC_COMBINE    0x08
# define UNDEFINED_COMBINE 0x0C
# define MSOFT1_COMBINE    0x10
# define STACK_COMBINE     0x14
# define COMMON_COMBINE    0x18
# define MSOFT2_COMBINE    0x1C 

# define SEG_64K           0x02
*/


/* This is the structure of an entry      */
/* These constants are used to access the */
/* various parts of an entry given the    */
/* address of the entry.                  */

#define CODE_PTR_OFFSET			0
#define DATA_PTR_OFFSET			4
#define DISP_PTR_OFFSET			8

/*--------------------------------------------------------*/
/* These offsets relative to EBP allows us ato access the */
/* following data:	     							      */
/*--------------------------------------------------------*/

#define FRAME_CALLERS_STATIC	8
#define FRAME_RETURN_ADDRESS    4
#define FRAME_CALLERS_FRAME		0
